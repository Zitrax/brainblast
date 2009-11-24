#include "SDL.h"
#include <stdlib.h>
#include "../engine/kyra.h"

#include "tutorial1.h"

using namespace grinliz;

#define SDL_TIMER_EVENT ( SDL_USEREVENT + 0 )
const int TIMER_INTERVAL = 80;

const int SCREENX = 640;
const int SCREENY = 480;

// A structure to store the spell effect.
struct SpellEffect
{
	KrSprite* particle;
	int xSpeed;
	int ySpeed;
};

Uint32 TimerCallback(Uint32 interval)
{
	SDL_Event event;
	event.type = SDL_TIMER_EVENT;

	SDL_PeepEvents( &event, 1, SDL_ADDEVENT, 0 );
	return TIMER_INTERVAL;
}


int main( int argc, char *argv[] )
{
	// code_d A linked list for our spell effects.
	GlCircleList< SpellEffect > spells;
	// code_d A random number generator.
	Random random;

	SDL_Surface* screen;

	const SDL_version* sdlVersion = SDL_Linked_Version();
	if ( sdlVersion->minor < 2 )
	{
		printf( "SDL version must be at least 1.2.0" );
		exit( 254 );
	}

	/* Initialize the SDL library */
	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE) < 0 ) {
		printf( "Couldn't initialize SDL: %s\n",SDL_GetError());
		exit(255);
	}

	/* Create a display for the image */
	screen = SDL_SetVideoMode( SCREENX, SCREENY, 0, SDL_SWSURFACE );
	if ( screen == NULL ) {
		exit(3);
	}

	KrEngine* engine = new KrEngine( screen );
	engine->Draw();

	// Load the dat file.
	// The dat file was carefully created in the sprite
	// editor. Loading allows us access to the 
	// MAGE, PARTICLE, and CARPET.
	if ( !engine->Vault()->LoadDatFile( "tutorial1.dat" ) )
	{
		printf( "Error loading the tutorial dat file\n" );
		exit( 255 );
	}

	// Get the CARPET resource
	KrSpriteResource* carpetRes = engine->Vault()->GetSpriteResource( TUT1_CARPET );
	GLASSERT( carpetRes );

	// Create the carpet sprite and add it to the tree
	KrSprite* carpet = new KrSprite( carpetRes );
	carpet->SetPos( SCREENX, SCREENY / 2 );
	engine->Tree()->AddNode( 0, carpet );
	// Get the MAGE resource, create the mage.
	KrSpriteResource* mageRes = engine->Vault()->GetSpriteResource( TUT1_MAGE );
	GLASSERT( mageRes );
	KrSprite* mage = new KrSprite( mageRes );

	// Add the Mage as a child of the carpet.
	engine->Tree()->AddNode( carpet, mage );
	// code_d Get the Particle resource
	KrSpriteResource* particleRes = engine->Vault()->GetSpriteResource( TUT1_PARTICLE );
	GLASSERT( particleRes );
	SDL_Event event;
	bool done = false;

	// Start timing!
	SDL_SetTimer( TIMER_INTERVAL, TimerCallback );

	while( !done && SDL_WaitEvent(&event) )
	{
		if ( event.type == SDL_QUIT )
			break;

		switch(event.type)
		{
			case SDL_KEYDOWN:
			{
				done = true;
			}
			break;

			case SDL_TIMER_EVENT:
			{
				// code_d Walk the list and move the particles
				GlCircleListIterator< SpellEffect > it( spells );
				for( it.Begin(); !it.Done(); it.Next() )
				{
					KrSprite* particle = it.Current().particle;

					// Check for something off the stage
					if (	 particle->X() < 0
						  || particle->X() > SCREENX
						  || particle->Y() < 0 
						  || particle->Y() > SCREENY )
					{
						// code_d Remove the sprite from the Tree
						engine->Tree()->DeleteNode( particle );

						// Back up the iterator, remove the next.
						it.Remove();
						it.Prev();
						continue;
					}
						  
					// This particle stays on the stage. So
					// move it based on speed.
					particle->SetPos( particle->X() + it.Current().xSpeed,
									  particle->Y() + it.Current().ySpeed );

					// Fade out the alpha channel, so that the
					// particles dim.
					KrColorTransform color = particle->CTransform();

					const int FADE = 2;
					if ( color.Alpha() > FADE )
					{
						color.SetAlpha( color.Alpha() - FADE );
						particle->SetColor( color );
					}

				}

				// Perhaps create particles
				if ( random.Rand( 4 ) == 0 )
				{
					int x, y;
					SpellEffect spellEffect;

					// We're creating a particle: Right hand or left?				
					// The offsets are measured in pixels, and 
					// measured in the source tga file.
					if ( random.Rand( 2 ) )
					{
						x = carpet->X() - 32;	
						y = carpet->Y() - 28;
					}
					else
					{
						x = carpet->X() - 32;	
						y = carpet->Y() + 28;
					}

					// code_d Create the particle
					spellEffect.particle = new KrSprite( particleRes );
					spellEffect.particle->SetPos( x, y );

					spellEffect.xSpeed = -8 + random.Rand( 3 );
					spellEffect.ySpeed = -2 + random.Rand( 4 );

					engine->Tree()->AddNode( 0, spellEffect.particle );

					// code_d Transform the color
					KrColorTransform colorXForm;

					// This takes the 3 color components, and scales
					// them back by a random percentage. This essentially
					// turns the white ball into a random color in
					// RGB space.
					colorXForm.SetRed( random.Rand( 256 ), 0 );
					colorXForm.SetGreen( random.Rand( 256 ), 0 );
					colorXForm.SetBlue( random.Rand( 256 ), 0 );

					spellEffect.particle->SetColor( colorXForm );

					spells.PushBack( spellEffect );
				}
				// Move the carpet.
				carpet->DoStep();
				if ( carpet->X() < 0 )
				{
					carpet->SetPos( SCREENX, carpet->Y() );
				}
				engine->Draw();
			}
			break;

			default:
				break;
		}

	}

	delete engine;

	SDL_Quit();	
	return 0;
}
