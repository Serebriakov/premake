#include "font.h"


Font::Font(RenderState *RenderStatePtr) : m_RenderState(RenderStatePtr)
{
}

/*
Get the image of a character.
If there's no such character in the font,
the function returns a null-pointer.
*/
Image *Font::GetImage(char Character)
{
	std::map<char, Image *>::iterator it = m_Images.find(Character);
	return (it == m_Images.end()) ? (NULL) : (it->second);
}


const RenderState *Font::GetRenderState() const
{
	return m_RenderState;
}
