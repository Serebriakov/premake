#include "enemy2.h"
#include "animationmanager.h"
#include "dyinganimation.h"
#include "glesrenderer.h"


Enemy2::Enemy2(const Level *LevelPtr) : EnemyBase(2, LevelPtr, true)
{
}

void Enemy2::SetStartPosition(const glm::vec2 &Position)
{
	m_StartPosition = Position;
}

const glm::vec2 &Enemy2::GetStartPosition() const
{
	return m_StartPosition;
}

void Enemy2::SetMaxWebLength(float MaxLength)
{
	m_MaxWebLength = MaxLength;
}

float Enemy2::GetMaxWebLength() const
{
	return m_MaxWebLength;
}

void Enemy2::SetSpeed(float Speed)
{
	m_Speed = Speed;
}

float Enemy2::GetSpeed() const
{
	return m_Speed;
}

void Enemy2::Update(const Player *PlayerPtr)
{
	if( m_Active )
	{
		// move up / down
		m_CurrentWebLength += m_CurrentDirection * m_Speed / 2.f;
		
		// change direction
		if( (m_CurrentDirection == DOWN) && (m_CurrentWebLength >= m_MaxWebLength) )
		{
			m_CurrentWebLength = m_MaxWebLength;
			m_CurrentDirection = UP;
		}
		else if( (m_CurrentDirection == UP) && (m_CurrentWebLength <= 0) )
		{
			m_CurrentWebLength = 0;
			m_CurrentDirection = DOWN;
		}

		// set frame according to moving direction
		m_Sprite->JumpToFrame((m_CurrentDirection == DOWN) ? (0) : (1));

		// set position
		m_Sprite->SetPosition(glm::vec2(m_StartPosition.x - m_Sprite->GetSize().x / 2.f, m_StartPosition.y + m_CurrentWebLength));
	}
}

void Enemy2::Die()
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

	Deactivate();
}

void Enemy2::Initialize()
{
	SetState(MOVING);
	Activate();
	m_StartPosition.y -= m_Sprite->GetSize().y;
	m_Sprite->SetPosition(m_StartPosition);

	m_CurrentWebLength = 0;
	m_CurrentDirection = DOWN;
}
