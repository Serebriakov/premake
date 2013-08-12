#include "renderstate.h"


RenderState::RenderState(GLSL1ShaderProgram *ShaderProgramPtr, BLENDING_MODE Blending) : m_ShaderProgram(ShaderProgramPtr), m_Blending(Blending)
{
}

bool RenderState::operator==(const RenderState &Other)
{
	return (m_ShaderProgram == Other.m_ShaderProgram) && (m_Blending == Other.m_Blending);
}

void RenderState::SetBlending(RenderState::BLENDING_MODE Blending)
{
	m_Blending = Blending;
}

RenderState::BLENDING_MODE RenderState::GetBlending() const
{
	return m_Blending;
}

GLSL1ShaderProgram *RenderState::GetShaderProgram()
{
	return m_ShaderProgram;
}
