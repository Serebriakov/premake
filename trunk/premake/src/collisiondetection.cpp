#include "collisiondetection.h"
#include "aabb.h"
#include "tile.h"
#include "platform.h"
#include "enemybase.h"


CollisionDetectionResult::CollisionDetectionResult()
{
	ObstacleTileX = NULL;
	ObstacleTileY = NULL;
	ObstaclePlatform = NULL;
	ObstacleEnemy = NULL;
}
