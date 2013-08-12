#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <glm/glm.hpp>

#include "textureobject.h"


/*
Selects a portion of a texture
*/
class Image
{
protected :
	TextureObject				*m_Texture;										// pointer to a texture object
	glm::vec2					m_Size;											// width and height of the image portion, in pixels
	glm::vec2					m_LowerLeftCornerUV;							// texture coordinates of the lower left corner
	glm::vec2					m_UpperRightCornerUV;							// texture coordinates of the upper right corner


public :
	Image(TextureObject *Texture, const glm::vec2 &Size, const glm::vec2 &LowerLeftUV, const glm::vec2 &UpperRightUV);

	const TextureObject *GetTexture() const;
	const glm::vec2 &GetSize() const;
	const glm::vec2 &GetLowerLeftUV() const;
	const glm::vec2 &GetUpperRightUV() const;
};

#endif
