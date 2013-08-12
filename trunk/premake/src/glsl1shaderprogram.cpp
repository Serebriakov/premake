#include <fstream>

#include "glsl1shaderprogram.h"
#include "logger.h"


GLSL1ShaderProgram::GLSL1ShaderProgram(GLuint ProgramID, GLuint VertexShaderID, GLuint FragmentShaderID) : m_ProgramID(ProgramID), m_VertexShaderID(VertexShaderID), m_FragmentShaderID(FragmentShaderID)
{
}

GLSL1ShaderProgram::~GLSL1ShaderProgram()
{
	glDeleteShader(m_VertexShaderID);
	glDeleteShader(m_FragmentShaderID);
	glDeleteProgram(m_ProgramID);
}


GLint GLSL1ShaderProgram::GetUniformLocation(const std::string &Name)
{
	GLint location;

	// check if the location of the uniform is cached
	std::map<std::string, GLint>::iterator it = m_UniformLocationCache.find(Name);
	// if it is not, get it and put it in the cache
	if( it == m_UniformLocationCache.end() )
	{
		location = glGetUniformLocation(m_ProgramID, Name.c_str());
		if( location == -1 )
		{
			//
			// Logger::Instance().Log(std::string("ERROR: Invalid uniform variable name '") + Name + "' (inactive or not existing)");
			//
		}
		else
		{
			m_UniformLocationCache[Name] = location;
		}
	}
	// return the cached value
	else
	{
		location = it->second;
	}

	return location;
}

/*
get the location of an attribute variable
*/
GLint GLSL1ShaderProgram::GetAttributeLocation(const std::string &Name)
{
	GLint location;

	// check if the location of the attribute is cached
	std::map<std::string, GLint>::iterator it = m_AttributeLocationCache.find(Name);
	// if it is not, get it and put it in the cache
	if( it == m_AttributeLocationCache.end() )
	{
		location = glGetAttribLocation(m_ProgramID, Name.c_str());
		if( location == -1 )
		{
			//
			// Logger::Instance().Log(std::string("ERROR: Invalid attribute variable name '") + Name + "' (inactive or not existing)");
			//
		}
		else
		{
			m_AttributeLocationCache[Name] = location;
		}
	}
	// return the cached value
	else
	{
		location = it->second;
	}

	return location;
}

GLuint GLSL1ShaderProgram::GetProgramID() const
{
	return m_ProgramID;
}

void GLSL1ShaderProgram::SetUniform(const std::string &Name, const glm::mat4x4 &Mat4x4)
{
	glUniformMatrix4fv(GetUniformLocation(Name), 1, GL_FALSE, glm::value_ptr(Mat4x4));
}

void GLSL1ShaderProgram::SetUniform(const std::string &Name, const glm::vec2 &Vec2)
{
	glUniform2fv(GetUniformLocation(Name), 1, glm::value_ptr(Vec2));
}

void GLSL1ShaderProgram::SetUniform(const std::string &Name, const glm::vec3 &Vec3)
{
	glUniform3fv(GetUniformLocation(Name), 1, glm::value_ptr(Vec3));
}

void GLSL1ShaderProgram::SetUniform(const std::string &Name, const glm::vec4 &Vec4)
{
	glUniform4fv(GetUniformLocation(Name), 1, glm::value_ptr(Vec4));
}

void GLSL1ShaderProgram::SetUniform(const std::string &Name, float Value)
{
	glUniform1fv(GetUniformLocation(Name), 1, &Value);
}

void GLSL1ShaderProgram::SetUniform(const std::string &Name, GLint Value)
{
	glUniform1i(GetUniformLocation(Name), Value);
}

void GLSL1ShaderProgram::SetAttribArray(const std::string &Name, GLint Components, GLsizei Stride, unsigned int MemoryOffset)
{
	GLint location = GetAttributeLocation(Name);
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, Components, GL_FLOAT, GL_FALSE, Stride, (GLvoid*)(MemoryOffset));
}
