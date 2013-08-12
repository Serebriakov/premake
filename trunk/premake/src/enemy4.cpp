#include "enemy4.h"
#include "animationmanager.h"
#include "dyinganimation.h"


Enemy4::Enemy4(const Level *LevelPtr) : EnemyBase(4, LevelPtr, true)
{
}

void Enemy4::SetRadius(float Radius)
{
	m_Radius = Radius;
}

float Enemy4::GetRadius() const
{
	return m_Radius;
}

void Enemy4::SetMaxAngle(float MaxAngle)
{
	m_MaxAngle = MaxAngle;
}

float Enemy4::GetMaxAngle() const
{
	return m_MaxAngle;
}

void Enemy4::SetOrigoPosition(const glm::vec2 &Position)
{
	m_OrigoPosition = Position;
}

const glm::vec2 &Enemy4::GetOrigoPosition() const
{
	return m_OrigoPosition;
}

void Enemy4::Update(const Player *PlayerPtr)
{
	if( m_Active )
	{
		// move left / right
		m_CurrentAngle += m_CurrentDirection * 1.5f;
		
		// change direction
		if( (m_CurrentDirection == CW) && (m_CurrentAngle <= -m_MaxAngle) )
		{
			m_CurrentAngle = -m_MaxAngle;
			m_CurrentDirection = CCW;
		}
		else if( (m_CurrentDirection == CCW) && (m_CurrentAngle >= m_MaxAngle) )
		{
			m_CurrentAngle = m_MaxAngle;
			m_CurrentDirection = CW;
		}

		// set position
		glm::vec2 pos = m_OrigoPosition + glm::vec2(cos((m_CurrentAngle - 90) * M_PI / 180.f), -1 * sin((m_CurrentAngle - 90) * M_PI / 180.f)) * m_Radius;
		pos.x -= m_Sprite->GetSize().x / 2.f;
		m_Sprite->SetPosition(pos);
	}
}

void Enemy4::Die()
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

void Enemy4::Initialize()
{
	SetState(MOVING);
	Activate();
	m_Sprite->JumpToFrame(1);
	
	m_OrigoPosition.y -= m_Sprite->GetSize().y;
	m_CurrentAngle = 0;
	m_CurrentDirection = CW;
}
