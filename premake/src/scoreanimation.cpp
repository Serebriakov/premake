#include "scoreanimation.h"


ScoreAnimation::ScoreAnimation(Uint32 Lifetime, Sprite *ScoreSprite, float MaxDistance) : AnimationBase(Lifetime), m_Sprite(ScoreSprite), m_MaxDistance(MaxDistance)
{
	m_StartY = m_Sprite->GetPosition().y;
}

ScoreAnimation::ScoreAnimation(const ScoreAnimation &Other) : AnimationBase(Other)
{
	m_Sprite = new Sprite(*(Other.m_Sprite));
	m_MaxDistance = Other.m_MaxDistance;
	m_StartY = Other.m_StartY;
}

ScoreAnimation::~ScoreAnimation()
{
	delete m_Sprite;
}

void ScoreAnimation::Animate()
{
	m_Sprite->Animate();

	glm::vec2 pos = m_Sprite->GetPosition();
	pos.y = m_StartY - (float)(m_Timer->GetCycleState() * m_MaxDistance);
	m_Sprite->SetPosition(pos);
}

const Sprite *ScoreAnimation::GetSprite() const
{
	return m_Sprite;
}

void ScoreAnimation::SetStartPosition(const glm::vec2 &Position)
{
	m_Sprite->SetPosition(Position);
	m_StartY = Position.y;
}

