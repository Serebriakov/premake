#include "enemy9.h"
#include "animationmanager.h"
#include "dyinganimation.h"


Enemy9::Enemy9(const Level *LevelPtr) : EnemyBase(9, LevelPtr, true)
{
}

void Enemy9::Update(const Player *PlayerPtr)
{
	if( m_Active )
	{
		m_CurrentPosition.x += m_CurrentDirection * m_Speed / 100.f;

		if( (m_CurrentDirection == LEFT) && (m_CurrentPosition.x <= m_BorderLeft) )
		{
			m_CurrentPosition.x = m_BorderLeft;
			SetDirection(RIGHT);
		}
		else if( (m_CurrentDirection == RIGHT) && (m_CurrentPosition.x >= m_BorderRight) )
		{
			m_CurrentPosition.x = m_BorderRight;
			SetDirection(LEFT);
		}

		// set sprite position & animate sprite
		m_Sprite->Animate();
		m_Sprite->SetPosition(glm::vec2(m_CurrentPosition.x - m_Sprite->GetSize().x / 2.f, m_CurrentPosition.y - m_Sprite->GetSize().y));
	}
}

void Enemy9::Die()
{
	// add dying animation
	Sprite *sprite = new Sprite(*m_Sprite);
	sprite->JumpToSequence(m_AnimationIndex[DYING]);
	DyingAnimation *animation = new DyingAnimation(2000, sprite, glm::vec2(14.f, 20.f));
	AnimationManager::Instance().AddAnimation(animation);

	// add score animation
	if( m_ScoreAnimation != NULL )
	{
		m_ScoreAnimation->SetStartPosition(m_Sprite->GetPosition());
		AnimationManager::Instance().AddAnimation(new ScoreAnimation(*m_ScoreAnimation));
	}

	// switch state
	Deactivate();
	SetState(DYING);
}

void Enemy9::Initialize()
{
	SetState(MOVING);
	Activate();
	
	m_CurrentDirection = RIGHT;
}

void Enemy9::SetCurrentPosition(const glm::vec2 &Position)
{
	m_CurrentPosition = Position;
}

void Enemy9::SetSpeed(unsigned char Speed)
{
	m_Speed = Speed;
}

unsigned char Enemy9::GetSpeed() const
{
	return m_Speed;
}

void Enemy9::SetBorders(float BorderLeft, float BorderRight)
{
	m_BorderLeft = BorderLeft;
	m_BorderRight = BorderRight;
}

float Enemy9::GetBorderLeft() const
{
	return m_BorderLeft;
}

float Enemy9::GetBorderRight() const
{
	return m_BorderRight;
}

void Enemy9::SetDirection(DIRECTION Direction)
{
	m_CurrentDirection = Direction;
	m_Sprite->FlipHorizontally(m_CurrentDirection == LEFT);
}
