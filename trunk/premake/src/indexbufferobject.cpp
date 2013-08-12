#include "indexbufferobject.h"


IndexBufferObject::IndexBufferObject(GLuint ID) : m_ID(ID)
{
}

IndexBufferObject::~IndexBufferObject()
{
	glDeleteBuffers(1, &m_ID);
}

GLuint IndexBufferObject::GetID() const
{
	return m_ID;
}
