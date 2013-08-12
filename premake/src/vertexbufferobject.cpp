#include "vertexbufferobject.h"


VertexBufferObject::VertexBufferObject(GLuint ID) : m_ID(ID)
{
}

VertexBufferObject::~VertexBufferObject()
{
	glDeleteBuffers(1, &m_ID);
}

GLuint VertexBufferObject::GetID() const
{
	return m_ID;
}
