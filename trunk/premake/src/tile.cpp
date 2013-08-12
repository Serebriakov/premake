#include "tile.h"


Tile::Tile(Sprite *SpritePtr0, Sprite *SpritePtr1, Sprite *SpritePtr2, Sprite *OverlayingSprite)
{
	m_Sprites[0] = SpritePtr0;
	m_Sprites[1] = SpritePtr1;
	m_Sprites[2] = SpritePtr2;

	m_OverlayingSprite = OverlayingSprite;

	m_Animated = false;
	m_ChangeWhenSteppedOn = false;

	// if the tile has 3 sprites, it is animated
	if( (m_Sprites[1] != NULL) && (m_Sprites[2] != NULL) )
	{
		m_Animated = true;
	}

	// if the tile ha 2 sprites, it can be changed by the player
	if( (m_Sprites[1] != NULL) && (m_Sprites[2] == NULL) )
	{
		m_ChangeWhenSteppedOn = true;
	}
}

Tile::~Tile()
{
	delete m_Sprites[0];
	delete m_Sprites[1];
	delete m_Sprites[2];
	delete m_OverlayingSprite;
}

void Tile::SetSideProperty(SIDE Side, SIDE_PROPERTY SideProperty)
{
	m_SideProperty[Side] = SideProperty;
}

void Tile::SetSideProperties(SIDE_PROPERTY Bottom, SIDE_PROPERTY Top, SIDE_PROPERTY Left, SIDE_PROPERTY Right)
{
	m_SideProperty[BOTTOM] = Bottom;
	m_SideProperty[TOP] = Top;
	m_SideProperty[LEFT] = Left;
	m_SideProperty[RIGHT] = Right;
}

Tile::SIDE_PROPERTY Tile::GetSideProperty(SIDE Side) const
{
	return m_SideProperty[Side];
}

void Tile::SetSlopeType(SLOPE_TYPE Type)
{
	m_SlopeType = Type;
}

Tile::SLOPE_TYPE Tile::GetSlopeType() const
{
	return m_SlopeType;
}

void Tile::SetReverseHeights(unsigned Left, unsigned Right)
{
	m_ReversedHeightLeft = Left;
	m_ReversedHeightRight = Right;
}

unsigned Tile::GetReverseHeightLeft() const
{
	return m_ReversedHeightLeft;
}

unsigned Tile::GetReverseHeightRight() const
{
	return m_ReversedHeightRight;
}

const Sprite *Tile::GetSprite(unsigned char Index) const
{
	return m_Sprites[Index];
}

const Sprite *Tile::GetOverlayingSprite() const
{
	return m_OverlayingSprite;
}

bool Tile::IsAnimated() const
{
	return m_Animated;
}

bool Tile::IsChangedWhenSteppedOn() const
{
	return m_ChangeWhenSteppedOn;
}
