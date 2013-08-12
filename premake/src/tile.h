#ifndef _TILE_H_
#define _TILE_H_

#include "sprite.h"


/*
Class of tile objects.
Levels contain a grid of tiles.
*/
class Tile
{
public :
	// identifiers of sides
	enum SIDE
	{
		BOTTOM,
		TOP,
		LEFT,
		RIGHT,
		NUMBER_OF_SIDES
	};

	// tile's side's property
	enum SIDE_PROPERTY
	{
		TRAVERSABLE,															// objects can go through the tile
		TRAVERSABLE_WHEN_PRESSING_DOWN,											// the player can fall through the tile while pressing down
		SOLID,																	// objects cannot pass through the tile
		DEADLY																	// touch and die...
	};

	// tile's slope type
	enum SLOPE_TYPE
	{
		HORIZONTAL_FLOOR,														// the tile is a horizontal floor, so reversed heights shouldn't be used
		SLOPE_DOWNRIGHT,														// the tile looks like a backslash
		SLOPE_UPRIGHT															// the tile looks like a slash character
	};


protected :
	SIDE_PROPERTY				m_SideProperty[NUMBER_OF_SIDES];				// sides' properties
	SLOPE_TYPE					m_SlopeType;									// the tile's type
	unsigned					m_ReversedHeightLeft;							// reversed height on the left side (only if the tile's not a horizontal floor)
	unsigned					m_ReversedHeightRight;							// reversed height on the right side (only if the tile's not a horizontal floor)
	Sprite						*m_Sprites[3];									// pointer(s) to the tile's sprite(s)
	Sprite						*m_OverlayingSprite;							// the sprite that overlays the background and the player, if the tile has one (null-pointer otherwise)
	bool						m_Animated;										// if true, the m_SpritePtr array stores 3 valid pointers, otherwise only 1
	bool						m_ChangeWhenSteppedOn;							// if true, the sprite changes to the 2nd one, while the player is standing on it


public :
	Tile(Sprite *SpritePtr0 = NULL, Sprite *SpritePtr1 = NULL, Sprite *SpritePtr2 = NULL, Sprite *OverlayingSprite = NULL);
	~Tile();

	void SetSideProperty(SIDE Side, SIDE_PROPERTY SideProperty);
	void SetSideProperties(SIDE_PROPERTY Bottom, SIDE_PROPERTY Top, SIDE_PROPERTY Left, SIDE_PROPERTY Right);
	SIDE_PROPERTY GetSideProperty(SIDE Side) const;

	void SetSlopeType(SLOPE_TYPE Type);
	SLOPE_TYPE GetSlopeType() const;

	void SetReverseHeights(unsigned Left, unsigned Right);
	unsigned GetReverseHeightLeft() const;
	unsigned GetReverseHeightRight() const;

	const Sprite *GetSprite(unsigned char Index) const;
	const Sprite *GetOverlayingSprite() const;

	bool IsAnimated() const;
	bool IsChangedWhenSteppedOn() const;
};

#endif
