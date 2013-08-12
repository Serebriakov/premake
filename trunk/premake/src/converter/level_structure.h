#ifndef _LEVEL_STRUCTURE_H_
#define _LEVEL_STRUCTURE_H_

/*
structure of level files:
________________________________________________________________________________________________________________
| tile map (256 * height b) | lookup table (512 b) | level specific tiles (n * 128 b) | remaining part of file |
----------------------------------------------------------------------------------------------------------------
*/

// a tile's resolution in pixels
#define TILE_RESOLUTION_X							16
#define TILE_RESOLUTION_Y							16

// levels' width in number of tiles
#define LEVEL_WIDTH									256

// number of 2-byte values in the lookup table that comes after the tilemap
#define LOOKUP_TABLE_COUNT							256

// maximal number of item sprite
#define MAX_ITEM_SPRITE								600


/*
offset is always relative to the end of level specific tiles,
size is always measured in bytes (if not defined, each element is a 1-byte value),
count is the number of elements.
*/

// tile properties byte 1
#define TILE_PROP_1_OFFSET							0
#define TILE_PROP_1_COUNT							256

// tile properties byte 2
#define TILE_PROP_2_OFFSET							256
#define TILE_PROP_2_COUNT							256

// tile properties byte 3
#define TILE_PROP_3_OFFSET							512
#define TILE_PROP_3_COUNT							256

// tile properties byte 4
#define TILE_PROP_4_OFFSET							4031
#define TILE_PROP_4_COUNT							256

// the position where the player starts the level,
// measured in the game's coordinate system, in pixels
#define START_X_OFFSET								770
#define START_X_SIZE								2
#define START_Y_OFFSET								772
#define START_Y_SIZE								2

// scrolling behavior
#define SCROLLING_BEHAVIOR_OFFSET					776
#define SCROLLING_BEHAVIOR_SIZE						1

// for tile opacity, there's a 256*2 byte field
// storing the number of overlaying sprite for each tile
#define TILE_OPACITY_OFFSET							777
#define TILE_OPACITY_SIZE							2
#define TILE_OPACITY_COUNT							256

// there are 20 gates, each takes 7 bytes
#define GATES_OFFSET								1289
#define GATES_SIZE									7
#define GATES_COUNT									20

// shifting tile blocks
#define SHIFTING_TILE_BLOCKS_OFFSET					1429
#define SHIFTING_TILE_BLOCKS_SIZE					10
#define SHIFTING_TILE_BLOCKS_COUNT					15

// enemies
#define ENEMIES_OFFSET								1579
#define ENEMY_BASE_SIZE								13
#define ENEMIES_SIZE_SUM							2048

// Item/platform sprite number offset; Actual_sprite_number = stored_sprite_number + 53 - offset
#define ITEM_PLATFORM_SPRITE_NUM_OFF_OFFSET			3627
#define ITEM_PLATFORM_SPRITE_NUM_OFF_SIZE			2
#define ITEM_PLATFORM_SPRITE_MAGIC_NR				53
                  
// Enemy sprite number offset; Actual_sprite_number = stored_sprite_number + 312 - offset
#define ENEMY_SPRITE_NUM_OFF_OFFSET					3629
#define ENEMY_SPRITE_NUM_OFF_SIZE					2
#define ENEMY_SPRITE_MAGIC_NR						312

// secrets
#define SECRETS_OFFSET								3631
#define SECRETS_SIZE								5
#define SECRETS_COUNT								80

// items
#define ITEMS_OFFSET								4287
#define ITEMS_SIZE									7
#define ITEMS_COUNT									70

// platforms
#define PLATFORMS_OFFSET							4777
#define PLATFORMS_SIZE								15
#define PLATFORMS_COUNT								16




#endif
