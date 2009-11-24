#ifndef MAPMAKER_LAYOUT_INCLUDED
#define MAPMAKER_LAYOUT_INCLUDED

#include "../engine/kyra.h"

class Layout;
class XmlUtil;
struct Layer;



class Layout
{
  public:
	Layout( SDL_Surface* surface, KrFontResource* font );
	virtual ~Layout()				{ delete engine; }

	KrEngine*	Engine()	{ return engine; }
	KrImNode*	Map()		{ return map; }
//	KrCanvas*	Preview()	{ return canvas; }
	
	void DisplayMapCoords( float x, float y );
	void SetRotation( int r );
	int  GetRotation()								{ return rotation; }

	void		SetLayer( Layer*, int offset );
	Layer*		GetLayer()	{ return currentLayer; }

	int NumOptionButtons()							{ return nOptionButtons; }
	KrToggleButton* OptionButtons( int index )		{ return optionButton[ index ]; }
	KrPushButton* Up()								{ return upButton; }
	KrPushButton* Down()							{ return downButton; }
	KrPushButton* SaveButton()						{ return saveButton; }

	enum
	{
		MAIN_VIEW,
		UI_VIEW,
		MINIMAP_VIEW,

		NUM_VIEWS
	};

	int PreviewSize()	{ return UI_WIDTH - UI_BORDER*3 - BUTTON_W; }

	enum
	{
		UI_WIDTH = 200,
		UI_BORDER = 10,	

		MAX_OPTION = 16,
		BUTTON_W = 80,
		BUTTON_H = 30,
		LIST_H   = 150,

	};
	
  private:
	int nOptionButtons;
	int rotation;

	KrEngine*		engine;
	KrImNode*		map;

	KrPushButton	*upButton,
					*downButton,
					*saveButton;
	KrToggleButton	*optionButton[ MAX_OPTION ];
	KrCanvasResource* canvasRes;
//	KrCanvas* canvas;
	Layer* currentLayer;
	KrTextBox* infoBox;
};

#endif