#ifndef _INDEXBUFFEROBJECT_H_
#define _INDEXBUFFEROBJECT_H_

#include "platformspecific.h"


class IndexBufferObject
{
protected :
	GLuint						m_ID;											// OpenGL ID for this IBO


public :
	IndexBufferObject(GLuint ID);
	~IndexBufferObject();
	GLuint GetID() const;
};

#endif
