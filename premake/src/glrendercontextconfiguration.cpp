#include <fstream>
#include <sstream>
#include <algorithm>
#include <json/json.h>

#include "logger.h"
#include "glrendercontextconfiguration.h"


/*
Constructor
Initializes the configration with default values
*/
GLRenderContextConfiguration::GLRenderContextConfiguration()
{
	GLVersionMajor = 2;
	GLVersionMinor = 0;
	Width		   = 800;
	Height		   = 600;
	Fullscreen	   = false;
	BitsPerPixel   = 32;
	Multisampling  = 0;
	Vsync		   = true;
}

/*
Read and parse a text file as a configuration descriptor
*/
GLRenderContextConfiguration::ERROR_CODE GLRenderContextConfiguration::LoadFromFile(const std::string &Path)
{
	std::ifstream file(Path.c_str());
	Json::Reader reader;
	Json::Value root;
	bool read_successful = reader.parse(file, root, false);
	if( read_successful == false )
	{
		Logger::Instance().Log("ERROR: Couldn't parse config file: " + Path);
		Logger::Instance().Log(reader.getFormatedErrorMessages());
		return PARSING_FAILED;
	}
	file.close();

	Width = root.get("width", Width).asInt();
	Height = root.get("height", Height).asInt();
	Fullscreen = root.get("fullscreen", Fullscreen).asBool();
	BitsPerPixel = root.get("bits_per_pixel", BitsPerPixel).asInt();
	Multisampling = root.get("multisampling", Multisampling).asUInt();
	Vsync = root.get("v-sync", Vsync).asBool();
	GLVersionMajor = root.get("gl_version_major", GLVersionMajor).asInt();
	GLVersionMinor = root.get("gl_version_minor", GLVersionMinor).asInt();

	return OK;
}
