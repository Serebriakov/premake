#ifndef _GLES_RENDERER_H_
#define _GLES_RENDERER_H_

#include <vector>
#include <set>
#include <glm/gtc/matrix_transform.hpp>

#include "glrendercontext.h"
#include "glsl1shaderprogram.h"
#include "vertexbufferobject.h"
#include "indexbufferobject.h"
#include "textureobject.h"
#include "dictionary.hpp"
#include "renderstate.h"
#include "imagesequence.h"
#include "sprite.h"
#include "font.h"


/*
Renderer using OpenGL ES 2
*/
class GLESRenderer
{
public :
	enum ERROR_CODE
	{
		OK,
		ALREADY_INITIALIZED,
		CREATING_RC_FAILED,
		INITIALIZING_SDL_IMG_FAILED,
		OPENING_FILE_FAILED,
		CREATING_SHADER_OBJECT_FAILED,
		SHADER_COMPILATION_FAILED,
		CREATING_PROGRAM_OBJECT_FAILED,
		LOADING_IMAGE_FAILED,
		CREATING_TEXTURE_OBJECT_FAILED,
		PARSING_FAILED,
		MISSING_IMAGE
	};


private :
	GLbitfield					m_BufferMask;									// the bitmask used when clearing the buffers

	// helper functions for flipping SDL surfaces
	// http://www.gamedev.net/topic/308570-sdl-mirror-surface/
	Uint32 GetPixel(SDL_Surface *Surface, int X, int Y);
	void SetPixel(SDL_Surface *Surface, int X, int Y, Uint32 NewColor);
	SDL_Surface *MirrorSurfaceX(SDL_Surface *Surface);
	SDL_Surface *MirrorSurfaceY(SDL_Surface *Surface);


protected :
	GLRenderContext				*m_RC;											// pointer to the render context to draw into
	glm::mat4					m_ViewMatrix;									// matrix setting the camera's position & orientation
	glm::mat4					m_ProjectionMatrix;								// matrix storing projection settings
	std::vector<RenderState *>	m_RenderStates;									// array of renderstates

	void SetRenderState(RenderState &RS);


	GLSL1ShaderProgram			*m_CurrentlyActiveShader;						// pointer to the currently used shaderprogram
	Dictionary<GLSL1ShaderProgram> m_Shaders;									// associative array of loaded shaders

	void LogShaderInfo(GLuint ID);
	void LogProgramInfo(GLuint ID);
	void UseShader(GLSL1ShaderProgram *ShaderProgram);

	TextureObject				*m_CurrentlyActiveTexture;						// pointer to the currently used texture object
	Dictionary<Image>			m_Images;										// associative array of loaded images
	Dictionary<ImageSequence>	m_ImageSequences;								// associative array of ImageSequence objects
	Dictionary<TextureObject>	m_Textures;										// associative array of loaded textures
	Dictionary<Font>			m_Fonts;										// associative array of loaded fonts

	void UseTexture(TextureObject *Texture);

	
	// type for mapping rendering settings to a vector of vertices and a vector of indices
	typedef std::map<SpriteRenderMode, std::pair<std::vector<SpriteVertexAttributes>, std::vector<GLushort>>> SpriteBatchData;
	SpriteBatchData				m_Sprites;										// client-side storage of vertex attributes and indices of the 2D sprites to be drawn
	VertexBufferObject			*m_VBOSprite2D;									// VBO used for rendering 2D sprites
	IndexBufferObject			*m_IBOSprite2D;									// IBO used for rendering 2D sprites


	static GLESRenderer *RendererInstance;
	GLESRenderer();
	GLESRenderer(const GLESRenderer &Other);
	GLESRenderer &operator=(const GLESRenderer &Other);


public :
	virtual ~GLESRenderer();

	static GLESRenderer *Instance();

	static ERROR_CODE Init(const std::string &ConfigFilePath = "");
	static void Destroy();

	GLRenderContextConfiguration &GetConfiguration();

	virtual void LogBasicInfo();
	virtual void QueryErrors();

	// set the color to fill the framebuffer with when clearing
	virtual void SetClearColor(float R, float G, float B, float A = 1.f);

	virtual void ClearBuffers();

	virtual void ToggleDepthTest(bool State);
	virtual void ToggleBackFaceCulling(bool State);
	virtual void ToggleTexturing(bool State);
	

	// shader management
	virtual ERROR_CODE LoadShaderFromFile(const std::string &VertexShaderPath, const std::string &FragmentShaderPath, const std::string &Alias = "");
	virtual ERROR_CODE LoadShaderFromString(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &Alias);
	virtual GLSL1ShaderProgram *GetShader(const std::string &Alias);
	virtual void ReleaseShader(const std::string &Alias);


	// texture and image management
	virtual ERROR_CODE LoadTextureFromFile(const std::string &TexturePath, bool Mipmapping, bool Compression);
	virtual ERROR_CODE LoadImageSequence(const std::string &FilePath);
	virtual ERROR_CODE LoadImages(const std::string &MapFilePath, const std::string &TexturePath);
	virtual void ReleaseTexture(const std::string &TexturePath);
	virtual void ReleaseImageSequence(const std::string &Alias);
	virtual void ReleaseImage(const std::string &Alias);
	virtual ImageSequence *GetImageSequence(const std::string &Alias);
	virtual Image *GetImage(const std::string &Alias);
	

	// font management
	virtual ERROR_CODE LoadFont(const std::string &FilePath, RenderState *RenderStatePtr, const std::string &Alias = "");
	virtual void ReleaseFont(const std::string &Alias);


	// drawing
	virtual RenderState *AddRenderState(const RenderState &RS);
	virtual void AddSprite2D(const Sprite *NewSprite);
	virtual void AddSprite2D(const Sprite *NewSprite, const glm::vec2 &Position);
	virtual void AddText(const std::string &FontAlias, const std::string &Text, const glm::vec2 &Position, const glm::vec2 &CharacterSizeMultiplier = glm::vec2(1.f, 1.f));
	virtual void FlushSprites();
	virtual void SwapBuffers();


	// projection settings
	virtual void SetProjection2D(float Left, float Right, float Bottom, float Top);
	virtual void SetViewPort(int X, int Y, int Width, int Height);
	virtual void SetCameraPosition(const glm::vec2 &Position);

	const glm::mat4 &GetViewMatrix() const;
	const glm::mat4 &GetProjectionMatrix() const;
};

#endif
