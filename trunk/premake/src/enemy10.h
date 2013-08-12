#ifndef _ENEMY_10_H_
#define _ENEMY_10_H_

#include "enemybase.h"
#include "aabb.h"
#include "stopwatch.h"


/*
"Spawns from the ground, walks some time, then goes underground again"
*/
class Enemy10 : public EnemyBase
{
public :
	enum DIRECTION
	{
		LEFT = -1,
		RIGHT = 1
	};


protected :
	AABB						m_Area;
	unsigned char				m_Speed;
	Stopwatch					m_Timer;
	glm::vec2					m_Position;
	DIRECTION					m_Direction;
	CollisionDetectionResult	m_CDResult;

	void Spawn();
	void Disappear();
	void SetDirection(DIRECTION Direction);
	

public :
	Enemy10(const Level *LevelPtr);

	virtual void Update(const Player *PlayerPtr);
	virtual void Die();
	virtual void Initialize();

	void SetArea(const AABB &Area);
	const AABB &GetArea() const;

	void SetSpeed(unsigned char Speed);
	unsigned char GetSpeed() const;
};

#endif
