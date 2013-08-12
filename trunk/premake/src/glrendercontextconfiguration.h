#ifndef _GL_RENDER_CONTEXT_CONFIGURATION_H_
#define _GL_RENDER_CONTEXT_CONFIGURATION_H_

#include <string>

#include "platformspecific.h"


/*
Stores the configuration of an OpenGL render context
*/
class GLRenderContextConfiguration
{
public :
	enum ERROR_CODE
	{
		OK,
		PARSING_FAILED
	};

	GLsizei						GLVersionMajor;									// major OpenGL version
	GLsizei						GLVersionMinor;									// minor OpenGL version
	GLsizei						Width;											// width of the render surface
	GLsizei						Height;											// height of the render surface
	bool						Fullscreen;										// windowed / fullscreen rendering
	int							BitsPerPixel;									// typically 24 or 32
	unsigned					Multisampling;									// number of samples per fragment, if zero, MSAA is off
	bool						Vsync;											// vertical synchronization enabled/disabled


	GLRenderContextConfiguration();
	ERROR_CODE LoadFromFile(const std::string &Path);
};

#endif
