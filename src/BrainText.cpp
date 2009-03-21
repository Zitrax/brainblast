/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; c-file-style:"stroustrup" -*-
 *
 * @date 2006-2009
 * @author Daniel Bengtsson
 */

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

bool BrainText::init()
{
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
}

BrainText::~BrainText()
{
	zap( m_font );
	zap( m_score_font );
	zap( m_title_font );
}

void BrainText::write(enum Box box, char* text, int line)
{
}

void BrainText::clear(enum Box box)
{
}

void BrainText::clearAll()
{
}
