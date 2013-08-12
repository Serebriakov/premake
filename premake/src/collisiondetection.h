#ifndef _COLLISION_DETECTION_H_
#define _COLLISION_DETECTION_H_

#include <glm/glm.hpp>

class Tile;
class Platform;
class EnemyBase;

/*
Results of collision detection
*/
class CollisionDetectionResult
{
public :
	bool						Collided;										// true if the object collided with something, false otherwise
	glm::vec2					NewPosition;									// the object's new position after the collision detection

	Tile						*ObstacleTileX;									// pointer to the touching tile horizontally
	Tile						*ObstacleTileY;									// pointer to the touching tile vertically
	Platform					*ObstaclePlatform;								// the platform the object collided with
	EnemyBase					*ObstacleEnemy;									// the enemy the object jumped on

	CollisionDetectionResult();
};

#endif
