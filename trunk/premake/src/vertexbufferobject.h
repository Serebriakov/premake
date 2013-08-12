#ifndef _VERTEXBUFFEROBJECT_H_
#define _VERTEXBUFFEROBJECT_H_

#include "platformspecific.h"


class VertexBufferObject
{
protected :
	GLuint						m_ID;											// OpenGL ID for this VBO


public :
	VertexBufferObject(GLuint ID);
	~VertexBufferObject();
	GLuint GetID() const;
};

#endif
