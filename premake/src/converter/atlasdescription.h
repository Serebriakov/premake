#ifndef _ATLAS_DESCRIPTION_H_
#define _ATLAS_DESCRIPTION_H_

#include <string>


/*
Stores a pair of an image file path and a text file path
*/
class AtlasDescription
{
public :
	std::string					TextureAtlasPath;								// path of the image file
	std::string					MapFilePath;									// path of the text file storing UV coordinates


	AtlasDescription(const std::string &TextureAtlasPath = "", const std::string &MapFilePath = "") : TextureAtlasPath(TextureAtlasPath), MapFilePath(MapFilePath)
	{
	}
};

#endif
