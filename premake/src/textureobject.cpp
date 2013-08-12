#include "textureobject.h"


TextureObject::TextureObject(GLuint ID, GLsizei Width, GLsizei Height) : m_ID(ID), m_Width(Width), m_Height(Height)
{
}

TextureObject::~TextureObject()
{
	glDeleteTextures(1, &m_ID);
}

void TextureObject::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

GLuint TextureObject::GetID() const
{
	return m_ID;
}

GLsizei TextureObject::GetWidth() const
{
	return m_Width;
}

GLsizei TextureObject::GetHeight() const
{
	return m_Height;
}
