#include "isogen.h"
#include "edges.h"
#include <vector>
#include "../../grinliz/gldynamic.h"

using namespace grinliz;
using namespace std;

typedef SDL_Surface* (*ImageLoaderFunc)( const char* );
ImageLoaderFunc ImageLoader = 0;

// Set by the top level <isogen> tag
string spriteName;
int isoWidth = 120;
int tileWidth = isoWidth - 2;
int tileHeight = isoWidth / 2;
int step = 10;

int outWidth = 800;
int outHeight = 600;

Color3F diffuse = { 0.4f, 0.4f, 0.4f };
Color3F ambient = { 0.6f, 0.6f, 0.6f };
Vector3F lightVector = { 0, 2, -3 };


const Vector3F ZFACE = { 0, 0, 1 };
const Vector3F YFACE = { 0, -1, 0 };
const Vector3F XFACE = { -1, 0, 0 };

int saveX = 0;
int saveY = 0;
int saveMax = 0;

vector< SetData > setData;
SetData* pSet = 0;		// the current setData
SDL_Surface* whiteSurface = 0;

void SetData::Clear()
{
	unsigned i;
	for( i=0; i<wallTexture.size(); ++i ) {
		if ( wallTexture[i].surface != whiteSurface )
			SDL_FreeSurface( wallTexture[i].surface );
	}

	for( i=0; i<floorTexture.size(); ++i ) {
		if ( floorTexture[i].surface != whiteSurface )
			SDL_FreeSurface( floorTexture[i].surface );
	}
}

Color4U8 Texture::GetPixel( int u, int v )
{
	GLASSERT( u >=0 && u < surface->w );
	GLASSERT( v >=0 && v < surface->h );

	// Remember that the texture can be any format:
	int bytes = surface->format->BytesPerPixel;
	U8 *p = (U8*)surface->pixels + (surface->h-1-v)*surface->pitch + u*bytes;

	U32 c = 0;
	switch(bytes) {
		case 1:		c = *p;			break;
		case 2:		c = *(U16*)p;	break;
		#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		case 3:		c = p[0] << 16 | p[1] << 8 | p[2];
		#else
		case 3:		c = p[0] | p[1] << 8 | p[2] << 16;
		#endif
		case 4:		c = *(U32*)p;

		default:	break;
	}

	Color4U8 color;
	SDL_GetRGBA( c, surface->format, &color.r, &color.g, &color.b, &color.a );
//	if ( bytes < 4 ) color.a = 255;	// give SDL a little help here. It should always be 255...

	if ( colorKey && ( ( color.r | color.g | color.b ) == 0 ) )
		color.a = 0;

	return color;
}

int Texture::WrapX( SDL_Surface* surface, int x )
{
	while ( x >= surface->w )
		x -= surface->w;
	while ( x < 0 )
		x += surface->w;
	return x;
}


int Texture::WrapY( SDL_Surface* surface, int y )
{
	while ( y >= surface->h )
		y -= surface->h;
	while ( y < 0 )
		y += surface->h;
	return y;
}


Color4U8 Texture::Lookup( int _x, int _y ) 
{
	// x,y: pixels on screen
	// u,x: texels in the texture, normalized
	// i,j: texels in the texture, pixels
	int x = _x - origin.x;
	int y = _y - origin.y;

	float uF = (float)x*a + (float)y*b;
	float vF = (float)x*c + (float)y*d;
	
	float iF = uF*(float)(surface->w);
	float jF = vF*(float)(surface->h);

	int i0 = (int)floorf( iF );
	int j0 = (int)floorf( jF );

	float di = iF-(float)i0;
	float dj = jF-(float)j0;

	GLASSERT( InRange( di, 0.0f, 1.0f ) );
	GLASSERT( InRange( dj, 0.0f, 1.0f ) );

	i0 = WrapX( surface, i0 );
	j0 = WrapY( surface, j0 );

	Color4U8 c01, c10, c11;
	Color4U8 c = { 0, 0, 0, 0 };
	Color4U8 c00 = GetPixel( i0, j0 );

	if ( pSet->useAA )
	{
		c10 = GetPixel( WrapX( surface, i0+1 ), j0 );
		c01 = GetPixel( i0, WrapY( surface, j0+1 ) );
		c11 = GetPixel( WrapX( surface, i0+1 ), WrapY( surface, j0+1 ) );
	}
	else
	{
		c10 = c01 = c11 = c00;
	}

	// Try to deal with the color ghosting problem. A pixel that isn't 0x00000000 probably is
	// a color with alpha. A truly black, transparent pixel is probably just extra. 

	if (    ( ( c00.a | c01.a | c10.a | c11.a ) == 0 )
		 || ( ( c00.a & c01.a & c10.a & c11.a ) == 255 ) )
	{
		float r = BilinearInterpolate( (float)c00.r, (float)c10.r, (float)c01.r, (float)c11.r, di, dj );
		float g = BilinearInterpolate( (float)c00.g, (float)c10.g, (float)c01.g, (float)c11.g, di, dj );
		float b = BilinearInterpolate( (float)c00.b, (float)c10.b, (float)c01.b, (float)c11.b, di, dj );

		c.r = (U8)(r);
		c.g = (U8)(g);
		c.b = (U8)(b);
		c.a = c00.a; // whatever the value is, they are all the same
	}
	else
	{
//		float weight = 0.0f;
//		float r = 0.0f, g = 0.0f, b = 0.0f;

		float qr[4] = { c00.r, c10.r, c01.r, c11.r };
		float qg[4] = { c00.g, c10.g, c01.g, c11.g };
		float qb[4] = { c00.b, c10.b, c01.b, c11.b };

		float w[4] = {	c00.a ? 1.0f : 0.0f,
						c01.a ? 1.0f : 0.0f,
						c10.a ? 1.0f : 0.0f,
						c11.a ? 1.0f : 0.0f };
						
		// The RGB channel uses a weighted interpolation to try to not pick up a transparent
		// pixel of the wrong color. The A channel is a simple bi-linear interp.
		c.r = (U8)BilinearInterpolate( qr, di, dj, w );
		c.g = (U8)BilinearInterpolate( qg, di, dj, w );
		c.b = (U8)BilinearInterpolate( qb, di, dj, w );
		c.a = (U8)BilinearInterpolate( (float)c00.a, (float)c10.a, (float)c01.a, (float)c11.a, di, dj );
	}
	return c;
}

/* Set up a texture. Requires:
	 - P the origin of the texture in screen pixels
	 - Q the u-stride of the texture in screen pixels
	 - R the v-stride of the texture in screen pixels
*/
void Texture::InitTexture(	SDL_Surface* s, 
							bool overlay,
							const Vector2I& p, 
							const Vector2I& _q,
							const Vector2I& _r,
							U8 alpha,
							bool emit )
{
	if ( !s ) {
		surface = 0;
		return;
	}
	this->alpha = alpha;
	this->emit = emit;
		
	if ( overlay && s->format->BytesPerPixel < 4 )
		colorKey = true;

	surface = s;
	origin = p;

	Vector2I q = { _q.x - p.x, _q.y - p.y };
	Vector2I r = { _r.x - p.x, _r.y - p.y };

	// derived from at q, u=1, v=0
	// at r, u=0, v=1

	int abDenom = q.y*r.x - q.x*r.y;
	int cdDenom = q.x*r.y - q.y*r.x;
	GLASSERT( abDenom );
	GLASSERT( cdDenom );

	a = (float)(-r.y) / (float)(abDenom);
	b = (float)(r.x)  / (float)(abDenom);
	c = (float)(-q.y) / (float)(cdDenom);
	d = (float)(q.x)  / (float)(cdDenom);
}


SDL_Surface* CreateSurface( int x, int y, int z )
{
	int sx = isoWidth*(x+y)/2;
	int sy = tileHeight*(1+z);

	SDL_Surface* surface = SDL_CreateRGBSurface(	SDL_SWSURFACE, 
													sx, sy,
													32, 
													0xff, 0xff<<8, 0xff<<16, 0xff<<24 );
	return surface;
}


void Render(	SDL_Surface* surface, 
				list< Edge* > *leftEdges, list< Edge* > *rightEdges, 
				const Vector3F& normal,
				Texture* textureArr,
				int numTexture )
{
	// Initialize the edges.
	Edge* left = leftEdges->front();
	leftEdges->pop_front();
	Edge* right = rightEdges->front();
	rightEdges->pop_front();

	// Compute the color components.
	Color3F light;
	float dot = DotProduct( normal, lightVector );
	GLASSERT( dot >= 0.0f );
	light.r = ambient.r + diffuse.r * dot;
	light.g = ambient.g + diffuse.g * dot;
	light.b = ambient.b + diffuse.b * dot;
	U8 r = Clamp( LRintf( 255.0f * light.r ), (long)0, (long)255 );
	U8 g = Clamp( LRintf( 255.0f * light.g ), (long)0, (long)255 );
	U8 b = Clamp( LRintf( 255.0f * light.b ), (long)0, (long)255 );

	// Main rendering loop.
	while ( left && right )
	{
		GLASSERT( left->Y() == right->Y() );

		int xmin = left->X();
		int xmax = right->X();

		GLASSERT( xmin <= xmax );

		if ( left->NoTouch() ) xmin++;
		if ( right->NoTouch() ) xmax--;

		GLASSERT( left->Y() >= 0 && left->Y() < surface->h );
		U32* scanline = (U32*)((U8*)surface->pixels + left->Y()*surface->pitch);

		const U32 color = (r) | (g<<8) | (b<<16) | (255<<24);
		Color4U8 frag;
		Color4U8 cPrime;

		for( int i=0; i<numTexture; ++i )
		{
			Texture* texture = &textureArr[i];

			for( int x=xmin; x<=xmax; ++x ) 
			{
				GLASSERT( x >= 0 && x < surface->w );
				#ifdef DEBUG
				if ( i == 0 ) GLASSERT( !texture || !texture->Valid() || texture->Alpha() == 255 );
				#endif

				if ( texture && texture->Valid() ) {

					// Get the textel color
					grinliz::Color4U8 textel = texture->Lookup( x, left->Y() );

					if ( !texture->Emit() ) {
						// Not emmissive, so apply the lighting.
						textel.r = textel.r * r / 255;
						textel.g = textel.g * g / 255;
						textel.b = textel.b * b / 255;
						// the alpha is uneffected
					}

					if ( texture->Alpha() == 255 && textel.a == 255 )
					{
						// Opaque
						scanline[x] = (textel.r) | (textel.g<<8) | (textel.b<<16) | (255<<24);
					}
					else if ( textel.a != 0 )
					{
						// Translucent (or possibly transparent)
						U32 alpha = texture->Alpha() * textel.a / 255;
						GLASSERT( alpha < 256 );

						frag.r = (scanline[x]>>0)  & 0xff;
						frag.g = (scanline[x]>>8)  & 0xff;
						frag.b = (scanline[x]>>16) & 0xff;

						cPrime.r = ( frag.r*(255-alpha) + textel.r*alpha )/255;
						cPrime.g = ( frag.g*(255-alpha) + textel.g*alpha )/255;
						cPrime.b = ( frag.b*(255-alpha) + textel.b*alpha )/255;

						scanline[x] = (cPrime.r) | (cPrime.g<<8) | (cPrime.b<<16) | (255<<24);
					}
				}
				else {	
					scanline[x] = color;
				}
			}
		}		
		// Through all the texture stuff. Back to edge processing.
		if ( !left->Next() ) {
			delete left;
			left = 0;
			if ( !leftEdges->empty() ) {
				left = leftEdges->front();
				leftEdges->pop_front();
			}
		}
		if ( !right->Next() ) {
			delete right;
			right = 0;
			if ( !rightEdges->empty() ) {
				right = rightEdges->front();
				rightEdges->pop_front();
			}
		}
	}
}


void RenderZ( SDL_Surface* surface, U32 flags )
{
	list< Edge* > left, right;

	int leftX0 = tileWidth/2-1;
	int leftX1 = 0;
	int leftY0min = 0;
	int leftY0max = tileHeight/2-1;
	int leftY1min = tileHeight/2;
	int leftY1max = tileHeight-1;

	int rightX0 = tileWidth/2;
	int rightX1 = tileWidth-1;
	int rightY0min = 0;
	int rightY0max = tileHeight/2-1;
	int rightY1min = tileHeight/2;
	int rightY1max = tileHeight-1;

	int ox = 1;

	if ( flags & NORTH )
	{
		left.push_back( new IsoEdgeNeg( leftX0-step*2+ox, leftY0min+step, leftY0max-step ));
		left.push_back( new IsoEdgePos( leftX1+step*2+ox, leftY1min-step, leftY1max-step*3 ));

		right.push_back( new IsoEdgePos( rightX0-step*2+ox, rightY0min+step, rightY0max-step ));
		right.push_back( new IsoEdgeNeg( rightX1-step*6+ox, rightY1min-step, rightY1max-step*3 ));		
	}
	else if ( flags & SOUTH )
	{
		left.push_back( new IsoEdgeNeg( leftX0+step*2+ox, leftY0min+step*3, leftY0max+step ));
		left.push_back( new IsoEdgePos( leftX1+step*6+ox, leftY1min+step, leftY1max-step ));

		right.push_back( new IsoEdgePos( rightX0+step*2+ox, rightY0min+step*3, rightY0max+step ));
		right.push_back( new IsoEdgeNeg( rightX1-step*2+ox, rightY1min+step, rightY1max-step ));		
	}
	else if ( flags & CENTER )
	{
		left.push_back( new IsoEdgeNeg( leftX0+ox, leftY0min+step*2, leftY0max ));
		left.push_back( new IsoEdgePos( leftX1+step*4+ox, leftY1min, leftY1max-step*2 ));

		right.push_back( new IsoEdgePos( rightX0+ox, rightY0min+step*2, rightY0max ));
		right.push_back( new IsoEdgeNeg( rightX1-step*4+ox, rightY1min, rightY1max-step*2 ));		
	}
	else if ( flags & EAST )
	{
		left.push_back( new IsoEdgeNeg( leftX0+step*2+ox, leftY0min+step, leftY0max-step ));
		left.push_back( new IsoEdgePos( leftX1+step*6+ox, leftY1min-step, leftY1max-step*2 ));

		right.push_back( new IsoEdgePos( rightX0+step*2+ox, rightY0min+step, rightY0max-step ));
		right.push_back( new IsoEdgeNeg( rightX1-step*2+ox, rightY1min-step, rightY1max-step*3 ));		
	}
	else if ( flags & WEST )
	{
		left.push_back( new IsoEdgeNeg( leftX0-step*2+ox, leftY0min+step*3, leftY0max+step ));
		left.push_back( new IsoEdgePos( leftX1+step*2+ox, leftY1min+step, leftY1max-step ));

		right.push_back( new IsoEdgePos( rightX0-step*2+ox, rightY0min+step*3, rightY0max+step ));
		right.push_back( new IsoEdgeNeg( rightX1-step*6+ox, rightY1min+step, rightY1max-step ));		
	}
	else
	{
		// Simple draw.
		left.push_back( new IsoEdgeNeg( leftX0+ox, leftY0min, leftY0max ));
		left.push_back( new IsoEdgePos( leftX1+ox, leftY1min, leftY1max ));

		right.push_back( new IsoEdgePos( rightX0+ox, rightY0min, rightY0max ));
		right.push_back( new IsoEdgeNeg( rightX1+ox, rightY1min, rightY1max ));
	}

	//Texture tex[ MAX_TEXTURE_COUNT ];
	std::vector< Texture > tex( pSet->floorTexture.size() );

	if ( flags & USE_TEXTURE )
	{
		// Note that for the Z face, the texture coordinates never change if
		// an inset is used.
		Vector2I p = { tileWidth/2-1+ox, tileHeight };
		Vector2I q = { p.x + isoWidth / 2, p.y - isoWidth / 4 };
		Vector2I r = { p.x - isoWidth / 2, p.y - isoWidth / 4 };

		for( unsigned i=0; i<pSet->floorTexture.size(); ++i ) {
			tex[i].InitTexture( pSet->floorTexture[i].surface, i>0, p, q, r, 
								pSet->floorTexture[i].alpha,
								pSet->floorTexture[i].emit );
		}
	}
	GLASSERT( pSet->floorTexture.size() >=1 );
	int numTexture = (flags & OVERLAYS) ? pSet->floorTexture.size() : 1;
	Render( surface, &left, &right, ZFACE, &tex[0], numTexture );
}

void RenderX( SDL_Surface* surface, U32 flags, int xFactor = 0, int yFactor = 0 )
{
	list< Edge* > left, right;

	int ox = 1 + xFactor*isoWidth/2;
	int oy = yFactor*isoWidth/2;
	if ( flags & OFFSET ) {
		ox += step*2;
		oy += -step;
	}

	int tileHeight3_2 = tileHeight + tileHeight/2;

	if ( flags & NORTH )
	{
		left.push_back( new VEdge(	-1+ox, tileHeight/2+oy,	tileHeight3_2-1+oy ));	// see comment in simple case about -1
		left.push_back( new IsoEdgePos(	0+ox, tileHeight3_2+oy, tileHeight3_2+step+oy ));

		Edge* edge = new IsoEdgePos( 0+ox, tileHeight/2+oy, tileHeight/2+step+oy );
		edge->SetNoTouch();
		right.push_back( edge );
		right.push_back( new VEdge( step*2+ox, tileHeight/2+step+1+oy, tileHeight3_2+step+oy ));
	}
	else if ( flags & SOUTH )
	{
		left.push_back( new VEdge(	tileWidth/2-1-2*step+ox, tileHeight-1-step+oy,	tileHeight*2-2-step+oy ));
		left.push_back( new IsoEdgePos(	tileWidth/2-1-2*step+ox, tileHeight*2-1-step+oy, tileHeight*2-1+oy ));

		Edge* edge = new IsoEdgePos( tileWidth/2-1-2*step+ox, tileHeight-1-step+oy, tileHeight-1+oy );
		edge->SetNoTouch();
		right.push_back( edge );
		right.push_back( new VEdge( tileWidth/2-1+ox, tileHeight+oy, tileHeight*2-1+oy ));
	}
	else if ( flags & CENTER )
	{
		left.push_back( new VEdge(	0+step*2+ox, tileHeight/2+step+oy,	tileHeight3_2-1+step+oy ));
		left.push_back( new IsoEdgePos(	0+step*2+ox, tileHeight3_2+step+oy, tileHeight*2-1-step+oy ));

		Edge* edge = new IsoEdgePos( 0+step*2+ox, tileHeight/2+step+oy, tileHeight-1-step+oy );
		edge->SetNoTouch();
		right.push_back( edge );
		right.push_back( new VEdge( tileWidth/2-1-step*2+ox, tileHeight-step+oy, tileHeight*2-1-step+oy ));
	}
	else
	{
		// Note the -1 "cheat" patches up texture seeming between tiles - filling
		// the difference between the isoSize and the tileWidth.
		//
		left.push_back( new VEdge(	-1+ox, tileHeight/2+oy,	tileHeight3_2-1+oy ));
		left.push_back( new IsoEdgePos(	0+ox, tileHeight3_2+oy, tileHeight*2-1+oy ));

		Edge* edge = new IsoEdgePos( 0+ox, tileHeight/2+oy, tileHeight-1+oy );
		edge->SetNoTouch();
		right.push_back( edge );
		right.push_back( new VEdge( tileWidth/2-1+ox, tileHeight+oy, tileHeight*2-1+oy ));
	}

	//Texture tex[ MAX_TEXTURE_COUNT ];
	std::vector< Texture > tex( pSet->wallTexture.size() );
	if ( flags & USE_TEXTURE )
	{
		Vector2I p = { -1+ox, -1+tileHeight + tileHeight/2 + oy};
		Vector2I q = { p.x + isoWidth / 2, p.y + isoWidth / 4 };
		Vector2I r = { p.x, p.y - isoWidth / 2 };
		for( unsigned i=0; i<pSet->wallTexture.size(); ++i ) {
			tex[i].InitTexture( pSet->wallTexture[i].surface, i>0, p, q, r, 
								pSet->wallTexture[i].alpha,
								pSet->wallTexture[i].emit );
		}
	}
	GLASSERT( pSet->wallTexture.size() >=1 );
	int numTexture = (flags & OVERLAYS) ? pSet->wallTexture.size() : 1;

	Render( surface, &left, &right, XFACE, &tex[0], numTexture );
}


void RenderY( SDL_Surface* surface, U32 flags, int xFactor=0, int yFactor=0 )
{
	list< Edge* > left, right;

	int ox = 1 + xFactor*isoWidth/2;
	int oy = 0 + yFactor*isoWidth/2;
	if ( flags & OFFSET ) {
		ox += -step*2;
		oy += -step;
	}

	int tileHeight3_2 = tileHeight+tileHeight/2;

	if ( flags & CENTER )
	{
		Edge* edge = new IsoEdgeNeg( tileWidth-1-step*2+ox, tileHeight/2+step+oy,  tileHeight-1-step+oy );
		edge->SetNoTouch();
		left.push_back( edge );
		left.push_back( new VEdge( tileWidth/2+step*2+ox, tileHeight-step+oy, tileHeight*2-1-step+oy ));

		right.push_back( new VEdge( tileWidth-1-step*2+ox, tileHeight/2+step+oy, tileHeight3_2-1+step+oy ));
		right.push_back( new IsoEdgeNeg( tileWidth-1-step*2+ox, tileHeight3_2+step+oy, tileHeight*2-1-step+oy ));	
	}
	else if ( flags & EAST )
	{
		Edge* edge = new IsoEdgeNeg( tileWidth-1+ox, tileHeight/2+oy,  tileHeight/2+step+oy );
		edge->SetNoTouch();
		left.push_back( edge );
		left.push_back( new VEdge( tileWidth-1-step*2+ox, tileHeight/2+step+1+oy, tileHeight3_2+step+oy ));

		// The +1 in the VEdge x patches the isoTile vs isoWidth discrepancy
		right.push_back( new VEdge( tileWidth-1+ox+1, tileHeight/2+oy, tileHeight3_2-1+oy ));
		right.push_back( new IsoEdgeNeg( tileWidth-1+ox, tileHeight3_2+oy, tileHeight3_2+step+oy ));
	}
	else if ( flags & WEST )
	{
		Edge* edge = new IsoEdgeNeg( tileWidth/2+step*2+ox, tileHeight-1-step+oy,  tileHeight-1+oy );
		edge->SetNoTouch();
		left.push_back( edge );
		left.push_back( new VEdge( tileWidth/2+ox, tileHeight+oy, tileHeight*2-1+oy ));

		right.push_back( new VEdge( tileWidth/2+step*2+ox, tileHeight-1-step+oy, tileHeight*2-2-step+oy ));
		right.push_back( new IsoEdgeNeg( tileWidth/2+step*2+ox, tileHeight*2-1-step+oy, tileHeight*2-1+oy ));
	}
	else
	{
		Edge* edge = new IsoEdgeNeg( tileWidth-1+ox, tileHeight/2+oy,  tileHeight-1+oy );
		edge->SetNoTouch();
		left.push_back( edge );
		left.push_back( new VEdge( tileWidth/2+ox, tileHeight+oy, tileHeight*2-1+oy ));

		// The erraint +1 in the x value of the VEdge account for the difference between
		// the isoWidth and the tileWidth
		right.push_back( new VEdge( tileWidth-1+ox+1, tileHeight/2+oy, tileHeight3_2-1+oy ));
		right.push_back( new IsoEdgeNeg( tileWidth-1+ox, tileHeight3_2+oy, tileHeight*2-1+oy ));
	}

	//Texture tex[ MAX_TEXTURE_COUNT ];
	std::vector< Texture > tex( pSet->wallTexture.size() );
	if ( flags & USE_TEXTURE )
	{
		Vector2I p = { tileWidth/2+ox, tileHeight*2-1+oy };
		Vector2I q = { p.x + isoWidth / 2, p.y - isoWidth / 4 };
		Vector2I r = { p.x, p.y - isoWidth / 2 };

		for( unsigned i=0; i<pSet->wallTexture.size(); ++i ) {
			tex[i].InitTexture( pSet->wallTexture[i].surface, i>0, p, q, r, 
								pSet->wallTexture[i].alpha,
								pSet->wallTexture[i].emit );
		}
	}
	GLASSERT( pSet->wallTexture.size() >=1 );
	int numTexture = (flags & OVERLAYS) ? pSet->wallTexture.size() : 1;

	Render( surface, &left, &right, YFACE, &tex[0], numTexture );
}

void RenderRamp( SDL_Surface* surface, U32 flags, int size )
{
	list< Edge* > left, right;

	int ox = 1;
	int oy = 0;
	int mult = size-1;

//	if ( flags & OFFSET ) {
//		ox += -step*2;
//		oy += -step;
//	}

	int tileHeight3_2 = tileHeight+tileHeight/2;

	Vector2I p, q, r;
	Vector3F normal;

	if ( flags & NORTH )
	{
		left.push_back( new IsoEdgeNeg( tileWidth/2-1+ox, 0+oy, tileHeight/2-1+oy ));
		left.push_back( new LineEdge( 0+ox, tileHeight/2+oy, 
									  tileWidth/2-1+ox+isoWidth*mult/2, tileHeight*2-1+oy+tileHeight3_2*mult ));

		right.push_back( new LineEdge(	tileWidth/2+ox, oy, 
										tileWidth-1+ox+isoWidth*mult/2, tileHeight3_2-1+oy+tileHeight3_2*mult ));
		right.push_back( new IsoEdgeNeg( tileWidth-1+ox+isoWidth*mult/2, tileHeight3_2+oy+tileHeight3_2*mult, tileHeight*2-1+oy+tileHeight3_2*mult ));

		p.x = tileWidth/2-1+ox+tileWidth*mult/2;		p.y = tileHeight*2+oy+tileHeight3_2*mult;
		q.x = p.x + isoWidth/2;		q.y = p.y - isoWidth / 4;
		r.x = p.x - isoWidth/2;		r.y = p.y - isoWidth * 3 / 4;

		normal.Set( 0, -1, 1 );
	}
	else
	{
		left.push_back( new LineEdge(	tileWidth/2-1+ox+mult*isoWidth/2, 0+oy, 
										0+ox, tileHeight3_2-1+oy+tileHeight3_2*mult ) );
		left.push_back( new IsoEdgePos( 0+ox, tileHeight3_2+oy+tileHeight3_2*mult, tileHeight*mult+tileHeight*2-1+oy+tileHeight3_2*mult ) );

		right.push_back( new IsoEdgePos( tileWidth/2+ox+mult*isoWidth/2, 0+oy, tileHeight/2-1+oy ) );
		right.push_back( new LineEdge(	tileWidth-1+ox+mult*isoWidth/2, tileHeight/2+oy, 
										tileWidth/2+ox, tileHeight*2-1+oy+tileHeight3_2*mult ) );

		p.x = tileWidth/2-1+ox;		p.y = tileHeight*2+oy+tileHeight3_2*mult;
		q.x = p.x + isoWidth/2;		q.y = p.y - isoWidth*3/ 4;
		r.x = p.x - isoWidth/2;		r.y = p.y - isoWidth / 4;

		normal.Set( -1, 0, 1 );
	}

	normal.Normalize();

	//Texture tex[ MAX_TEXTURE_COUNT ];
	std::vector< Texture > tex( pSet->floorTexture.size() );
	if ( flags & USE_TEXTURE )
	{
		for( unsigned i=0; i<pSet->floorTexture.size(); ++i ) {
			tex[i].InitTexture( pSet->floorTexture[i].surface, i>0, p, q, r, 
								pSet->floorTexture[i].alpha,
								pSet->floorTexture[i].emit );
		}
	}
	Render( surface, &left, &right, normal, 
			&tex[0], (flags&OVERLAYS) ? pSet->floorTexture.size() : 1);
}

enum {
	FLOOR_FLAT,
	CUBE_FLAT,
	FLOOR_BASIC,
	CUBE_BASIC,

	JOIN_NWSE,
	WALL_NS,
	WALL_WE,

	JOIN_NW,
	JOIN_NE,
	JOIN_WS,
	JOIN_SE,

	JOIN_NWS,
	JOIN_NSE,
	JOIN_NWE,
	JOIN_WSE,

	RAMP_E,
	RAMP_N,

	JOIN_NWSE_DOUBLE,
	WALL_NS_DOUBLE,
	WALL_WE_DOUBLE,
	JOIN_NW_DOUBLE,
	JOIN_NE_DOUBLE,
	JOIN_WS_DOUBLE,
	JOIN_SE_DOUBLE,
	JOIN_NWS_DOUBLE,
	JOIN_NSE_DOUBLE,
	JOIN_NWE_DOUBLE,
	JOIN_WSE_DOUBLE,
	RAMP_E_DOUBLE,
	RAMP_N_DOUBLE,

	ICON_TYPE_COUNT
};


const char* const gIconName[ICON_TYPE_COUNT] = 
{
	"BASE_FLOOR",
	"BASE_CUBE",
	"FLOOR",
	"CUBE",
	
	"JOIN.NWSE",
	"WALL.NS",
	"WALL.WE",

	"JOIN.NW",
	"JOIN.NE",
	"JOIN.WS",
	"JOIN.SE",

	"JOIN.NWS",
	"JOIN.NSE",
	"JOIN.NWE",
	"JOIN.WSE",

	"RAMP.E",
	"RAMP.N",

	"JOIN.NWSE.2",
	"WALL.NS.2",
	"WALL.WE.2",

	"JOIN.NW.2",
	"JOIN.NE.2",
	"JOIN.WS.2",
	"JOIN.SE.2",

	"JOIN.NWS.2",
	"JOIN.NSE.2",
	"JOIN.NWE.2",
	"JOIN.WSE.2",

	"RAMP.E.2",
	"RAMP.N.2",
};



void WriteIconToXML( TiXmlNode* xmlNode, const char* name, int x, int y, int w, int h, int hotX, int hotY )
{
	string actionName = pSet->action + "." + name;

	TiXmlElement frame( "Frame" );
	frame.SetAttribute( "x", x );
	frame.SetAttribute( "y", y );
	frame.SetAttribute( "width", w );
	frame.SetAttribute( "height", h );
	frame.SetAttribute( "hotspotx", hotX );
	frame.SetAttribute( "hotspoty", hotY );

	TiXmlElement action( "Action" );
	action.InsertEndChild( frame );
	action.SetAttribute( "name", actionName.c_str() );

	xmlNode->InsertEndChild( action );
}


void DrawIcons( SDL_Surface* saveSurface, TiXmlNode* xmlNode )
{
	static bool flatWritten = false;

	bool done = false;

	for( int count=0; !done; ++count )
	{
		SDL_Surface* surface = 0;
		int hotX = 0;
		int hotY = 0;

		switch (count)
		{
			case FLOOR_FLAT:
			if ( ( pSet->drawAll || pSet->drawBasic ) && !flatWritten )
			{
				// Draw the top z plane. This
				// is for creating more tiles and checking color.
				surface = CreateSurface( 1, 1, 1 );
				RenderZ( surface, 0 );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case CUBE_FLAT:
			if ( ( pSet->drawAll || pSet->drawBasic ) && !flatWritten )
			{
				flatWritten = true;

				// Draw a white cube. This
				// is for creating more tiles and checking color.
				surface = CreateSurface( 1, 1, 1 );
				surface = CreateSurface( 1, 1, 1 );
				RenderZ( surface, 0 );
				RenderX( surface, 0 );
				RenderY( surface, 0 );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;
			
			case FLOOR_BASIC:
			if ( pSet->drawAll || pSet->drawBasic ) 
			{
				// Draw the top z plane textured.
				surface = CreateSurface( 1, 1, 1 );
				RenderZ( surface, USE_TEXTURE | OVERLAYS );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case CUBE_BASIC:
			if ( pSet->drawAll || pSet->drawBasic ) 
			{
				// Draw the basic cube
				surface = CreateSurface( 1, 1, 1 );
				RenderZ( surface, USE_TEXTURE | OVERLAYS );
				RenderX( surface, USE_TEXTURE | OVERLAYS);
				RenderY( surface, USE_TEXTURE | OVERLAYS);

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case JOIN_NWSE:
			if ( pSet->drawAll || pSet->drawJoin ) 
			{
				// Draw a NWSE wall (the first one because
				// it tends to break when something goes wrong).
				surface = CreateSurface( 1, 1, 1 );

				RenderX( surface, USE_TEXTURE | NORTH | OFFSET );
				RenderY( surface, USE_TEXTURE | EAST | OFFSET );

				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET );
				RenderX( surface, USE_TEXTURE | CENTER );

				RenderY( surface, USE_TEXTURE | WEST | OFFSET );
				RenderY( surface, USE_TEXTURE | CENTER );

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | SOUTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | EAST );
				RenderZ( surface, USE_TEXTURE | WEST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}	
			break;

			case WALL_NS:
			if ( pSet->drawAll || pSet->drawWall ) 
			{
				// Draw a NS wall
				surface = CreateSurface( 1, 1, 1 );
				RenderX( surface, USE_TEXTURE | OFFSET | OVERLAYS);

				RenderY( surface, USE_TEXTURE | CENTER );

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | SOUTH );
				RenderZ( surface, USE_TEXTURE | CENTER );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}	
			break;

			case WALL_WE:
			if ( pSet->drawAll || pSet->drawWall ) 
			{
				// Draw a WE wall
				surface = CreateSurface( 1, 1, 1 );
				RenderY( surface, USE_TEXTURE | OFFSET | OVERLAYS);

				RenderX( surface, USE_TEXTURE | CENTER );

				RenderZ( surface, USE_TEXTURE | EAST );
				RenderZ( surface, USE_TEXTURE | WEST );
				RenderZ( surface, USE_TEXTURE | CENTER );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}	
			break;

			case JOIN_NW:
			if ( pSet->drawAll || pSet->drawJoin )
			{
				surface = CreateSurface( 1, 1, 1 );

				RenderX( surface, USE_TEXTURE | NORTH | OFFSET );
				RenderX( surface, USE_TEXTURE | CENTER );

				RenderY( surface, USE_TEXTURE | WEST | OFFSET );
				RenderY( surface, USE_TEXTURE | CENTER | OFFSET);

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | WEST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case JOIN_NE:
			if ( pSet->drawAll || pSet->drawJoin )
			{
				surface = CreateSurface( 1, 1, 1 );

				RenderX( surface, USE_TEXTURE | NORTH | OFFSET );
				RenderX( surface, USE_TEXTURE | CENTER | OFFSET );

				RenderY( surface, USE_TEXTURE | EAST | OFFSET );
				RenderY( surface, USE_TEXTURE | CENTER | OFFSET);

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | EAST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case JOIN_WS:
			if ( pSet->drawAll || pSet->drawJoin )
			{
				surface = CreateSurface( 1, 1, 1 );

				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET );
				RenderX( surface, USE_TEXTURE | CENTER );

				RenderY( surface, USE_TEXTURE | WEST | OFFSET );
				RenderY( surface, USE_TEXTURE | CENTER);

				RenderZ( surface, USE_TEXTURE | WEST );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | SOUTH );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case JOIN_SE:
			if ( pSet->drawAll || pSet->drawJoin )
			{
				surface = CreateSurface( 1, 1, 1 );

				RenderY( surface, USE_TEXTURE | EAST | OFFSET );
				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET );
				RenderX( surface, USE_TEXTURE | CENTER | OFFSET );
				RenderY( surface, USE_TEXTURE | CENTER );

				RenderZ( surface, USE_TEXTURE | EAST );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | SOUTH );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;
			
			case JOIN_NWS:
			if ( pSet->drawAll || pSet->drawJoin ) 
			{
				// NWS
				surface = CreateSurface( 1, 1, 1 );

				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET );
				RenderX( surface, USE_TEXTURE | NORTH | OFFSET );
				RenderX( surface, USE_TEXTURE | CENTER );

				RenderY( surface, USE_TEXTURE | WEST | OFFSET );
				RenderY( surface, USE_TEXTURE | CENTER );

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | SOUTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | WEST );				

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case JOIN_NSE:
			if ( pSet->drawAll || pSet->drawJoin || pSet->drawWallJoin ) 
			{
				// NSE
				surface = CreateSurface( 1, 1, 1 );

				RenderY( surface, USE_TEXTURE | EAST | OFFSET );
				RenderY( surface, USE_TEXTURE | CENTER );

				RenderX( surface, USE_TEXTURE | OFFSET | OVERLAYS );

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | SOUTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | EAST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case JOIN_NWE:
			if ( pSet->drawAll || pSet->drawJoin || pSet->drawWallJoin ) 
			{
				// NWE
				surface = CreateSurface( 1, 1, 1 );

				RenderX( surface, USE_TEXTURE | NORTH | OFFSET );
				RenderX( surface, USE_TEXTURE | CENTER );

				RenderY( surface, USE_TEXTURE | OFFSET | OVERLAYS );

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | EAST );
				RenderZ( surface, USE_TEXTURE | WEST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case JOIN_WSE:
			if ( pSet->drawAll || pSet->drawJoin ) 
			{
				// WSE

				surface = CreateSurface( 1, 1, 1 );

				RenderY( surface, USE_TEXTURE | EAST | OFFSET );
				RenderY( surface, USE_TEXTURE | WEST | OFFSET );
				RenderY( surface, USE_TEXTURE | CENTER );
				
				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET );
				RenderX( surface, USE_TEXTURE | CENTER );

				RenderZ( surface, USE_TEXTURE | SOUTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | EAST );
				RenderZ( surface, USE_TEXTURE | WEST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case RAMP_E:
			if ( pSet->drawAll || pSet->drawRamp )
			{
				surface = CreateSurface( 1, 1, 1 );

				RenderRamp( surface, USE_TEXTURE | EAST | OVERLAYS, 1 );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;

			case RAMP_N:
			if ( pSet->drawAll || pSet->drawRamp )
			{
				surface = CreateSurface( 1, 1, 1 );

				RenderRamp( surface, USE_TEXTURE | NORTH | OVERLAYS, 1 );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 2 - 1;
			}
			break;
			
			case JOIN_NWSE_DOUBLE:
			if ( (pSet->drawAll || pSet->drawJoin ) && pSet->drawDouble )
			{
				// Draw a NWSE wall (the first one because
				// it tends to break when something goes wrong).
				surface = CreateSurface( 1, 1, 2 );

				RenderX( surface, USE_TEXTURE | NORTH | OFFSET, 0, 0 );
				RenderX( surface, USE_TEXTURE | NORTH | OFFSET, 0, 1 );
				RenderY( surface, USE_TEXTURE | EAST | OFFSET, 0, 0 );
				RenderY( surface, USE_TEXTURE | EAST | OFFSET, 0, 1 );

				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET, 0, 0 );
				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET, 0, 1 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 1 );

				RenderY( surface, USE_TEXTURE | WEST | OFFSET, 0, 0 );
				RenderY( surface, USE_TEXTURE | WEST | OFFSET, 0, 1 );
				RenderY( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderY( surface, USE_TEXTURE | CENTER, 0, 1 );

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | SOUTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | EAST );
				RenderZ( surface, USE_TEXTURE | WEST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}	
			break;

			case WALL_NS_DOUBLE:
			if ( (pSet->drawAll || pSet->drawWall) && pSet->drawDouble )
			{
				// Draw a NS wall
				surface = CreateSurface( 1, 1, 2 );
				RenderX( surface, USE_TEXTURE | OFFSET | OVERLAYS, 0, 0 );
				RenderX( surface, USE_TEXTURE | OFFSET | OVERLAYS, 0, 1 );

				RenderY( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderY( surface, USE_TEXTURE | CENTER, 0, 1 );

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | SOUTH );
				RenderZ( surface, USE_TEXTURE | CENTER );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}	
			break;

			case WALL_WE_DOUBLE:
			if ( (pSet->drawAll || pSet->drawWall )&& pSet->drawDouble )
			{
				// Draw a WE wall
				surface = CreateSurface( 1, 1, 2 );
				RenderY( surface, USE_TEXTURE | OFFSET | OVERLAYS, 0, 0 );
				RenderY( surface, USE_TEXTURE | OFFSET | OVERLAYS, 0, 1 );

				RenderX( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 1 );

				RenderZ( surface, USE_TEXTURE | EAST );
				RenderZ( surface, USE_TEXTURE | WEST );
				RenderZ( surface, USE_TEXTURE | CENTER );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}	
			break;

			case JOIN_NW_DOUBLE:
			if ( (pSet->drawAll || pSet->drawJoin ) && pSet->drawDouble )
			{
				surface = CreateSurface( 1, 1, 2 );

				RenderX( surface, USE_TEXTURE | NORTH | OFFSET, 0, 0 );
				RenderX( surface, USE_TEXTURE | NORTH | OFFSET, 0, 1 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 1 );

				RenderY( surface, USE_TEXTURE | WEST | OFFSET, 0, 0 );
				RenderY( surface, USE_TEXTURE | WEST | OFFSET, 0, 1 );
				RenderY( surface, USE_TEXTURE | CENTER | OFFSET, 0, 0);
				RenderY( surface, USE_TEXTURE | CENTER | OFFSET, 0, 1);

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | WEST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}
			break;

			case JOIN_NE_DOUBLE:
			if ( (pSet->drawAll || pSet->drawJoin ) && pSet->drawDouble )
			{
				surface = CreateSurface( 1, 1, 2 );

				RenderX( surface, USE_TEXTURE | NORTH | OFFSET, 0, 0 );
				RenderX( surface, USE_TEXTURE | NORTH | OFFSET, 0, 1 );
				RenderX( surface, USE_TEXTURE | CENTER | OFFSET, 0, 0 );
				RenderX( surface, USE_TEXTURE | CENTER | OFFSET, 0, 1 );

				RenderY( surface, USE_TEXTURE | EAST | OFFSET, 0, 0 );
				RenderY( surface, USE_TEXTURE | EAST | OFFSET, 0, 1 );
				RenderY( surface, USE_TEXTURE | CENTER | OFFSET, 0, 0);
				RenderY( surface, USE_TEXTURE | CENTER | OFFSET, 0, 1);

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | EAST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}
			break;

			case JOIN_WS_DOUBLE:
			if ( (pSet->drawAll || pSet->drawJoin ) && pSet->drawDouble )
			{
				surface = CreateSurface( 1, 1, 2 );

				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET , 0, 0);
				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET , 0, 1);
				RenderX( surface, USE_TEXTURE | CENTER , 0, 0);
				RenderX( surface, USE_TEXTURE | CENTER , 0, 1);

				RenderY( surface, USE_TEXTURE | WEST | OFFSET , 0, 0);
				RenderY( surface, USE_TEXTURE | WEST | OFFSET , 0, 1);
				RenderY( surface, USE_TEXTURE | CENTER, 0, 0);
				RenderY( surface, USE_TEXTURE | CENTER, 0, 1);

				RenderZ( surface, USE_TEXTURE | WEST );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | SOUTH );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}
			break;

			case JOIN_SE_DOUBLE:
			if ( (pSet->drawAll || pSet->drawJoin ) && pSet->drawDouble )
			{
				surface = CreateSurface( 1, 1, 2 );

				RenderY( surface, USE_TEXTURE | EAST | OFFSET , 0, 0);
				RenderY( surface, USE_TEXTURE | EAST | OFFSET , 0, 1);
				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET , 0, 0);
				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET , 0, 1);
				RenderX( surface, USE_TEXTURE | CENTER | OFFSET , 0, 0);
				RenderX( surface, USE_TEXTURE | CENTER | OFFSET , 0, 1);
				RenderY( surface, USE_TEXTURE | CENTER , 0, 0);
				RenderY( surface, USE_TEXTURE | CENTER , 0, 1);

				RenderZ( surface, USE_TEXTURE | EAST );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | SOUTH );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}
			break;

			case JOIN_NWS_DOUBLE:
			if ( (pSet->drawAll || pSet->drawJoin ) && pSet->drawDouble )
			{
				// NWS
				surface = CreateSurface( 1, 1, 2 );

				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET, 0, 0 );
				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET, 0, 1 );
				RenderX( surface, USE_TEXTURE | NORTH | OFFSET, 0, 0 );
				RenderX( surface, USE_TEXTURE | NORTH | OFFSET, 0, 1 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 1 );

				RenderY( surface, USE_TEXTURE | WEST | OFFSET, 0, 0 );
				RenderY( surface, USE_TEXTURE | WEST | OFFSET, 0, 1 );
				RenderY( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderY( surface, USE_TEXTURE | CENTER, 0, 1 );

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | SOUTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | WEST );				

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}
			break;

			case JOIN_NSE_DOUBLE:
			if ( (pSet->drawAll || pSet->drawJoin ) && pSet->drawDouble )
			{
				// NSE
				surface = CreateSurface( 1, 1, 2 );

				RenderY( surface, USE_TEXTURE | EAST | OFFSET, 0, 0 );
				RenderY( surface, USE_TEXTURE | EAST | OFFSET, 0, 1 );
				RenderY( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderY( surface, USE_TEXTURE | CENTER, 0, 1 );

				RenderX( surface, USE_TEXTURE | OFFSET | OVERLAYS, 0, 0 );
				RenderX( surface, USE_TEXTURE | OFFSET | OVERLAYS, 0, 1 );

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | SOUTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | EAST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}
			break;

			case JOIN_NWE_DOUBLE:
			if ( (pSet->drawAll || pSet->drawJoin ) && pSet->drawDouble )
			{
				// NWE
				surface = CreateSurface( 1, 1, 2 );

				RenderX( surface, USE_TEXTURE | NORTH | OFFSET, 0, 0 );
				RenderX( surface, USE_TEXTURE | NORTH | OFFSET, 0, 1 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 1 );

				RenderY( surface, USE_TEXTURE | OFFSET | OVERLAYS, 0, 0 );
				RenderY( surface, USE_TEXTURE | OFFSET | OVERLAYS, 0, 1 );

				RenderZ( surface, USE_TEXTURE | NORTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | EAST );
				RenderZ( surface, USE_TEXTURE | WEST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}
			break;

			case JOIN_WSE_DOUBLE:
			if ( (pSet->drawAll || pSet->drawJoin ) && pSet->drawDouble )
			{
				// WSE

				surface = CreateSurface( 1, 1, 2 );

				RenderY( surface, USE_TEXTURE | EAST | OFFSET, 0, 0 );
				RenderY( surface, USE_TEXTURE | EAST | OFFSET, 0, 1 );
				RenderY( surface, USE_TEXTURE | WEST | OFFSET, 0, 0 );
				RenderY( surface, USE_TEXTURE | WEST | OFFSET, 0, 1 );
				RenderY( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderY( surface, USE_TEXTURE | CENTER, 0, 1 );
				
				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET, 0, 0 );
				RenderX( surface, USE_TEXTURE | SOUTH | OFFSET, 0, 1 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 0 );
				RenderX( surface, USE_TEXTURE | CENTER, 0, 1 );

				RenderZ( surface, USE_TEXTURE | SOUTH );
				RenderZ( surface, USE_TEXTURE | CENTER );
				RenderZ( surface, USE_TEXTURE | EAST );
				RenderZ( surface, USE_TEXTURE | WEST );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight * 3 - 1;
			}
			break;

			case RAMP_E_DOUBLE:
			if ( pSet->drawDouble && (pSet->drawRamp || pSet->drawAll ))
			{
				surface = CreateSurface( 2, 1, 3 );

				RenderRamp( surface, USE_TEXTURE | EAST | OVERLAYS, 2 );
				RenderRamp( surface, USE_TEXTURE | EAST | OVERLAYS, 2 );

				hotX = isoWidth / 2 - 1;
				hotY = tileHeight*3 + tileHeight/2 - 1;
			}
			break;

			case RAMP_N_DOUBLE:
			if ( pSet->drawDouble && (pSet->drawRamp || pSet->drawAll ))
			{
				surface = CreateSurface( 1, 2, 3 );

				RenderRamp( surface, USE_TEXTURE | NORTH | OVERLAYS, 2 );
				RenderRamp( surface, USE_TEXTURE | NORTH | OVERLAYS, 2 );

				hotX = isoWidth - 1;
				hotY = tileHeight*3 + tileHeight/2 - 1;
			}
			break;
	
			default:
				done = true;
		}

		// Save off the surface we just used.
		if ( surface )
		{
			int border = 0;
			if ( saveX + surface->w >= saveSurface->w ) {
				saveX = 0;
				saveY += saveMax;
				saveMax = 0;
			}
			saveMax = Max( saveMax, surface->h );

			SDL_Rect src = { 0, 0, surface->w, surface->h };
			SDL_Rect dst = { saveX, saveY, surface->w, surface->h };
			saveX += surface->w + border;

			SDL_BlitSurface( surface, &src, saveSurface, &dst );
			SDL_FreeSurface( surface );

			WriteIconToXML( xmlNode, gIconName[count], dst.x, dst.y, dst.w, dst.h, dst.x+hotX, dst.y+hotY );
		}
	}
}


TiXmlNode* WriteXMLHeader( TiXmlDocument* doc, const char* imageName, const char* spriteName )
{
	TiXmlElement definition( "Definition" );
	definition.SetAttribute( "filename", imageName );
	definition.SetAttribute( "Transparent0", "#000000" );
	
	TiXmlElement sprite( "Sprite" );
	sprite.SetAttribute( "name", spriteName );

	definition.InsertEndChild( sprite );
	doc->InsertEndChild( definition );

	return doc->FirstChildElement()->FirstChildElement();
}


void ParseTF( TiXmlElement* ele, const char* value, bool *result ) 
{
	if ( ele->Attribute( value ) )
	{
		if ( strcmp( ele->Attribute( value ), "true" ) == 0 ) {
			*result = true;
		}
		else if ( strcmp( ele->Attribute( value ), "false" ) == 0 ) {
			*result = false;
		}
	}
}


void ReadOverlayAttributes( TiXmlElement* ele, TexData* tex )
{
	if ( ele->Attribute( "alpha" ) ) {
		double alpha = 1.0f;
		ele->Attribute( "alpha", &alpha );
		tex->alpha = (U8) Clamp( LRint( alpha*255.0 ), (long)0, (long)255 );
	}
	ParseTF( ele, "emit", &tex->emit );
}


bool ParseXML( TiXmlDocument* doc )
{
	static int nameCount = 0;

	TiXmlHandle docH( doc );

	TiXmlElement* isogen = docH.FirstChildElement( "Isogen" ).Element();
	if ( isogen ) {
		bool wallInsetSet = false;

		if ( isogen->Attribute( "sprite" )) {
			spriteName = isogen->Attribute( "sprite" );
		}
		else {
			printf( "Required 'sprite' attribute in <Isogen> not specified.\n" );
			return false;
		}
		if ( isogen->Attribute( "isoWidth" )) {
			isogen->Attribute( "isoWidth", &isoWidth );
			isoWidth = ((isoWidth+3)/4)*4;
			tileWidth = isoWidth - 2;
			tileHeight = isoWidth / 2;
		}
		if ( isogen->Attribute( "wallInset" )) {
			isogen->Attribute( "wallInset", &step );
			wallInsetSet = true;
		}
		if ( isogen->Attribute( "width" )) {
			isogen->Attribute( "width", &outWidth );
		}
		if ( isogen->Attribute( "height" )) {
			isogen->Attribute( "height", &outHeight );
		}
		if ( isogen->Attribute( "ambient" )) {
			sscanf( isogen->Attribute( "ambient" ), "%f %f %f", &ambient.r, &ambient.g, &ambient.b );
		}
		if ( isogen->Attribute( "diffuse" )) {
			sscanf( isogen->Attribute( "diffuse" ), "%f %f %f", &diffuse.r, &diffuse.g, &diffuse.b );
		}
		if ( isogen->Attribute( "lightVector" )) {
			sscanf( isogen->Attribute( "lightVector" ), "%f %f %f", &lightVector.x, &lightVector.y, &lightVector.z );
		}

		if ( !wallInsetSet )
			step = isoWidth / 12;

		lightVector.Normalize();
		lightVector.x = -lightVector.x;
		lightVector.y = -lightVector.y;
		lightVector.z = -lightVector.z;

		TiXmlElement* setEle = 0;
		for(	setEle = isogen->FirstChildElement( "Set" ); 
				setEle; 
				setEle = setEle->NextSiblingElement( "Set" ) )
		{
			setData.resize( setData.size() + 1 );
			pSet = &setData[ setData.size()-1 ];

			pSet->Init();

			ParseTF( setEle, "useAA", &pSet->useAA );
			ParseTF( setEle, "drawAll", &pSet->drawAll );
			ParseTF( setEle, "drawBasic", &pSet->drawBasic );
			ParseTF( setEle, "drawWall", &pSet->drawWall );
			ParseTF( setEle, "drawJoin", &pSet->drawJoin );
			ParseTF( setEle, "drawWallJoin", &pSet->drawWallJoin );
			ParseTF( setEle, "drawRamp", &pSet->drawRamp );
			ParseTF( setEle, "drawDouble", &pSet->drawDouble );

			pSet->floorTexture.resize(1);
			pSet->floorTexture[0].Clear();
			pSet->floorTexture[0].surface = whiteSurface;
			
			pSet->wallTexture.resize(1);
			pSet->wallTexture[0].Clear();
			pSet->wallTexture[0].surface = whiteSurface;

			if ( setEle->Attribute( "floor" ) ) {
				GLASSERT( pSet->floorTexture.size() == 1 );
				pSet->floorTexture[0].surface = ImageLoader( setEle->Attribute( "floor" ) );
				if ( !pSet->floorTexture[0].surface ) {
					printf( "Failed to load texture '%s'\n", setEle->Attribute( "floor" ) );
					return false;
				}
			}
			if ( setEle->Attribute( "wall" ) ) {
				GLASSERT( pSet->wallTexture.size() == 1 );
				pSet->wallTexture[0].surface = ImageLoader( setEle->Attribute( "wall" ) );
				if ( !pSet->wallTexture[0].surface ) {
					printf( "Failed to load texture '%s'\n", setEle->Attribute( "wall" ) );
					return false;
				}
			}

			if ( setEle->Attribute( "action" )) {
				pSet->action = setEle->Attribute( "action" );
			}
			else {
				char buf[256];
				sprintf( buf, "ICON%d", nameCount++ );
				pSet->action = buf;
			}

			TexData dummy;
			dummy.Clear();

			TiXmlElement* overEle = 0;
			for(	overEle = setEle->FirstChildElement( "FloorOverlay" );
					overEle;
					overEle = overEle->NextSiblingElement( "FloorOverlay" ) )
			{
				if ( overEle->Attribute( "name" ) ) {
					pSet->floorTexture.push_back( dummy );
					TexData* tex = &pSet->floorTexture.back();

					tex->surface = ImageLoader( overEle->Attribute( "name" ) );
					if ( !tex->surface ) {
						printf( "Failed to load texture '%s'\n", overEle->Attribute( "name" ) );
						return false;
					}
					ReadOverlayAttributes( overEle, tex );
				}
			}
			for(	overEle = setEle->FirstChildElement( "WallOverlay" );
					overEle;
					overEle = overEle->NextSiblingElement( "WallOverlay" ) )
			{
				if ( overEle->Attribute( "name" ) ) {
					pSet->wallTexture.push_back( dummy );
					TexData* tex = &pSet->wallTexture.back();
					
					tex->surface = ImageLoader( overEle->Attribute( "name" ) );
					if ( !tex->surface ) {
						printf( "Failed to load texture '%s'\n", overEle->Attribute( "name" ) );
						return false;
					}
					ReadOverlayAttributes( overEle, tex );
				}
			}
			//printf( "Set has %d wall and %d floor\n", pSet->wallTexture.size(), pSet->floorTexture.size() );
		}
		pSet = 0;
		return true;
	}	
	return false;
}


int main( int argc, char* argv[] ) 
{
	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE) < 0 ) {
		printf( "Couldn't initialize SDL: %s\n", SDL_GetError() );
		return 255;
	}

	void* handle = GL_LoadLibrary( "SDL_image" );
	if ( !handle )
	{	
		exit( 1 );
	}
	ImageLoader = (ImageLoaderFunc) GL_LoadFunction( handle, "IMG_Load" );
	GLASSERT( ImageLoader );	

	if ( argc < 2 ) {
		printf( "isogen minimal usage:\n"
				"  isogen sprite='spriteName'\n"
				"Where the 'spriteName' is the name of the output XML and BMP file.\n"
				"To provide floor and wall textures:\n"
				"  isogen sprite='spriteName' floor='floor.bmp' wall='wall.bmp'\n"
				"To use an isogen XML file:\n"
				"  isogen data.xml\n"
				"isogen has many more options - please see the docs.\n" );
		return 1;
	}

	TiXmlDocument doc;

	if ( argc == 2 && strstr( argv[1], ".xml" ) ) {
		doc.LoadFile( argv[1] );
	}
	else {
		string input = "<Isogen ";
		int i;
		for( i=1; i<argc; ++i ) {
			input += argv[i];
			input += " ";
		}
		input += "><Set ";
		for( i=1; i<argc; ++i ) {
			input += argv[i];
			input += " ";
		}
		input += " /></Isogen>";

		doc.Parse( input.c_str() );
	}

	if ( doc.Error() ) {
		printf( "Error parsing input arguments.\n" );
		printf( "%s\n", doc.ErrorDesc() );
		return 1;
	}

	if ( ParseXML( &doc ) == false ) {
		printf( "Error parsing command line or XML.\n" );
		return 1;
	}

	// Create the surface to save to:
	SDL_Surface* surface = SDL_CreateRGBSurface(	SDL_SWSURFACE, 
													outWidth, outHeight,
													32, 
													0xff, 0xff<<8, 0xff<<16, 0xff<<24 );
	Uint32 color = SDL_MapRGBA( surface->format, 0, 0, 0, 255 );
	SDL_FillRect( surface, 0, color );

	whiteSurface = SDL_CreateRGBSurface(	SDL_SWSURFACE, 
											4, 4,
											32, 
											0xff, 0xff<<8, 0xff<<16, 0xff<<24 );
	memset( whiteSurface->pixels, 255, whiteSurface->pitch * whiteSurface->h );

	string saveXMLName = spriteName + "_encoder.xml";
	string saveImageName = spriteName + ".bmp";

	TiXmlDocument outputDoc( saveXMLName.c_str() );
	TiXmlNode* node = WriteXMLHeader( &outputDoc, saveImageName.c_str(), spriteName.c_str() );

	for( unsigned i=0; i<setData.size(); ++i )
	{
		pSet = &setData[i];
		DrawIcons( surface, node );
		pSet->Clear();
	}

	printf( "Writing image file '%s'.\n", saveImageName.c_str() );
	SDL_SaveBMP( surface, saveImageName.c_str() );
	printf( "Writing kyra encoder file '%s'.\n", outputDoc.Value() );
	outputDoc.SaveFile();

	SDL_FreeSurface( surface );
	surface = 0;
	SDL_FreeSurface( whiteSurface );
	whiteSurface = 0;

	SDL_Quit();
	return 0;
}
