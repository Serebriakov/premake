#include "enemy1.h"


Enemy1::Enemy1(const Level *LevelPtr) : EnemyBase(1, LevelPtr, false)
{
}

void Enemy1::SetPosition(const glm::vec2 &Position)
{
	m_Position = Position;
}

const glm::vec2 &Enemy1::GetPosition() const
{
	return m_Position;
}

void Enemy1::Update(const Player *PlayerPtr)
{
}

void Enemy1::Die()
{
	Deactivate();
}

void Enemy1::Initialize()
{
	SetState(IDLE);
	// Activate();
	m_Sprite->SetPosition(m_Position);
}
