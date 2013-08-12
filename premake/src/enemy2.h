#ifndef _ENEMY_2_H_
#define _ENEMY_2_H_

#include "enemybase.h"


/*
"Moves up and down on a web (Spiders)"
*/
class Enemy2 : public EnemyBase
{
protected :
	enum DIRECTION
	{
		UP = -1,
		DOWN = 1
	};

	glm::vec2					m_StartPosition;
	float						m_MaxWebLength;
	float						m_CurrentWebLength;
	float						m_Speed;
	DIRECTION					m_CurrentDirection;


public :
	Enemy2(const Level *LevelPtr);

	void SetStartPosition(const glm::vec2 &Position);
	const glm::vec2 &GetStartPosition() const;
	void SetMaxWebLength(float MaxLength);
	float GetMaxWebLength() const;
	void SetSpeed(float Speed);
	float GetSpeed() const;

	virtual void Update(const Player *PlayerPtr);
	virtual void Die();
	virtual void Initialize();
};

#endif
