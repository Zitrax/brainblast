/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "Brainblast.h"

#include "../images/bb.h"
#include "../images/bb_bg.h"
#include "grinliz/glrandom.h"
#include "SDL_ttf.h"

#include <sstream>  // ostringstream
#include <iomanip>  // setfill setw

using namespace grinliz;
using namespace brain;
using namespace std;

Brainblast* Brainblast::s_instance;

Brainblast::Brainblast() : m_play(false),
						   m_sound(new BrainSound),
						   m_player1(0),
						   m_players(1),
						   m_currentLvl1(0),
						   m_currentLvl2(0),
						   m_current_lvl(1),
                           m_screen( SDL_SetVideoMode( VIDEOX, VIDEOY, VIDEOBITS, SDL_HWSURFACE ) ),
                           m_field1(0),
                           m_field2(0),
						   m_bricks(),
						   m_total_bricks(0),
						   m_engine(0),
						   m_bgTree(0),
						   m_fgTree(0),
						   m_start_time(0),
						   m_sprites(0),
                           red    ( SDL_MapRGB(m_screen->format, 0xff, 0x00, 0x00) ),
                           blue   ( SDL_MapRGB(m_screen->format, 0x00, 0x00, 0xff) ),
                           black  ( SDL_MapRGB(m_screen->format, 0x00, 0x00, 0x00) ),
                           green  ( SDL_MapRGB(m_screen->format, 0x00, 0xff, 0x00) ),
                           white  ( SDL_MapRGB(m_screen->format, 0xff, 0xff, 0xff) ),
                           yellow ( SDL_MapRGB(m_screen->format, 0xff, 0xff, 0x00) ),
                           cyan   ( SDL_MapRGB(m_screen->format, 0x00, 0xff, 0xff) ),
                           magenta( SDL_MapRGB(m_screen->format, 0xff, 0x00, 0xff) ),
						   m_bg_vault( new KrResourceVault() ),
						   m_bg_sprite(0)
{
    if(bbc::debug) cerr << "Brainblast::Brainblast() Videomode(" << VIDEOX << "," << VIDEOY << ")\n";

	s_instance = this;

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
    {
        printf("=== ERROR: Can't init SDL:  %s ===\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit); 

    if(m_screen == NULL)
    {
        printf("=== ERROR: Can't set video mode: %s ===\n", SDL_GetError());
        exit(1);
    }   

	if( TTF_Init() == -1 )
    {
        printf("=== ERROR: Can't initialize ttf support: (%s) ===\n",TTF_GetError());
        exit(1);
    }   

	m_center_text_rect.x=400; m_center_text_rect.y=300; m_center_text_rect.w=300; m_center_text_rect.h=50;
	m_topleft_text_rect.x=10; m_topleft_text_rect.y=10; m_topleft_text_rect.w=300; m_topleft_text_rect.h=50;
}

bool Brainblast::setupFields(int players)
{
	// Clear all fields
	zap(m_field1);
	zap(m_field2);

	switch(players)
	{
	case 1:
		m_field1 = new SDL_Rect;
		m_field1->x = 262;
		m_field1->y = MARGIN; 
		m_field1->w = 500;
		m_field1->h = m_field1->w;
		return true;
		break;
	case 2:
		m_field1 = new SDL_Rect;
		m_field1->x = MARGIN; 
		m_field1->y = MARGIN; 
		m_field1->w = VIDEOX/2-4*MARGIN; 
		m_field1->h = m_field1->w;
		
		m_field2 = new SDL_Rect;
		m_field2->x = m_field1->w+3*MARGIN; 
		m_field2->y = MARGIN; 
		m_field2->w = m_field1->w; 
		m_field2->h = m_field1->h;
		return true;
		break;
	default:
		return false;
		break;
	}
	
	return false;
}

Brainblast::~Brainblast()
{
    if(bbc::debug) cerr << "Brainblast::~Brainblast()\n";
 
    cleanup();

	TTF_Quit();
}

void
Brainblast::cleanup()
{
	map<int,Brick*>::iterator it;
	map<int,Brick*>::iterator end = m_bricks.end();
	for( it = m_bricks.begin(); it!=end; ++it)
	{
		// The sprite is never inserted in a node tree
		// so we need to delete it. The brick itself
		// can not know whether someone else has deleted it.
		delete it->second->getSprite();  
		delete it->second;
	}
    
    // m_screen is deleted by SDL_Quit
	//	zap( m_bgTree );   - Deleted along with the engine
	//	zap( m_fgTree );   - Deleted along with the engine

	zap( m_sound );
    zap( m_currentLvl1 );
    zap( m_currentLvl2 );
    zap( m_field1 ); 
	zap( m_field2 );
	zap( m_engine );
	zap( m_bg_vault );
}

void
Brainblast::makeRandomLevel(int w,int h,int n)
{
	if(bbc::debug) cerr << "Brainblast::makeRandomLevel(" 
						<< w << "," << h << "," << n << ")\n";

	assert(n<=w*h);

	m_currentLvl1 = new Puzzle(w,h,*m_field1);
	if( m_players > 1 )
		m_currentLvl2 = new Puzzle(w,h,*m_field2);

	// Vector to make sure we use up the indexes
	// I guess a nicer solution would be to randomize
	// and iterate over a vector/array instead but...
	vector<int> indexes;
	for(int i=0; i<w*h; ++i)
		indexes.push_back(i);

	for(int i=0; i<n; ++i)
	{
		int type = bbc::randint(0,m_bricks.size()-1);
		map<int,Brick*>::iterator it = m_bricks.begin();
		for(int j=0;j<type;++j)
			++it;
		assert(it!=m_bricks.end());

		int idxidx  = bbc::randint(0,indexes.size()-1);
		int idx = indexes[idxidx];
		indexes.erase(find(indexes.begin(),indexes.end(),idx));

		m_currentLvl1->setSolutionBrickWithIdx(it->second,idx);
		if( m_players > 1 )
			m_currentLvl2->setSolutionBrickWithIdx(it->second,idx);
	}
	
}

bool
Brainblast::makeLevel(int lvl)
{
    if(bbc::debug) cerr << "Brainblast::makeLevel(" << lvl << ")\n";

	zap(m_currentLvl1);
	zap(m_currentLvl2);

    if(lvl == 0) 
    {
		makeRandomLevel(4,4,4);
        return true;
    }

    else 
    {

        char* filename  = static_cast<char*>(malloc(40));
      
        char* filebase = "../lvl/lvl%03d.txt";
      
        sprintf(filename, filebase, lvl);
        if( bbc::debug ) cerr << "Level file: " << filename << "\n";
      
        ifstream in(filename);
        if ( !in ) { 
            cerr << "=== ERROR: Level file could not be opened... ===\n"; 
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
				if( m_players > 1 )
					m_currentLvl2 = new Puzzle( height, width, *m_field2 );
            }
            else 
            {
                if(bbc::debug) cerr << val << " ";
                if( i%2 == 0 ) tmp = val;
                else  {
					map<int,Brick*>::iterator it = m_bricks.find(val);
					if( it == m_bricks.end() )
					{
						cerr << "=== ERROR: Level file contain invalid brick type (" << val << ") ===\n";
						return false;
					}
					if( tmp > height*width )
					{
						cerr << "=== ERROR: Level file contain invalid brick position (" << tmp << ") ===\n";
						return false;
					}

                    m_currentLvl1->setSolutionBrickWithIdx(it->second,tmp-1);
					if( m_players > 1 )
						m_currentLvl2->setSolutionBrickWithIdx(it->second,tmp-1);
				}
            }
            i++;
        }

        if(bbc::debug) cerr << "\n";
        free(filename);
        in.close();
    } 

    return true;
}

void
Brainblast::createBricks()
{
    if(bbc::debug) cerr << "Brainblast::createBricks()\n";

	GlSListIterator<KrResource*> rit = m_engine->Vault()->GetResourceIterator();
	for(rit.Begin(); !rit.Done(); rit.Next())
	{
		KrSpriteResource* sr = rit.Current()->ToSpriteResource();
		if( sr )
		{
			KrSprite* b = new KrSprite(sr);
			m_bricks.insert(pair<int,Brick*>(sr->ResourceId(),new Brick(b, sr->ResourceId())));
			m_total_bricks++;
			assert(m_total_bricks<MAX_NOF_BRICK_TYPES);
		}
	}
}

void
Brainblast::clearFloor()
{
	// Clear the floor of existing sprites here.
	vector<BrainSprite*>::iterator it  = m_sprites.begin();
	vector<BrainSprite*>::iterator end = m_sprites.end();
	while(it!=end)
	{
		if(*it!=m_player1)
		{
			m_engine->Tree()->DeleteNode(*it);
			it=m_sprites.erase(it);
			end = m_sprites.end();
		}
		else
			++it;
	}

	m_currentLvl1->stopSelection();
	if( m_players > 1 )
		m_currentLvl2->stopSelection();
}

bool
Brainblast::changeLevel(int lvl)
{
	if( !makeLevel(lvl) )
	{
		printf("=== ERROR: Can't make level ===\n");
		return false;
	}

	m_current_lvl = lvl;

	if( !createBoards() )
	{
		printf("=== ERROR: Can't create boards ===\n");
		return false;		
	}

	clearFloor();

	// Make sure texts are cleared
 	grinliz::Rectangle2I r1,r2,r3;
//  	r1.Set(m_center_text_rect.x,m_center_text_rect.y,
// 		   m_center_text_rect.w,m_center_text_rect.h);
// 	r2.Set(m_topleft_text_rect.x,m_topleft_text_rect.y,
// 		   m_topleft_text_rect.w,m_topleft_text_rect.h);
// 	m_engine->InvalidateRectangle(r1);
// 	m_engine->InvalidateRectangle(r2);

	// Smaller rectangles are not entirely correct
	// so just invalidate the entire screen.
	// It's not expensive to do that here.
	r3.Set(0,0,VIDEOX,VIDEOY);
	m_engine->InvalidateRectangle(r3);

	time(&m_start_time);
	m_play = false;

	return true;
}

bool
Brainblast::createBoards()
{
	if(bbc::debug) cerr << "Brainblast::createBoards()\n";

	// Create the master tile
	KrTileResource* tileRes = m_engine->Vault()->GetTileResource( BB_REDSTAR );
	if( !tileRes )
		return false;
	KrTile tile(tileRes); // The puzzles will clone it

	m_currentLvl1->setBackgroundTile(&tile);
	if( m_players > 1 )
		m_currentLvl2->setBackgroundTile(&tile);

	return true;
}

void
Brainblast::drawBoard(SDL_Surface* s, SDL_Rect* dim, Puzzle* p)
{
//    if(bbc::debug) cerr << "Brainblast::drawBoard()\n";

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
Brainblast::startGame(int players)
{
	m_players = players;

	if( !setupFields(players) )
	{
		printf("=== ERROR: Could not setup fields. ===\n");
		return false;
	}

    if( !initGameKyra() )
	{
		printf("=== ERROR: Could not init kyra. ===\n");
		return false;
	}

	if( !initGame(0) )
	{
		printf("=== ERROR: Could not init game. ===\n");
		return false;
	}

    eventLoop();

	return true;
}

bool 
Brainblast::initGameKyra()
{
	srand(time(0));

    m_engine = new KrEngine( m_screen );
    m_engine->Draw(); 

	// Load the dat file.
	// The dat file was carefully created in the sprite
	// editor.
	if ( !m_engine->Vault()->LoadDatFile( "../images/bb.dat" ) )
	{
		printf( "=== Error: Loading the sprites file. ===\n" );
		return false;	
	}

	// Load the background vault
	if( !m_bg_vault->LoadDatFile( "../images/bb_bg.dat" ) )
	{
		printf( "=== Error: Loading backgrounds. ===\n" );
	}

	// Add background and foreground trees
	m_bgTree = new KrImNode;
	m_fgTree = new KrImNode;
	m_bgTree->SetZDepth(-10);
	m_fgTree->SetZDepth(11);
	m_engine->Tree()->AddNode(0, m_bgTree);
	m_engine->Tree()->AddNode(0, m_fgTree);
	
	// Add the background
	KrSpriteResource* bg = m_bg_vault->GetSpriteResource( BB_BG_STARS_BG );
	assert(bg);
	m_bg_sprite = new KrSprite(bg);
	m_engine->Tree()->AddNode( 0, m_bg_sprite );
	m_bg_sprite->SetZDepth(-20);

	// Get the WIZARD resource
	KrSpriteResource* wizardRes = m_engine->Vault()->GetSpriteResource( BB_WIZARD );
	assert( wizardRes );

	// Create the wizard sprite and add it to the tree
	// TODO: Player should be created with a selected name instead of wizard
	m_player1 = new BrainPlayer( wizardRes, "wizard" );
	m_player1->SetNodeId(BB_WIZARD);
	m_player1->SetPos( rand()%VIDEOX, 0);
	m_engine->Tree()->AddNode( m_fgTree, m_player1 );
	m_sprites.push_back(m_player1);

	return true;
}

BrainSprite* Brainblast::addSprite()
{
	if( !m_play && (difftime(time(0),m_start_time) <= WAITTIME) )
		return 0;

	KrSpriteResource* spriteRes = 0;
	// TODO: Can we have different types on the other level ?
	vector<int> types = m_currentLvl1->getSolutionTypes();
	int r = bbc::randint(0,types.size()-1);
	spriteRes = m_engine->Vault()->GetSpriteResource( types[r] );
	assert( spriteRes );

	// Create the wizard sprite and add it to the tree
	BrainSprite* sprite = new BrainSprite( spriteRes, "rand", true );
	sprite->SetNodeId(types[r]);
	sprite->SetPos( rand()%VIDEOX, 0);
	sprite->setSpeed(double(bbc::randint(-10,10)),0);
	m_engine->Tree()->AddNode( m_bgTree, sprite );
	m_sprites.push_back(sprite);
	return sprite;
}

bool
Brainblast::initGame(int lvl)
{
    createBricks();

	if( !changeLevel(lvl) )
		return false;

    // SDL_WM_ToggleFullScreen(m_screen);

    //SDL_Delay(1000); 

	// Start music
	if( !(m_sound->initializeSound() &&
		  //m_sound->loadMusic("../music/Instant Remedy - Outrun.mp3") &&
		  m_sound->loadMusic("../music/onward.xm") &&
		  m_sound->playMusic()) )
		printf("=== ERROR: Could not start music === \n");
	
	if( m_sound->isInitialized() )
	{
		m_sound->addSample("../sounds/click.wav",CLICK);
		m_sound->addSample("../sounds/bounce.wav",BOUNCE);
	}

	return true;
}

#define SDL_DRAW_EVENT ( SDL_USEREVENT + 0 )
#define SDL_ADD_SPRITE_EVENT ( SDL_USEREVENT + 1 )
Uint32 TimerCallback(Uint32 interval, void* event)
{
	SDL_PushEvent( static_cast<SDL_Event*>(event) );
	return interval;
}

int Brainblast::eventLoop()
{
	// There all sort of things that can be
	// optimized in this loop. But lets get it
	// working properly first.

	assert(m_engine);

    SDL_Event event;
	bool done = false;
    // Start timing!
	SDL_Event draw_event; draw_event.type = SDL_DRAW_EVENT;
	SDL_AddTimer( 40, TimerCallback, &draw_event );	
	SDL_Event add_sprite_event; add_sprite_event.type = SDL_ADD_SPRITE_EVENT;
	SDL_AddTimer( 2000, TimerCallback, &add_sprite_event );	

	bool keysHeld[323] = {false};
	bool game_over = false;

    while( !done && SDL_WaitEvent(&event) )
	{
		if ( event.type == SDL_QUIT )
			break;
        
		time_t now = time(0);

		switch(event.type)
		{
        case SDL_KEYDOWN:

			printf( "%s\n", SDL_GetKeyName(event.key.keysym.sym));

			// M = TOGGLE SOUND
			if( event.key.keysym.sym == SDLK_m )
				m_sound->toggleMusic();
			// F = TOGGLE FULLSCREEN
			else if( event.key.keysym.sym == SDLK_f )
				SDL_WM_ToggleFullScreen(m_screen);
			else if( (event.key.keysym.sym == SDLK_LEFT) && 
					 (m_currentLvl1->isSelecting()) )
			{
				m_currentLvl1->navigate(Puzzle::LEFT);
				
			}
			else if( (event.key.keysym.sym == SDLK_RIGHT) && 
					 (m_currentLvl1->isSelecting()) )
			{
				m_currentLvl1->navigate(Puzzle::RIGHT);
				
			}
			else if( event.key.keysym.sym == SDLK_UP )  
			{
				if( m_currentLvl1->isSelecting() )
					m_currentLvl1->navigate(Puzzle::UP);
				else if( m_player1->isCarrying() )
				{
					BrainSprite* o = m_player1->drop(0);
					o->setStatic(true);
					m_currentLvl1->startSelection(o);
				}
				else					
					keysHeld[event.key.keysym.sym] = true;
					
			}
			else if( (event.key.keysym.sym == SDLK_DOWN) && 
					 (m_currentLvl1->isSelecting()) )
			{
				m_currentLvl1->navigate(Puzzle::DOWN);
				
			}
			else if( (event.key.keysym.sym == SDLK_RETURN) && 
					 (m_currentLvl1->isSelecting()) )
			{
				m_sound->playSample(CLICK);
				BrainSprite* s = 0;
				int cscore = m_currentLvl1->brickScore();
				m_currentLvl1->select(&s) ? m_player1->addScore(cscore) : m_player1->addScore(-1*cscore/10);
				if( s )
				{
					vector<BrainSprite*>::iterator it = find(m_sprites.begin(),m_sprites.end(),s);
					m_sprites.erase(it);

					if( checkSolution(m_currentLvl1) )
					{
						if( !changeLevel(m_current_lvl+1) )
							changeLevel(1);
					}
					if( (m_players > 1) && checkSolution(m_currentLvl2) )
					{

					}
				}
			}
			else if( event.key.keysym.sym == SDLK_SPACE )
			{
				if( game_over )
				{
					m_player1->setScore(0);
					changeLevel(1);
				}
				else if( !m_play )
					m_play = true;
			}

			else
				keysHeld[event.key.keysym.sym] = true;

			break;
			
		case SDL_KEYUP:
			keysHeld[event.key.keysym.sym] = false;
			break;

		case SDL_ADD_SPRITE_EVENT:
			if( !game_over )
				addSprite();
			break;
			
        case SDL_DRAW_EVENT:
        {
			vector<BrainSprite*>::iterator it  = m_sprites.begin();
			vector<BrainSprite*>::iterator end = m_sprites.end();
			while(it!=end)
			{
				// First check if we should delete this sprite after a timeout
				if( (*it)->temporary() && (difftime(now,(*it)->creationTime()) > 30) )
				{
					m_engine->Tree()->DeleteNode(*it);
					// Set it to the next valid element after erasing
					it = m_sprites.erase(it);
					end = m_sprites.end(); // Only recalculate this when we might be invalidated
				}
				else
				{
// 					// <Collision between bricks experiment>
// 					double x1 = (*it)->speedX();
// 					double y1 = (*it)->speedY();

// 					if( (x1>=15) || (y1>=0.15) )
// 					{ 

// 						vector<KrImage*> collides;
// 						if( ((*it) != m_player1) && m_engine->Tree()->CheckAllCollision(*it,&collides) )
// 						{
// 							BrainSprite* c = dynamic_cast<BrainSprite*>(*collides.begin());
// 							if( c && (c != m_player1) ) {
								
// 								double x2 = c->speedX();
// 								double y2 = c->speedY();
								
// 								if( (x2 < 5) && (y2 < 5) )
// 									(*it)->setSpeed(-0.5*x1,-0.5*y1);
// 								else
// 								{
									
// 									(*it)->setSpeed(x2,y2);
// 									c->setSpeed(x1,y1);
									
// 								}
// 							}
// 						}
// 					}
// 					// </Collision between bricks experiment>

					(*it)->move();
					++it;
				}
			}

			m_engine->Tree()->Walk();

			// Detect collisions
			if(!m_player1->isCarrying() && keysHeld[SDLK_UP] ) {
				vector<KrImage*> collides;
				if( m_engine->Tree()->CheckChildCollision(m_player1,m_bgTree,&collides) )
				{  
					printf("Collision!\n");
					// Use of dynamic cast as we only want to try to pick
					// up BrainSprites and not ordinary KrSprites as in the
					// Bricks. 
					// Otherwise we might be able to pick up the solution in the beginning :)
					BrainSprite* c = dynamic_cast<BrainSprite*>(*collides.begin());
					if( c ) 
						m_player1->pickUp(reparentSprite(c,m_player1));
				}
			}

			if( m_play || (difftime(now,m_start_time) > WAITTIME) )
			{
				m_currentLvl1->setVisibleSolution(false);
				if( m_players > 1 )
					m_currentLvl2->setVisibleSolution(false);
				m_play = true;
			}

			m_engine->Draw();

        }
        break;
        
        default:
            break;
		}

		// The following section checks for keys that are held down
		// and should continuosly do something.

		if( keysHeld[SDLK_ESCAPE] )
			done = true;
		if( keysHeld[SDLK_LEFT] )
		{
			if( m_player1->isCarrying() )
				m_player1->SetAction("HOLDING.LEFT");
			else
				m_player1->SetAction("WALKING.LEFT");
			m_player1->DoStep();
		}
		else if( keysHeld[SDLK_RIGHT] )
		{
			if( m_player1->isCarrying() )
				m_player1->SetAction("HOLDING.RIGHT");
			else
				m_player1->SetAction("WALKING.RIGHT");
			m_player1->DoStep();
		}
		if( keysHeld[SDLK_RCTRL] )
		{
			m_player1->jump();
		}
		if( keysHeld[SDLK_F1] )
			addSprite();
		if( keysHeld[SDLK_DOWN] ) 
		{
			BrainSprite* bs = m_player1->drop(m_bgTree);
			if(bs)
				m_sprites.push_back(bs);
		}

		// Time left
		int basetime = m_play ? 60 : WAITTIME;
		int sec = static_cast<int>(basetime - difftime(now,m_start_time));
		game_over = sec <= 0;
		if( m_play && game_over ) sec = 0;
		int min = sec/60;
		sec -= min*60;

		ostringstream score_str;
		score_str << "Score: " << m_player1->getScore() << "   Time: " 
				  << setw(2) << setfill('0') << min << ":" 
				  << setw(2) << setfill('0') << sec;
		drawText(score_str.str().c_str(),m_topleft_text_rect);
		if( m_play && game_over )
		{
			drawText("Game Over",m_center_text_rect,32);
			clearFloor();
		}
		
	}
    
    return 0;
}

void Brainblast::handleKeyEvent(SDL_KeyboardEvent* key)
{
    assert(key);
    
    printf( "%s\n", SDL_GetKeyName(key->keysym.sym));
}

BrainSprite* Brainblast::reparentSprite(BrainSprite* bs, KrImNode* parent)
{
	BrainSprite* clone = static_cast<BrainSprite*>(bs->Clone());
	m_engine->Tree()->AddNode(parent,clone);
	vector<BrainSprite*>::iterator it = find(m_sprites.begin(),m_sprites.end(),bs);
	if( it != m_sprites.end() )
		m_sprites.erase(it);
	if( !parent )
		m_sprites.push_back(clone);
    m_engine->Tree()->DeleteNode(bs);
	return clone;
}

void Brainblast::drawText(const char* text, SDL_Rect pos, int size)
{
	// TODO: We could keep the fonts open for performance

	TTF_Font *font;
	font=TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", size);
	if(!font) {
		printf("=== Error: TTF_OpenFont: (%s) === \n", TTF_GetError());
		return;
	}

	SDL_Color color={255,100,0,0};
	SDL_Surface* text_surface;
	if( !(text_surface=TTF_RenderUTF8_Blended(font,text,color)) ) {
		printf("=== Error: TTF_Rend erUTF8_Blended: (%s) === \n", TTF_GetError());
	} 
	else {
		SDL_FillRect(m_screen,&pos,0);
		SDL_BlitSurface(text_surface,NULL,m_screen,&pos);
        SDL_UpdateRects( m_screen, 1, &pos);
		//perhaps we can reuse it, but I assume not for simplicity.
		SDL_FreeSurface(text_surface);
	}

	TTF_CloseFont(font);
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
