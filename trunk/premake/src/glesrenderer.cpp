#include <fstream>
#include <sstream>
#include <algorithm>
#include <SDL_image.h>
#include <json/json.h>

#include "platformspecific.h"
#include "glesrenderer.h"
#include "logger.h"


// --- static members ---
GLESRenderer *GLESRenderer::RendererInstance = NULL;


/*
private helper function for flipping SDL surfaces
http://www.gamedev.net/topic/308570-sdl-mirror-surface/
*/
Uint32 GLESRenderer::GetPixel(SDL_Surface *Surface, int X, int Y)
{
	int bpp = Surface->format->BytesPerPixel;
	
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8*)Surface->pixels + Y * Surface->pitch + X * bpp;

	switch(bpp)
	{
	case 1:
		return *p;

	case 2:
		return *(Uint16 *)p;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;

	case 4:
		return *(Uint32 *)p;
	}

	return 0;
}

/*
private helper function for flipping SDL surfaces
http://www.gamedev.net/topic/308570-sdl-mirror-surface/
*/
void GLESRenderer::SetPixel(SDL_Surface *Surface, int X, int Y, Uint32 NewColor)
{
	Uint8 *ubuff8;
	Uint16 *ubuff16;
	Uint32 *ubuff32;
	Uint32 color = NewColor;
	char c1, c2, c3;

	/* Lock the Surface, if needed */
	if( SDL_MUSTLOCK(Surface) )
	{
		if(SDL_LockSurface(Surface) < 0) 
			return;
	}

	/* How we draw the pixel depends on the bitdepth */
	switch(Surface->format->BytesPerPixel) 
	{
	case 1: 
		ubuff8 = (Uint8*) Surface->pixels;
		ubuff8 += (Y * Surface->pitch) + X;
		*ubuff8 = (Uint8)color;
		break;

	case 2:
		ubuff8 = (Uint8*) Surface->pixels;
		ubuff8 += (Y * Surface->pitch) + (X * 2);
		ubuff16 = (Uint16*)ubuff8;
		*ubuff16 = (Uint16)color; 
		break;  

	case 3:
		ubuff8 = (Uint8*)Surface->pixels;
		ubuff8 += (Y * Surface->pitch) + (X * 3);

		if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
		{
			c1 = (color & 0xFF0000) >> 16;
			c2 = (color & 0x00FF00) >> 8;
			c3 = (color & 0x0000FF);
		}
		else
		{
			c3 = (color & 0xFF0000) >> 16;
			c2 = (color & 0x00FF00) >> 8;
			c1 = (color & 0x0000FF);	
		}

		ubuff8[0] = c3;
		ubuff8[1] = c2;
		ubuff8[2] = c1;
		break;

	case 4:
		ubuff8 = (Uint8*) Surface->pixels;
		ubuff8 += (Y * Surface->pitch) + (X * 4);
		ubuff32 = (Uint32*)ubuff8;
		*ubuff32 = color;
		break;
	}

	if( SDL_MUSTLOCK(Surface) )
	{
		SDL_UnlockSurface(Surface);
	}
}

SDL_Surface *GLESRenderer::MirrorSurfaceX(SDL_Surface *Surface)
{
	SDL_Surface *new_surface = SDL_CreateRGBSurface(0, Surface->w, Surface->h, Surface->format->BitsPerPixel, Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);

	for( int y = 0; y < Surface->h; ++y )
	{
		for( int x = 0; x < Surface->w; ++x )
		{
			SetPixel(new_surface, x, y, GetPixel( Surface, Surface->w - x - 1, y ));
		}
	}

	return new_surface;
}

SDL_Surface *GLESRenderer::MirrorSurfaceY(SDL_Surface *Surface)
{
	SDL_Surface* new_surface = SDL_CreateRGBSurface(0, Surface->w, Surface->h, Surface->format->BitsPerPixel, Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);

	for( int y = 0; y < Surface->h; ++y )
	{
		for( int x = 0; x < Surface->w; ++x )
		{
			SetPixel(new_surface, x, y, GetPixel(Surface, x, Surface->h - y - 1 ));
		}
	}

	return new_surface;
}

void GLESRenderer::SetRenderState(RenderState &RS)
{
	// blending settings
	switch( RS.GetBlending() )
	{
		case RenderState::BLENDING_DISABLED :
		{
			glDisable(GL_BLEND);
			break;
		}

		case RenderState::BLENDING_ENABLED :
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		}
	}

	// activate the shader
	UseShader(RS.GetShaderProgram());
}

/*
Log shader compilation messages
*/
void GLESRenderer::LogShaderInfo(GLuint ID)
{
	GLint info_length = 0;
	glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &info_length);
	if( info_length > 1 )
	{
		char *info_log = new(std::nothrow) char[info_length];
		if( info_log )
		{
			glGetShaderInfoLog(ID, info_length, NULL, info_log);
			Logger::Instance().Log(info_log);
			delete[] info_log;
		}
	}
	else
	{
		Logger::Instance().Log("  shader compiled without any message");
	}
}

/*
Log program linkage messages
*/
void GLESRenderer::LogProgramInfo(GLuint ID)
{
	GLint info_length = 0;
	glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &info_length);
	if( info_length > 1 )
	{
		char *info_log = new(std::nothrow) char[info_length];
		if( info_log )
		{
			glGetProgramInfoLog(ID, info_length, NULL, info_log);
			Logger::Instance().Log(info_log);
			delete[] info_log;
		}
	}
	else
	{
		Logger::Instance().Log("  program object linked without any message");
	}
}

void GLESRenderer::UseShader( GLSL1ShaderProgram *ShaderProgram )
{
	if( m_CurrentlyActiveShader != ShaderProgram )
	{
		m_CurrentlyActiveShader = ShaderProgram;
		glUseProgram(m_CurrentlyActiveShader->GetProgramID());
	}
}

void GLESRenderer::UseTexture(TextureObject *Texture)
{
	if( m_CurrentlyActiveTexture != Texture )
	{
		m_CurrentlyActiveTexture = Texture;
		m_CurrentlyActiveTexture->Bind(GL_TEXTURE0);
	}
}


GLESRenderer::GLESRenderer()
{
	m_RC = new GLRenderContext();

	m_CurrentlyActiveShader = NULL;
	m_CurrentlyActiveTexture = NULL;

	m_VBOSprite2D = NULL;
	m_IBOSprite2D = NULL;

	m_BufferMask = GL_COLOR_BUFFER_BIT;
}

GLESRenderer::~GLESRenderer()
{
	IMG_Quit();
	delete m_VBOSprite2D;
	delete m_IBOSprite2D;
	delete m_RC;

	for( size_t i = 0; i < m_RenderStates.size(); ++i )
	{
		delete m_RenderStates[i];
	}
}

GLESRenderer *GLESRenderer::Instance()
{
	return RendererInstance;
}

GLESRenderer::ERROR_CODE GLESRenderer::Init(const std::string &ConfigFilePath)
{
	if( RendererInstance != NULL )
	{
		return ALREADY_INITIALIZED;
	}

	RendererInstance = new GLESRenderer();

	// create or recreate render context
	RendererInstance->m_RC->Destroy();
	if( RendererInstance->m_RC->Create(ConfigFilePath) != OK )
	{
		Logger::Instance().Log("ERROR: Creating render context failed");
		return CREATING_RC_FAILED;
	}

	// initialize SDL_image library
	if( (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG)) != (IMG_INIT_JPG | IMG_INIT_PNG) )
	{
		Logger::Instance().Log(std::string("ERROR: Initializing SDL_image failed: ") + IMG_GetError());
		return INITIALIZING_SDL_IMG_FAILED;
	}

	// use default projection & viewport settings
	RendererInstance->SetViewPort(0, 0, RendererInstance->m_RC->Configuration.Width, RendererInstance->m_RC->Configuration.Height);

	// initialize 2D sprite rendering
	GLuint id;
	glGenBuffers(1, &id);
	RendererInstance->m_VBOSprite2D = new VertexBufferObject(id);
	glGenBuffers(1, &id);
	RendererInstance->m_IBOSprite2D = new IndexBufferObject(id);

	// global OpenGL settings
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	RendererInstance->ToggleBackFaceCulling(true);
	RendererInstance->ToggleDepthTest(false);
	RendererInstance->ToggleTexturing(true);

	return OK;
}

void GLESRenderer::Destroy()
{
	delete RendererInstance;
	RendererInstance = NULL;
}

GLRenderContextConfiguration &GLESRenderer::GetConfiguration()
{
	return m_RC->Configuration;
}

void GLESRenderer::LogBasicInfo()
{
	Logger::Instance().Log("OpenGL ES renderer info:");

	Logger::Instance().Log(std::string("  Vendor: ") + (const char *)glGetString(GL_VENDOR));
	Logger::Instance().Log(std::string("  Renderer: ") + (const char *)glGetString(GL_RENDERER));
	Logger::Instance().Log(std::string("  OpenGL version: ") + (const char *)glGetString(GL_VERSION));
	Logger::Instance().Log(std::string("  Shading language version: ") + (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void GLESRenderer::QueryErrors()
{
	int error_code;
	if( (error_code = glGetError()) != GL_NO_ERROR )
	{
		Logger::Instance().Log(std::string("OpenGL error: ") + (const char*)gluErrorString(error_code));
	}
}

/*
set the color to fill the framebuffer with when clearing
*/
void GLESRenderer::SetClearColor(float R, float G, float B, float A)
{
	glClearColor(R, G, B, A);
}

void GLESRenderer::ClearBuffers()
{
	glClear(m_BufferMask);
}

void GLESRenderer::ToggleDepthTest(bool State)
{
	if( State )
	{
		glEnable(GL_DEPTH_TEST);
		m_BufferMask |= GL_DEPTH_BUFFER_BIT;
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
		m_BufferMask &= ~(GL_DEPTH_BUFFER_BIT);
	}
}

void GLESRenderer::ToggleBackFaceCulling(bool State)
{
	if( State )
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
}

void GLESRenderer::ToggleTexturing(bool State)
{
	if( State )
	{
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		m_CurrentlyActiveTexture = NULL;
	}
}


/*
Load shader from file.
Creates a GLSL1 shader object reading the source from files.
If the reading and compilation is successful, the created shader object is stored
and can later be accessed with its alias.
Default value for the alias is: the two paths concatenated.
*/
GLESRenderer::ERROR_CODE GLESRenderer::LoadShaderFromFile(const std::string &VertexShaderPath, const std::string &FragmentShaderPath, const std::string &Alias)
{
	// open the files
	std::ifstream vs_file(VertexShaderPath.c_str());
	std::ifstream fs_file(FragmentShaderPath.c_str());
	if( vs_file.is_open() == false )
	{
		Logger::Instance().Log("ERROR: Couldn't open vertex shader file " + VertexShaderPath);
		return OPENING_FILE_FAILED;
	}
	if( fs_file.is_open() == false )
	{
		Logger::Instance().Log("ERROR: Couldn't open fragment shader file " + FragmentShaderPath);
		return OPENING_FILE_FAILED;
	}

	// read the contents of the shader source files
	std::string vs_src((std::istreambuf_iterator<char>(vs_file)), std::istreambuf_iterator<char>());
	std::string fs_src((std::istreambuf_iterator<char>(fs_file)), std::istreambuf_iterator<char>());
	vs_file.close();
	fs_file.close();

	// utilize the other function...
	return LoadShaderFromString(vs_src, fs_src, (Alias == "") ? (VertexShaderPath + "|" + FragmentShaderPath) : (Alias));
}

/*
Load shader from memory.
Creates a GLSL1 shader object reading the source from the given strings.
If the compilation is successful, the created shader object is stored
and can later be accessed with its alias.
The alias must be specified.
*/
GLESRenderer::ERROR_CODE GLESRenderer::LoadShaderFromString(const std::string &VertexShaderSource, const std::string &FragmentShaderSource, const std::string &Alias)
{
	// if there is already a shader object in the container with this alias, delete that first
	if( m_Shaders.Find(Alias) != m_Shaders.IteratorEnd() )
	{
		Logger::Instance().Log("WARNING: Overwriting shader: " + Alias);
		m_Shaders.Erase(Alias);
	}

	// create shader objects
	GLuint vs_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs_id = glCreateShader(GL_FRAGMENT_SHADER);
	if( (vs_id == 0) || (fs_id == 0) )
	{
		Logger::Instance().Log("ERROR: glCreateShader failed");
		return CREATING_SHADER_OBJECT_FAILED;
	}

	// compile shader objects
	GLint vs_result;
	GLint fs_result;
	const GLchar *vs_src[] = { VertexShaderSource.c_str() };
	const GLchar *fs_src[] = { FragmentShaderSource.c_str() };
	glShaderSource(vs_id, 1, vs_src, NULL);
	glShaderSource(fs_id, 1, fs_src, NULL);
	glCompileShader(vs_id);
	glCompileShader(fs_id);
	glGetShaderiv(vs_id, GL_COMPILE_STATUS, &vs_result);
	glGetShaderiv(fs_id, GL_COMPILE_STATUS, &fs_result);
	#ifdef _DEBUG
		Logger::Instance().Log("Vertex shader log:");
		LogShaderInfo(vs_id);
		Logger::Instance().Log("Fragment shader log:");
		LogShaderInfo(fs_id);
	#endif
	if( (vs_result == GL_FALSE) || (fs_result == GL_FALSE) )
	{
		Logger::Instance().Log("ERROR: shader compilation failed");
		glDeleteShader(vs_id);
		glDeleteShader(fs_id);
		return SHADER_COMPILATION_FAILED;
	}

	// create shader program
	GLuint program_id = glCreateProgram();
	if( program_id == 0 )
	{
		Logger::Instance().Log("ERROR: glCreateProgram failed");
		glDeleteShader(vs_id);
		glDeleteShader(fs_id);
		return CREATING_PROGRAM_OBJECT_FAILED;
	}

	// attach shaders to the program object and link it
	GLint program_result;
	glAttachShader(program_id, vs_id);
	glAttachShader(program_id, fs_id);
	glLinkProgram(program_id);
	glGetProgramiv(program_id, GL_LINK_STATUS, &program_result);
	#ifdef _DEBUG
		Logger::Instance().Log("Linking shader program:");
		LogProgramInfo(program_id);
	#endif
	if( program_result == GL_FALSE )
	{
		glDeleteShader(vs_id);
		glDeleteShader(fs_id);
		glDeleteProgram(program_id);
	}

	// create shader object and store it
	m_Shaders.Insert(Alias, new GLSL1ShaderProgram(program_id, vs_id, fs_id));

	return OK;
}

/*
Find shaders by alias.
If the shader with the specified alias cannot be found, a nullpointer is returned.
*/
GLSL1ShaderProgram *GLESRenderer::GetShader(const std::string &Alias)
{
	Dictionary<GLSL1ShaderProgram>::iterator it = m_Shaders.Find(Alias);
	return (it == m_Shaders.IteratorEnd()) ? (NULL) : (it->second);
}

/*
Release the shaders object with the alias specified
*/
void GLESRenderer::ReleaseShader(const std::string &Alias)
{
	Dictionary<GLSL1ShaderProgram>::iterator it = m_Shaders.Find(Alias);
	if( it != m_Shaders.IteratorEnd() )
	{
		if( it->second == m_CurrentlyActiveShader )
		{
			m_CurrentlyActiveShader = NULL;
		}
		m_Shaders.Erase(Alias);
	}
}

/*
Load texture from file.
Creates an OpenGL texture object reading the contents of a file.
If the reading and creation is successful, the created texture object is stored
and can later be accessed with its alias, which is the path the texture was loaded from.
*/
GLESRenderer::ERROR_CODE GLESRenderer::LoadTextureFromFile(const std::string &TexturePath, bool Mipmapping, bool Compression)
{
	// don't load the same thing twice
	if( m_Textures.Find(TexturePath) != m_Textures.IteratorEnd() )
	{
		return OK;
	}

	// load the texture file
	SDL_Surface *surface = IMG_Load(TexturePath.c_str());
	if( !surface )
	{
		Logger::Instance().Log(std::string("ERROR: Loading image file '") + TexturePath + "' failed: " + IMG_GetError());
		return LOADING_IMAGE_FAILED;
	}

	// mirror the SDL surface
	const bool flip_image = true;
	if( flip_image )
	{
		SDL_Surface *surface_flipped = MirrorSurfaceY(surface);
		SDL_FreeSurface(surface);
		surface = surface_flipped;
	}

	// create texture object
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	if( texture_id == 0 )
	{
		Logger::Instance().Log("ERROR: glGenTextures failed");
		SDL_FreeSurface(surface);
		return CREATING_TEXTURE_OBJECT_FAILED;
	}
	glBindTexture(GL_TEXTURE_2D, texture_id);
	m_Textures.Insert(TexturePath, new TextureObject(texture_id, surface->w, surface->h));

	// create texture level 0
	GLenum format = (surface->format->BytesPerPixel == 4) ? (GL_RGBA) : (GL_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// TO BE FIXED
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// create mipmap chain if required
	if( Mipmapping )
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	// TODO: texture compression

	// free client-side memory
	SDL_FreeSurface(surface);

	return OK;
}

/*
Creates and stores an ImageSequence object (if not yet created).
FilePath is the json file describing what texture atlases are required,
and the list of image names the sequence consists of.
FilePath is also used as the key of the ImageSequence object.
*/
GLESRenderer::ERROR_CODE GLESRenderer::LoadImageSequence(const std::string &FilePath)
{
	// if the sequence is already loaded, there's nothing to do
	if( m_ImageSequences.Find(FilePath) != m_ImageSequences.IteratorEnd() )
	{
		return OK;
	}
	
	// open and read the contents of sequence descriptor file
	std::ifstream file(FilePath.c_str());
	Json::Reader reader;
	Json::Value root;
	bool read_successful = reader.parse(file, root, false);
	if( read_successful == false )
	{
		Logger::Instance().Log("ERROR: Couldn't parse image sequence file: " + FilePath);
		Logger::Instance().Log(reader.getFormatedErrorMessages());
		return PARSING_FAILED;
	}
	file.close();

	// load the required images
	const Json::Value &images = root["images"];
	for( unsigned i = 0, size = images.size(); i < size; ++i )
	{
		const Json::Value &v = images[i];
		LoadImages(v["map_path"].asString(), v["atlas_path"].asString());
	}

	// create a new ImageSequence object
	ImageSequence::PLAYBACK_TYPE playback_type;
	const std::string &playback_type_str = root["playback_type"].asString();
	if( playback_type_str == "loop" )
	{
		playback_type = ImageSequence::LOOP;
	}
	else if( playback_type_str == "forward_backward" )
	{
		playback_type = ImageSequence::FORWARD_BACKWARD;
	}
	else if( playback_type_str == "once" )
	{
		playback_type = ImageSequence::ONCE;
	}

	unsigned frametime = root["frametime"].asUInt();

	ImageSequence *img_seq = new ImageSequence(playback_type, frametime);

	// add images to the sequence
	const Json::Value &sequence = root["sequence"];
	for( unsigned i = 0, size = sequence.size(); i < size; ++i )
	{
		const Json::Value &v = sequence[i];
		Image *img = GetImage(v.asString());
		if( img == NULL )
		{
			delete img_seq;
			return MISSING_IMAGE;
		}

		img_seq->AddImage(img);
	}

	// store the image sequence object
	m_ImageSequences.Insert(FilePath, img_seq);

	return OK;
}

/*
Parse a texture atlas descriptor file and add elements to the Images dictionary.
If the texture with the specified path is already loaded, the function uses that object,
otherwise it loads and stores the texture.
*/
GLESRenderer::ERROR_CODE GLESRenderer::LoadImages(const std::string &MapFilePath, const std::string &TexturePath)
{
	TextureObject *texture_ptr;

	// if the texture has not been loaded, load it now
	Dictionary<TextureObject>::iterator it = m_Textures.Find(TexturePath);
	if( it == m_Textures.IteratorEnd() )
	{
		ERROR_CODE err = LoadTextureFromFile(TexturePath, false, false);
		if( err != OK )
		{
			return err;
		}

		texture_ptr = m_Textures.Find(TexturePath)->second;
	}
	else
	{
		texture_ptr = it->second;
	}

	// open the file describing the mappings
	std::ifstream file(MapFilePath.c_str());
	if( file.is_open() == false )
	{
		Logger::Instance().Log("ERROR: Couldn't open atlas map file: " + MapFilePath);
		return OPENING_FILE_FAILED;
	}

	// read the contents line by line
	std::string line;
	std::string alias;
	char dummy;
	unsigned x, y;
	unsigned w, h;
	unsigned texture_w = texture_ptr->GetWidth();
	unsigned texture_h = texture_ptr->GetHeight();
	glm::vec2 lower_left, upper_right;
	while( file.good() )
	{
		std::getline(file, line);
		std::stringstream ss(line);

		// tokenize the line
		ss >> alias >> dummy >> x >> y >> w >> h;

		// calc. and store the UV coords.
		float w_p = (float)w / (float)texture_w;
		float h_p = (float)h / (float)texture_h;

		upper_right.x = (float)x / (float)texture_w + w_p;
		upper_right.y = 1.f - ((float)y / (float)texture_h);
		lower_left.x = (float)x / (float)texture_w;
		lower_left.y = 1.f - ((float)y / (float)texture_h) - h_p;
		
		m_Images.Insert(alias, new Image(texture_ptr, glm::vec2(w, h), lower_left, upper_right));
	}
	file.close();

	return OK;
}

void GLESRenderer::ReleaseTexture(const std::string &TexturePath)
{
	m_Textures.Erase(TexturePath);
}

/*
Release an ImageSequence object
*/
void GLESRenderer::ReleaseImageSequence(const std::string &Alias)
{
	m_ImageSequences.Erase(Alias);
}

/*
Release an image
*/
void GLESRenderer::ReleaseImage(const std::string &Alias)
{
	m_Images.Erase(Alias);
}

/*
Get pointer to an imagesequence identified by its alias.
If the object cannot be found, a nullpointer is returned.
*/
ImageSequence *GLESRenderer::GetImageSequence(const std::string &Alias)
{
	Dictionary<ImageSequence>::iterator it = m_ImageSequences.Find(Alias);

	if( it == m_ImageSequences.IteratorEnd() )
	{
		Logger::Instance().Log("ERROR: Cannot find imagesequence: " + Alias);
		return NULL;
	}

	return it->second;
}

/*
Get pointer to an image identified by its alias.
If the image cannot be found, a nullpointer is returned.
*/
Image *GLESRenderer::GetImage(const std::string &Alias)
{
	Dictionary<Image>::iterator it = m_Images.Find(Alias);

	if( it == m_Images.IteratorEnd() )
	{
		Logger::Instance().Log("ERROR: Cannot find image: " + Alias);
		return NULL;
	}

	return it->second;
}


/*
Load a font
*/
GLESRenderer::ERROR_CODE GLESRenderer::LoadFont(const std::string &FilePath, RenderState *RenderStatePtr, const std::string &Alias)
{
	// if the alias is not specified, use the file path
	const std::string &alias = (Alias == "") ? FilePath : Alias;

	// don't load the same thing twice
	if( m_Fonts.Find(alias) != m_Fonts.IteratorEnd() )
	{
		return OK;
	}

	// open and read the contents of the font descriptor file
	std::ifstream file(FilePath.c_str());
	Json::Reader reader;
	Json::Value root;
	bool read_successful = reader.parse(file, root, false);
	if( read_successful == false )
	{
		Logger::Instance().Log("ERROR: Couldn't parse font descriptor file: " + FilePath);
		Logger::Instance().Log(reader.getFormatedErrorMessages());
		return PARSING_FAILED;
	}
	file.close();

	// load the images required by the font
	const Json::Value &images = root["images"];
	for( unsigned i = 0, size = images.size(); i < size; ++i )
	{
		const Json::Value &v = images[i];
		GLESRenderer::Instance()->LoadImages(v["map_path"].asString(), v["atlas_path"].asString());
	}

	// create a new font object and add characters to it
	Font *font = new Font(RenderStatePtr);
	const Json::Value &characters = root["characters"];
	for( unsigned i = 0, size = characters.size(); i < size; ++i )
	{
		const Json::Value &v = characters[i];
		char c = v["character"].asString()[0];
		Image *img = GetImage(v["image"].asString());
		font->m_Images[c] = img;
	}

	// store the font object
	m_Fonts.Insert(alias, font);

	return OK;
}

void GLESRenderer::ReleaseFont(const std::string &Alias)
{
	m_Fonts.Erase(Alias);
}


/*
Stores a RenderState object and returns its address
*/
RenderState *GLESRenderer::AddRenderState(const RenderState &RS)
{
	// check if an identical renderstate is already in the array
	for( std::vector<RenderState *>::iterator it = m_RenderStates.begin(), end = m_RenderStates.end(); it != end; ++it )
	{
		if( (**it) == RS )
		{
			return *it;
		}
	}

	m_RenderStates.push_back(new RenderState(RS));
	return m_RenderStates.back();
}

/*
Add a 2D sprite to the queue
*/
void GLESRenderer::AddSprite2D(const Sprite *NewSprite)
{
	if( NewSprite == NULL )
	{
		return;
	}

	AddSprite2D(NewSprite, NewSprite->GetPosition());
}

/*
Add a 2D sprite, at a specified position instead of the sprite's position
*/
void GLESRenderer::AddSprite2D(const Sprite *NewSprite, const glm::vec2 &Position)
{
	if( NewSprite == NULL )
	{
		return;
	}

	SpriteRenderMode render_mode(NewSprite->GetCurrentImage()->GetTexture(), NewSprite->GetRenderState());

	std::pair<std::vector<SpriteVertexAttributes>, std::vector<GLushort>> &it= m_Sprites[render_mode];
	std::vector<SpriteVertexAttributes> &vertices = it.first;
	std::vector<GLushort> &indices = it.second;

	glm::vec2 lower_left_uv, upper_right_uv;
	if( NewSprite->IsFlippedHorizontally() )
	{
		const Image *img_ptr = NewSprite->GetCurrentImage();
		lower_left_uv = glm::vec2(img_ptr->GetUpperRightUV().x, img_ptr->GetLowerLeftUV().y);
		upper_right_uv = glm::vec2(img_ptr->GetLowerLeftUV().x, img_ptr->GetUpperRightUV().y);
	}
	else
	{
		lower_left_uv = NewSprite->GetCurrentImage()->GetLowerLeftUV();
		upper_right_uv = NewSprite->GetCurrentImage()->GetUpperRightUV();
	}

	// calculate and store vertex attributes
	SpriteVertexAttributes attributes;
	attributes.PositionX = Position.x;
	attributes.PositionY = Position.y;
	attributes.TextureCoordinateX = lower_left_uv.x;
	attributes.TextureCoordinateY = upper_right_uv.y;
	vertices.push_back(attributes);						// vertex 0 (upper left corner)
	attributes.PositionY += NewSprite->GetSize().y;
	attributes.TextureCoordinateX = lower_left_uv.x;
	attributes.TextureCoordinateY = lower_left_uv.y;
	vertices.push_back(attributes);						// vertex 1 (lower left corner)
	attributes.PositionX += NewSprite->GetSize().x;
	attributes.TextureCoordinateX = upper_right_uv.x;
	attributes.TextureCoordinateY = lower_left_uv.y;
	vertices.push_back(attributes);						// vertex 2 (lower right corner)
	attributes.PositionY = Position.y;
	attributes.TextureCoordinateX = upper_right_uv.x;
	attributes.TextureCoordinateY = upper_right_uv.y;
	vertices.push_back(attributes);						// vertex 3 (upper right corner)

	// store the indices
	GLushort last_index = vertices.size() - 4;
	indices.push_back(last_index);
	indices.push_back(last_index + 1);
	indices.push_back(last_index + 2);
	indices.push_back(last_index + 2);
	indices.push_back(last_index + 3);
	indices.push_back(last_index);

	/*
	if there are so many sprites in this array that
	adding another one would cause an overflow,
	flush the sprites now
	*/
	if( last_index + 4 >= USHRT_MAX - 4 )
	{
		FlushSprites();
	}
}

/*
Add text to the queue. Each character of the text is added as a simple 2D sprite.
FontAlias selects the font to be used, Position is the lower left corner of the text,
and CharacterSizeMultiplier can be used to scale the images used by the characters.
*/
void GLESRenderer::AddText(const std::string &FontAlias, const std::string &Text, const glm::vec2 &Position, const glm::vec2 &CharacterSizeMultiplier)
{
	// find the font to be used
	Dictionary<Font>::iterator font = m_Fonts.Find(FontAlias);
	
	#ifdef _DEBUG
		if( font == m_Fonts.IteratorEnd() )
		{
			Logger::Instance().Log("ERROR: Missing font: " + FontAlias);
			return;
		}

		if( Text.length() == 0 )
		{
			Logger::Instance().Log("WARNING: Rendering an empty string doesn't make much sense to me...");
			return;
		}
	#endif

	// create a RenderMode according to the first valid character of the text
	// WARNING: it won't work if the font uses images stored in more than one texture
	// a new RenderMode should be constructed for each character of the text, inside the loop...
	Image *img = NULL;
	for( size_t i = 0, length = Text.length(); (i < length) && (img == NULL); ++i )
	{
		img = font->second->GetImage(Text[i]);
	}
	if( img == NULL )
	{
		return;
	}		
	SpriteRenderMode render_mode(img->GetTexture(), font->second->GetRenderState());
	
	// get references to the appropriate sprite data containers
	std::pair<std::vector<SpriteVertexAttributes>, std::vector<GLushort>> &it= m_Sprites[render_mode];
	std::vector<SpriteVertexAttributes> &vertices = it.first;
	std::vector<GLushort> &indices = it.second;

	// add the characters of the text as 2D sprites
	SpriteVertexAttributes attributes;
	glm::vec2 current_pos = Position;
	for( size_t i = 0, length = Text.length(); i < length; ++i )
	{
		// get the image of the current character
		Image *img = font->second->GetImage(Text[i]);

		// skip characters with missing image
		if( img == NULL )
		{
			continue;
		}

		// size of the current character
		glm::vec2 current_size = img->GetSize() * CharacterSizeMultiplier;

		// store the vertices
		attributes.PositionX = current_pos.x;
		attributes.PositionY = current_pos.y - current_size.y;
		attributes.TextureCoordinateX = img->GetLowerLeftUV().x;
		attributes.TextureCoordinateY = img->GetUpperRightUV().y;
		vertices.push_back(attributes);						// vertex 0 (upper left corner)
		attributes.PositionY = current_pos.y;
		attributes.TextureCoordinateX = img->GetLowerLeftUV().x;
		attributes.TextureCoordinateY = img->GetLowerLeftUV().y;
		vertices.push_back(attributes);						// vertex 1 (lower left corner)
		attributes.PositionX += current_size.x;
		attributes.TextureCoordinateX = img->GetUpperRightUV().x;
		attributes.TextureCoordinateY = img->GetLowerLeftUV().y;
		vertices.push_back(attributes);						// vertex 2 (lower right corner)
		attributes.PositionY = current_pos.y - current_size.y;;
		attributes.TextureCoordinateX = img->GetUpperRightUV().x;
		attributes.TextureCoordinateY = img->GetUpperRightUV().y;
		vertices.push_back(attributes);						// vertex 3 (upper right corner)

		// move the "cursor"
		current_pos.x += current_size.x;

		// store the indices
		GLushort last_index = vertices.size() - 4;
		indices.push_back(last_index);
		indices.push_back(last_index + 1);
		indices.push_back(last_index + 2);
		indices.push_back(last_index + 2);
		indices.push_back(last_index + 3);
		indices.push_back(last_index);

		/*
		if there are so many sprites in this array that
		adding another one would cause an overflow,
		flush the sprites now
		*/
		if( last_index + 4 >= USHRT_MAX - 4 )
		{
			FlushSprites();
		}
	}
}

/*
Render all the sprites that has been added with AddSprite2D(...)
*/
void GLESRenderer::FlushSprites()
{
	// bind the VBO & IBO of sprites
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOSprite2D->GetID());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBOSprite2D->GetID());

	// for all the different rendering modes
	for( SpriteBatchData::iterator it = m_Sprites.begin(), end = m_Sprites.end(); it != end; ++it )
	{
		// skip empty lists
		if( it->second.second.size() == 0 )
		{
			continue;
		}

		// upload data to VRAM
		glBufferData(GL_ARRAY_BUFFER, it->second.first.size() * sizeof(SpriteVertexAttributes), (GLvoid*)(&it->second.first[0]), GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, it->second.second.size() * sizeof(GLushort), (GLvoid*)(&it->second.second[0]), GL_DYNAMIC_DRAW);

		// set the renderstate and bind the texture
		SetRenderState(*(it->first.RenderStatePtr));
		UseTexture(it->first.Texture);
		// set the shader variables
		GLSL1ShaderProgram *shader_ptr = it->first.RenderStatePtr->GetShaderProgram();
		shader_ptr->SetAttribArray("a_Position", 2, sizeof(SpriteVertexAttributes), 0);
		shader_ptr->SetAttribArray("a_TextureCoordinate", 2, sizeof(SpriteVertexAttributes), sizeof(float) * 2);
		shader_ptr->SetUniform("u_Texture", 0);
		shader_ptr->SetUniform("u_View", m_ViewMatrix);
		shader_ptr->SetUniform("u_Projection", m_ProjectionMatrix);

		// draw the sprites
		glDrawElements(GL_TRIANGLES, it->second.second.size(), GL_UNSIGNED_SHORT, NULL);

		// delete vertices and indices
		it->second.first.clear();
		it->second.second.clear();
	}
}

void GLESRenderer::SwapBuffers()
{
	m_RC->SwapBuffers();
}


void GLESRenderer::SetProjection2D(float Left, float Right, float Bottom, float Top)
{
	m_ProjectionMatrix = glm::ortho(Left, Right, Bottom, Top);
}


void GLESRenderer::SetViewPort(int X, int Y, int Width, int Height)
{
	glViewport(X, Y, Width, Height);
}

void GLESRenderer::SetCameraPosition(const glm::vec2 &Position)
{
	m_ViewMatrix = glm::translate(glm::mat4(), glm::vec3(-Position.x, -Position.y, 0));
}

const glm::mat4 &GLESRenderer::GetViewMatrix() const
{
	return m_ViewMatrix;
}

const glm::mat4 &GLESRenderer::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}
