/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006
 * @author Daniel Bengtsson
 */

#include "Brainblast.h"
#include "BrainSprite.h"

#include "../images/bb.h"

using namespace grinliz;
using namespace brain;

int main(int argc, char *argv[])
{
    const SDL_version* sdlVersion = SDL_Linked_Version();
	if ( sdlVersion->minor < 2 )
	{
		printf( "ERROR: SDL version must be at least 1.2.0" );
		exit( 254 );
	}

    Brainblast* bb = new Brainblast();
    bb->startGame();
    zap(bb);
}

Brainblast::Brainblast() : m_currentLvl(0),
                           m_screen( SDL_SetVideoMode( VIDEOX, VIDEOY, VIDEOBITS, SDL_HWSURFACE ) ),
                           m_field1(0),
                           m_field2(0),
                           m_bricks(0),
						   m_engine(0),
                           red    ( SDL_MapRGB(m_screen->format, 0xff, 0x00, 0x00) ),
                           blue   ( SDL_MapRGB(m_screen->format, 0x00, 0x00, 0xff) ),
                           black  ( SDL_MapRGB(m_screen->format, 0x00, 0x00, 0x00) ),
                           green  ( SDL_MapRGB(m_screen->format, 0x00, 0xff, 0x00) ),
                           white  ( SDL_MapRGB(m_screen->format, 0xff, 0xff, 0xff) ),
                           yellow ( SDL_MapRGB(m_screen->format, 0xff, 0xff, 0x00) ),
                           cyan   ( SDL_MapRGB(m_screen->format, 0x00, 0xff, 0xff) ),
                           magenta( SDL_MapRGB(m_screen->format, 0xff, 0x00, 0xff) )
{
    if(bbc::debug) std::cerr << "Brainblast::Brainblast() Videomode(" << VIDEOX << "," << VIDEOY << ")\n";

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
    {
        printf("ERROR: Can't init SDL:  %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit); 

    if(m_screen == NULL)
    {
        printf("ERROR: Can't set video mode: %s\n", SDL_GetError());
        exit(1);
    }   

    m_field1 = new SDL_Rect;
    m_field1->x = MARGIN; m_field1->y=MARGIN; 
    m_field1->w = VIDEOX/2-4*MARGIN; m_field1->h = VIDEOY-2*MARGIN;
  
    m_field2 = new SDL_Rect;
    m_field2->x = m_field1->w+3*MARGIN; m_field2->y=MARGIN; 
    m_field2->w = m_field1->w; m_field2->h = m_field1->h;
}

// Copy constructor
Brainblast::Brainblast(const Brainblast& bb) : m_currentLvl(0),
                                               m_screen(0),
                                               m_field1(0),
                                               m_field2(0),
                                               m_bricks(0),
											   m_engine(0),
                                               red(bb.red),
                                               blue(bb.blue),
                                               black(bb.black),
                                               green(bb.green),
                                               white(bb.white),
                                               yellow(bb.yellow),
                                               cyan(bb.cyan),
                                               magenta(bb.magenta)
{
  
    m_currentLvl = new Puzzle( *bb.m_currentLvl );
    assert( m_currentLvl );

    m_screen = new SDL_Surface( *bb.m_screen );
    assert( m_screen );

    m_field1 = new SDL_Rect( *bb.m_field1 );
    m_field2 = new SDL_Rect( *bb.m_field2 );

    for(int i=0; i<NOF_BRICK_TYPES; i++)
        m_bricks[i] = new Brick( *bb.m_bricks[i] );
}

// Assignment operator
Brainblast&
Brainblast::operator=(const Brainblast& bb)
{
    // Important to clean up the old memory before
    // assigning the new ones
    cleanup();

    m_screen     = new SDL_Surface( *bb.m_screen );
    m_currentLvl = new Puzzle( *bb.m_currentLvl );

    m_field1 = new SDL_Rect( *bb.m_field1 );
    m_field2 = new SDL_Rect( *bb.m_field2 );

    m_bricks = new Brick*[NOF_BRICK_TYPES];
    for(int i=0; i<NOF_BRICK_TYPES; i++)
        m_bricks[i] = new Brick( *bb.m_bricks[i] );

	// Do we really need to copy ?
	if( bb.m_engine )
		m_engine = new KrEngine( *bb.m_engine );

    // Correct ?
    return *this;
}

Brainblast::~Brainblast()
{
    if(bbc::debug) std::cerr << "Brainblast::~Brainblast()\n";
  
    cleanup();
}

void
Brainblast::cleanup()
{
    if( m_bricks )
        for(int i=0; i<NOF_BRICK_TYPES; i++)
            zap( m_bricks[i] );
    zapArr( m_bricks );
    
    // m_screen is deleted by SDL_Quit
    // zap( m_screen );

    zap( m_currentLvl );

    zap(m_field1); zap(m_field2);

	zap( m_engine );
}

bool
Brainblast::makeLevel(int lvl)
{
    if(bbc::debug) std::cerr << "Brainblast::makeLevel(" << lvl << ")\n";

    if(lvl == 0) 
    {
        m_currentLvl = new Puzzle();
        // placeBricksRandom();
#warning "Should implement placeBricksRandom"
        return true;
    }

    else 
    {

        char* filename  = static_cast<char*>(malloc(40));
      
        char* filebase = "../lvl/lvl%03d.txt";
      
        sprintf(filename, filebase, lvl);
        if( bbc::debug ) std::cerr << "Level file: " << filename << "\n";
      
        ifstream in(filename);
        if ( !in ) { 
            cerr << "Level file could not be opened...\n"; 
            free(filename);
            return false;
        }
    
        // Parse Level File
        int val; int i=0; int tmp; int width=0; int height=0;
        while( in >> val ) 
        { 
            if( i==0 ) { width = val; }
            else if ( i==1 ) 
            { 
                height = val; 
                m_currentLvl = new Puzzle( height, width );
            }
            else 
            {
                if(bbc::debug) std::cerr << val << " ";
                if( i%2 == 0 ) tmp = val;
                else  
                    m_currentLvl->setSolutionBrickWithIdx(m_bricks[val],tmp-1);
            }
            i++;
        }

        if(bbc::debug) std::cerr << "\n";
        free(filename);
        in.close();
    } 

    return true;
}

void
Brainblast::createBricks()
{
    if(bbc::debug) std::cerr << "Brainblast::createBricks()\n";

    char* filename  = static_cast<char*>(malloc(40));
  
    char* filebase = "../images/brick%03d.bmp";

    m_bricks = new Brick*[NOF_BRICK_TYPES];

    for(int i=0; i<NOF_BRICK_TYPES; i++) 
    {
        cout << "i = " << i << " ";
      
        // What is the equivalency for this in C++ ?
        sprintf(filename, filebase, i);
        cout << filename << "\n";
        m_bricks[i] = new Brick(filename, i);
    }

    free(filename);
}

void
Brainblast::drawBoard(SDL_Surface* s, SDL_Rect* dim, Puzzle* p, int xTiles, int yTiles)
{
    if(bbc::debug) std::cerr << "Brainblast::drawBoard()\n";

    assert(s); 
    assert(dim);
    assert(p);

    /* Lock the screen, if needed */
    if(SDL_MUSTLOCK(m_screen)) {
        if(SDL_LockSurface(m_screen) < 0) 
            return;
    }

    int xSpace = bbc::round(static_cast<double>(dim->w) / p->getWidth ());
    int ySpace = bbc::round(static_cast<double>(dim->h) / p->getHeight());

//   int xSpace = dim->w / p->getWidth();
//   int ySpace = dim->h / p->getHeight();

    for(int i=0; i<=yTiles; i++)     
        bbc::line(s, dim->x, dim->y+i*ySpace, dim->x+dim->w, dim->y+i*ySpace, magenta);

    for(int i=0; i<=xTiles; i++) 
        bbc::line(s, dim->x+i*xSpace, dim->y, dim->x+i*xSpace, dim->y+dim->h, yellow);

    /* Unlock the screen if needed */
    if(SDL_MUSTLOCK(m_screen)) {
        SDL_UnlockSurface(m_screen);
    }
  
    SDL_Rect destination;
    destination.x = 10;
    destination.y = 10;

    SDL_Flip(m_screen);
}

void
Brainblast::drawBrickAtIdx(SDL_Surface* s, Puzzle* p, SDL_Rect* dim, int idx, bool solution)
{
    if(bbc::debug) std::cerr << "Brainblast::drawBrickAtIdx(" << idx << ")\n";
    
    assert(s);
    assert(p);
    assert(dim);
    
    /* Lock the screen, if needed */
    if(SDL_MUSTLOCK(m_screen)) 
    {
        if(SDL_LockSurface(m_screen) < 0) 
            return;
    }
    
    Brick* b = solution ? 
        p->getSolutionBrickWithIdx(idx) :
        p->getCurrentBrickWithIdx(idx);

    // This is the space for each cell in the puzzle
    int xSpace = bbc::round(static_cast<double>(dim->w) / p->getWidth ());
    int ySpace = bbc::round(static_cast<double>(dim->h) / p->getHeight());
  
    // If there was a brick, draw it
    if(b) 
    {
        // Find the coordinate of the current brick
        // @todo Should probably be moved to the brick class
        uint x = bbc::round(dim->x + (idx%p->getWidth()  + 0.5)*xSpace - b->getWidth() /2.0);
        uint y = bbc::round(dim->y + (idx/p->getHeight() + 0.5)*ySpace - b->getHeight()/2.0);
        if(bbc::debug) std::cerr << "x = " << x << ", y = " << y << "\n";
        b->setPos(x, y);
        b->draw(m_screen);      
    }
    else
    {
        SDL_Rect destination;
        destination.x = dim->x + (idx%p->getWidth())*xSpace;
        destination.y = dim->y + (idx/p->getHeight())*ySpace;
        destination.h = ySpace;
        destination.w = xSpace;

        SDL_FillRect(m_screen, &destination, black);
    }

    /* Unlock the screen if needed */
    if(SDL_MUSTLOCK(m_screen)) 
    {
        SDL_UnlockSurface(m_screen);
    }
  
}

void
Brainblast::drawAllBricks(SDL_Surface* s, Puzzle* p, SDL_Rect* dim, bool solution)
{
    if(bbc::debug) std::cerr << "Brainblast::drawAllBricks()\n";
  
    assert(s); 
    assert(p);
    assert(dim);

    for(uint i=0; i<(p->getWidth()*p->getHeight()); i++)
        drawBrickAtIdx(s, p, dim, i, solution);
  
}

bool
Brainblast::checkSolution(Puzzle* puzzle)
{
    assert(puzzle);
    
    if(puzzle)
        return puzzle->checkSolution();

    return false;
}

void 
Brainblast::startGame()
{
    initGameKyra();
    eventLoop();
}

#define SDL_TIMER_EVENT ( SDL_USEREVENT + 0 )
const int TIMER_INTERVAL = 40;
Uint32 TimerCallback(Uint32 interval)
{
	SDL_Event event;
	event.type = SDL_TIMER_EVENT;

	SDL_PeepEvents( &event, 1, SDL_ADDEVENT, 0 );
	return TIMER_INTERVAL;
}

void 
Brainblast::initGameKyra()
{
    Random random(time(0));
    m_engine = new KrEngine( m_screen );
    m_engine->Draw(); 

	// Load the dat file.
	// The dat file was carefully created in the sprite
	// editor. Loading allows us access to the 
	// MAGE, PARTICLE, and CARPET.
	if ( !m_engine->Vault()->LoadDatFile( "../images/bb.dat" ) )
	{
		printf( "Error loading the sprites file.\n" );
		exit( 255 );
	}

	// Get the PAPRICE resource
	KrSpriteResource* papriceRes = m_engine->Vault()->GetSpriteResource( BB_PAPRICE );
	GLASSERT( papriceRes );

	// Create the paprice sprite and add it to the tree
	BrainSprite* paprice = new BrainSprite( papriceRes );
	paprice->SetNodeId(BB_PAPRICE);
	paprice->SetPos( random.Rand(VIDEOX), 0);
	m_engine->Tree()->AddNode( 0, paprice );
}

void
Brainblast::initGame(int lvl)
{
    createBricks();

    if( !makeLevel(lvl) )
        exit(1);

//    SDL_WM_ToggleFullScreen(m_screen);

//     drawAllBricks(m_screen, m_currentLvl, m_field1, true);
//     drawBoard(m_screen, m_field1, m_currentLvl, PWIDTH, PHEIGHT);

//     drawAllBricks(m_screen, m_currentLvl, m_field2, true);
//     drawBoard(m_screen, m_field2, m_currentLvl, PWIDTH, PHEIGHT);
  
//     SDL_Delay(3000); 

//     drawAllBricks(m_screen, m_currentLvl, m_field1);
//     drawBoard(m_screen, m_field1, m_currentLvl, PWIDTH, PHEIGHT);

//     drawAllBricks(m_screen, m_currentLvl, m_field2);
//     drawBoard(m_screen, m_field2, m_currentLvl, PWIDTH, PHEIGHT);
}

int Brainblast::eventLoop()
{
	assert(m_engine);

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
			static int t = 0;
			const float a = 1.01;
			BrainSprite* paprice = static_cast<BrainSprite*>(m_engine->Tree()->FindNodeById( BB_PAPRICE ));
			if( paprice )
			{
				paprice->move();
				printf("pos = %i,%i\n", paprice->X(), paprice->Y());
			}
			m_engine->Draw();

        }
        break;
        
        default:
            break;
		}
        
	}
    
    return 0;
}

void Brainblast::handleKeyEvent(SDL_KeyboardEvent* key)
{
    assert(key);
    
    printf( "%s\n", SDL_GetKeyName(key->keysym.sym));
}

/* Definition of a level file ...

-------------------------------------------------------------
|    1    |    2    |    3    |    4    |   ...   |    N    |
|---------+---------+---------+---------+---------+---------|
|   N+1   |   N+2   |   N+3   |   N+4   |   ...   |   2*N   |
|---------+---------+---------+---------+---------+---------|
|    :    |    :    |    :    |    :    |    :    |    :    |
|---------+---------+---------+---------+---------+---------|
|(M-1)*N+1|(M-1)*N+2|(M-1)*N+3|(M-1)*N+4|   ...   |   M*N   |
-------------------------------------------------------------

WIDTH
HEIGHT
PLACE-1  BRICKTYPE-1
PLACE-2  BRICKTYPE-2
:         :
PLACE-MN BRICKTYPE-MN

*/
