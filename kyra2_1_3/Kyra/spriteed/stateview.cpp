/*--License:
	Kyra Sprite Engine
	Copyright Lee Thomason (Grinning Lizard Software) 2001-2005
	www.grinninglizard.com/kyra
	www.sourceforge.net/projects/kyra

	Kyra is provided under the LGPL. 
	
	I kindly request you display a splash screen (provided in the HTML documentation)
	to promote Kyra and acknowledge the software and everyone who has contributed to it, 
	but it is not required by the license.

--- LGPL License --

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	The full text of the license can be found in lgpl.txt
*/

#include "states.h"
#include "sharedstate.h"
#include "../engine/canvas.h"
#include "../engine/textbox.h"
#include "../engine/canvasresource.h"
#include "../engine/boxresource.h"
#include "../engine/box.h"
#include "statemachine.h"
#include "dom.h"

using namespace grinliz;


EdStateView::EdStateView( Editor* _machine ) : EdState( _machine )
{
    ALIGNMENT_COLOR[0].Set( 000, 000, 000 );
    ALIGNMENT_COLOR[1].Set( 000, 000, 000 );
    ALIGNMENT_COLOR[2].Set( 200, 200, 200 );
    ALIGNMENT_COLOR[3].Set( 200, 200, 200 );

	// Create the alignment bars. They persist, but are not
	// always visible.
	KrRect size = shared->Engine()->ScreenBounds();

	alignVRes = new KrBoxResource(	"alignV",
									1,
									size.Height(),
									ALIGNMENT_COLOR,
									ALIGNMENT_NCOLORS,
									0 );
	alignHRes = new KrBoxResource(	"alignH",
									size.Width(),
									1,
									ALIGNMENT_COLOR,
									ALIGNMENT_NCOLORS,
									0 );
	// Don't link to the view: it gets transformed.
	// Use the ImNode:
	KrBox* box;
	box = new KrBox( alignVRes );
	box->SetNodeName( "Vertical" );
	shared->Engine()->Tree()->AddNode( shared->ImNode(), box );
	box->SetVisible( false );
	box->SetZDepth( DEPTH_ALIGN );

	box = new KrBox( alignHRes );
	box->SetNodeName( "Horizontal" );
	shared->Engine()->Tree()->AddNode( shared->ImNode(), box );
	box->SetVisible( false );
	box->SetZDepth( DEPTH_ALIGN );

	dragRes = 0;
	canvas = 0;
	scale = 1;
	xPos = yPos = 0;

}


EdStateView::~EdStateView()
{
	KrImNode* krimNode = shared->Engine()->Tree()->FindNodeByName( "Dragging" );
	if ( krimNode )
	{
		shared->Engine()->Tree()->DeleteNode( krimNode );
		delete dragRes;
		dragRes = 0;
	}

	delete alignVRes;
	delete alignHRes;
}


void EdStateView::StateOpening()
{
	viewNode = new KrImNode();
	shared->Engine()->Tree()->AddNode( shared->ImNode(), viewNode );

	// The canvas lives at the back of the image -- depth 0.
	// The bounding boxes get depth 1,
	// and the alignment bars get depth 2.
	canvas = new KrCanvas( shared->CanvasResource() );
	canvas->SetNodeName( "editing" );
	GLASSERT( canvas );

	shared->Engine()->Tree()->AddNode( viewNode, canvas );
	canvas->SetZDepth( DEPTH_CANVAS );

	// Add a background
	KrCanvasResource* backgroundRes = shared->BackgroundRes();
	for ( int i=0; i<canvas->Width() / backgroundRes->Width() + 1; i++ )
	{
		for( int j=0; j<canvas->Height() / backgroundRes->Height() + 1; j++ )
		{
			KrCanvas* backCanvas = new KrCanvas( backgroundRes );
			shared->Engine()->Tree()->AddNode( viewNode, backCanvas );
			backCanvas->SetZDepth( DEPTH_BACKGROUND );
			backCanvas->SetPos( i*backgroundRes->Width(), j*backgroundRes->Height() );
		}
	}
	shared->WidgetNode()->SetVisible( true );
	shared->ConsoleNode()->SetVisible( true );
	shared->SetInfoBox( true );

	// restore the previous scale and position
	viewNode->SetScale( scale, scale );
	shared->WidgetNode()->SetScale( scale, scale );

	viewNode->SetPos( xPos, yPos );
	shared->WidgetNode()->SetPos( xPos, yPos );
}


void EdStateView::StateClosing()
{
	shared->Engine()->Tree()->DeleteNode( viewNode );
	shared->WidgetNode()->SetVisible( false );
//	viewNode->SetPos( 0, 0 );
//	viewNode->SetScale( 1, 1 );
//	shared->WidgetNode()->SetPos( 0, 0 );
//	shared->WidgetNode()->SetScale( 1, 1 );
	canvas = 0;
}


void EdStateView::ScreenToCanvas( int x, int y, Vector2I* object )
{
	// Get the click point, in canvas coordinates:
	Vector2< GlFixed > fixedObj;
	canvas->ScreenToObject( x, y, &fixedObj );
	object->x = fixedObj.x.ToIntRound();
	object->y = fixedObj.y.ToIntRound();
}


void EdStateView::RightClick( int x, int y )
{
	// We now have where the canvas was clicked, in canvas coordinates.
	Vector2I object;
	ScreenToCanvas( x, y, &object );
// 	GLOUTPUT( "Canvas: x=%d y=%d\n", object.x, object.y );

	if ( scale < 4 )
	{
		scale *= 2;
		KrRect sb = shared->Engine()->ScreenBounds();
		int screenW = sb.Width();
		int screenH = sb.Height() - shared->Console()->TextBox()->Height();

		GlFixed vx = GlFixed( screenW / 2 ) - scale * object.x;
		GlFixed vy = GlFixed( screenH / 2 ) - scale * object.y;

		xPos = vx.ToInt();
		yPos = vy.ToInt();

		viewNode->SetScale( scale, scale );
		viewNode->SetPos( xPos, yPos );
		shared->WidgetNode()->SetScale( scale, scale );
		shared->WidgetNode()->SetPos( xPos, yPos );
		Pan( 0, 0 );
	}
}


void EdStateView::ZoomIn()
{
	if ( scale < 4 )
	{
		scale *= 2;
		viewNode->SetScale( scale, scale );
		shared->WidgetNode()->SetScale( scale, scale );
		Pan( 0, 0 );
	}
}


void EdStateView::ZoomOut()
{
	if ( scale.v > GlFixed_1 / 4 )
	{
		scale /= 2;
		viewNode->SetScale( scale, scale );
		shared->WidgetNode()->SetScale( scale, scale );
		Pan( 0, 0 );
	}
}

void EdStateView::MakeSquare( KrRect* rect )
{
	if ( rect->Width() < rect->Height() )
		rect->max.y = rect->min.y + rect->Width() - 1;
	else
		rect->max.x = rect->min.x + rect->Height() - 1;
}


void EdStateView::FreeMove( int x, int y, bool command )
{
	// Don't need to translate x and y since the alignment
	// lines are all the krimnode, not the viewnode.

	KrImNode* imageH = shared->Engine()->Tree()->FindNodeByName( "Horizontal" );
	KrImNode* imageV = shared->Engine()->Tree()->FindNodeByName( "Vertical" );

	GLASSERT( imageH );
	GLASSERT( imageV );

	if ( command )
	{
		imageH->SetVisible( true );
		imageH->SetPos( 0, y );

		imageV->SetVisible( true );
		imageV->SetPos( x, 0 );
	}
	else
	{
		imageH->SetVisible( false );
		imageV->SetVisible( false );
	}

	// Get the mouse point, in canvas coordinates:
	Vector2I object;
	ScreenToCanvas( x, y, &object );

	shared->SetInfoBoxMouse( object.x, object.y );
}


void EdStateView::Click( int mouseX, int mouseY, bool command, int keymod )
{
	// Waste some clock cycles to have less code repetition:
	Vector2I coord;
	ScreenToCanvas( mouseX, mouseY, &coord );
	keymod &= KMOD_SHIFT;

	if ( !command )
	{
		// Special case the "change size" keys, in the kmod case.
		// The next case is the standard click case.

		if (    keymod == KMOD_LSHIFT 
			 || keymod == KMOD_RSHIFT )
		{

			if ( shared->CurrentFrame() )
			{
				KrDom::Frame frameData = shared->CurrentFrame()->GetFrameData();

				// The shift key used sets one corner or the other.
				if ( keymod == KMOD_LSHIFT )
				{
					if (    coord.x < frameData.x + frameData.width 
					     && coord.y < frameData.y + frameData.height )
					{
						frameData.width  = frameData.x + frameData.width  - coord.x;
						frameData.height = frameData.y + frameData.height - coord.y;
						frameData.x = coord.x;
						frameData.y = coord.y;
					}
				}
				else
				{
					if (    coord.x > frameData.x
					     && coord.y > frameData.y )
					{
						frameData.width  = coord.x - frameData.x + 1;
						frameData.height = coord.y - frameData.y + 1;
					}
				}

				shared->CurrentFrame()->SetFrameData( frameData );
			}
		}
		else
		{	
			// Find what was clicked on:
			std::vector<KrImage*> hittest;
			int window;

			shared->Engine()->Tree()->HitTest(	mouseX, mouseY, 0, 
												KrImageTree::ALWAYS_INSIDE_BOX | KrImageTree::GET_ALL_HITS,
												&hittest,
												&window );

			GLOUTPUT(( "Hittest returned %d items\n", hittest.size() ));

			// We are interested in the first box. Find it and select it:
			unsigned i;
			for( i=0; i<hittest.size(); i++ )
			{
				if ( hittest[i]->ToBox() )
				{
					GLOUTPUT(( "Box hit. Searching...\n" ));
		
					// Now find the widget that uses the box.
					EdWidget* widget = shared->Widget()->FindWidget( hittest[i]->ToBox(), 0 );
					// There are other boxes -- namely used for the crosshares --
					// that can be found. So widget can come back null.
					if ( widget )
					{
	// 					widget->Select( true );
						if ( widget->Type() == EdWidget::FRAME )
						{
							shared->SetCurrentSprite( widget->Parent()->Parent() );
							shared->SetCurrentAction( widget->Parent() );
							shared->SetCurrentFrame(  widget );
						}
						else if ( widget->Type() == EdWidget::TILE )
						{
							shared->SetCurrentTile( widget );
						}
						break;
					}
				}
			}
		}
	}
	else
	{
        if ( shared->CurrentFrame() )
		{
			KrDom::Frame frameData = shared->CurrentFrame()->GetFrameData();

			frameData.hotspotX = coord.x;
			frameData.hotspotY = coord.y;

			shared->CurrentFrame()->SetFrameData( frameData );
		}
	}
}


int EdStateView::CalcFrameIndex( TiXmlElement* action, TiXmlElement* frame )
{
	int index = 0;
	TiXmlNode* node;

	for ( node = action->FirstChild( "Frame" ); node; node=node->NextSibling( "Frame" ) )
	{
		if ( node == (TiXmlNode*) frame )
		{
			return index;
		}
		index++;
	}
	return -1;
}


void EdStateView::Pan( int deltaX, int deltaY )
{
	GLOUTPUT(( "pan dx=%d dy=%d\n", deltaX, deltaY ));

	xPos = viewNode->X() + deltaX;
	yPos = viewNode->Y() + deltaY;
	GlFixed width  = viewNode->XScale() * shared->CanvasResource()->Width();
	GlFixed height = viewNode->YScale() * shared->CanvasResource()->Height();

	KrRect engineRect  = shared->Engine()->ScreenBounds();
	const KrRect& consoleRect = shared->Console()->CompositeBounds( 0 );
	
	if ( width + xPos < engineRect.max.x )
		xPos = engineRect.max.x - width.ToInt() - 1;
	if ( height + yPos < ( engineRect.max.y - consoleRect.Height() ) )
		yPos = engineRect.max.y - height.ToInt() - consoleRect.Height() - 1;

	if ( xPos > 0 )
		xPos = 0;
	if ( yPos > 0 )
		yPos = 0;

	viewNode->SetPos( xPos, yPos );
	shared->WidgetNode()->SetPos( xPos, yPos );
}


void EdStateView::Drag( const KrRect& _drag, bool complete )
{
	KrRect drag = _drag;

	Vector2I minbound, maxbound;
	ScreenToCanvas( drag.min.x, drag.min.y, &minbound );
	ScreenToCanvas( drag.max.x, drag.max.y, &maxbound );
	drag.min.x = minbound.x;
	drag.min.y = minbound.y;
	drag.max.x = maxbound.x;
	drag.max.y = maxbound.y;

	// Clip to the bitmap surface.
	KrRect bounds;
	bounds.min.Set( 0, 0 );
	bounds.max.Set( shared->CanvasResource()->Width() - 1, shared->CanvasResource()->Height() - 1 );
	drag.DoIntersection( bounds );

	// Remove the old drag rectangle, if it exists. Before
	// we can remove the resource, get rid of the sprite that uses 
	// it.
	KrImNode* krimNode = shared->Engine()->Tree()->FindNodeByName( "Dragging" );
	if ( krimNode )
	{
		shared->Engine()->Tree()->DeleteNode( krimNode );
		delete dragRes;
		dragRes = 0;
	}

	if ( shared->CurrentObject() == SharedStateData::TILE )
	{
		MakeSquare( &drag );
	}

	if ( complete )
	{
		AddFrame( drag.min.x, drag.min.y, drag.Width(), drag.Height() );
	}
	else
	{
		KrRGBA rgba[4];
		rgba[0].Set( 0, 255, 0 );
		rgba[1].Set( 0, 255, 0 );
		rgba[2].Set( 0, 0, 255 );
		rgba[3].Set( 0, 0, 255 );

		if ( drag.min.x > drag.max.x ) Swap( &drag.min.x, &drag.max.x );
		if ( drag.min.y > drag.max.y ) Swap( &drag.min.y, &drag.max.y );

		dragRes = new KrBoxResource(	"DraggingResource",
										drag.Width(), drag.Height(),
										rgba, 4,
										0 );

		krimNode = new KrBox( dragRes );
		krimNode->SetPos( drag.min.x, drag.min.y );
		krimNode->SetNodeName( "Dragging" );
		krimNode->SetZDepth( DEPTH_DRAGGING );
		shared->Engine()->Tree()->AddNode( viewNode, krimNode );
	}
}


void EdStateView::AddFrame( int x, int y, int width, int height )
{
	if ( shared->CurrentObject() == SharedStateData::SPRITE )
	{
		// Add a new frame!
		if ( !shared->CurrentSprite() )
			shared->Console()->Print( "No current sprite for 'create frame'.\n" );
		if ( !shared->CurrentAction() )
			shared->Console()->Print( "No current action for 'create frame'.\n" );
		if ( !shared->CurrentSprite() || !shared->CurrentAction() )
			return;


		KrDom::Frame frameData;

		frameData.x = x;
		frameData.y = y;
		frameData.width = width;
		frameData.height = height;
		frameData.hotspotX = x;
		frameData.hotspotY = y;
		frameData.deltaX = 0;
		frameData.deltaY = 0;

		// Add it in.
		EdWidgetFrame* newFrame = new EdWidgetFrame( shared->Engine()->Tree(),
													 shared->WidgetNode(),
													 shared->CurrentAction(),
													 shared->FontResource(),
													 frameData );
		
		// Must push to the back so they don't get out of order!
		shared->CurrentAction()->children.PushBack( newFrame );
		shared->SetCurrentFrame( newFrame );
	}
	else
	{
		// Set a tile! (Note the tile must be named, and
		// hence created for this to work.)
		EdWidgetTile* tile = shared->CurrentTile();
		if ( tile )
		{
			tile->Set( x, y, width );
			shared->SetCurrentTile( tile );
		}
	}
}	


void EdStateView::DeleteCurrent()
{
	if ( shared->CurrentObject() == SharedStateData::SPRITE )
	{
		// Deleting a frame.
		if ( shared->CurrentFrame() )
		{
			EdWidget* frame = shared->CurrentFrame();
			shared->SetCurrentFrame( 0 );
			frame->Parent()->children.Pop( frame );

			// Renumber the siblings:
			GlSListIterator< EdWidget* > it( frame->Parent()->children );
			for( it.Begin(); !it.Done(); it.Next() )
			{
				it.Current()->Update();
			}
			delete frame;
		}
		else
		{
			shared->Console()->Print( "No current frame.\n" );
		}
	}
	else
	{
		// Deleting a tile.
		if ( shared->CurrentTile() )
		{
			EdWidget* tile = shared->CurrentTile();
			shared->SetCurrentTile( 0 );
			shared->Widget()->children.Pop( tile );
			delete tile;
		}
		else
		{
			shared->Console()->Print( "No current tile.\n" );
		}
	}
}


void EdStateView::SetSpriteName( const char* name )
{
	// Find the sprite, create it if it does not exist.
	EdWidget* sprite = 0;
	EdWidget* action = 0;
	EdWidget* frame = 0;
	
	GlSListIterator<EdWidget*> spriteIt( shared->Widget()->children );

	for( spriteIt.Begin(); !spriteIt.Done(); spriteIt.Next() )
	{
		EdWidgetSprite* spriteWidget = spriteIt.Current()->ToSprite();
	
		// There are tiles and sprites at this level.
		if ( spriteWidget )
		{
			if ( name && name == spriteWidget->Name() )
			{
				sprite = spriteIt.Current();
				if ( !sprite->children.Empty() )
				{
					action = sprite->children.Front();
					if ( !action->children.Empty() )
					{
						frame = action->children.Front();
					}
				}
				break;
			}
		}
	}
				
	if ( sprite )
	{
		shared->SetCurrentSprite( sprite );
		shared->SetCurrentAction( action );
		shared->SetCurrentFrame(  frame );
	}
	else
	{
		EdWidgetSprite* newSprite = new EdWidgetSprite( shared->Engine()->Tree(),
														shared->WidgetNode(),
														shared->Widget(),
														name );
		shared->Widget()->children.PushFront( newSprite );

		shared->Console()->Print( "New sprite '%s' added.\n", name );
		shared->SetCurrentSprite( newSprite );
		SetAction( "NONE" );
	}
}


void EdStateView::SetAction( const char* name )
{
	EdWidget* sprite = shared->CurrentSprite();
	EdWidget* action = 0;
	EdWidget* frame = 0;

	// Find the action, create it if it does not exist.
	if ( sprite )
	{
		GlSListIterator<EdWidget*> actionIt( sprite->children );

		for( actionIt.Begin(); !actionIt.Done(); actionIt.Next() )
		{
			EdWidgetAction* actionWidget = actionIt.Current()->ToAction();
			GLASSERT( actionWidget );
			
			if ( name && name == actionWidget->Name() )
			{
				action = actionIt.Current();
				if ( !action->children.Empty() )
				{
					frame = action->children.Front();
				}
			}
		}
		if ( action )
		{
			shared->SetCurrentSprite( sprite );
			shared->SetCurrentAction( action );
			shared->SetCurrentFrame( frame );
		}
		else
		{
			EdWidgetAction* newAction = new EdWidgetAction( shared->Engine()->Tree(),
															shared->WidgetNode(),
															sprite,
															name );
			sprite->children.PushFront( newAction );

			shared->Console()->Print( "New Action '%s' added.\n", name );
			shared->SetCurrentAction( newAction );
		}
	}	
	else
	{	
		shared->Console()->Print( "No current sprite.\n" );
	}
}


void EdStateView::SetTileName( const std::string& name )
{
	// Find the tile, create it if it does not exist.
	EdWidget* tile = 0;

	GlSListIterator<EdWidget*> tileIt( shared->Widget()->children );

	for( tileIt.Begin(); !tileIt.Done(); tileIt.Next() )
	{
		EdWidgetTile* tileWidget = tileIt.Current()->ToTile();
		if ( tileWidget )
		{
			if ( name == tileWidget->Name() )
			{
				tile = tileWidget;
				shared->SetCurrentTile( tileWidget );
				break;
			}
		}
	}
	if ( !tile )
	{
		// The tile does not exist; create it.
		EdWidgetTile* newTile = new EdWidgetTile(	shared->Engine()->Tree(),
													shared->WidgetNode(),
													shared->Widget(),
													name,
													0,
													0,
													1  );

		shared->Widget()->children.PushFront( newTile );

		shared->SetCurrentTile( newTile );
	}
}


void EdStateView::Command( int index, CommandArgument* arg, int nArg )
{ 
	switch( index )
	{
		case COMMAND_NAME:
			SetSpriteName( arg[0].CharString() );
			break;

		case COMMAND_ACTION:
			SetAction( arg[0].CharString() );
			break;

		case COMMAND_DELETE_CURRENT_FRAME:
			DeleteCurrent();
			break;

		case COMMAND_TILE:
			SetTileName( arg[0].String() );
			break;

		case COMMAND_DISPERSE:
			{
				if ( !shared->CurrentAction() )
				{
					shared->Console()->Print( "There must be a current action to disperse.\n" );
				}
				else
				{
					int dx = arg[0].Integer();
					int dy = arg[1].Integer();
					
					GlSListIterator< EdWidget* > it( shared->CurrentAction()->children );

					for( it.Begin(); !it.Done(); it.Next() )
					{
						EdWidgetFrame* frame = it.Current()->ToFrame();	
						GLASSERT( frame );

						KrDom::Frame frameData;
						frameData = frame->GetFrameData();
						frameData.deltaX = dx;
						frameData.deltaY = dy;
						frame->SetFrameData( frameData );
					}
				}
			}
			break;

/*		case COMMAND_ISOTILE:
			{
				int width = arg[0].Integer();

				if ( !shared->CurrentFrame() )
				{
					shared->Console()->Print( "There must be a current frame to isotile.\n" );
				}
				else if ( width % 4 )
				{
					shared->Console()->Print( "Width must be a multiple of 4.\n" );
				}
				else
				{
					KrDom::Frame fd = shared->CurrentFrame()->GetFrameData();
					fd.isotile = width;
					shared->CurrentFrame()->SetFrameData( fd );
				}
				shared->SetInfoBox( true );
			}
			break;
*/

		default:
			GLASSERT( 0 );
			shared->Console()->Print( "Unrecognized command\n" );
			break;
	}
}
