#ifndef _GLSL1_SHADERPROGRAM_H_
#define _GLSL1_SHADERPROGRAM_H_

#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "platformspecific.h"


/*
Wrapper for GLSLv1 shader program objects
*/
class GLSL1ShaderProgram
{
protected :
	GLuint						m_VertexShaderID;								// OpenGL ID of the vertex shader in this program object
	GLuint						m_FragmentShaderID;								// OpenGL ID of the fragment shader in this program object
	GLuint						m_ProgramID;									// OpenGL ID of this program object
	std::map<std::string, GLint> m_UniformLocationCache;						// cache storing uniform locations
	std::map<std::string, GLint> m_AttributeLocationCache;						// cache storing attrib locations


public :
	GLSL1ShaderProgram(GLuint ProgramID, GLuint VertexShaderID, GLuint FragmentShaderID);
	~GLSL1ShaderProgram();

	GLint GetUniformLocation(const std::string &Name);
	GLint GetAttributeLocation(const std::string &Name);
	GLuint GetProgramID() const;

	void SetUniform(const std::string &Name, const glm::mat4x4 &Mat4x4);
	void SetUniform(const std::string &Name, const glm::vec2 &Vec2);
	void SetUniform(const std::string &Name, const glm::vec3 &Vec3);
	void SetUniform(const std::string &Name, const glm::vec4 &Vec4);
	void SetUniform(const std::string &Name, float Value);
	void SetUniform(const std::string &Name, GLint Value);

	void SetAttribArray(const std::string &Name, GLint Components, GLsizei Stride, unsigned int MemoryOffset);
};

#endif
