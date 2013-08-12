#ifndef _RENDERSTATE_H_
#define _RENDERSTATE_H_

#include <string>

#include "glsl1shaderprogram.h"


/*
Renderstate settings
*/
class RenderState
{
public :
	enum BLENDING_MODE
	{
		BLENDING_DISABLED,														// no color blending
		BLENDING_ENABLED														// default blending
	};


protected :
	GLSL1ShaderProgram			*m_ShaderProgram;								// pointer to the shader used in this renderstate
	BLENDING_MODE				m_Blending;										// type of blending to be done


public :
	RenderState(GLSL1ShaderProgram *ShaderProgramPtr, BLENDING_MODE Blending);

	bool operator==(const RenderState &Other);

	void SetBlending(BLENDING_MODE Blending);

	BLENDING_MODE GetBlending() const;
	GLSL1ShaderProgram *GetShaderProgram();
};

#endif
