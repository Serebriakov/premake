#ifndef _CONVERTER_H_
#define _CONVERTER_H_

#include <string>
#include <vector>
#include <json/json.h>

#include "color.h"
#include "palette.h"
#include "atlasdescription.h"


/*
Converter functions...
*/
class Converter
{
public :
	// type for return values
	enum ERROR_CODE
	{
		OK,
		CANNOT_OPEN_FILE,
		INVALID_PALETTE_INDEX,
		ENCODING_FAILED,
		LEVEL_CONVERSION_FAILED
	};


protected :
	// load palette files
	ERROR_CODE LoadPalettes(const std::string &Path, std::vector<Palette> &Palettes);

	// check if a bit is set
	static bool IsBitSet(unsigned char Byte, unsigned char BitNr);

	// get the value of a group of bits
	static unsigned char GetValueOfBits(unsigned char Byte, unsigned char FirstBitNr, unsigned char LastBitNr);

	// get first / second byte of a 2-byte value
	static unsigned char GetValueOfFirstByte(unsigned short Word);
	static unsigned char GetValueOfSecondByte(unsigned short Word);

	// concatenate 2 1-byte values to get a 2-byte value
	static unsigned short ConcatenateBytes(unsigned char LSB, unsigned char MSB);

	// convert a sprite number to a string with appropriate prefix
	std::string SpriteNumberToString(int SpriteNumber, const std::string &TilePrefix, const std::string &CommonPrefix = "UNION_");


public :
	// convert an image stored in "4-bit planar" format to a png image file
	ERROR_CODE Conv4BitPlanarToPng(const unsigned char *SrcData, const std::string &DstPath, unsigned SrcWidth, unsigned SrcHeight, const std::string &PalettePath, unsigned PaletteIndex);
	ERROR_CODE Conv4BitPlanarToPng(const std::string &SrcPath, const std::string &DstPath, unsigned SrcWidth, unsigned SrcHeight, const std::string &PalettePath, unsigned PaletteIndex);

	// convert tiles stored in "4-bit planar" format to individual png files
	ERROR_CODE ConvTilesFrom4BitPlanarToPng(const std::string &SrcPath, const std::string &DstPath, unsigned TileNum, unsigned TileWidth, unsigned TileHeight, const std::string &PalettePath, unsigned PaletteIndex);

	// convert an image stored in "8-bit packed" format to a png image file
	ERROR_CODE Conv8BitPackedToPng(const std::string &SrcPath, const std::string &DstPath, unsigned SrcWidth, unsigned SrcHeight);

	// read a binary level file and save data into json format
	ERROR_CODE ProcessLevel(const std::string &SrcPath, const std::string &DstPath, unsigned LevelHeight, const std::string &PalettePath, unsigned PaletteIndex, bool SaveTiles, const std::string &TilePrefix, const std::vector<AtlasDescription> &Images, const std::string &BackgroundImageAlias, const std::string &OverlayingSpritePrefix, const std::string &MusicFilePath = "");
};

#endif
