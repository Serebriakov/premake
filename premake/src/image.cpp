#include "image.h"


Image::Image(TextureObject *Texture, const glm::vec2 &Size, const glm::vec2 &LowerLeftUV, const glm::vec2 &UpperRightUV) : m_Texture(Texture), m_Size(Size), m_LowerLeftCornerUV(LowerLeftUV), m_UpperRightCornerUV(UpperRightUV)
{
}

const TextureObject *Image::GetTexture() const
{
	return m_Texture;
}

const glm::vec2 &Image::GetSize() const
{
	return m_Size;
}

const glm::vec2 &Image::GetLowerLeftUV() const
{
	return m_LowerLeftCornerUV;
}

const glm::vec2 &Image::GetUpperRightUV() const
{
	return m_UpperRightCornerUV;
}
