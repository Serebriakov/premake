#ifndef _GL_RENDER_CONTEXT_H_
#define _GL_RENDER_CONTEXT_H_

#include "glrendercontextconfiguration.h"


class GLRenderContext
{
protected :
	bool						m_Created;										// Create() has been called


public :
	enum ERROR_CODE
	{
		OK,
		LOADING_CONFIG_FAILED,
		CREATING_WINDOW_FAILED,
		CREATING_RC_FAILED,
		INITIALIZING_GLEW_FAILED
	};

	GLRenderContextConfiguration Configuration;									// the configuration currently in use

	#ifdef _PLATFORM_WIN
		SDL_Window				*Window;
		SDL_GLContext			GLContext;
	#endif


	GLRenderContext();
	~GLRenderContext();

	ERROR_CODE Create(const std::string &ConfigFilePath);
	void Destroy();
	void SwapBuffers() const;
};

#endif
