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
#include "BrainPlayerManager.h"

#include <assert.h>
#include <sstream>   // ostringstream
#include <iomanip>   // setfill setw
#include <algorithm> // for_each

using namespace grinliz;
using namespace brain;
using namespace std;

// Statics
Brainblast* Brainblast::s_instance;
int TextListener::m_text_id = 0;
const double Brainblast::WAITTIME = 10.0;

Brainblast::Brainblast() : m_gamestate(TITLE),
						   m_start_time(0),
						   m_sound(new BrainSoundFMOD),
						   m_level_data(7,8),  // 7x8 is maximum size if you want to avoid overlappings
						   m_current_levels(),
						   m_fields(),
						   m_current_lvl(1),
                           m_screen( SDL_SetVideoMode( VIDEOX, VIDEOY, VIDEOBITS, SDL_HWSURFACE ) ),
						   m_bricks(),
						   m_engine(0),
						   m_bgTree(0),
						   m_fgTree(0),
						   m_sprites(0),
						   m_bg_vault( new KrResourceVault() ),
						   m_bg_sprite(0),
						   m_font(0),
						   m_score_font(0),
						   m_title_font(0),
						   m_left_score_text_box(0),
						   m_right_score_text_box(0),
						   m_center_text_box(0),
						   m_top_center_text_box(0),
						   m_high_score_text_box(0),
						   m_player_manager(0),
						   m_human_players(1),
						   m_computer_players(0),
						   m_level_set(NORMAL),
						   m_text_listeners(),
						   m_text_queue(),
						   m_time_bonus_timer(0),
						   m_time_bonus_event()

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
	zap( m_title_font );
}

void
Brainblast::makeRandomLevel(LevelData& lvl)
{
	if(bbc::debug) cerr << "Brainblast::makeRandomLevel" << lvl << "\n"; 

	for(unsigned int i=0; i<m_player_manager->playerCount(); ++i)
		m_current_levels.push_back(new Puzzle(lvl.w(),lvl.h(),m_fields[i]));

	// Vector to make sure we use up the indexes
	// I guess a nicer solution would be to randomize
	// and iterate over a vector/array instead but...
	vector<int> indexes;
	for(unsigned int i=0; i<lvl.size(); ++i)
		indexes.push_back(i);

	for(unsigned int i=0; i<lvl.n(); ++i)
	{
		int type = bbc::randint(0,lvl.types()-1);
		map<int,Brick*>::iterator it = m_bricks.begin();
		for(int j=0;j<type;++j)
			++it;
		assert(it!=m_bricks.end());

		int idxidx = bbc::randint(0,indexes.size()-1);
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

void
Brainblast::deleteLevels()
{
	for(unsigned int i=0; i<m_current_levels.size(); ++i)
		zap(m_current_levels[i] );
	m_current_levels.clear();
	for(unsigned int i=0; i < m_player_manager->playerCount(); ++i)
		m_player_manager->getPlayer(i)->setLevel(0);
}

bool
Brainblast::makeLevel(int lvl)
{
    if(bbc::debug) cerr << "Brainblast::makeLevel(" << lvl << ")\n";
	
	deleteLevels();

    if( m_level_set == RANDOM ) 
    {
		if( (m_current_lvl > 1) && !m_level_data.increaseDifficulty() )
			return false;

		makeRandomLevel(m_level_data);
        return true;
    }

    else 
    {

        char* filename  = static_cast<char*>(malloc(128));
      
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
						cerr << "=== WARNING: Level file '" << filename << "' contain invalid brick type (" << val << ") will use another one ===\n";
						it = m_bricks.begin();
					}
					if( tmp > height*width )
					{
						cerr << "=== ERROR: Level file contain invalid brick position (" << tmp << ") ===\n";
						free(filename);
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
			if( sr->ResourceId() == BB_WIZARD ||
				sr->ResourceId() == BB_WIZARD2 )
				continue;

			KrSprite* b = new KrSprite(sr);
			m_bricks.insert(pair<int,Brick*>(sr->ResourceId(),new Brick(b, sr->ResourceId())));
			assert(m_bricks.size()<MAX_NOF_BRICK_TYPES);
		}
	}

	m_level_data.setMaxTypes(m_bricks.size());
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

	m_gamestate = PLAY_WAIT;

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
	m_sound->loadMusic("/usr/share/games/brainblast/music/enigmatic_path.it");
	m_sound->playMusic();

	// Leave title screen
	m_gamestate = PLAY_WAIT;
	clearTextBox(m_center_text_box);

	if( !m_player_manager->addPlayers(m_human_players,m_computer_players) )
	{
		printf("=== ERROR: Could not create players. ===\n");
		return false;
	}

	// FIXME: Currently only two player support
	m_player_manager->getPlayer(0)->setScoreBox(m_left_score_text_box);
	if( m_player_manager->playerCount() > 1 )
		m_player_manager->getPlayer(1)->setScoreBox(m_right_score_text_box);

	if( !setupFields(m_player_manager->playerCount()) )
	{
		printf("=== ERROR: Could not setup fields. ===\n");
		return false;
	}

	for(unsigned int i=0; i<m_player_manager->playerCount(); ++i)
		m_player_manager->getPlayer(i)->setLevelSet(m_level_set);

	if( !changeLevel(1) )
		return false; 

	return true;
}

KrFontResource* Brainblast::loadFont(const char* file, int glyphs)
{
	// Note: Make sure the image file is using indexed colors before 
	//       using them in this function.

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
		printf( "=== Error: Loading font (%s) === \n", file );
		SDL_FreeSurface(raw);
		return 0;
	}

	if(SDL_BlitSurface( raw, 0, s32, 0 ))
	{
		printf( "=== Error: Blit error - loading font (%s) === \n", file );
		SDL_FreeSurface(s32);
		SDL_FreeSurface(raw);
		return 0;
	}
		
	// FIXME: We might need different names when using many fonts ?
	KrPaintInfo pi(s32);
	KrFontResource* fr = new KrFontResource("LOADED_FONT", &pi, 32, 0, KrFontResource::FIXED, glyphs);

	SDL_FreeSurface(s32);
	SDL_FreeSurface(raw);

	return fr;
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
	m_title_font = loadFont("/usr/share/games/brainblast/images/goldfont.png",100);

	if( !m_font || !m_score_font || !m_title_font ) 
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

 	m_center_text_box = new KrTextBox(m_title_font,600,600,8);
	m_center_text_box->SetPos(300,280);
	m_engine->Tree()->AddNode(0,m_center_text_box);

 	m_top_center_text_box = new KrTextBox(m_title_font,200,50,1);
	m_top_center_text_box->SetPos(350,10);
	m_engine->Tree()->AddNode(0,m_top_center_text_box);

 	m_high_score_text_box = new KrTextBox(m_title_font,600,600,10);
	m_high_score_text_box->SetPos((VIDEOX-600)/2,(VIDEOY-600)/2);
	m_engine->Tree()->AddNode(0,m_high_score_text_box);

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

	m_player_manager = new BrainPlayerManager();
	m_text_listeners.push_back(m_player_manager);
	
    createBricks();

	// Start music
	if( !(m_sound->initializeSound() &&
		  m_sound->addSample("/usr/share/games/brainblast/sounds/click.wav",CLICK) &&
		  m_sound->addSample("/usr/share/games/brainblast/sounds/bounce.wav",BOUNCE) && 
		  m_sound->addSample("/usr/share/games/brainblast/sounds/warning.wav",WARNING)) )
		printf("=== ERROR: Sound/Music error === \n");
	
	titleScreen();

	return true;
}

void Brainblast::titleScreen()
{
	m_sound->loadMusic("/usr/share/games/brainblast/music/Acidstorm.it");
	m_sound->playMusic();

	m_gamestate = TITLE;

	// Stop all play
	for(unsigned int i=0; i<m_current_levels.size(); ++i)
		delete m_current_levels[i];
	m_current_levels.clear();
	m_player_manager->removePlayers();

	m_left_score_text_box->SetTextChar("MUSIC BY SAGA MUSIX, HTTP://SAGAMUSIX.DE/",0);
	m_right_score_text_box->SetTextChar("              CODE: DANIEL BENGTSSON",0);

	titleScreenUpdateText();
}

void Brainblast::titleScreenUpdateText()
{
	clearTextBox(m_center_text_box);
	clearTextBox(m_high_score_text_box);

	m_center_text_box->SetTextChar("BRAINBLAST 0.2",0);
	m_center_text_box->SetTextChar("",1);

	ostringstream str;

	str << "F1: Human Players - " << m_human_players;
	m_center_text_box->SetTextChar(str.str(),2);

	str.str(""); 
	str << "F2: Computer Players - " << m_computer_players;
	m_center_text_box->SetTextChar(str.str(),3);

	str.str("");
	string set = levelSetToString(m_level_set);
	str << "F3: Level set - " << set;
	m_center_text_box->SetTextChar(str.str(),4);

	str.str("");
	string difficulty = m_player_manager->difficultyString();
	str << "F4: Difficulty - " << difficulty;
	m_center_text_box->SetTextChar(str.str(),5);

	m_center_text_box->SetTextChar("F5: Highscores",6);
	m_center_text_box->SetTextChar("",7);
	m_center_text_box->SetTextChar("SPACE: Start game",8);
	m_center_text_box->SetTextChar("",9);

	m_top_center_text_box->SetTextChar("",0);
}

BrainSprite* Brainblast::addSprite()
{
	if( m_gamestate != PLAYING && !(m_gamestate == TITLE || m_gamestate == HIGH_SCORE) )
		return 0;

	KrSpriteResource* spriteRes = 0;

	vector<int> types;
	if( m_current_levels.empty() )
	{
		map<int,Brick*>::iterator it;
		map<int,Brick*>::iterator end = m_bricks.end();
		for( it = m_bricks.begin(); it!=end; ++it)
			types.push_back(it->first);
	}
	else
	{
		// TODO:  Can we have different types on the other level ?
		// FIXME: This should be fixed.
		types = m_current_levels[0]->getSolutionTypes();
	}

	int r = bbc::randint(0,types.size()-1);
	spriteRes = m_engine->Vault()->GetSpriteResource( types[r] );
	assert( spriteRes );

	BrainSprite* sprite = new BrainSprite( spriteRes, "rand", true );
	sprite->SetNodeId(types[r]);
	sprite->SetPos( rand()%VIDEOX, 0);
	sprite->setSpeed(double(bbc::randint(-10,10)),0);
	m_engine->Tree()->AddNode( m_bgTree, sprite );
	m_sprites.push_back(sprite);
	return sprite;
}

#define SDL_DRAW_EVENT ( SDL_USEREVENT + 0 )
#define SDL_ADD_SPRITE_EVENT ( SDL_USEREVENT + 1 )
#define SDL_TIME_BONUS_EVENT ( SDL_USEREVENT + 2 )
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
	SDL_AddTimer( 35, TimerCallback, &draw_event );	
	SDL_Event add_sprite_event; add_sprite_event.type = SDL_ADD_SPRITE_EVENT;
	SDL_AddTimer( 2000, TimerCallback, &add_sprite_event );	

	bool keysHeld[323] = {false};

    while( !done && SDL_WaitEvent(&event) )
	{
		if ( event.type == SDL_QUIT )
			break;
        
		switch(event.type)
		{
        case SDL_KEYDOWN:

			if( bbc::debug )
				printf( "%s\n", SDL_GetKeyName(event.key.keysym.sym));

			if( !m_text_queue.empty() )
			{
				textInput(event.key.keysym.sym);
			}

			// M = TOGGLE SOUND
			else if( event.key.keysym.sym == SDLK_m )
				m_sound->toggleMusic();
			// F = TOGGLE FULLSCREEN
			else if( event.key.keysym.sym == SDLK_f )
				SDL_WM_ToggleFullScreen(m_screen);
			else if( event.key.keysym.sym == SDLK_SPACE || 
					 event.key.keysym.sym == SDLK_RETURN )
			{
				switch(m_gamestate)
				{
				case TITLE:
					m_player_manager->resetScores();
					if( !startGame() )
					{
						printf("=== ERROR: Could not start game ===\n");
						done = true; // Just abort if we could not start the game
					}
					break;
				case GAME_OVER: 
				case HIGH_SCORE:
					titleScreen(); break;
				case PLAY_WAIT:
					finishInitialWait(); break;
				case TIME_BONUS:
					speedyTimeBonus(); break;
				case PLAYING:
					break;
				}
			}
			else if( event.key.keysym.sym == SDLK_ESCAPE )
			{
				switch(m_gamestate)
				{
				case TITLE:
					done = true; break;
				case PLAY_WAIT:
				case PLAYING:
				{
					// If no one had any points, go directly to title screen
					m_player_manager->allScoresNull() ? titleScreen() : gameOver();
					clearFloor();
				}
				break;
				case GAME_OVER: 
				case HIGH_SCORE:
					titleScreen();
					break;
				case TIME_BONUS:
					break;
				}
			}
			else if( m_gamestate==TITLE && event.key.keysym.sym == SDLK_F1 )
			{
				m_sound->playSample(CLICK);
				m_human_players++;
				if( m_human_players > 2 )
					m_human_players = 1;
				if( (m_human_players + m_computer_players) > 2 )
					m_computer_players--;
				titleScreenUpdateText();
			}
			else if( m_gamestate==TITLE && event.key.keysym.sym == SDLK_F2 )
			{
				m_sound->playSample(CLICK);
				m_computer_players++;
				if( m_computer_players > 2 )
					m_computer_players = 1;
				if( (m_human_players + m_computer_players) > 2 )
					m_human_players--;
				titleScreenUpdateText();
			}
			else if( m_gamestate==TITLE && event.key.keysym.sym == SDLK_F3 )
			{
				m_sound->playSample(CLICK);
				switch( m_level_set )
				{
				case NORMAL:
					m_level_set = RANDOM; break;
				case RANDOM:
					m_level_set = NORMAL; break;
				default:
					m_level_set = NORMAL; break;
				}
				titleScreenUpdateText();
			}
			else if( m_gamestate==TITLE && event.key.keysym.sym == SDLK_F4 )
			{
				m_player_manager->toggleDifficulty();
				titleScreenUpdateText();
			}
			else if( m_gamestate==TITLE && event.key.keysym.sym == SDLK_F5 )
			{
				showHighScore();
			}
			else
				if( !m_player_manager->handleKeyDown(event.key.keysym.sym) )
					keysHeld[event.key.keysym.sym] = true;

			break;
			
		case SDL_KEYUP:
			keysHeld[event.key.keysym.sym] = false;
			break;

		case SDL_ADD_SPRITE_EVENT:
			if( m_gamestate != GAME_OVER )
				addSprite();
			break;
			
		case SDL_TIME_BONUS_EVENT:
		{
			int* current = static_cast<int*>(event.user.data2);

			assert(*current < 1000);
			if( *current <= 0 )
			{
				// Delete event and stop timer
				SDL_RemoveTimer(m_time_bonus_timer);
				m_time_bonus_timer = 0;

				delete current;

				if( !changeLevel(m_current_lvl+1) )
				{
					gameOver();
				}

				m_center_text_box->SetTextChar("",1);
			}
			else
			{
				ostringstream str;
				str << "Time Bonus: " << *current;
				(*current)--;
				m_center_text_box->SetTextChar(str.str(),1);
				m_sound->playSample(BOUNCE);
				BrainPlayer* player = static_cast<BrainPlayer*>(event.user.data1);
				if( player )
					player->addScore(player->getLevel()->brickScore()/10);
			}

		}
		break;

        case SDL_DRAW_EVENT:
        {
			vector<BrainSprite*>::iterator it  = m_sprites.begin();
			vector<BrainSprite*>::iterator end = m_sprites.end();
			while(it!=end)
			{
				// First check if we should delete this sprite after a timeout
				if( (*it)->temporary() && (difftime(time(0),(*it)->creationTime()) > 30) )
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

			if( m_start_time && m_gamestate==PLAY_WAIT && (difftime(time(0),m_start_time) > WAITTIME) )
				finishInitialWait();
			
			m_engine->Draw();

        }
        break;
	
        default:
            break;
		}

		// The following section checks for keys that are held down
		// and should continuosly do something.

		if( keysHeld[SDLK_F1] )
			addSprite();
		else
			m_player_manager->handleKeyHeld(keysHeld);

		if( m_gamestate != TITLE )
		{
			bool was_game_over = m_gamestate == GAME_OVER;
			writeScoreAndTime();
			
			if( m_gamestate==GAME_OVER && !was_game_over )
				gameOver();
		}
		
	}
    
    return 0;
}

void Brainblast::speedyTimeBonus()
{
	// Did not see a better way to change the timer than to delete it and
	// create a new one.
	SDL_RemoveTimer(m_time_bonus_timer);
	m_time_bonus_timer = SDL_AddTimer( 1, TimerCallback, &m_time_bonus_event );	
}
	
void Brainblast::gameOver()
{
	m_gamestate = GAME_OVER;
	m_center_text_box->SetTextChar("Game Over",0);
	m_player_manager->gameOver();
	clearFloor();
}

void Brainblast::clearTextBox( KrTextBox* tb )
{
	if( !tb )
		return;

	int lines = tb->NumLines();

	for(int i=0; i<lines; ++i)
		tb->SetTextChar("",i);
}

void Brainblast::showHighScore()
{
	m_gamestate = HIGH_SCORE;

	clearTextBox(m_center_text_box);

	unsigned int max = m_player_manager->hs_max_entries();

	vector<HighScore::Entry> entries = m_player_manager->getHighScoreEntries();
	
	int len = (entries.size() > max) ? max : entries.size();

	ostringstream str;

	m_high_score_text_box->SetTextChar("Highscores",0);

	str << setw(12) << "Name" << setw(8) << "Score" << setw(6) << "Level" << setw(6) << "Mode";
	m_high_score_text_box->SetTextChar(str.str(),1);
	
	str.str("");
	for(int i=0; i<len; ++i)
	{
		str << setw(2)  << setfill('0') << i+1 << setfill(' ') 
			<< setw(10) << entries[i].name
			<< setw(8)  << entries[i].score
			<< setw(6)  << entries[i].level << " "
			<< setw(6)  << levelSetToString(entries[i].level_set);
			
		m_high_score_text_box->SetTextChar(str.str(),i+2);
		str.str("");
	}
	for(int i=len; i<=8; i++)
		m_high_score_text_box->SetTextChar("",i+2);
}

void Brainblast::nextTextInput()
{
	if( m_text_queue.empty() )
	{
		showHighScore();
		return;
	}
	
	m_center_text_box->SetTextChar(m_text_queue.begin()->second,3);	
	m_center_text_box->SetTextChar("",4);
}

int Brainblast::startTextInput(string label)
{
	int id = TextListener::id();
	m_text_queue[id] = label;

	// If we filled an empty queue
	// we clear and start a new text
	// If not it will just lie in the queue
	// until the next string is about to 
	// be entered.
	if( m_text_queue.size() == 1 ) 
		nextTextInput();

	return id;
}

void Brainblast::textInput(SDLKey k)
{
	if( k == SDLK_RETURN )
	{
		// We are always working on the lowest key
		// and maps are sorted by key.
		int id = m_text_queue.begin()->first;
		m_text_queue.erase(id);
		
		string s;
		m_center_text_box->GetTextChar(&s,4);
		if( s.size() )
		{
			for_each(m_text_listeners.begin(),m_text_listeners.end(),text_ready(s,id));
			clearTextBox(m_center_text_box);
			nextTextInput();
		}
		else
		{
			playSample(WARNING);
		}
		return;
	}

	// For now only letters and numbers
	else if( (k >= SDLK_0 && k <= SDLK_9) ||
			 (k >= SDLK_a && k <= SDLK_z) )
	{
		string s;
		m_center_text_box->GetTextChar(&s,4);
		
		if( s.size() == 8 ) 
		{
			playSample(WARNING);
			return;
		}

		s += k;
		m_center_text_box->SetTextChar(s,4);
		m_sound->playSample(CLICK);
	}

	else if( k == SDLK_BACKSPACE )
	{
		string s;
		m_center_text_box->GetTextChar(&s,4);
		if( s.size() )
		{
			s.resize(s.size()-1);
			m_center_text_box->SetTextChar(s,4);
			m_sound->playSample(CLICK);
		}
		else
			playSample(WARNING);
	}
}

void Brainblast::finishInitialWait()
{
	if( m_gamestate == GAME_OVER ||
		m_gamestate == TITLE )
		return;

	for(unsigned int i=0; i<m_current_levels.size(); ++i)
		m_current_levels[i]->setVisibleSolution(false);

	m_gamestate = PLAYING;
	m_center_text_box->SetTextChar("",0);
}

BrainSprite* Brainblast::collisionCheck(BrainPlayer* player)
{
	vector<KrImage*> collides;
	if( m_engine->Tree()->CheckChildCollision(player,m_bgTree,&collides) )
	{  
		// Use of dynamic cast as we only want to try to pick
		// up BrainSprites and not ordinary KrSprites as in the
		// Bricks. 
		// Otherwise we might be able to pick up the solution in the beginning :)
		return dynamic_cast<BrainSprite*>(*collides.begin());
	}
	return 0;
}

unsigned int Brainblast::secondsLeft() const
{
	time_t now = time(0);
	int basetime = m_gamestate==PLAYING ? 60 : static_cast<int>(WAITTIME);
	int sec = static_cast<int>(basetime - difftime(now,m_start_time));
	assert(sec <= basetime);
	bool game_over = sec <= 0;
	return (m_gamestate==PLAYING && game_over) ? 0 : sec;
}

void Brainblast::writeScoreAndTime()
{
	int sec = secondsLeft();
	int min = sec/60;
	sec -= min*60;
	
	ostringstream score_str;

	if( m_gamestate == PLAYING ||
		m_gamestate == PLAY_WAIT )
	{
		score_str << "       "
				  << setw(2) << setfill('0') << min << ":" 
				  << setw(2) << setfill('0') << sec;
		
		m_top_center_text_box->SetTextChar(score_str.str(),0);
		score_str.str("");
	}

	for(unsigned int i=0; i<m_player_manager->playerCount(); ++i)
	{
		// When we are done with the last level 
		// we will have no level to look at.
		if( !(m_current_levels.size()>i && m_current_levels[i]) )
			continue;
		
		score_str << "SCORE: " << m_player_manager->getPlayer(i)->getScore()
				  << " BRICKS: " << m_current_levels[i]->correctBricks()
				  << "/" << m_current_levels[i]->totalSolutionBricks();
		
		m_player_manager->getPlayer(i)->getScoreBox()->SetTextChar(score_str.str(),0);

		score_str.str("");
	}
	
	if( (m_gamestate == PLAYING) && !sec && !min )
		m_gamestate = GAME_OVER;
}

void Brainblast::select(Puzzle& lvl, BrainPlayer& player)
{
	if( m_gamestate == TIME_BONUS )
	{
		speedyTimeBonus();
		return;
	}
	else if( m_gamestate == PLAY_WAIT )
	{
		finishInitialWait();
		return;
	}
	else if( !lvl.isSelecting() )
		return;

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
			clearTextBox(m_center_text_box);
			
			ostringstream str;
			str << "Player " << m_player_manager->getPlayerNumber(player)
				<< " wins level " << m_current_lvl;
			m_center_text_box->SetTextChar(str.str(),0);

			// Apply time bonus
			int* seconds = new int(secondsLeft());
			player.addScore(*seconds*cscore/10);
			if(bbc::debug) cerr << "Brainblast::select() TimeBonus (" << *seconds << "*" 
								<< cscore/10 << "): " << *seconds*cscore/10 << "\n";

			m_time_bonus_event.type       = SDL_TIME_BONUS_EVENT;
			m_time_bonus_event.user.data1 = static_cast<void*>(m_player_manager->getPlayer(m_player_manager->getPlayerNumber(player)-1)); // FIXME
			m_time_bonus_event.user.data2 = static_cast<void*>(seconds);
			m_time_bonus_timer = SDL_AddTimer( 100, TimerCallback, &m_time_bonus_event );	
			m_gamestate = TIME_BONUS;
			clearFloor();

			// Level is changed when counting the bonus is finished

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

Brainblast::LevelData::LevelData(unsigned int max_width,
								 unsigned int max_height)
	: m_width(0),
	  m_height(0),
	  m_bricks(0),
	  m_types(0),
	  m_max_types(0),
	  m_max_width(max_width),
	  m_max_height(max_height)
{ 
	reset(); 
}

bool Brainblast::LevelData::increaseDifficulty()
{
	assert(m_max_types);
	if( !m_max_types )
		return false;

	// 1. Types 2. Bricks 3. Size

	if( (m_types < m_max_types) &&
		(m_types < m_bricks) &&
		(m_types < m_width*m_height) )
	{
		m_types++;
	}

	else if( m_bricks < m_width*m_height )
	{
		m_types = 1;
		m_bricks++;
	}

	else if( m_width < m_height )
	{
		if( m_width == m_max_width )
			return false;

		m_types = 1;
		m_bricks = 1;
		m_width++;
	}

	else
	{
		if( m_height == m_max_height )
			return false;

		m_types = 1;
		m_bricks = 1;
		m_height++;		
	}

	return true;
}

void Brainblast::LevelData::reset()
{
	m_width	 = 2;
	m_height = 2;
	m_bricks = 1;
	m_types	 = 1;
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
