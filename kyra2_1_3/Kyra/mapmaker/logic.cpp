#include "logic.h"
#include "layout.h"
#include "xmlutil.h"

UILogic::UILogic(	Layout* _layout, 
					XmlUtil* _util )
{
	layout = _layout;
	util   = _util;
//	layer = 0;

//	layout->LayerBox()->AddListener( this );
	for( int i=0; i<layout->NumOptionButtons(); ++i )
	{
		layout->OptionButtons( i )->AddListener( this );
	}
	currentImage = 0;
	buttonOffset = 0;

	layout->Up()->AddListener( this );
	layout->Down()->AddListener( this );
	layout->SaveButton()->AddListener( this );
}


void UILogic::SetRotation( int rot )
{
	KrImNode* preview = layout->Engine()->Tree()->FindNodeByName( "preview" );
	if ( preview && preview->ToTile() )
	{
		preview->ToTile()->SetRotation( rot );
	}
}


void UILogic::SetPreview( KrImage* image )
{
	Layer* layer = layout->GetLayer();

	int size = layout->PreviewSize();

	if ( image->ToSprite() )
	{
		KrSprite* s = image->ToSprite();
		KrSpriteResource* res = s->SpriteResource();

		KrRect bounds;
		s->GetBoundingBox( &bounds, 0 );

		GlFixed scale = GlMin( GlFixed( size ) / GlFixed( bounds.Width() ),
							   GlFixed( size ) / GlFixed( bounds.Height() ) );
		s->SetPos( Layout::UI_BORDER, Layout::UI_BORDER );
		s->SetScale( scale, scale );
	}
	else if ( image->ToTile() )
	{
		KrTile* t = image->ToTile();
		KrTileResource* res = t->TileResource();

		GlFixed scale = GlMin( GlFixed( size ) / GlFixed( t->Size() ),
							   GlFixed( size ) / GlFixed( t->Size() ) );
		t->SetPos( Layout::UI_BORDER, Layout::UI_BORDER );
		t->SetScale( scale, scale );
		t->SetRotation( layout->GetRotation() );
	}
	image->SetVisible( false );
	image->SetVisible( true, Layout::UI_VIEW );
}


bool UILogic::HandleWidgetEvent(	KrWidget* source, 
									U32 event, 
									U32 data, 
									const SDL_Event* sdlEvent,
									const char* command, 
									const char* arg )
{

	Layer* layer = layout->GetLayer();

	if ( event == ACTIVATED && source == layout->Up() )
	{
		if ( buttonOffset > 0 )
			buttonOffset = GlMax( 0, buttonOffset - layout->NumOptionButtons() );
		layout->SetLayer( layer, buttonOffset );
	}
	else if ( event == ACTIVATED && source == layout->Down() )
	{
		if ( buttonOffset + layout->NumOptionButtons() < layer->images.Count() )
			buttonOffset  += layout->NumOptionButtons();
		layout->SetLayer( layer, buttonOffset );
	}
	else if ( event == ACTIVATED && source == layout->SaveButton() )
	{
		util->Save();
	}
	else if ( event == ACTIVATED && layer )
	{
		for( int i=0; i<layout->NumOptionButtons(); ++i )
		{
			if ( source == layout->OptionButtons( i ) )
			{
				if ( i + buttonOffset < layer->images.Count() )
				{
					// Set the preview
					KrImNode* preview = layout->Engine()->Tree()->FindNodeByName( "preview" );
					if ( preview )
						layout->Engine()->Tree()->DeleteNode( preview );

					KrImage* currentImage = layer->images[i + buttonOffset]->ToImage();
					KrImage* image = currentImage->Clone()->ToImage();
					image->SetNodeName( "preview" );
					layout->Engine()->Tree()->AddNode( 0, image );
					
					SetPreview( image );
				}
			}
		}
	}
	return false;
}


KrImage* UILogic::CurrentImage()
{
	KrImNode* preview = layout->Engine()->Tree()->FindNodeByName( "preview" );
	if ( preview )
		return preview->ToImage();
	return 0;
}
