#include "layout.h"
#include "xmlutil.h"
#include "../gui/button.h"
#include "../gui/textwidget.h"


Layout::Layout( SDL_Surface* surface,
				KrFontResource* font )
{
	KrRect rect[NUM_VIEWS];

	SDL_EnableUNICODE( true );
	SDL_EnableKeyRepeat( SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL );

	rect[MAIN_VIEW].Set(	UI_WIDTH, 0, surface->w - 1, surface->h - 1 );
	rect[UI_VIEW].Set(		0, 0, UI_WIDTH-1, surface->h - UI_WIDTH - 1 );
	rect[MINIMAP_VIEW].Set( 0, surface->h - UI_WIDTH, UI_WIDTH-1, surface->h - 1 );

	engine = new KrEngine(	surface,
							NUM_VIEWS,			// number of windows
							rect,
							0 );

	map = new KrImNode();
	engine->Tree()->AddNode( 0, map );
	map->SetVisible( false, UI_VIEW );


	KrRGBA red, green, blue;
	red.Set( 100, 0, 0 );
	green.Set( 0, 100, 0 );
	blue.Set( 0, 0, 100 );

	engine->FillBackgroundWindow( MAIN_VIEW, &red );
	engine->FillBackgroundWindow( UI_VIEW, &green );
	engine->FillBackgroundWindow( MINIMAP_VIEW, &blue );

	//focusManager = new KrFocusManager( engine );

	//-------- UI ------------------
	KrScheme scheme( font );

	upButton = new KrPushButton( BUTTON_W, BUTTON_H, scheme );
	engine->Tree()->AddNode( 0, upButton );
	upButton->SetVisible( false );
	upButton->SetVisible( true, UI_VIEW );
	upButton->SetPos( UI_WIDTH - BUTTON_W - UI_BORDER, UI_BORDER );
	upButton->SetAccelerator( KMOD_NUM, 280 );
	upButton->SetTextChar( "<up>" );
	//focusManager->AddMouseEventListener( true, upButton->NodeId(), UI_VIEW );

	downButton = new KrPushButton( BUTTON_W, BUTTON_H, scheme );
	engine->Tree()->AddNode( 0, downButton );
	downButton->SetVisible( false );
	downButton->SetVisible( true, UI_VIEW );
	downButton->SetPos( UI_WIDTH - BUTTON_W - UI_BORDER, surface->h - UI_WIDTH - UI_BORDER - BUTTON_H );
	downButton->SetAccelerator( KMOD_NUM, 281 );
	downButton->SetTextChar( "<down>" );

	// How many options will fit?
	int availableSpace = ( surface->h - UI_WIDTH - UI_BORDER*2 - BUTTON_H*2 );
	nOptionButtons =  availableSpace / BUTTON_H;
	int spacing = ( availableSpace - nOptionButtons * BUTTON_H ) / nOptionButtons;

	for( int i=0; i<nOptionButtons; ++i )
	{
		optionButton[i] = new KrToggleButton( BUTTON_W, BUTTON_H, scheme );
		engine->Tree()->AddNode( 0, optionButton[i] );
		optionButton[i]->SetVisible( false );
		optionButton[i]->SetVisible( true, UI_VIEW );
		optionButton[i]->SetPos( UI_WIDTH - BUTTON_W - UI_BORDER,
								 UI_BORDER + BUTTON_H + BUTTON_H*i + spacing*i + spacing / 2 );
	}

//	canvasRes = new KrCanvasResource( "preview", PreviewSize(), PreviewSize(), false );
//	canvas = new KrCanvas( canvasRes );
//	engine->Tree()->AddNode( 0, canvas );
//	canvas->SetVisible( false );
//	canvas->SetVisible( true, UI_VIEW );
//	canvas->SetPos( UI_BORDER, UI_BORDER );

	infoBox = new KrTextBox( font, PreviewSize(), PreviewSize(), 0 );
	engine->Tree()->AddNode( 0, infoBox );
	infoBox->SetVisible( false );
	infoBox->SetVisible( true, UI_VIEW );
	infoBox->SetPos( UI_BORDER, UI_BORDER*2 + PreviewSize() );
	infoBox->SetTextChar( "info box", 0 );

	saveButton = new KrPushButton( BUTTON_W, BUTTON_H, scheme );
	engine->Tree()->AddNode( 0, saveButton );
	saveButton->SetVisible( false );
	saveButton->SetVisible( true, UI_VIEW );
	saveButton->SetPos( UI_BORDER, UI_BORDER*3 + PreviewSize()*2 );
	saveButton->SetTextChar( "Save" );

	rotation = 0;
}


void Layout::SetLayer( Layer* layer, int offset )
{
	currentLayer = layer;

	int count = GlMin( nOptionButtons, 
	                   (int) layer->images.Count() - offset );
	for( int i=0; i<count; ++i )
	{
		optionButton[i]->SetTextChar( layer->imageDesc[i+offset] );
	}
	for( ; i<nOptionButtons; ++i )
	{
		optionButton[i]->SetTextChar( "" );
	}
}


void Layout::DisplayMapCoords( float x, float y )
{
	char buf[ 256 ];
	sprintf( buf, "Map: %.1f, %.1f", x, y );
	infoBox->SetTextChar( buf, 0 );
}


void Layout::SetRotation( int r )
{
	char buf[ 256 ];
	sprintf( buf, "Rotation: %d", r );
	infoBox->SetTextChar( buf, 1 );
	rotation = r;
}


