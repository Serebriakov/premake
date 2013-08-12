#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <vector>

#include "camera2d.h"
#include "tile.h"
#include "item.h"
#include "itemconfiguration.h"
#include "platform.h"
#include "aabb.h"
#include "gate.h"
#include "player.h"
#include "enemybase.h"
#include "stopwatch.h"
#include "collisiondetection.h"


/*
Type of levels
*/
class Level
{
protected :
	int							m_Width;										// width of the level in number of tiles
	int							m_Height;										// height of the level in number of tiles
	std::vector<Tile *>			m_Tiles;										// array of tiles (m_Width * m_Height elements)
	Tile						*m_TileUnderPlayer;								// pointer to the tile the player is currently standing on
	bool						m_HasOverlayingSprites;							// indicates if the level has at least 1 overlaying sprite
	glm::vec2					m_TileSize;										// 1 tile's resolution in pixels
	unsigned char				m_AnimatedTileFrameNumber;						// frame number of the animated tiles (0..2)
	std::vector<Item *>			m_Items;										// array of items
	std::vector<Platform *>		m_Platforms;									// array of platforms
	std::vector<Gate *>			m_Gates;										// array of gates
	std::vector<EnemyBase *>	m_Enemies;										// array of enemies
	Sprite						*m_BackgroundSprite;							// static background image
	glm::vec2					m_StartPosition;								// this is where the player starts
	Stopwatch					m_BackgroundAnimTimer;							// timer used for animating background tiles


public :
	enum ERROR_CODE
	{
		OK,
		PARSING_FAILED
	};
	
	Level();
	~Level();

	int GetWidth() const;
	int GetHeight() const;

	const glm::vec2 &GetTileSize() const;

	ERROR_CODE LoadFromFile(const std::string &Path, const std::string &EnemyConfigPath, ItemConfiguration &ItemConfig, RenderState *RSBackgroundPtr, RenderState *RSTilesPtr);
	void ReleaseResources();

	void DrawBackground(const Camera2D &Camera);
	void DrawOverlayingSprites(const Camera2D &Camera);

	void UpdatePlatforms();
	void DrawPlatforms();

	void UpdateItems(const Camera2D &Camera);
	void DrawItems();

	// pick up an item if the character intersects with it and return its type
	Item *PickItem(const Player *PlayerPtr);

	void UpdateEnemies(const Player *PlayerPtr);
	void DrawEnemies();
	const EnemyBase *KillEnemies(const Weapon *WeaponPtr);
	const EnemyBase *GetDamagingEnemy(const Player *PlayerPtr);

	const glm::vec2 &GetStartPosition() const;

	void MoveAABB(const AABB &Hitbox, const glm::vec2 &DeltaPosition, CollisionDetectionResult &Result, bool CheckTiles, bool PassThroughHatches, bool CheckPlatforms, bool CheckEnemies) const;
	glm::vec2 MovePlayer(const Player *PlayerPtr, CollisionDetectionResult &Result);

	// return the gate the player comes out of
	Gate *TeleportPlayer(const Player *PlayerPtr);

	const Tile &GetTile(int Row, int Column) const;

	void AnimateTiles();

	// convert coordinates between coordinate systems
	glm::vec2 WorldToTileCoordinates(const glm::vec2 &WorldPos) const;
	glm::vec2 TileToWorldCoordinates(const glm::vec2 &TileIndices) const;

	// find the first ground tile, starting from a given tile
	glm::vec2 FindFirstGroundTileAbove(const glm::vec2 &StartingTileIndices) const;

	// get the tile the player is standing on
	const Tile *GetTileUnderPlayer() const;
};

#endif
