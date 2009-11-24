#ifndef MAPMAKER_XMLUTIL_INCLUDED
#define MAPMAKER_XMLUTIL_INCLUDED

#include "../tinyxml/tinyxml.h"
#ifdef _MSC_VER
// Disable the no-exception handling warning.
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include "../engine/krmath.h"

class KrListBox;
class KrResourceVault;
class KrImage;
class KrImNode;
class KrEngine;
class KrSquareWorldMap;

struct Layer
{
	Layer() : behavior( TILE_BEHAVIOR )	{}


	enum
	{
		TILE_BEHAVIOR,
		ROAD_BEHAVIOR,
		OBJECT_BEHAVIOR
	};

	std::string				name;
	int						behavior;
	GlDynArray< KrImage* >	images;
	GlDynArray< std::string > imageDesc;
	KrImNode*				krimNode;
	int						depth;
};


class XmlUtil
{
  public:
	XmlUtil( TiXmlDocument* doc );
	~XmlUtil()							{}

	const std::string& DatFileName()	{ return datFilename; }
	bool IsValidMap()					{ return valid; }

	void Init( KrResourceVault* vault,
			   KrEngine* engine,
			   KrImNode* layerParent,
			   KrSquareWorldMap* worldMap );

//	void InsertLayers( KrListBox* box );
	Layer* GetLayer( const std::string& layerName );
	KrVector2 Size();

	void Save();
		
  private:
	TiXmlDocument* doc;
		bool valid;
	std::string datFilename;

	GlDynArray< Layer > layerArray;
	KrVector2 size;
	KrSquareWorldMap* worldMap;
};

#endif