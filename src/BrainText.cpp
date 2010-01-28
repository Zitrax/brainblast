/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

#include "BrainText.h"
#include "consolefont.h"
#include "SDL_image.h"
#include "bbc.h"
#include "Brainblast.h"

BrainText::BrainText() :
    m_font(0),
    m_score_font(0),
    m_title_font(0),
    m_left_score_text_box(0),
    m_right_score_text_box(0),
    m_center_text_box(0),
    m_top_center_text_box(0),
    m_high_score_text_box(0)
{
}

bool BrainText::init(KrEngine& engine, int width, int height)
{
	// Set up the font
 	m_font = KrEncoder::CreateFixedFontResource( "CONSOLE", CONSOLEFONT_DAT, CONSOLEFONT_SIZE );
	
	Brainblast* bb = Brainblast::instance();

	AutoCStr f1(bb->addBaseDir("images/bubblemad_8x8.png"));
	AutoCStr f2(bb->addBaseDir("images/goldfont.png"));

	m_score_font = loadFont(f1,83);
	m_title_font = loadFont(f2,100);
    
	if( !m_font || !m_score_font || !m_title_font ) 
	{
		printf( "=== Error: Loading font ===\n" );
		return false;
	}

 	m_left_score_text_box = new KrTextBox(m_score_font,400,50,1);
	m_left_score_text_box->SetPos(10,10);
	engine.Tree()->AddNode(0,m_left_score_text_box);

 	m_right_score_text_box = new KrTextBox(m_score_font,400,50,1);
	m_right_score_text_box->SetPos(width-310,10);
	engine.Tree()->AddNode(0,m_right_score_text_box);

 	m_center_text_box = new KrTextBox(m_title_font,600,600,8);
	m_center_text_box->SetPos(300,280);
	engine.Tree()->AddNode(0,m_center_text_box);

 	m_top_center_text_box = new KrTextBox(m_title_font,200,50,1);
	m_top_center_text_box->SetPos(350,10);
	engine.Tree()->AddNode(0,m_top_center_text_box);

 	m_high_score_text_box = new KrTextBox(m_title_font,600,600,10);
	m_high_score_text_box->SetPos((width-600)/2,(height-600)/2);
	engine.Tree()->AddNode(0,m_high_score_text_box);

	return true;
}

KrFontResource* BrainText::loadFont(const char* file, int glyphs)
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

BrainText::~BrainText()
{
	zap( m_font );
	zap( m_score_font );
	zap( m_title_font );
}

void BrainText::write(enum Box box, const string& text, int line)
{
	assert( box != UNKNOWN );

    getBox(box)->SetTextChar(text,line);
}

void BrainText::read(enum Box box, string* text, int line)
{
	assert( box != UNKNOWN );
	assert( text );
	assert( getBox(box) );

    getBox(box)->GetTextChar(text,line);
}

void BrainText::clear(enum Box box)
{
	assert( box != UNKNOWN );
	
    KrTextBox* tb = getBox(box);
    int lines = tb->NumLines();
	for(int i=0; i<lines; ++i)
		tb->SetTextChar("",i);
}

void BrainText::clearAll()
{
    clear(TOP_LEFT);
    clear(TOP_RIGHT);
    clear(TOP_CENTER);
    clear(CENTER);
    clear(HIGH_SCORE);
}

KrTextBox* BrainText::getBox(enum Box box) const
{
	assert( box != UNKNOWN );

    switch(box)
    {
    case TOP_LEFT   : return m_left_score_text_box;
    case TOP_RIGHT  : return m_right_score_text_box;
    case TOP_CENTER : return m_top_center_text_box;
    case CENTER     : return m_center_text_box;
    case HIGH_SCORE : return m_high_score_text_box;
	case UNKNOWN    : return 0;
    }

    assert(!"Should not get here");
}
