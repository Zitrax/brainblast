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
#include "sdlutil.h"
#include "image.h"
#include "sprite.h"
#include "SDL.h"
#include "SDL_endian.h"
#include "rle.h"
#include "spriteresource.h"
#include "canvasresource.h"
#include "encoder.h"
#include "../../grinliz/glgeometry.h"


using namespace grinliz;


const std::string KrSpriteResource::spriteName = "Sprite";


KrSpriteResource::KrSpriteResource( U32 size, 
									SDL_RWops* data ) 
{
	std::string name;
	ReadString( data, &name );
	U32 id = SDL_ReadLE32( data );
	SetNameAndId( name, id );

	GLOUTPUT(( "Sprite or Font resource '%s' id=%d\n", name.c_str(), id ));

	U32 nAction = SDL_ReadLE32( data );
	actionArr.resize( nAction );
//
//	actionMap   = new std::map< std::string, KrAction* >();
//	actionIdMap = new std::map< U32, KrAction* >();

	for ( unsigned i=0; i<nAction; i++ )
	{
		actionArr[ i ] = new KrAction( data );

		//actionMap->Add(   actionArr[i]->Name(), actionArr[i] );
		//actionIdMap->Add( actionArr[i]->Id(), actionArr[i] );
		actionMap[ actionArr[i]->Name() ] = actionArr[i];
		actionIdMap[ actionArr[i]->Id() ] = actionArr[i];
	}
}


KrSpriteResource::KrSpriteResource( const std::string& _name )
{
	SetNameAndId( _name, 0 );

//	actionMap   = new GlMap< std::string, KrAction*, GlStringHash >( 0 );
//	actionIdMap = new GlMap< U32, KrAction*, GlNumberHash<U32> >( 0 );
}


KrSpriteResource::~KrSpriteResource()
{
//	delete actionMap;
//	delete actionIdMap;

	for ( U32 i=0; i<actionArr.size(); i++ )
	{
		delete actionArr[i];
	}
//	delete [] actionArr;
}


void KrSpriteResource::Draw(	KrPaintInfo* paintInfo, 
								const std::string& actionname,
								int frame,
								const KrMatrix2& xForm,
								const KrColorTransform& cForm,
								const Rectangle2I& clip,
								int quality )
{
	//KrAction* act = 0;
	//actionMap->Find( actionname, &act );
	std::map< std::string, KrAction* >::iterator act = actionMap.find( actionname );
	if ( act != actionMap.end() )
	{
		act->second->Draw( paintInfo, frame, xForm, cForm, clip );
	}
	#ifdef DEBUG
	else
	{
		GLOUTPUT(( "WARING KrSpriteResource::Draw failed to find action %s\n", actionname.c_str() ));
	}
	#endif
}


KrAction* KrSpriteResource::GetAction( const std::string& actionName )
{
//	KrAction* ret = 0;
//	actionMap->Find( actionName, &ret );
	std::map< std::string, KrAction* >::iterator act = actionMap.find( actionName );
	if ( act == actionMap.end() )
		return 0;
	else
		return act->second;
}


KrAction* KrSpriteResource::GetAction( U32 id )
{
	std::map< U32, KrAction* >::iterator act = actionIdMap.find( id );
	if ( act == actionIdMap.end() )
		return 0;
	else
		return act->second;
}


void KrSpriteResource::AddAction( KrAction* _action )
{
	actionArr.push_back( _action );
//	actionMap->Add( _action->Name(), actionArr[ actionArr.size()-1 ] );
//	actionIdMap->Add( _action->Id(), actionArr[ actionArr.size()-1 ] );
	actionMap[ _action->Name() ] = actionArr[ actionArr.size()-1 ];
	actionIdMap[ _action->Id() ] = actionArr[ actionArr.size()-1 ];
}


KrCanvasResource* KrSpriteResource::CreateCanvasResource( const std::string& actionName, int frame, int* hotx, int* hoty )
{
//	KrAction* action = 0;
	std::map< std::string, KrAction* >::iterator act = actionMap.find( actionName );
//	actionMap->Find( actionName, &action );
	if ( act != actionMap.end() )
		return act->second->CreateCanvasResource( frame, hotx, hoty );
	return 0;
}


KrCanvasResource* KrSpriteResource::CreateCanvasResource( U32 id, int frame, int* hotx, int* hoty )
{
	//KrAction* action = 0;
	//actionIdMap->Find( id, &action );
	std::map< U32, KrAction* >::iterator act = actionIdMap.find( id );
	if ( act != actionIdMap.end() )
		return act->second->CreateCanvasResource( frame, hotx, hoty );
	return 0;
}



void KrSpriteResource::CacheScale( GlFixed xScale, GlFixed yScale )
{
	for( int i=0; i<NumActions(); ++i )
	{
		actionArr[i]->CacheScale( xScale, yScale );
	}
}


bool KrSpriteResource::IsScaleCached( GlFixed xScale, GlFixed yScale )
{
	// All actions should have the same cache. The first
	// action is used for simplicity.
	if ( actionArr[0] )
		return actionArr[0]->IsScaleCached( xScale, yScale );
	return 0;
}


void KrSpriteResource::FreeScaleCache()
{
	for( int i=0; i<NumActions(); ++i )
	{
		actionArr[i]->FreeScaleCache();
	}
}


KrCollisionMap* KrSpriteResource::GetCollisionMap( KrImage* state, int window )
{
	GLASSERT( state && state->ToSprite() );
	KrSprite* sprite = state->ToSprite();
	GLASSERT( sprite->SpriteResource() == this );

	if ( state->XScale( window ) != 1 || state->YScale( window ) != 1 )
	{
		if ( !IsScaleCached( state->CompositeXForm( window ).xScale,
							 state->CompositeXForm( window ).yScale ) )
		{
			// For sprites, only cached scales can be used!
			#ifdef DEBUG
			GLOUTPUT(( "ERROR: Collision only allowed with cahed sprites!\n" ));
			GLOUTPUT(( "SpriteResource '%s' for Action '%s' Frame %d scale %f, %f\n",
					  ResourceName().c_str(),
					  sprite->GetAction()->Name().c_str(),
					  sprite->Frame(),
					  state->CompositeXForm( window ).xScale.ToDouble(),
					  state->CompositeXForm( window ).xScale.ToDouble() ));
			#endif
			GLASSERT( 0 );
			return 0;
		}
	}
	KrAction* action = sprite->GetAction();
	return action->GetCollisionMap( state->CompositeXForm( window ).xScale,
									state->CompositeXForm( window ).yScale,
									sprite->Frame() );
}


void KrSpriteResource::Save( KrEncoder* encoder )
{
	encoder->StartTag( KYRATAG_SPRITE );

	WriteString( encoder->Stream(), ResourceName() );
	encoder->WriteCached( ResourceName() );

	SDL_WriteLE32( encoder->Stream(), NumActions() );

	for( int i=0; i<NumActions(); ++i )
	{
		actionArr[i]->Save( encoder );
	}
	encoder->EndTag();
}


