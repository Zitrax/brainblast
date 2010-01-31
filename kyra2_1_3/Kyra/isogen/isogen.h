#ifndef KYRA_ISOGEN_INCLUDED
#define KYRA_ISOGEN_INCLUDED

#include "SDL.h"
#include <list>
#include <string>
#include <vector>

#include "../../grinliz/gldebug.h"
#include "../../grinliz/gltypes.h"
#include "../../grinliz/glutil.h"
#include "../../grinliz/glvector.h"
#include "../../grinliz/glgeometry.h"
#include "../../grinliz/glcolor.h"

#include "../../tinyxml/tinyxml.h"

enum {
	USE_TEXTURE =	0x01,
	NORTH		=	0x02,
	SOUTH		=   0x04,
	EAST		=	0x08,
	WEST		=	0x10,
	CENTER		=   0x20,
	OFFSET		=   0x40,
	OVERLAYS    =	0x80
};

// Data structure that contains all the information to render a texture
class Texture
{
  private:
	SDL_Surface* surface;			// a pointer to the surface that contains the texture
									// data. Not owned by this class.
	grinliz::Vector2I origin;		// origin of the texture, in screen coordinates.
	
	// Mapping constants.
	// u = x*a + y*b
	// v = x*c + y*d
	float a;
	float b;
	float c;
	float d;

	grinliz::Color4U8 GetPixel( int u, int v );
	inline int WrapX( SDL_Surface* surface, int x );
	inline int WrapY( SDL_Surface* surface, int y );

	U8 alpha;
	bool emit;
	bool colorKey;

  public:
	Texture()	{	surface = 0; 
					origin.Set( 0, 0 ); 
					alpha = 255;
					emit = false;
					colorKey = false;
				}

	void InitTexture(	SDL_Surface* surface, 
						bool overlay,
						const grinliz::Vector2I& p, 
						const grinliz::Vector2I& _q,
						const grinliz::Vector2I& _r,
						U8 alpha = 255,
						bool emit = false );

	grinliz::Color4U8 Lookup( int x, int y );

	bool Valid()	{ return surface != 0; }
	bool Emit()		{ return emit; }

	U8 Alpha()		{ return alpha; }
};

// Information to initialize a texture (read from the XML file)
struct TexData
{
	void Clear() { surface = 0; alpha = 255; emit=false; }

	SDL_Surface* surface;
	U8 alpha;
	bool emit;
};

// Information to draw a set of icons
struct SetData
{
	bool useAA;

	bool drawAll;
	bool drawDouble;

	bool drawBasic;
	bool drawWall;
	bool drawJoin;
	bool drawWallJoin;	// the 2 joins that have a wall to overlay to
	bool drawRamp;

	std::vector< TexData > wallTexture;
	std::vector< TexData > floorTexture;
	std::string action;

	void Init() {
		useAA = true;
		drawAll = true;
		drawBasic = false;
		drawWall = false;
		drawJoin = false;
		drawWallJoin = false;
		drawRamp = false;
		drawDouble = false;
	}
	void Clear();
};



#endif
