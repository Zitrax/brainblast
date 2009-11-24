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
#include "sprite.h"
#include "SDL.h"
#include "engine.h"

using namespace grinliz;

KrSprite::KrSprite( KrSpriteResource* res )
	: resource( res ),
	  frame( 0 )
{
	// Get the first action as a default.
	action = resource->GetActionByIndex( 0 );

//	for( int i=0; i<KR_MAX_WINDOWS; ++i )
//		oldBounds[i] = bounds[i];
}


void KrSprite::SetFrame( int i )
{
	GLASSERT( i>=0 && i<action->NumFrames() );
	if ( i != frame )
	{
		Invalidate( KR_ALL_WINDOWS );
		frame = i;

		frame = Clamp( frame, 0, action->NumFrames()-1 );
	}
}


void KrSprite::SetAction( const std::string& actionName )
{ 
	KrAction* newaction = resource->GetAction( actionName ); 
	GLASSERT( newaction );
	if ( newaction && newaction != action )
	{
		Invalidate( KR_ALL_WINDOWS );
		action = newaction;
		frame = Clamp( frame, 0, action->NumFrames()-1 );
	}
}


void KrSprite::SetActionRotated( const std::string& actionName, int rotation )
{
	char buf[ 16 ];
	while( rotation < 0 )
		rotation += 360;
	while( rotation >= 360 )
		rotation -= 360;
	
	sprintf( buf, ".ROT%03d", rotation );
	std::string str = actionName + buf;

	SetAction( str );
}


void KrSprite::SetAction( U32 id )
{ 
	KrAction* newaction = resource->GetAction( id ); 
	if ( newaction != action )
	{
		Invalidate( KR_ALL_WINDOWS );
		action = newaction;
		frame = Clamp( frame, 0, action->NumFrames()-1 );
	}
}


void KrSprite::Draw(	KrPaintInfo* paintInfo, 
						const Rectangle2I& clip,
						int win )
{	
	GLASSERT( IsVisible(win) );
	GLASSERT( action );
	// This happens if an empty action (often "NONE" gets exported.)
	if ( action->NumFrames() == 0 ) return;
	action->Draw(	paintInfo,
					frame, 
					CompositeXForm(win),
					CompositeCForm(win),
					clip );
}


void KrSprite::DoStep()
{
	GLASSERT( action );
	GLASSERT( Engine() );
	if ( action )
	{
		Vector2I delta = action->Frame( frame ).StepSize();
		
		int count = KR_MAX_WINDOWS;
		if ( Engine() ) count = Engine()->NumWindows();
		for( int i=0; i<count; ++i )
		{
			if ( XTransform(i).IsScaled() )
			{
				int x = X(i) + ( XTransform(i).xScale * delta.x ).ToIntRound();
				int y = Y(i) + ( XTransform(i).yScale * delta.y ).ToIntRound();
				SetPos( x, y, i );
			}
			else
			{
				SetPos( X(i) + delta.x, Y(i) + delta.y, i );
			}
		}
		SetFrame( ( frame + 1 ) % action->NumFrames() );
	}
}


Vector2I KrSprite::Stride()
{
	Vector2I stride;
	stride.x = 0;
	stride.y = 0;

	GLASSERT( action );
	if ( action )
	{
		for( int i=0; i<action->NumFrames(); i++ )
		{
			stride += action->Frame( i ).StepSize();
		}
	}
	return stride;
}


void KrSprite::CalcTransform( int win )
{
//	GLASSERT( oldBounds[win] == bounds[win] );
	// Calculate our new coordinates, and then bounding info.
	KrImNode::CalcTransform( win );

	// Remember are coordinates are based on the hotspot, but
	// are bounds are based on the sprite.
	action->CalculateBounds( frame, CompositeXForm(win), &bounds[win] );
//	oldBounds[win] = bounds[win];
}


bool KrSprite::HitTest( int x, int y, int flags, std::vector<KrImage*>* results, int window )
{
	if ( action )
	{
		int i = window;
		if (    IsVisible( i )
			 && CompositeCForm(i).Alpha() != 0 )
		{

			Vector2< GlFixed > object;
			ScreenToObject( x, y, &object, i );

			if ( action->HitTestTransformed( frame,
											 object.x.ToIntRound(), object.y.ToIntRound(),
											 flags ) )
			{	
				results->push_back( this );
				return true;
			}
		}
	}
	return false;
}


void KrSprite::QueryBoundingBox( Rectangle2I* rect, int win )
{
	rect->SetInvalid();
	if ( action )
	{
		KrRle* rle = action->GetFrame( frame );
		rle->CalculateBounds( CompositeXForm( win ), rect );
	}
}


KrImNode* KrSprite::Clone()
{
	KrSprite* sprite = new KrSprite( resource );
	sprite->SetAction( action->Id() );
	sprite->SetFrame( frame );
	return sprite;
}

