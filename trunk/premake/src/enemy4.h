#ifndef _ENEMY_4_H_
#define _ENEMY_4_H_

#include "enemybase.h"


/*
"Pendulum spiders"
*/
class Enemy4 : public EnemyBase
{
protected :
	enum DIRECTION
	{
		CW = -1,
		CCW = 1
	};

	float						m_Radius;
	float						m_MaxAngle;
	float						m_CurrentAngle;
	glm::vec2					m_OrigoPosition;
	DIRECTION					m_CurrentDirection;


public :
	Enemy4(const Level *LevelPtr);

	void SetRadius(float Radius);
	float GetRadius() const;
	void SetMaxAngle(float MaxAngle);
	float GetMaxAngle() const;
	void SetOrigoPosition(const glm::vec2 &Position);
	const glm::vec2 &GetOrigoPosition() const;

	virtual void Update(const Player *PlayerPtr);
	virtual void Die();
	virtual void Initialize();
};

#endif
