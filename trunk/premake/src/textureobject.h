#ifndef _TEXTUREOBJECT_H_
#define _TEXTUREOBJECT_H_

#include "platformspecific.h"


/*
OpenGL texture object class
*/
class TextureObject
{
protected :
	GLuint						m_ID;											// OpenGL ID for this texture object
	GLsizei						m_Width;										// width of the texture
	GLsizei						m_Height;										// height of the texture


public :
	// type of internal format
	enum FORMAT
	{
		RGB,
		RGBA
	};

	TextureObject(GLuint ID, GLsizei Width, GLsizei Height);
	~TextureObject();

	void Bind(GLenum TextureUnit);

	GLuint GetID() const;
	GLsizei GetWidth() const;
	GLsizei GetHeight() const;
};

#endif
