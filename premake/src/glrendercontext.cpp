#include "glrendercontext.h"
#include "constants.h"
#include "logger.h"


GLRenderContext::GLRenderContext()
{
	m_Created = false;
	#ifdef _PLATFORM_WIN
		Window = NULL;
		GLContext = NULL;
	#endif
}

GLRenderContext::~GLRenderContext()
{
	Destroy();
}

GLRenderContext::ERROR_CODE GLRenderContext::Create(const std::string &ConfigPath)
{
	if( (ConfigPath != "") && (Configuration.LoadFromFile(ConfigPath)) )
	{
		Logger::Instance().Log("ERROR: Loading configuration file failed: " + ConfigPath);
		m_Created = false;
		return LOADING_CONFIG_FAILED;
	}

	#ifdef _PLATFORM_WIN
		// set the rc's attributes
		int err;
		err = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, Configuration.GLVersionMajor);
		err = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, Configuration.GLVersionMinor);
		err = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		err = SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		err = SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, Configuration.Multisampling > 0 ? 1 : 0);
		err = SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, Configuration.Multisampling);
		err = SDL_GL_SetSwapInterval(Configuration.Vsync ? 1 : 0);

		// create the SDL2 window
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | (Configuration.Fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
		Window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Configuration.Width, Configuration.Height, flags);

		if( !Window )
		{
			Logger::Instance().Log("ERROR: Creating SDL window failed");
			return CREATING_WINDOW_FAILED;
		}

		// create the OpenGL context
		GLContext = SDL_GL_CreateContext(Window);

		if( !GLContext )
		{
			Logger::Instance().Log("ERROR: Creating OpenGL context failed");
			return CREATING_RC_FAILED;
		}

		// initialize glew
		if( glewInit() != GLEW_OK )
		{
			Logger::Instance().Log("ERROR: Initializing GLEW failed");
			return INITIALIZING_GLEW_FAILED;
		}

		m_Created = true;

		return OK;
	#else
		// TODO
		return -1;
	#endif
}

void GLRenderContext::Destroy()
{
	if( m_Created )
	{
		#ifdef _PLATFORM_WIN
			SDL_GL_DeleteContext(GLContext);
			SDL_DestroyWindow(Window);
		#endif
		m_Created = false;
	}
}

void GLRenderContext::SwapBuffers() const
{
	#ifdef _PLATFORM_WIN
		SDL_GL_SwapWindow(Window);
	#endif
}
