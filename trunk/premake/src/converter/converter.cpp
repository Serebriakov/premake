#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

#include "converter.h"
#include "level_structure.h"
#include "level_types.h"
#include "lodepng.h"
#include "reader.h"


/*
Load palette files
*/
Converter::ERROR_CODE Converter::LoadPalettes(const std::string &Path, std::vector<Palette> &Palettes)
{
	// open and read source file
	std::ifstream file_in(Path.c_str(), std::ios::in | std::ios::binary);
	if( file_in.is_open() == false )
	{
		return CANNOT_OPEN_FILE;
	}
	std::istreambuf_iterator<char> start(file_in), end;
	std::vector<unsigned char> data(start, end);
	file_in.close();

	Palette palette;

	for( unsigned byte = 0, color_index = 0; byte < data.size(); byte += 4 )
	{
		palette.Colors[color_index] = Color4(data[byte + 2], data[byte + 1], data[byte + 0], 255 - data[byte + 3]);
		color_index++;
		if( color_index == 16 )
		{
			color_index = 0;
			Palettes.push_back(palette);
		}
	}

	return OK;
}

/*
Helper function;
check if a bit is set
*/
bool Converter::IsBitSet(unsigned char Byte, unsigned char BitNr)
{
	return (Byte & (1 << BitNr)) > 0;
}

/*
Helper function;
get the value of a group of bits
*/
unsigned char Converter::GetValueOfBits(unsigned char Byte, unsigned char FirstBitNr, unsigned char LastBitNr)
{
	unsigned char mask = 0;
	for( unsigned char i = 0; i < (LastBitNr - FirstBitNr + 1); ++i )
	{
		mask |= (1 << i);
	}

	return Byte & (mask << FirstBitNr);
}

/*
Get first / second byte of a 2-byte value
*/
unsigned char Converter::GetValueOfFirstByte(unsigned short Word)
{
	return (unsigned char)(Word & 0xFF);
}

unsigned char Converter::GetValueOfSecondByte(unsigned short Word)
{
	return (unsigned char)((Word >> 8) & 0xFF);
}

/*
Concatenate 2 1-byte values to get a 2-byte value
*/
unsigned short Converter::ConcatenateBytes(unsigned char LSB, unsigned char MSB)
{
	return LSB | ((unsigned short)(MSB) << 8);
}

/*
Convert a sprite number to a string with appropriate prefix
*/
std::string Converter::SpriteNumberToString(int SpriteNumber, const std::string &TilePrefix, const std::string &CommonPrefix)
{
	// 256 means transparent, < 0 is invalid
	if( (SpriteNumber < 0) || (SpriteNumber == 256) )
	{
		return "";
	}

	std::stringstream ss;

	// < 256 means level-specific tile
	if( SpriteNumber < 256 )
	{
		ss << TilePrefix << std::setfill('0') << std::setw(3) << SpriteNumber;
	}
	// > 256 refers to a common sprite
	else
	{
		ss << CommonPrefix << std::setfill('0') << std::setw(3) << (SpriteNumber - 256);
	}

	return ss.str();
}

/*
Convert an image stored in "4-bit planar" mode to a png image file. Reads from memory, writes to file.
"The bitmap format used for the 4-bit pics is planar; so, you get 4 monochrome
pictures one after the other, one per plane."
*/
Converter::ERROR_CODE Converter::Conv4BitPlanarToPng(const unsigned char *SrcData, const std::string &DstPath, unsigned SrcWidth, unsigned SrcHeight, const std::string &PalettePath, unsigned PaletteIndex)
{
	// read palette file
	std::vector<Palette> palettes;
	ERROR_CODE result = LoadPalettes(PalettePath, palettes);
	if( result != OK )
	{
		return result;
	}

	// select palette
	if( PaletteIndex >= palettes.size() )
	{
		return INVALID_PALETTE_INDEX;
	}
	const Palette &palette = palettes[PaletteIndex];

	// allocate buffer for converted data
	std::vector<Color4> dst_data;
	dst_data.resize(SrcWidth * SrcHeight);

	// convert pixel-by-pixel
	for( unsigned y = 0; y < SrcHeight; ++y )
	{
		for( unsigned x = 0; x < SrcWidth; ++x )
		{
			unsigned i = y * SrcWidth + x;

			unsigned char pixel = 0;

			// there are 4 bits for each pixel
			for( unsigned short b = 0; b < 4; ++b )
			{
				unsigned bit = i + (b * SrcWidth * SrcHeight);
				unsigned byte_index = bit / 8;
				unsigned char offset = 7 - (bit % 8);

				pixel |= (SrcData[byte_index] & (1 << offset)) > 0 ? (1 << b) : 0;
			}

			dst_data[i] = palette.Colors[pixel];

			// first color of the palette is the colorkey
			if( pixel == 0 )
			{
				dst_data[i].A = 0;
			}
		}
	}

	// write to the output file
	if( lodepng::encode(DstPath, &(dst_data[0].R), SrcWidth, SrcHeight) == 0 )
	{
		return OK;
	}
	else
	{
		return ENCODING_FAILED;
	}
}

/*
Convert an image stored in "4-bit planar" mode to a png image file.
"The bitmap format used for the 4-bit pics is planar; so, you get 4 monochrome
pictures one after the other, one per plane."
*/
Converter::ERROR_CODE Converter::Conv4BitPlanarToPng(const std::string &SrcPath, const std::string &DstPath, unsigned SrcWidth, unsigned SrcHeight, const std::string &PalettePath, unsigned PaletteIndex)
{
	// open and read source file
	std::ifstream file_in(SrcPath.c_str(), std::ios::in | std::ios::binary);
	if( file_in.is_open() == false )
	{
		return CANNOT_OPEN_FILE;
	}
	std::istreambuf_iterator<char> start(file_in), end;
	std::vector<unsigned char> src_data(start, end);
	file_in.close();

	return Conv4BitPlanarToPng(&src_data[0], DstPath, SrcWidth, SrcHeight, PalettePath, PaletteIndex);
}

/*
Convert tiles stored in "4-bit planar" format to individual png files
*/
Converter::ERROR_CODE Converter::ConvTilesFrom4BitPlanarToPng(const std::string &SrcPath, const std::string &DstPath, unsigned TileNum, unsigned TileWidth, unsigned TileHeight, const std::string &PalettePath, unsigned PaletteIndex)
{
	// open and read source file
	std::ifstream file_in(SrcPath.c_str(), std::ios::in | std::ios::binary);
	if( file_in.is_open() == false )
	{
		return CANNOT_OPEN_FILE;
	}
	std::istreambuf_iterator<char> start(file_in), end;
	std::vector<unsigned char> src_data(start, end);
	file_in.close();

	unsigned tile_size = TileWidth * TileHeight / 2; // size of each tile in bytes in the tile-pack (4 bit / pixel)

	for( unsigned tile = 0; tile < TileNum; ++tile )
	{
		std::stringstream ss;
		ss << DstPath << "_" << std::setfill('0') << std::setw(3) << tile << ".png";
		Conv4BitPlanarToPng(&src_data[tile * tile_size], ss.str(), TileWidth, TileHeight, PalettePath, PaletteIndex);
	}

	return OK;
}

/*
Convert an image stored in "8-bit packed" mode to a png image file.
"8-bit pics are simply packed pixel format, with an included palette. The
palette comes first at offset 0, and consists of 256 RGB triplets where each
component is in the range 0 to 63. After that, at offset 768, comes exactly
65536 bytes of pixel data, as it would appear in VGA memory. It's 320*200 plus
some padding."
*/
Converter::ERROR_CODE Converter::Conv8BitPackedToPng(const std::string &SrcPath, const std::string &DstPath, unsigned SrcWidth, unsigned SrcHeight)
{
	// open and read source file
	std::ifstream file_in(SrcPath.c_str(), std::ios::in | std::ios::binary);
	if( file_in.is_open() == false )
	{
		return CANNOT_OPEN_FILE;
	}
	std::istreambuf_iterator<char> start(file_in), end;
	std::vector<unsigned char> src_data(start, end);
	file_in.close();

	// create palette
	std::vector<Color3> palette;
	palette.reserve(256);
	for( unsigned i = 0; i < 768; i += 3 )
	{
		palette.push_back(Color3(src_data[i] * 4, src_data[i+1] * 4, src_data[i+2] * 4));
	}

	// allocate buffer for converted data
	std::vector<Color3> dst_data;
	dst_data.reserve(SrcWidth * SrcHeight);

	// convert pixel-by-pixel
	unsigned offset = 768;
	for( unsigned y = 0; y < SrcHeight; ++y )
	{
		for( unsigned x = 0; x < SrcWidth; ++x )
		{
			dst_data.push_back(palette[src_data[offset]]);
			offset++;
		}
	}

	if( lodepng::encode(DstPath, &(dst_data[0].R), SrcWidth, SrcHeight, LCT_RGB) == 0 )
	{
		return OK;
	}
	else
	{
		return ENCODING_FAILED;
	}
}

/*
Read a binary level file from SrcPath and save data into json format, into directory DstPath.
*/
Converter::ERROR_CODE Converter::ProcessLevel(const std::string &SrcPath, const std::string &DstPath, unsigned LevelHeight, const std::string &PalettePath, unsigned PaletteIndex, bool SaveTiles, const std::string &TilePrefix, const std::vector<AtlasDescription> &Images, const std::string &BackgroundImageAlias, const std::string &OverlayingSpritePrefix, const std::string &MusicFilePath)
{
	// open and read source file
	Reader reader;
	if( reader.ReadFile(SrcPath) != Reader::OK )
	{
		return LEVEL_CONVERSION_FAILED;
	}

	/*
	the first part of each level file is a tilemap, sized 256*LevelHeight bytes
	(each level is 256 tiles wide but the height varies...)
	*/
	std::vector<unsigned char> tilemap;
	tilemap.reserve(LEVEL_WIDTH * LevelHeight);
	for( unsigned i = 0; i < LEVEL_WIDTH * LevelHeight; ++i )
	{
		unsigned char value;
		reader.GetByte(&value);
		tilemap.push_back(value);
	}

	/*
	after the tilemap comes a lookup table that stores 256 2-byte numbers,
	the values in the tilemap are indices into this lookup table.
	"The table contains 3 types of numbers: less than 256, equal to 256 and greater than 256.
	Tile 256 is transparent and shows the background picture. Tiles 0..255 are level-specific tiles,
	while tiles 257+ are shared among the levels."
	*/
	std::vector<unsigned short> lookup_table;
	lookup_table.reserve(LOOKUP_TABLE_COUNT);
	for( unsigned i = 0; i < LOOKUP_TABLE_COUNT; ++i )
	{
		unsigned short value;
		reader.GetUnsignedShort(&value);
		lookup_table.push_back(value);
	}

	/*
	determine the maximum value among the level-specific tiles
	*/
	int max_level_specific_tile = -1;
	for( unsigned i = 0; i < lookup_table.size(); ++i )
	{
		if( (lookup_table[i] < 256) && (lookup_table[i] > max_level_specific_tile) )
		{
			max_level_specific_tile = lookup_table[i];
		}
	}

	/*
	all the max_level_specific_tile tiles are stored in the level file, right after the lookup table.
	these sprites are in the 4-bit planar format and are 16*16 pixels, so each one takes 128 bytes.
	save them in the output folder if necessary
	*/
	if( SaveTiles )
	{
		for( unsigned short i = 0; i <= max_level_specific_tile; ++i )
		{
			std::stringstream ss;
			ss << DstPath << TilePrefix << std::setfill('0') << std::setw(3) << i << ".png";
			Conv4BitPlanarToPng(reader.GetPointer() + (LEVEL_WIDTH * LevelHeight) + (LOOKUP_TABLE_COUNT * 2) + (i * TILE_RESOLUTION_X * TILE_RESOLUTION_Y / 2), ss.str(), TILE_RESOLUTION_X, TILE_RESOLUTION_Y, PalettePath, PaletteIndex);
		}
	}

	/*
	________________________________________________________________________________________________________________
	| tile map (256 * height b) | lookup table (512 b) | level specific tiles (n * 128 b) | remaining part of file |
	----------------------------------------------------------------------------------------------------------------
	*/
	// offset to the "remaining part of file"
	int offset = (LEVEL_WIDTH * LevelHeight) + (LOOKUP_TABLE_COUNT * 2) + ((max_level_specific_tile + 1) * (TILE_RESOLUTION_X * TILE_RESOLUTION_Y / 2));

	/*
	read tile properties bytes
	there are 4 256 byte long arrays for tile properties
	*/
	reader.JumpTo(offset + TILE_PROP_1_OFFSET);
	std::vector<unsigned char> tile_prop_1;
	tile_prop_1.reserve(TILE_PROP_1_COUNT);
	for( unsigned i = 0; i < TILE_PROP_1_COUNT; ++i )
	{
		unsigned char value;
		reader.GetByte(&value);
		tile_prop_1.push_back(value);
	}

	reader.JumpTo(offset + TILE_PROP_2_OFFSET);
	std::vector<unsigned char> tile_prop_2;
	tile_prop_2.reserve(TILE_PROP_2_COUNT);
	for( unsigned i = 0; i < TILE_PROP_2_COUNT; ++i )
	{
		unsigned char value;
		reader.GetByte(&value);
		tile_prop_2.push_back(value);
	}

	reader.JumpTo(offset + TILE_PROP_3_OFFSET);
	std::vector<unsigned char> tile_prop_3;
	tile_prop_3.reserve(TILE_PROP_3_COUNT);
	for( unsigned i = 0; i < TILE_PROP_3_COUNT; ++i )
	{
		unsigned char value;
		reader.GetByte(&value);
		tile_prop_3.push_back(value);
	}

	reader.JumpTo(offset + TILE_PROP_4_OFFSET);
	std::vector<unsigned char> tile_prop_4;
	tile_prop_4.reserve(TILE_PROP_4_COUNT);
	for( unsigned i = 0; i < TILE_PROP_4_COUNT; ++i )
	{
		unsigned char value;
		reader.GetByte(&value);
		tile_prop_4.push_back(value);
	}

	/*
	the position where the player starts, in pixel coordinates, originally in the game's coordinate system
	*/
	reader.JumpTo(offset + START_X_OFFSET);
	unsigned short start_x;
	reader.GetUnsignedShort(&start_x);
	
	reader.JumpTo(offset + START_Y_OFFSET);
	unsigned short start_y;
	reader.GetUnsignedShort(&start_y);

	/*
	scrolling behavior
	*/
	reader.JumpTo(offset + SCROLLING_BEHAVIOR_OFFSET);
	unsigned char scrolling_behavior;
	reader.GetByte(&scrolling_behavior);

	/*
	For tile opacity, there's a 256*2 byte field storing the number of overlaying sprite for each tile.
	*/
	reader.JumpTo(offset + TILE_OPACITY_OFFSET);
	std::vector<unsigned short> tile_opacity;
	tile_opacity.reserve(TILE_OPACITY_COUNT);
	for( unsigned i = 0; i < TILE_OPACITY_COUNT; ++i )
	{
		unsigned short value;
		reader.GetUnsignedShort(&value);
		tile_opacity.push_back(value);
	}

	/*
	20 gates
	*/
	reader.JumpTo(offset + GATES_OFFSET);
	std::vector<Gate> gates;
	gates.reserve(GATES_COUNT);
	for( unsigned i = 0; i < GATES_COUNT; ++i )
	{
		Gate gate;
		reader.GetByte(&gate.x_in);
		reader.GetByte(&gate.y_in);
		reader.GetByte(&gate.x_screen);
		reader.GetByte(&gate.y_screen);
		reader.GetByte(&gate.x_out);
		reader.GetByte(&gate.y_out);
		reader.GetByte(&gate.scroll);

		gates.push_back(gate);
	}
	
	/*
	15 shifting tile blocks
	*/
	reader.JumpTo(offset + SHIFTING_TILE_BLOCKS_OFFSET);
	std::vector<ShiftingTileBlock> shifting_tile_blocks;
	shifting_tile_blocks.reserve(SHIFTING_TILE_BLOCKS_COUNT);
	for( unsigned i = 0; i < SHIFTING_TILE_BLOCKS_COUNT; ++i )
	{
		ShiftingTileBlock st_block;
		reader.GetByte(&st_block.x);
		reader.GetByte(&st_block.y);
		reader.GetByte(&st_block.width);
		reader.GetByte(&st_block.height);
		reader.GetByte(&st_block.x_act);
		reader.GetByte(&st_block.y_act);
		reader.JumpWith(2);					// skip the unknown constant values
		reader.GetByte(&st_block.dist);
		reader.JumpWith(1);					// skip the unknown constant values

		shifting_tile_blocks.push_back(st_block);
	}

	/*
	Enemies
	*/
	reader.JumpTo(offset + ENEMIES_OFFSET);
	std::vector<Enemy> enemies;
	unsigned enemies_bytes_read = 0;
	while( enemies_bytes_read < ENEMIES_SIZE_SUM )
	{
		Enemy enemy;
		reader.GetByte(&enemy.length);

		// when reached an invalid length value, there are no more valid enemy records
		if( enemy.length == UCHAR_MAX )
		{
			break;
		}

		reader.GetByte(&enemy.type_expert);
		reader.GetUnsignedShort(&enemy.sprite);
		reader.JumpWith(1);
		reader.GetByte(&enemy.hitpoints);
		reader.GetByte(&enemy.pause);
		reader.JumpWith(1);
		reader.GetByte(&enemy.score);
		reader.GetUnsignedShort(&enemy.x);
		reader.GetUnsignedShort(&enemy.y);
		reader.GetBytes(enemy.type_specific, enemy.length - ENEMY_BASE_SIZE);

		enemies.push_back(enemy);

		enemies_bytes_read += enemy.length;
	}
	
	// Item/platform sprite number offset; Actual_sprite_number = stored_sprite_number + 53 - offset
	// These need to be used to convert stored sprite numbers to actual sprite numbers.
	reader.JumpTo(offset + ITEM_PLATFORM_SPRITE_NUM_OFF_OFFSET);
	unsigned short item_platform_sprite_offset;
	reader.GetUnsignedShort(&item_platform_sprite_offset);

	// Enemy sprite number offset; Actual_sprite_number = stored_sprite_number + 312 - offset
	// These need to be used to convert stored sprite numbers to actual sprite numbers.
	reader.JumpTo(offset + ENEMY_SPRITE_NUM_OFF_OFFSET);
	unsigned short enemy_sprite_offset;
	reader.GetUnsignedShort(&enemy_sprite_offset);

	/*
	80 secrets
	*/
	reader.JumpTo(offset + SECRETS_OFFSET);
	std::vector<Secret> secrets;
	secrets.reserve(SECRETS_COUNT);
	for( unsigned i = 0; i < SECRETS_COUNT; ++i )
	{
		Secret secret;
		reader.GetByte(&secret.tile_from);
		reader.GetByte(&secret.tile_to);
		reader.GetByte(&secret.bonus_health);
		reader.GetByte(&secret.x);
		reader.GetByte(&secret.y);

		secrets.push_back(secret);
	}

	/*
	70 items
	*/
	reader.JumpTo(offset + ITEMS_OFFSET);
	std::vector<Item> items;
	items.reserve(ITEMS_COUNT);
	for( unsigned i = 0; i < ITEMS_COUNT; ++i )
	{
		Item item;
		reader.GetUnsignedShort(&item.pos_x);
		reader.GetUnsignedShort(&item.pos_y);
		reader.GetUnsignedShort(&item.sprite);
		reader.JumpWith(1);					// skip the constant

		items.push_back(item);
	}

	/*
	16 platforms
	*/
	reader.JumpTo(offset + PLATFORMS_OFFSET);
	std::vector<Platform> platforms;
	platforms.reserve(PLATFORMS_COUNT);
	for( unsigned i = 0; i < PLATFORMS_COUNT; ++i )
	{
		Platform platform;
		reader.GetUnsignedShort(&platform.pos_x);
		reader.GetUnsignedShort(&platform.pos_y);
		reader.GetUnsignedShort(&platform.sprite);
		reader.GetByte(&platform.behavior);
		reader.GetByte(&platform.speed);
		reader.JumpWith(1);					// skip the constant FF
		reader.GetByte(&platform.drop_delay);
		reader.GetUnsignedShort(&platform.distance);
		reader.JumpWith(1);					// skip the constant 0
		reader.GetByte(&platform.drop1);
		reader.GetByte(&platform.drop2);

		platforms.push_back(platform);
	}


	/*
	write converted data into json format
	*/
	Json::Value root;
	
	// images to load...
	Json::Value image;
	for( size_t i = 0; i < Images.size(); ++i )
	{
		image["atlas_path"] = Images[i].TextureAtlasPath;
		image["map_path"] = Images[i].MapFilePath;

		root["images"].append(image);
	}

	// background music
	root["background_music"] = MusicFilePath;
	
	// level size
	root["width"] = LEVEL_WIDTH;
	root["height"] = LevelHeight;

	// tiles' resolution
	root["tile_resolution_x"] = TILE_RESOLUTION_X;
	root["tile_resolution_y"] = TILE_RESOLUTION_Y;

	// save the alias of the background image
	root["background_image"] = BackgroundImageAlias;
	
	// start position, in world coordinates
	root["start_position"]["x"] = start_x;
	root["start_position"]["y"] = start_y;
	
	// tiles
	for( size_t i = 0; i < tilemap.size(); ++i )
	{
		Json::Value tile;

		// sprite of the tile
		std::string sprite_0_str = SpriteNumberToString(lookup_table[tilemap[i]], TilePrefix);
		tile["sprite_0"] = sprite_0_str;
		
		if( sprite_0_str != "" )
		{
			// if the sprite is frame 0
			if( GetValueOfBits(tile_prop_3[tilemap[i]], 7, 7) != 0 )
			{
				tile["sprite_1"] = SpriteNumberToString(lookup_table[tilemap[i] + 1], TilePrefix);
				tile["sprite_2"] = SpriteNumberToString(lookup_table[tilemap[i] + 2], TilePrefix);
			}
			// sprite is frame 1
			else if( (tilemap[i] >= 1) && (GetValueOfBits(tile_prop_3[tilemap[i] - 1], 7, 7) != 0) )
			{
				tile["sprite_1"] = SpriteNumberToString(lookup_table[tilemap[i] + 1], TilePrefix);
				tile["sprite_2"] = SpriteNumberToString(lookup_table[tilemap[i] - 1], TilePrefix);
			}
			// sprite is frame 2
			else if( (tilemap[i] >= 2) && (GetValueOfBits(tile_prop_3[tilemap[i] - 2], 7, 7) != 0) )
			{
				tile["sprite_1"] = SpriteNumberToString(lookup_table[tilemap[i] - 2], TilePrefix);
				tile["sprite_2"] = SpriteNumberToString(lookup_table[tilemap[i] - 1], TilePrefix);
			}
		}

		// overlaying sprite
		if( IsBitSet(tile_prop_3[tilemap[i]], 6) )
		{
			std::stringstream ss;
			ss << OverlayingSpritePrefix << std::setw(3) << std::setfill('0') << tile_opacity[tilemap[i]];
			tile["overlaying_sprite"] = ss.str();
		}

		// sides' properties
		std::string side_property;
		Json::Value side_properties;
		// bottom
		side_property = "traversable";
		if( IsBitSet(tile_prop_3[tilemap[i]], 0) )
		{
			side_property = "solid";
		}
		if( IsBitSet(tile_prop_3[tilemap[i]], 1) )
		{
			side_property = "deadly";
		}
		side_properties.append(Json::Value(side_property));
		// top
		switch( tile_prop_2[tilemap[i]] )
		{
			case 1 : 
			case 2 : 
			case 3 : 
			case 4 : side_property = "solid"; break;
			case 5 : side_property = "traversable_when_pressing_down"; break;
			case 6 : side_property = "deadly"; break;
			default : side_property = "traversable"; break;
		}
		side_properties.append(Json::Value(side_property));
		// left & right
		switch( tile_prop_1[tilemap[i]] )
		{
			case 1 : side_property = "solid"; break;
			case 2 : side_property = "deadly"; break;
			default : side_property = "traversable"; break;
		}
		side_properties.append(Json::Value(side_property));
		side_properties.append(Json::Value(side_property));
		tile["side_property"] = side_properties;

		// slope information
		std::string slope_type;
		if( IsBitSet(tile_prop_4[tilemap[i]], 4) )
		{
			slope_type = "slope_downright";
		}
		else if( IsBitSet(tile_prop_4[tilemap[i]], 5) )
		{
			slope_type = "slope_upright";
		}
		else
		{
			slope_type = "horizontal_floor";
		}
		tile["slope_type"] = slope_type;

		// reversed heights
		// if the tile is a horizontal floor, reversed heights are 0 and are NOT saved in the level file
		if( slope_type != "horizontal_floor" )
		{
			// reversed height of left corner are stored on bits 0..3
			tile["reversed_height_left"] = GetValueOfBits(tile_prop_4[tilemap[i]], 0, 3);
			// height of right corner depends on adjacent tile
			tile["reversed_height_right"] = GetValueOfBits(tile_prop_4[tilemap[i + 1]], 0, 3);
		}

		// for some tiles the sprite should be changed to the next one, when stepped on
		if( IsBitSet(tile_prop_3[tilemap[i]], 5) )
		{
			tile["sprite_1"] = SpriteNumberToString(lookup_table[tilemap[i] + 1], TilePrefix);
		}

		root["tiles"].append(tile);
	}

	// scrolling behavior
	root["scrolling_behavior"] = scrolling_behavior;
	
	// gates
	Json::Value gate;
	for( size_t i = 0; i < gates.size(); ++i )
	{
		// skip "invalid" elements
		if( (gates[i].x_in == UCHAR_MAX) && (gates[i].y_in == UCHAR_MAX) && (gates[i].x_out == UCHAR_MAX) && (gates[i].y_out == UCHAR_MAX) )
		{
			continue;
		}

		// entrance
		gate["in"]["tile_x"] = gates[i].x_in;
		gate["in"]["tile_y"] = gates[i].y_in;
		// exit
		gate["out"]["tile_x"] = gates[i].x_out;
		gate["out"]["tile_y"] = gates[i].y_out;
		gate["out"]["screen_x"] = gates[i].x_screen;
		gate["out"]["screen_y"] = gates[i].y_screen;
		gate["out"]["scrolling_enabled"] = (gates[i].scroll > 0);

		root["gates"].append(gate);
	}
	
	// secrets
	Json::Value secret;
	for( size_t i = 0; i < secrets.size(); ++i )
	{
		// skip "invalid" elements
		if( (secrets[i].x == UCHAR_MAX) && (secrets[i].y == UCHAR_MAX) )
		{
			continue;
		}

		secret["initial_tile"] = SpriteNumberToString(lookup_table[secrets[i].tile_from], TilePrefix);
		secret["tile_x"] = secrets[i].x;
		secret["tile_y"] = secrets[i].y;
		secret["bonus_health"] = secrets[i].bonus_health;

		root["secrets"].append(secret);
	}

	// items
	Json::Value item;
	for( size_t i = 0; i < items.size(); ++i )
	{
		// skip "invalid" elements
		if( ((items[i].pos_x == USHRT_MAX) && (items[i].pos_y == USHRT_MAX)) || (items[i].sprite > MAX_ITEM_SPRITE) )
		{
			continue;
		}

		item["pos_x"] = items[i].pos_x;
		item["pos_y"] = items[i].pos_y;
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(3) << (items[i].sprite + ITEM_PLATFORM_SPRITE_MAGIC_NR - item_platform_sprite_offset);
		item["sprite"] = ss.str();

		root["items"].append(item);
	}

	// platforms
	Json::Value platform;
	const std::string dir_str[] = { "up", "upright", "right", "downright", "down", "downleft", "left", "upleft" };
	for( size_t i = 0; i < platforms.size(); ++i )
	{
		// skip "invalid" elements
		if( platforms[i].behavior == UCHAR_MAX )
		{
			continue;
		}
		
		// position
		platform["pos_x"] = platforms[i].pos_x;
		platform["pos_y"] = platforms[i].pos_y;
		std::stringstream ss;
		ss << std::setfill('0') << std::setw(3) << (platforms[i].sprite + ITEM_PLATFORM_SPRITE_MAGIC_NR - item_platform_sprite_offset);
		// sprite
		platform["sprite"] = ss.str();
		/* direction of moving - if the platform is a dropdown, it might still be "up"...
        7 0 1
        6 * 2
        5 4 3
		*/
		unsigned char dir = GetValueOfBits(platforms[i].behavior, 0, 2);
		platform["direction"] = dir_str[dir];
		// dropdown property
		bool is_dropdown = IsBitSet(platforms[i].behavior, 3);
		platform["dropdown"] = is_dropdown;
		// drop delay - saved only if the platform is a dropdown platform
		if( is_dropdown )
		{
			platform["drop_delay"] = platforms[i].drop_delay;
		}
		// speed & distance
		platform["speed"] = platforms[i].speed;
		platform["distance"] = platforms[i].distance;
		// trigger
		platform["triggered_by_player"] = (IsBitSet(platforms[i].behavior, 7) == false);

		root["platforms"].append(platform);
	}

	// shifting tile blocks (???)
	Json::Value shifting_tile_block;
	for( size_t i = 0; i < shifting_tile_blocks.size(); ++i )
	{
		// skip (probably) invalid / unused elements
		if( (shifting_tile_blocks[i].x == UCHAR_MAX) && (shifting_tile_blocks[i].y == UCHAR_MAX) )
		{
			continue;
		}

		shifting_tile_block["x"] = shifting_tile_blocks[i].x;
		shifting_tile_block["y"] = shifting_tile_blocks[i].y;
		shifting_tile_block["width"] = shifting_tile_blocks[i].width;
		shifting_tile_block["height"] = shifting_tile_blocks[i].height;
		shifting_tile_block["x_act"] = shifting_tile_blocks[i].x_act;
		shifting_tile_block["y_act"] = shifting_tile_blocks[i].y_act;
		shifting_tile_block["dist"] = shifting_tile_blocks[i].dist;

		printf(" !!! shifting tile block saved !!! \n");

		root["shifting_tile_blocks"].append(shifting_tile_block);
	}

	// enemies
	std::map<unsigned char, std::pair<int, std::string>> score_map;
	score_map[0].first = 100;
	score_map[0].second = "effect_score_100";
	score_map[1].first = 200;
	score_map[1].second = "effect_score_200";
	score_map[2].first = 300;
	score_map[2].second = "effect_score_300";
	score_map[3].first = 500;
	score_map[3].second = "effect_score_500";
	score_map[4].first = 600;
	score_map[4].second = "effect_score_600";
	score_map[5].first = 700;
	score_map[5].second = "effect_score_700";
	score_map[6].first = 750;
	score_map[6].second = "effect_score_750";
	score_map[7].first = 800;
	score_map[7].second = "effect_score_800";
	score_map[8].first = 1000;
	score_map[8].second = "effect_score_1000";
	score_map[9].first = 2000;
	score_map[9].second = "effect_score_2000";
	score_map[10].first = 5000;
	score_map[10].second = "effect_score_5000";
	score_map[11].first = 8000;
	score_map[11].second = "effect_score_8000";
	score_map[12].first = 10000;
	score_map[12].second = "effect_score_10000";
	score_map[13].first = 20000;
	score_map[13].second = "effect_score_20000";
	score_map[14].first = 30000;
	score_map[14].second = "effect_score_30000";
	score_map[15].first = 60000;
	score_map[15].second = "effect_score_60000";
	score_map[16].first = 100000;
	score_map[16].second = "effect_score_100000";
	for( size_t i = 0; i < enemies.size(); ++i )
	{
		Json::Value enemy;

		// common data
		unsigned char type = GetValueOfBits(enemies[i].type_expert, 0, 6);
		enemy["type"] = type;
		enemy["expert_only"] = IsBitSet(enemies[i].type_expert, 7);
		enemy["decoration_only"] = (enemy["type"].asUInt() == 1);
		enemy["hitpoints"] = enemies[i].hitpoints;
		enemy["pause"] = enemies[i].pause;
		enemy["score"] = enemies[i].score;
		if( (type != 0) && (type != 10) )
		{
			enemy["x"] = enemies[i].x;
			enemy["y"] = enemies[i].y;
		}
		std::stringstream ss;
		ss << enemies[i].sprite;
		enemy["sprite"] = ss.str();
		
		// type-specific data
		switch( type )
		{
			case 0 :
			{
				enemy["area"]["x"] = GetValueOfFirstByte(enemies[i].x);
				enemy["area"]["y"] = GetValueOfSecondByte(enemies[i].x);
				enemy["area"]["width"] = GetValueOfFirstByte(enemies[i].y);
				enemy["area"]["height"] = GetValueOfSecondByte(enemies[i].y);
				break;
			}

			case 2 :
			{
				enemy["max_web_length"] = enemies[i].type_specific[0];
				enemy["speed"] = enemies[i].type_specific[1];
				break;
			}

			case 3 :
			{
				enemy["activation_distance"] = enemies[i].type_specific[0];
				break;
			}

			case 4 :
			{
				enemy["radius"] = enemies[i].type_specific[0];
				enemy["angle"] = enemies[i].type_specific[1];
				break;
			}

			case 5 :
			{
				enemy["width_of_rectangle"] = enemies[i].type_specific[0];
				enemy["height_of_rectangle"] = enemies[i].type_specific[1];
				enemy["speed"] = enemies[i].type_specific[2];
				break;
			}

			case 6 :
			{
				enemy["activation_distance"] = enemies[i].type_specific[0];
				break;
			}

			case 7 :
			{
				enemy["activation_distance"] = enemies[i].type_specific[0];
				enemy["speed"] = enemies[i].type_specific[1];
				break;
			}

			case 8 :
			{
				enemy["distance_x"] = enemies[i].type_specific[0];
				enemy["jump_power_y"] = enemies[i].type_specific[1];
				enemy["jump_power_x"] = enemies[i].type_specific[2];
				enemy["distance_y"] = enemies[i].type_specific[3];
				break;
			}

			case 9 :
			{
				enemy["border_left"] = ConcatenateBytes(enemies[i].type_specific[0], enemies[i].type_specific[1]);
				enemy["border_right"] = ConcatenateBytes(enemies[i].type_specific[2], enemies[i].type_specific[3]);
				enemy["speed"] = enemies[i].type_specific[5];
				break;
			}

			case 10 :
			{
				enemy["area"]["x"] = GetValueOfFirstByte(enemies[i].x);
				enemy["area"]["y"] = GetValueOfSecondByte(enemies[i].x);
				enemy["area"]["width"] = GetValueOfFirstByte(enemies[i].y);
				enemy["area"]["height"] = GetValueOfSecondByte(enemies[i].y);
				enemy["speed"] = enemies[i].type_specific[0];
				break;
			}

			case 11 :
			{
				enemy["speed_x"] = enemies[i].type_specific[0];
				enemy["starting_jump_power"] = enemies[i].type_specific[1];
				enemy["speed_y"] = enemies[i].type_specific[2];
				break;
			}

			case 12 :
			{
				enemy["speed"] = enemies[i].type_specific[0];
				break;
			}
		}

		root["enemies"].append(enemy);
	}
	
	Json::StyledWriter writer;
	std::ofstream file_out(std::string(DstPath + "level.json").c_str());
	file_out << writer.write(root);
	file_out.close();

	
	return OK;
}
