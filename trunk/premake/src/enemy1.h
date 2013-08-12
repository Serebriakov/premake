#ifndef _ENEMY_1_H_
#define _ENEMY_1_H_

#include "enemybase.h"


/*
"Does not move, attack or die... Just a plain decoration (Spiderwebs)"
*/
class Enemy1 : public EnemyBase
{
protected :
	glm::vec2					m_Position;


public :
	Enemy1(const Level *LevelPtr);

	void SetPosition(const glm::vec2 &Position);
	const glm::vec2 &GetPosition() const;

	virtual void Update(const Player *PlayerPtr);
	virtual void Die();
	virtual void Initialize();
};

#endif
