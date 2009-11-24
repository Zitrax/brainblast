#ifndef KR_OPENGL_TEXTURE_INCLUDED
#define KR_OPENGL_TEXTURE_INCLUDED

#ifdef KYRA_SUPPORT_OPENGL
	#include "SDL_opengl.h"
	#define KYRA_GLuint GLuint
#else
	#define KYRA_GLuint int
#endif
#include "krmath.h"
#include "color.h"
#include "../../grinliz/glgeometry.h"

/*	A wrapper to the actual OpenGL texture.
*/
class KrOglTexture
{
	friend class KrTexture;

  public:
	KrOglTexture(	const KrRGBA*, int width, int height,
					grinliz::Rectangle2F* textCoords,
					grinliz::Rectangle2I* pixelCoords );

	~KrOglTexture();

	void AddRef()		{ ++refCount; }
	void RemoveRef()	{	--refCount;
							if ( refCount == 0 )
								delete this;
						}

	KYRA_GLuint	Id()					{ return textureId; }
	bool	Scale()					{ return scaled; }


  private:
	void SetTexture(	const KrRGBA* pixels,
						const grinliz::Rectangle2I& pixelCoords );

	void SetScaledTexture(	const KrRGBA* pixels, int w, int h );


	void	FillRow( U8* target, const KrRGBA* source, int width );
	void	CreateTexture(	U8* target, int targetW, int targetH, int targetScan,
							const KrRGBA* source, int sourceW, int sourceH );

	void	CreateScaledTexture(	U8* target, int targetW, int targetH, int targetScan,
									const KrRGBA* source, int sourceW, int sourceH );

	int		refCount;
	KYRA_GLuint  textureId;
	bool	canSetTexture;
	bool	scaled;
};


/*	A texture is presented to the Kyra engine.
	It has (opelGL) bounds, an id number, and may or may not be scaled.
	Scaled textures occur when a texture that is requested is too large,
	either because a given OpenGL system doesn't support it,
	or because KrEngine::MaxOglTextureSize() has restricted memory usage.
*/
class KrTexture
{
  public:
	KrTexture(	KrOglTexture* oglTexture,
				const grinliz::Rectangle2F& bounds,
				const grinliz::Rectangle2I& pixelBounds );

	~KrTexture();

	const grinliz::Rectangle2F& Bounds()	{ return bounds; }
	KYRA_GLuint Id()						{ return oglTexture->Id(); }
	bool   Scale()					{ return oglTexture->Scale(); }

	void SetTexture( const KrRGBA* image, int width, int height );

  private:
	KrOglTexture* oglTexture;
	grinliz::Rectangle2F	bounds;
	grinliz::Rectangle2I	pixelBounds;
};


/*	Singleton class to manage textures.
	Conceptually, the TextureManager returns a pointer to newly created
	Texture. The Texture is a wrapper to an OglTexture, which describes
	the actually memory held by openGL. Multiple Textures could wrap the
	same OglTexture.

	At this point, there is a one to one relationship from the Texture
	to the OglTexture, so some functionality is unused.
*/
class KrTextureManager
{
  public:
	~KrTextureManager();
	static KrTextureManager* Instance();

	// Call this to allocate and return a new texture, set up with 'data'
	KrTexture* CreateTexture( const KrRGBA* data, int width, int height );

	static int TextureIndex()		{ return ( instance ) ? instance->oglTextureCount : 0; }

	// When the ogl texture deletes, it warns the manager. Called by destructor.
	void OglTextureDeleting( KrOglTexture* goingAway );

  private:
	KrTextureManager() : oglTextureCount( 0 )		{}

	static KrTextureManager* instance;
	int oglTextureCount;

//	GlDynArray< KrOglTexture* > texArray;
};

#endif
