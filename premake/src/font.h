#ifndef _FONT_H_
#define _FONT_H_

#include <map>

#include "image.h"
#include "renderstate.h"


/*
Class of bitmap fonts
*/
class Font
{
protected :
	RenderState					*m_RenderState;									// pointer to the renderstate used by the font


public :
	std::map<char, Image *>		m_Images;										// character -> Image* mapping, the font stores an image for each character

	Font(RenderState *RenderStatePtr);
	Image *GetImage(char Character);
	const RenderState *GetRenderState() const;
};

#endif
