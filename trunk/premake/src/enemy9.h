#ifndef _ENEMY_9_H_
#define _ENEMY_9_H_

#include "enemybase.h"
#include "aabb.h"


/*
"Goes left-right, ignoring walls (ignoring walls depends on byte 4 (unknown1), in the case of penguins at level 7, can go by the slopes...
But in other cases, not affected by gravity.)"
*/
class Enemy9 : public EnemyBase
{
public :
	enum DIRECTION
	{
		LEFT = -1,
		RIGHT = 1
	};


protected :
	unsigned char				m_Speed;
	float						m_BorderLeft;
	float						m_BorderRight;
	glm::vec2					m_CurrentPosition;
	DIRECTION					m_CurrentDirection;

	void SetDirection(DIRECTION Direction);
	

public :
	Enemy9(const Level *LevelPtr);

	virtual void Update(const Player *PlayerPtr);
	virtual void Die();
	virtual void Initialize();

	void SetCurrentPosition(const glm::vec2 &Position);
	void SetSpeed(unsigned char Speed);
	unsigned char GetSpeed() const;
	void SetBorders(float BorderLeft, float BorderRight);
	float GetBorderLeft() const;
	float GetBorderRight() const;
};

#endif
