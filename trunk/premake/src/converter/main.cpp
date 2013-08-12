#include <cmath>

#include "converter.h"

const unsigned LEVEL_HEIGHTS[] = { 49, 104, 49, 45, 128, 128, 128, 86, 110, 12, 24, 51, 51, 38, 173, 84 };
const unsigned LEVEL_PALETTE[] = {  0,   1,  3,  4,   5,   6,   7,  8,   9,  9,  2,  2,  2,  2,   9,  8 };

int main(int argc, char *argv[])
{
	Converter *converter = new Converter();

	// converter->Conv4BitPlanarToPng("data/converter/in/BACK1.SQZ", "data/converter/out/BACK1.png", 320, 200, "data/converter/in/Pre2.pal", 4);


	// LEVEL 1
	std::vector<AtlasDescription> images;
	images.push_back(AtlasDescription("data/textures/union.png", "data/textures/union.txt"));
	images.push_back(AtlasDescription("data/textures/sprites.png", "data/textures/sprites.txt"));
	images.push_back(AtlasDescription("data/textures/front.png", "data/textures/front.txt"));
	images.push_back(AtlasDescription("data/textures/level1.png", "data/textures/level1.txt"));
	images.push_back(AtlasDescription("data/textures/background0.png", "data/textures/background0.txt"));
	converter->ProcessLevel("data/converter/in/LEVEL1.SQZ", "data/converter/out/levels/1/", LEVEL_HEIGHTS[0], "data/converter/in/Pre2.pal", LEVEL_PALETTE[0], true, "level_1_tile_", images, "BACK0", "FRONT_", "data/music/MINES.mod");
	
	// LEVEL 4
	/*
	std::vector<AtlasDescription> images;
	images.push_back(AtlasDescription("data/textures/union.png", "data/textures/union.txt"));
	images.push_back(AtlasDescription("data/textures/sprites.png", "data/textures/sprites.txt"));
	images.push_back(AtlasDescription("data/textures/front.png", "data/textures/front.txt"));
	images.push_back(AtlasDescription("data/textures/level4.png", "data/textures/level4.txt"));
	images.push_back(AtlasDescription("data/textures/background1.png", "data/textures/background1.txt"));
	converter->ProcessLevel("data/converter/in/LEVEL4.SQZ", "data/converter/out/levels/4/", LEVEL_HEIGHTS[3], "data/converter/in/Pre2.pal", LEVEL_PALETTE[3], true, "level_4_tile_", images, "BACK1", "FRONT_", "data/music/MINES.mod");
	*/

	// LEVEL 11
	/*
	std::vector<AtlasDescription> images;
	images.push_back(AtlasDescription("data/textures/union.png", "data/textures/union.txt"));
	images.push_back(AtlasDescription("data/textures/sprites.png", "data/textures/sprites.txt"));
	images.push_back(AtlasDescription("data/textures/front.png", "data/textures/front.txt"));
	images.push_back(AtlasDescription("data/textures/level11.png", "data/textures/level11.txt"));
	images.push_back(AtlasDescription("data/textures/background4.png", "data/textures/background4.txt"));
	converter->ProcessLevel("data/converter/in/LEVEL11.SQZ", "data/converter/out/levels/11/", LEVEL_HEIGHTS[10], "data/converter/in/Pre2.pal", LEVEL_PALETTE[10], false, "level_11_tile_", images, "BACK4", "FRONT_", "data/music/MINES.mod");
	*/

	delete converter;

	return 0;
}
