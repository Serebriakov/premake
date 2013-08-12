#ifndef _ENEMY_8_H_
#define _ENEMY_8_H_

#include "enemybase.h"
#include "collisiondetection.h"
#include "stopwatch.h"


/*
"Jumps (Dinos, leopards, little wormies...)"
*/
class Enemy8 : public EnemyBase
{
public :
	enum DIRECTION
	{
		LEFT = -1,
		RIGHT = 1
	};


protected :
	glm::vec2					m_ActivationDistance;							// if the player is closer than this, the enemy changes state from idle to moving (attacking)
	glm::vec2					m_JumpPower;
	glm::vec2					m_Position;
	glm::vec2					m_Velocity;
	DIRECTION					m_CurrentDirection;
	Stopwatch					m_Timer;
	CollisionDetectionResult	m_CDResult;

	void SetDirection(DIRECTION Direction);


public :
	Enemy8(const Level *LevelPtr);

	virtual void Update(const Player *PlayerPtr);
	virtual void Die();
	virtual void Initialize();

	void SetActivationDistance(const glm::vec2 &Distance);
	const glm::vec2 &GetActivationDistance() const;
	void SetJumpPower(const glm::vec2 &Power);
	const glm::vec2 &GetJumpPower() const;
	void SetPosition(const glm::vec2 &Position);
	const glm::vec2 &GetPosition() const;
};

#endif
