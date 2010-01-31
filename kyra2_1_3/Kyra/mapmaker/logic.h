#ifndef KYRA_MAPMAKER_LOGIC_INCLUDED
#define KYRA_MAPMAKER_LOGIC_INCLUDED

#include "../engine/kyra.h"


class Layout;
class XmlUtil;


class UILogic : public IKrWidgetListener
{
  public:
	UILogic( Layout* layout, 
			 XmlUtil* util );

	KrImage* CurrentImage();

	virtual bool HandleWidgetEvent(	KrWidget* source, 
									U32 event, 
									U32 data, 
									const SDL_Event* sdlEvent,
									const char* command, 
									const char* arg );

	void SetRotation( int rot );

  private:
	void SetPreview( KrImage* clone );

	Layout* layout;
	XmlUtil* util;

	KrImage* currentImage;
	int buttonOffset;
};

#endif