#include "xmlutil.h"
#include "../kyra.h"

XmlUtil::XmlUtil( TiXmlDocument* _doc )
{
	doc = _doc;
	valid = false;

	TiXmlElement* root = doc->FirstChildElement( "Map" );
	if ( !root ) return;

	if ( !root->Attribute( "datfile" ) ) return;
	datFilename = *root->Attribute( "datfile" );

	if ( !datFilename.empty() )	valid = true;
	worldMap = 0;
}


void XmlUtil::Init( KrResourceVault* vault, KrEngine* engine, KrImNode* parent, KrSquareWorldMap* _worldMap )
{
	worldMap = _worldMap;

	TiXmlElement* element = doc->FirstChildElement( "Map" );
	if ( element ) 
	{
		TiXmlElement* sourceElement = element->FirstChildElement( "Source" );
		if ( sourceElement ) 
		{
			bool needSize = true;
			int  layerIndex = 0;
			for( TiXmlElement* e = sourceElement->FirstChildElement( "Layer" );
				 e;
				 e = e->NextSiblingElement( "Layer" ) )
			{
				if( e->Attribute( "name" ) )
				{
					Layer dummy;
					layerArray.PushBack( dummy );
					Layer* layer = &layerArray[ layerArray.Count() - 1 ];
					++layerIndex;

					//////////////////// NAME ///////////
					layer->name = *e->Attribute( "name" );

					//////////////////// BEHAVIOR ///////////
					int b = Layer::TILE_BEHAVIOR;

					if ( e && e->Attribute( "behavior" ) && *e->Attribute( "behavior" ) == "road" )
						b = Layer::ROAD_BEHAVIOR;
					else if ( e && e->Attribute( "behavior" ) && *e->Attribute( "behavior" ) == "object" )
						b = Layer::OBJECT_BEHAVIOR;

					layer->behavior = b;

					//////////////////// IMAGES ///////////
					for( TiXmlElement* source = e->FirstChildElement();
						 source;
						 source = source->NextSiblingElement() )
					{
						std::string name = "NONE";
						if ( source->Attribute( "name" ) )
						{
							name = *source->Attribute( "name" );
						}

						if ( StrEqual( source->Value(), "Tile" ) )
						{
							KrTileResource* tileRes = vault->GetTileResource( name );
							if ( !tileRes )
							{
								GLLOG( "WARNING: Tile '%s' not found.\n", name.c_str() );
							}
							else
							{
								KrTile* tile = new KrTile( tileRes );
								layer->images.PushBack( tile );
								layer->imageDesc.PushBack( tileRes->ResourceName() );

								if ( needSize )
								{
									size.x = tile->Size();
									size.y = tile->Size();
									needSize = false;
								}
							}
						}
						else if ( StrEqual( source->Value(), "Sprite" ) )
						{
							KrSpriteResource* spriteRes = vault->GetSpriteResource( name );
							if ( !spriteRes )
							{
								GLLOG( "WARNING: Sprite '%s' not found.\n", name.c_str() );
							}
							else
							{
								for( int i=0; i<spriteRes->NumActions(); ++i )
								{
									KrSprite* sprite = new KrSprite( spriteRes );
									sprite->SetAction( spriteRes->GetActionByIndex( i )->Name() );

									if ( needSize )
									{
										size.x = sprite->GetAction()->GetFrame( 0 )->Width() + 2;
										size.y = sprite->GetAction()->GetFrame( 0 )->Height();
										needSize = false;
									}

									layer->images.PushBack( sprite );
									char buf[ 512 ];
									sprintf( buf, "%s::%s", spriteRes->ResourceName().c_str(),
															sprite->GetAction()->Name().c_str() );
									layer->imageDesc.PushBack( buf );

								}
							}
						}
					}
					///////////// Parent /////////////
					layer->krimNode = new KrImNode();
					engine->Tree()->AddNode( parent, layer->krimNode );
					layer->krimNode->SetZDepth( layerIndex );
					layer->depth = layerIndex;
				}
			}
		}
	}
	TiXmlElement* dataElement = element->FirstChildElement( "MapData" );
	if ( dataElement ) 
	{
		worldMap->LoadXml( dataElement, vault );
	}
}


Layer* XmlUtil::GetLayer( const std::string& name )
{
	return &layerArray[0];
}


void XmlUtil::Save()
{
	TiXmlElement* map = doc->FirstChildElement( "Map" );
	if ( !map )
	{
		GLLOG( "Map element not found!\n" );
		return;
	}

	TiXmlElement* mapData = map->FirstChildElement( "MapData" );
	if ( mapData )
	{
		map->RemoveChild( mapData );
		mapData = 0;
	}

	TiXmlElement newMapData( "MapData" );
	TiXmlNode* node = map->InsertEndChild( newMapData );
	worldMap->SaveXml( node );

	doc->SaveFile();
}
