#ifndef _LEVEL_TYPES_H_
#define _LEVEL_TYPES_H_


/*
Gates:
	xin: 1
	yin: 1
	xscreen: 1
	yscreen: 1
	xout: 1
	yout: 1
	scroll: 1

"In/out are the entrance/exit, screen is the screen's position after exiting
the gate, scroll indicates whether the screen can scroll after going through."

Entrance and exit are tile numbers in the game's coordinate system.
*/
struct Gate
{
	unsigned char x_in;
	unsigned char y_in;
	unsigned char x_screen;
	unsigned char y_screen;
	unsigned char x_out;
	unsigned char y_out;
	unsigned char scroll;
};


/*
Shifting tile blocks:
	x: 1
	y: 1
	width: 1
	height: 1
	xact: 1
	yact: 1
	FFFF: 2 (constant)
	dist: 1
	00: 1 (constant)

"No description yet."
*/
struct ShiftingTileBlock
{
	unsigned char x;
	unsigned char y;
	unsigned char width;
	unsigned char height;
	unsigned char x_act;
	unsigned char y_act;
	unsigned char dist;
};

/*
Enemy records: (size of each record varies)
	length: 1 (length of this record)
	type/expert: 1
	sprite: 2
	unknown1: 1
	hitpoints: 1
	pause: 1
	unknown2: 1
	score: 1
	x: 2
	y: 2
	type-specific: variable length (remainder of record)
*/
struct Enemy
{
	unsigned char length;
	unsigned char type_expert;
	unsigned short sprite;
	unsigned char hitpoints;
	unsigned char pause;
	unsigned char score;
	unsigned short x;
	unsigned short y;
	unsigned char type_specific[10];
};

/*
Secrets:
	FromTile: 1
	ToTile: 1
	Bonus/health: 1
	x: 1
	y: 1

"FromTile: Initial tile
ToTile: Not used, tile changes to the tile on the map (in original levels tileTo=tile on the map, changing does no effect).
Bonus/Health:
    00 to 3F: 1 to 64 small bonuses
    40 to 7F: 1 to 64 hits needed. Stars, dirt bits etc go out, depending on the level (maybe stored somewhere, maybe in exe, not yet known)
    80 to FF: Big bonus, hitpoints=(value-127)*x (or so) x is about 5 or 6
If several such tiles are adjacent, they all change.
If one of them changes and you hit several of them at one time, Bonus/Health of rightmost of the lowest row (from the ones you hit) is used (needs to be checked)."

X and Y are tile numbers in the game's coordinate system.
*/
struct Secret
{
	unsigned char tile_from;
	unsigned char tile_to;
	unsigned char bonus_health;
	unsigned char x;
	unsigned char y;
};


/*
Items:
	posx: 2
	posy: 2
	sprite: 2
	00: 1 (constant)

"Should also be clear. The level exit is also among the items.
The last byte in the items records is always zero; if set to anything else, nothing changes ingame."
*/
struct Item
{
	unsigned short pos_x;
	unsigned short pos_y;
	unsigned short sprite;
};


/*
Platforms:
	posx: 2
	posy: 2
	sprite: 2
	behavior: 1
	speed: 1
	????: 1 (always FF)
	drop delay: 1
	distance: 2
	????: 1 (always 0)
	drop1?: 1
	drop2?: 1

"Behavior:
    Bit  Meaning
    0..2 Direction of moving
         7 0 1
         6 * 2
         5 4 3
         I.e. when 5, moves downwards towards the left.
    3    Dropdown
    7    If set, then platform works by itself; if not set, starts moving when
         player is on and stops at end of the cycle when he steps off
  Speed:
    Max speed of movement. If speed = zero and behavior = 08, then if stepped
    on platform drops down quickly until it hits solid ground (or into the
    abyss, if there is none) and waits, until player steps off
  Drop delay:
    If not dropdown - then FF
    If dropdown, the delay before dropping
  Distance:
    Time of moving on max speed if not dropdown, else zero
    Distance of moving: speed*(time+speed-1). Additional speed^2-speed is due
    to accelerating and braking.
  Drop1?:
    If not dropdown - zero, else = drop delay
  Drop2?:
    If not dropdown - zero, else = FF"
*/
struct Platform
{
	unsigned short pos_x;
	unsigned short pos_y;
	unsigned short sprite;
	unsigned char behavior;
	unsigned char speed;
	unsigned char drop_delay;
	unsigned short distance;
	unsigned char drop1;
	unsigned char drop2;
};


#endif
