/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "Brainblast.h"

#include "../images/bb.h"

#include "grinliz/glrandom.h"

using namespace grinliz;
using namespace brain;

Brainblast* Brainblast::s_instance;

Brainblast::Brainblast() : m_sound(new BrainSound),
						   m_currentLvl1(0),
						   m_currentLvl2(0),
                           m_screen( SDL_SetVideoMode( VIDEOX, VIDEOY, VIDEOBITS, SDL_HWSURFACE ) ),
                           m_field1(0),
                           m_field2(0),
                           m_bricks(0),
						   m_engine(0),
						   m_bgTree(0),
						   m_fgTree(0),
						   m_sprites(0),
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

	s_instance = this;

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
    m_field1->x = MARGIN; 
	m_field1->y = MARGIN; 
    m_field1->w = VIDEOX/2-4*MARGIN; 
	//m_field1->h = VIDEOY-2*MARGIN;
	m_field1->h = m_field1->w;
  
    m_field2 = new SDL_Rect;
    m_field2->x = m_field1->w+3*MARGIN; 
	m_field2->y = MARGIN; 
    m_field2->w = m_field1->w; 
	m_field2->h = m_field1->h;
}

// Copy constructor
Brainblast::Brainblast(const Brainblast& bb) : m_currentLvl1(0),
											   m_currentLvl2(0),
                                               m_screen(0),
                                               m_field1(0),
                                               m_field2(0),
                                               m_bricks(0),
											   m_engine(0),
											   m_bgTree(0),
											   m_fgTree(0),
											   m_sprites(0),
                                               red(bb.red),
                                               blue(bb.blue),
                                               black(bb.black),
                                               green(bb.green),
                                               white(bb.white),
                                               yellow(bb.yellow),
                                               cyan(bb.cyan),
                                               magenta(bb.magenta)
{
#warning "Whats the point of this really?"
  
    m_currentLvl1 = new Puzzle( *bb.m_currentLvl1 );
    m_currentLvl2 = new Puzzle( *bb.m_currentLvl2 );
    assert( m_currentLvl1 );
    assert( m_currentLvl2 );

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
#warning "Whats the point of this really?"
    // Important to clean up the old memory before
    // assigning the new ones
    cleanup();

    m_screen      = new SDL_Surface( *bb.m_screen );
    m_currentLvl1 = new Puzzle( *bb.m_currentLvl1 );
    m_currentLvl2 = new Puzzle( *bb.m_currentLvl2 );

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

    zap( m_currentLvl1 );
    zap( m_currentLvl2 );
    zap( m_field1 ); 
	zap( m_field2 );
	zap( m_engine );
	zap( m_bgTree );
	zap( m_fgTree );
}

bool
Brainblast::makeLevel(int lvl)
{
    if(bbc::debug) std::cerr << "Brainblast::makeLevel(" << lvl << ")\n";

    if(lvl == 0) 
    {
        m_currentLvl1 = new Puzzle(5,5,*m_field1);
        m_currentLvl2 = new Puzzle(5,5,*m_field2);
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
                m_currentLvl1 = new Puzzle( height, width, *m_field1 );
                m_currentLvl2 = new Puzzle( height, width, *m_field2 );
            }
            else 
            {
                if(bbc::debug) std::cerr << val << " ";
                if( i%2 == 0 ) tmp = val;
                else  {
                    m_currentLvl1->setSolutionBrickWithIdx(m_bricks[val],tmp-1);
                    m_currentLvl2->setSolutionBrickWithIdx(m_bricks[val],tmp-1);
				}
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

    m_bricks = new Brick*[NOF_BRICK_TYPES];

    for(int i=0; i<NOF_BRICK_TYPES; i++) 
    {
		if(bbc::debug) std::cerr << "Brainblast::createBricks() Creating brick " << i+1 << "\n";

		GlSListIterator<KrResource*> rit = m_engine->Vault()->GetResourceIterator();
		for(rit.Begin(); !rit.Done(); rit.Next())
		{
			KrSpriteResource* sr = rit.Current()->ToSpriteResource();
			if( sr )
			{
				KrSprite* b = new KrSprite(sr);
				m_bricks[i] = new Brick(b, i);
				
			}
		}
    }
}

bool
Brainblast::createBoards()
{
	if(bbc::debug) std::cerr << "Brainblast::createBoards()\n";

	// Create the master tile
	KrTileResource* tileRes = m_engine->Vault()->GetTileResource( BB_WOOD );
	if( !tileRes )
		return false;
	KrTile* tile = new KrTile(tileRes);

	m_currentLvl1->setBackgroundTile(tile);
	m_currentLvl2->setBackgroundTile(tile);

	return true;
}

void
Brainblast::drawBoard(SDL_Surface* s, SDL_Rect* dim, Puzzle* p)
{
//    if(bbc::debug) std::cerr << "Brainblast::drawBoard()\n";

	return;

    assert(s); 
    assert(dim);
    assert(p);

    /* Lock the screen, if needed */
    if(SDL_MUSTLOCK(m_screen)) {
        if(SDL_LockSurface(m_screen) < 0) 
            return;
    }

    int xSpace = bbc::round(static_cast<double>(dim->w) / p->width ());
    int ySpace = bbc::round(static_cast<double>(dim->h) / p->height());

//   int xSpace = dim->w / p->getWidth();
//   int ySpace = dim->h / p->getHeight();

    for(unsigned int i=0; i<=p->height(); i++)     
        bbc::line(s, dim->x, dim->y+i*ySpace, dim->x+dim->w, dim->y+i*ySpace, magenta);

    for(unsigned int i=0; i<=p->width(); i++) 
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

bool
Brainblast::checkSolution(Puzzle* puzzle)
{
    assert(puzzle);
    
    if(puzzle)
        return puzzle->checkSolution();

    return false;
}

bool
Brainblast::startGame()
{
    if( !initGameKyra() )
		return false;

	if( !initGame() )
		return false;

    eventLoop();

	return true;
}

#define SDL_TIMER_EVENT ( SDL_USEREVENT + 0 )
const int TIMER_INTERVAL = 40;
Uint32 TimerCallback(Uint32 /*interval*/)
{
	SDL_Event event;
	event.type = SDL_TIMER_EVENT;

	SDL_PeepEvents( &event, 1, SDL_ADDEVENT, 0 );
	return TIMER_INTERVAL;
}

bool 
Brainblast::initGameKyra()
{
	srand(time(0));

    m_engine = new KrEngine( m_screen );
    m_engine->Draw(); 

	// Load the dat file.
	// The dat file was carefully created in the sprite
	// editor. Loading allows us access to the 
	// MAGE, PARTICLE, and CARPET.
	if ( !m_engine->Vault()->LoadDatFile( "../images/bb.dat" ) )
	{
		printf( "Error loading the sprites file.\n" );
		return false;	}

	// Add background and foreground trees
	KrImNode* m_bgTree = new KrImNode;
	KrImNode* m_fgTree = new KrImNode;
	m_engine->Tree()->AddNode(0, m_bgTree);
	m_engine->Tree()->AddNode(0, m_fgTree);
	m_bgTree->SetZDepth(1);
	m_fgTree->SetZDepth(20);
	
	// Get the PAPRICE resource
	KrSpriteResource* papriceRes = m_engine->Vault()->GetSpriteResource( BB_PAPRICE );
	assert( papriceRes );

	// Create the paprice sprite and add it to the tree
	BrainSprite* paprice = new BrainSprite( papriceRes, "paprice" );
	paprice->SetNodeId(BB_PAPRICE);
	paprice->SetPos( rand()%VIDEOX, 0);
	m_engine->Tree()->AddNode( m_fgTree, paprice );
	m_sprites.push_back(paprice);

	return true;
}

BrainSprite* Brainblast::createStar()
{
	// Get the STAR resource
	KrSpriteResource* starRes = m_engine->Vault()->GetSpriteResource( BB_STAR );
	GLASSERT( starRes );

	// Create the paprice sprite and add it to the tree
	BrainSprite* star = new BrainSprite( starRes, "star" );
	star->SetNodeId(BB_STAR);
	star->SetPos( rand()%VIDEOX, 0);
	m_engine->Tree()->AddNode( m_bgTree, star );
	m_sprites.push_back(star);
	return star;
}

bool
Brainblast::initGame(int lvl)
{
    createBricks();

    if( !makeLevel(lvl) )
	{
		printf("ERROR: Can't make level\n");
		return false;
	}

	if( !createBoards() )
	{
		printf("ERROR: Can't create boards\n");
		return false;		
	}

    // SDL_WM_ToggleFullScreen(m_screen);

    SDL_Delay(1000); 

	// Start music
	if( !(m_sound->initializeSound() &&
		  m_sound->loadMusic("../music/Instant Remedy - Outrun.mp3") &&
		  m_sound->playMusic()) )
		printf("ERROR: Could not start music\n");
	

	return true;
}

int Brainblast::eventLoop()
{
	assert(m_engine);

    SDL_Event event;
	bool done = false;
    // Start timing!
	SDL_SetTimer( TIMER_INTERVAL, TimerCallback );

	bool keysHeld[323] = {false};

    while( !done && SDL_WaitEvent(&event) )
	{
		if ( event.type == SDL_QUIT )
			break;
        
		switch(event.type)
		{
        case SDL_KEYDOWN:
			keysHeld[event.key.keysym.sym] = true;
			printf( "%s\n", SDL_GetKeyName(event.key.keysym.sym));

			if( event.key.keysym.sym == SDLK_m )
				m_sound->toggleMusic();

			break;
			
		case SDL_KEYUP:
			keysHeld[event.key.keysym.sym] = false;
			break;
			
        case SDL_TIMER_EVENT:
        {
// 			static int t = 0;
// 			const float a = 1.01;
 			BrainSprite* paprice = static_cast<BrainSprite*>(m_engine->Tree()->FindNodeById( BB_PAPRICE ));
// 			BrainSprite* star = static_cast<BrainSprite*>(m_engine->Tree()->FindNodeById( BB_STAR ));
// 			if( paprice && star )
// 			{
// 				paprice->move();
// 				star->move();
// 			}

			std::vector<BrainSprite*>::iterator it;
			std::vector<BrainSprite*>::iterator end = m_sprites.end();
			for(it = m_sprites.begin(); it!=end; ++it)
				(*it)->move();

			m_engine->Tree()->Walk();

			// Detect collisions
			if(!paprice->isCarrying() && keysHeld[SDLK_UP] ) {
				std::vector<KrImage*> collides;
				if( m_engine->Tree()->CheckAllCollision(paprice,&collides) )
				{  
					printf("Collision!\n");
					paprice->pickUp(reparentSprite((BrainSprite*)*collides.begin(),paprice));
// 					std::vector<KrImage*>::iterator cit;
// 					std::vector<KrImage*>::iterator cend = collides.end();
// 					for(cit = collides.begin(); cit != cend; ++cit)
// 					{
// 						paprice->pickUp(reparentSprite((BrainSprite*)*cit,paprice));
// 					}
				}
			}

			m_engine->Draw();

        }
        break;
        
        default:
            break;
		}

		if( keysHeld[SDLK_ESCAPE] )
			done = true;
		if( keysHeld[SDLK_LEFT] )
		{
			BrainSprite* paprice = static_cast<BrainSprite*>(m_engine->Tree()->FindNodeById( BB_PAPRICE ));
			paprice->left();
		}
		if( keysHeld[SDLK_RIGHT] )
		{
			BrainSprite* paprice = static_cast<BrainSprite*>(m_engine->Tree()->FindNodeById( BB_PAPRICE ));
			paprice->right();
		}
// 		if( keysHeld[SDLK_UP] )
// 		{
// 			BrainSprite* paprice = static_cast<BrainSprite*>(m_engine->Tree()->FindNodeById( BB_PAPRICE ));
// 			paprice->jump();
// 		}
		if( keysHeld[SDLK_F1] )
			createStar();
		if( keysHeld[SDLK_DOWN] ) 
		{
			BrainSprite* paprice = static_cast<BrainSprite*>(m_engine->Tree()->FindNodeById( BB_PAPRICE ));
			paprice->drop();
		}
	}
    
    return 0;
}

void Brainblast::handleKeyEvent(SDL_KeyboardEvent* key)
{
    assert(key);
    
    printf( "%s\n", SDL_GetKeyName(key->keysym.sym));
}

BrainSprite* Brainblast::reparentSprite(BrainSprite* bs, BrainSprite* parent)
{
	BrainSprite* clone = static_cast<BrainSprite*>(bs->Clone());
	m_engine->Tree()->AddNode(parent,clone);
	std::vector<BrainSprite*>::iterator it = find(m_sprites.begin(),m_sprites.end(),bs);
	if( it != m_sprites.end() )
		m_sprites.erase(find(m_sprites.begin(),m_sprites.end(),bs));
	if( !parent )
		m_sprites.push_back(clone);
    m_engine->Tree()->DeleteNode(bs);
	return clone;
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
