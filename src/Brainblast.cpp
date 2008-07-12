/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2008
 * @author Daniel Bengtsson
 */

#include "Brainblast.h"

#include "../images/bb.h"
#include "../images/bb_bg.h"
#include "grinliz/glrandom.h"
#include "consolefont.h"
#include "BrainSoundFMOD.h"
#include "SDL_image.h"

#include <sstream>  // ostringstream
#include <iomanip>  // setfill setw

using namespace grinliz;
using namespace brain;
using namespace std;

Brainblast* Brainblast::s_instance;

Brainblast::Brainblast() : m_play(false),
						   m_start_time(0),
						   m_sound(new BrainSoundFMOD),
						   m_current_levels(),
						   m_fields(),
						   m_current_lvl(1),
                           m_screen( SDL_SetVideoMode( VIDEOX, VIDEOY, VIDEOBITS, SDL_HWSURFACE ) ),
						   m_bricks(),
						   m_total_bricks(0),
						   m_engine(0),
						   m_bgTree(0),
						   m_fgTree(0),
						   m_sprites(0),
						   m_bg_vault( new KrResourceVault() ),
						   m_bg_sprite(0),
						   m_font(0),
						   m_score_font(0),
						   m_left_score_text_box(0),
						   m_right_score_text_box(0),
						   m_center_text_box(0),
						   m_player_manager(0)
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

}

bool Brainblast::setupFields(int players)
{
	// Clear all fields
	for(unsigned int i=0; i<m_current_levels.size(); ++i)
		zap(m_current_levels[i] );
	m_fields.clear();

	switch(players)
	{
	case 1:
	{
		SDL_Rect field1;
		field1.x = 262;
		field1.y = MARGIN; 
		field1.w = 500;
		field1.h = field1.w;
		m_fields.push_back(field1);
		return true;
	}
		break;
	case 2:
	{
		SDL_Rect field1;
		field1.x = MARGIN; 
		field1.y = MARGIN; 
		field1.w = VIDEOX/2-4*MARGIN; 
		field1.h = field1.w;
		
		SDL_Rect field2;
		field2.x = field1.w+3*MARGIN; 
		field2.y = MARGIN; 
		field2.w = field1.w; 
		field2.h = field1.h;

		m_fields.push_back(field1);
		m_fields.push_back(field2);
		return true;
	}
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

	for(unsigned int i=0; i<m_current_levels.size(); ++i)
		zap(m_current_levels[i] );
	for(unsigned int i=0; i<m_current_levels.size(); ++i)
		zap(m_current_levels[i] );
	m_current_levels.clear();
	m_fields.clear();

	zap( m_sound );
	zap( m_engine );
	zap( m_bg_vault );
	zap( m_player_manager );
	zap( m_font );
	zap( m_score_font );
}

void
Brainblast::makeRandomLevel(int w,int h,int n)
{
	if(bbc::debug) cerr << "Brainblast::makeRandomLevel(" 
						<< w << "," << h << "," << n << ")\n";

	assert(n<=w*h);

	for(unsigned int i=0; i<m_player_manager->playerCount(); ++i)
		m_current_levels.push_back(new Puzzle(w,h,m_fields[i]));

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

		// Currently same solution on all levels
		for(unsigned int i=0; i < m_current_levels.size(); ++i)
			m_current_levels[i]->setSolutionBrickWithIdx(it->second,idx);
	}

	// FIXME: If we want to have several players (co-op) on one level
	//        this must be fixed. ( Or levels without players? )
	assert( m_player_manager->playerCount() == m_current_levels.size() );

	for(unsigned int i=0; i < m_current_levels.size(); ++i)
		m_player_manager->getPlayer(i)->setLevel(m_current_levels[i]);
}

bool
Brainblast::makeLevel(int lvl)
{
    if(bbc::debug) cerr << "Brainblast::makeLevel(" << lvl << ")\n";
	
	for(unsigned int i=0; i<m_current_levels.size(); ++i)
		zap(m_current_levels[i] );
	m_current_levels.clear();
	
    if(lvl == 0) 
    {
		// 7x8 is maximum size if you want to avoid overlapping
		makeRandomLevel(4,4,4);
        return true;
    }

    else 
    {

        char* filename  = static_cast<char*>(malloc(40));
      
        const char* filebase = "/usr/share/games/brainblast/lvl/lvl%03d.txt";
      
        sprintf(filename, filebase, lvl);
        if( bbc::debug ) cerr << "Level file: " << filename << "\n";
      
        ifstream in(filename);
        if ( !in ) { 
            cerr << "=== ERROR: Level file could not be opened... ===\n"; 
            free(filename);
            return false;
        }
    
        // Parse Level File
        int val; int i=0; int tmp=0; int width=0; int height=0;
        while( in >> val ) 
        { 
            if( i==0 ) { width = val; }
            else if ( i==1 ) 
            { 
                height = val; 
				for(unsigned int i=0; i<m_player_manager->playerCount(); ++i)
					m_current_levels.push_back(new Puzzle( height, width, m_fields[i] ));
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

					for(unsigned int i=0; i<m_current_levels.size(); ++i)
						m_current_levels[i]->setSolutionBrickWithIdx(it->second,tmp-1);
				}
            }
            i++;
        }

        if(bbc::debug) cerr << "\n";
        free(filename);
        in.close();
    } 

	// FIXME: If we want to have several players (co-op) on one level
	//        this must be fixed. ( Or levels without players? )
	assert( m_player_manager->playerCount() == m_current_levels.size() );

	for(unsigned int i=0; i < m_current_levels.size(); ++i)
		m_player_manager->getPlayer(i)->setLevel(m_current_levels[i]);

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
			// FIXME: We do not want the player sprites here
			// so aborting. But ideally they should be separated
			// into different vaults or in another better way than this.
			if( sr->ResourceId() == BB_WIZARD )
				continue;

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
		if(!dynamic_cast<BrainPlayer*>(*it))
		{
			m_engine->Tree()->DeleteNode(*it);
			it=m_sprites.erase(it);
			end = m_sprites.end();
		}
		else
			++it;
	}

	for(unsigned int i=0; i<m_current_levels.size(); ++i)
		m_current_levels[i]->stopSelection();

	// Make the players drop what they are carrying.
	for(unsigned int i=0; i<m_player_manager->playerCount(); ++i)
		dropPlayerSprite(m_player_manager->getPlayer(i),true);
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

	for(unsigned int i=0; i<m_current_levels.size(); ++i)
		m_current_levels[i]->setBackgroundTile(&tile);

	return true;
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
	{
		printf("=== ERROR: Could not init kyra. ===\n");
		return false;
	}

	m_player_manager = new BrainPlayerManager(1,0);

	// FIXME: Currently only two player support
	m_player_manager->getPlayer(0)->setScoreBox(m_left_score_text_box);
	if( m_player_manager->playerCount() > 1 )
		m_player_manager->getPlayer(1)->setScoreBox(m_right_score_text_box);

	if( !setupFields(m_player_manager->playerCount()) )
	{
		printf("=== ERROR: Could not setup fields. ===\n");
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

KrFontResource* Brainblast::loadFont(const char* file, int glyphs)
{
	// Make surface from font file
	SDL_Surface* raw = IMG_Load(file);

	if(!raw)
	{
		printf( "=== Error: Loading font (%s) === \n", IMG_GetError() );
		return 0;
	}

	SDL_Surface* s32 = SDL_CreateRGBSurface(SDL_SWSURFACE,
											raw->w,
											raw->h,
											32,
											0xff << ( KrRGBA::RED * 8 ),
											0xff << ( KrRGBA::GREEN * 8 ),
											0xff << ( KrRGBA::BLUE * 8 ),
											0xff << ( KrRGBA::ALPHA * 8 ) );

	if(!s32)
	{
		printf( "=== Error: Loading font (%s) === \n", IMG_GetError() );
		return 0;
	}

	SDL_BlitSurface( raw, 0, s32, 0 );
		
	// FIXME: We might need different names when using many fonts ?
	KrPaintInfo pi(s32);
	return new KrFontResource("LOADED_FONT", &pi, 32, 0, KrFontResource::FIXED, glyphs);
}

bool 
Brainblast::initGameKyra()
{
	srand(time(0));

    m_engine = new KrEngine( m_screen );
    m_engine->Draw(); 

	// Set up the font
 	m_font = KrEncoder::CreateFixedFontResource( "CONSOLE", CONSOLEFONT_DAT, CONSOLEFONT_SIZE );
	
	m_score_font = loadFont("/usr/share/games/brainblast/images/bubblemad_8x8.png",83);

	if( !m_font || !m_score_font ) 
	{
		printf( "=== Error: Loading font ===\n" );
		return false;
	}

 	m_left_score_text_box = new KrTextBox(m_score_font,400,50,1);
	m_left_score_text_box->SetPos(10,10);
	m_engine->Tree()->AddNode(0,m_left_score_text_box);

 	m_right_score_text_box = new KrTextBox(m_score_font,400,50,1);
	m_right_score_text_box->SetPos(VIDEOX-310,10);
	m_engine->Tree()->AddNode(0,m_right_score_text_box);

 	m_center_text_box = new KrTextBox(m_score_font,300,50,1);
	m_center_text_box->SetPos(475,300);
	m_engine->Tree()->AddNode(0,m_center_text_box);

	// Load the dat file.
	// The dat file was carefully created in the sprite
	// editor.
	if ( !m_engine->Vault()->LoadDatFile( "/usr/share/games/brainblast/images/bb.dat" ) )
	{
		printf( "=== Error: Loading the sprites file. ===\n" );
		return false;	
	}

	// Load the background vault
	if( !m_bg_vault->LoadDatFile( "/usr/share/games/brainblast/images/bb_bg.dat" ) )
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

	return true;
}

BrainSprite* Brainblast::addSprite()
{
	if( !m_play && (difftime(time(0),m_start_time) <= WAITTIME) )
		return 0;

	KrSpriteResource* spriteRes = 0;
	// TODO:  Can we have different types on the other level ?
	// FIXME: This should be fixed.
	vector<int> types = m_current_levels[0]->getSolutionTypes();
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

	// Start music
	if( !(m_sound->initializeSound() &&
		  m_sound->loadMusic("/usr/share/games/brainblast/music/enigmatic_path.it") &&
		  //m_sound->loadMusic("../music/crazy_memories.it") &&
		  m_sound->playMusic()) )
		printf("=== ERROR: Could not start music === \n");
	
	//if( m_sound->initializeSound() )
	{
		m_sound->addSample("/usr/share/games/brainblast/sounds/click.wav",CLICK);
		m_sound->addSample("/usr/share/games/brainblast/sounds/bounce.wav",BOUNCE);
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
			else if( event.key.keysym.sym == SDLK_SPACE )
			{
				if( game_over )
				{
					for(unsigned int i=0; i<m_player_manager->playerCount(); ++i)
						m_player_manager->getPlayer(i)->setScore(0);
					changeLevel(1);
				}
				else if( !m_play )
					m_play = true;
				m_center_text_box->SetTextChar("",0);
			}
			else
				if( !m_player_manager->handleKeyDown(event.key.keysym.sym) )
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

			m_player_manager->move();

			m_engine->Tree()->Walk();

			// Detect collisions
			// FIXME: This should go into the player manager class
			for(unsigned int i=0; i<m_player_manager->playerCount(); ++i)
			{

				if(!m_player_manager->getPlayer(i)->isCarrying() && keysHeld[SDLK_UP] ) {
					vector<KrImage*> collides;
					if( m_engine->Tree()->CheckChildCollision(m_player_manager->getPlayer(i),m_bgTree,&collides) )
					{  
						printf("Collision!\n");
						// Use of dynamic cast as we only want to try to pick
						// up BrainSprites and not ordinary KrSprites as in the
						// Bricks. 
						// Otherwise we might be able to pick up the solution in the beginning :)
						BrainSprite* c = dynamic_cast<BrainSprite*>(*collides.begin());
						if( c ) 
							m_player_manager->getPlayer(i)->pickUp(reparentSprite(c,m_player_manager->getPlayer(i)));
					}
				}
			}

			if( m_play || (difftime(now,m_start_time) > WAITTIME) )
			{
				for(unsigned int i=0; i<m_current_levels.size(); ++i)
					m_current_levels[i]->setVisibleSolution(false);
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
		else if( keysHeld[SDLK_F1] )
			addSprite();
		else
			m_player_manager->handleKeyHeld(keysHeld);

		game_over = writeScoreAndTime(now);

		if( m_play && game_over )
		{
			m_center_text_box->SetTextChar("Game Over",0);
			clearFloor();
		}
		
	}
    
    return 0;
}

bool Brainblast::writeScoreAndTime(time_t& now)
{
	// Time left
	int basetime = m_play ? 60 : static_cast<int>(WAITTIME);
	int sec = static_cast<int>(basetime - difftime(now,m_start_time));
	bool game_over = sec <= 0;
	if( m_play && game_over ) sec = 0;
	int min = sec/60;
	sec -= min*60;
	
	ostringstream score_str;

	for(unsigned int i=0; i<m_player_manager->playerCount(); ++i)
	{

		score_str << "SCORE: " << m_player_manager->getPlayer(i)->getScore() << " TIME: " 
				  << setw(2) << setfill('0') << min << ":" 
				  << setw(2) << setfill('0') << sec 
				  << " BRICKS: " << m_current_levels[i]->correctBricks()
				  << "/" << m_current_levels[i]->totalSolutionBricks();
		
		m_player_manager->getPlayer(i)->getScoreBox()->SetTextChar(score_str.str(),0);

		score_str.str("");
	}
	
	return game_over;
}

void Brainblast::select(Puzzle& lvl, BrainPlayer& player)
{
	BrainSprite* s = 0;
	int cscore = lvl.brickScore();
	lvl.select(&s) ? player.addScore(cscore) : player.addScore(-1*cscore/10);
	if( s )
	{
		m_sound->playSample(CLICK);
		vector<BrainSprite*>::iterator it = find(m_sprites.begin(),m_sprites.end(),s);
		m_sprites.erase(it);

		if( checkSolution(&lvl) )
		{
			if( !changeLevel(m_current_lvl+1) )
				changeLevel(0);
		}
	}	
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

void Brainblast::dropPlayerSprite(BrainPlayer* player, bool remove)
{
	BrainSprite* bs = player->drop(m_bgTree);
	
	if( !bs )
		return;
	
	if( remove ) 
	{
		m_engine->Tree()->DeleteNode(bs);
	}
	else
		m_sprites.push_back(bs);
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
