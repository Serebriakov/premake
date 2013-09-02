#include "dyinganimation.h"


DyingAnimation::DyingAnimation(Uint32 Lifetime, Sprite *SpritePtr, const glm::vec2 &InitialVelocity, float Angle) : AnimationBase(Lifetime), m_Sprite(SpritePtr), m_InitialVelocity(InitialVelocity), m_Angle(Angle)
{
	m_StartPosition = m_Sprite->GetPosition();
}

DyingAnimation::DyingAnimation(const DyingAnimation &Other) : AnimationBase(Other)
{
	m_Sprite = new Sprite(*(Other.m_Sprite));
	m_Angle = Other.m_Angle;
	m_StartPosition = Other.m_StartPosition;
	m_InitialVelocity = Other.m_InitialVelocity;
}

DyingAnimation::~DyingAnimation()
{
	delete m_Sprite;
}

void DyingAnimation::Animate()
{
	m_Sprite->Animate();

	float t = (float)(m_Timer->GetCycleState() * 50.f);
	
	glm::vec2 current_pos;
	current_pos.x = m_StartPosition.x + (m_InitialVelocity.x * t * (float)cos(m_Angle * M_PI / 180.f));
	current_pos.y = m_StartPosition.y - (m_InitialVelocity.y * t * (float)sin(m_Angle * M_PI / 180.f) - (t * t));

	m_Sprite->SetPosition(current_pos);
}

const Sprite *DyingAnimation::GetSprite() const
{
	return m_Sprite;
}

void DyingAnimation::SetStartPosition(const glm::vec2 &Position)
{
	m_StartPosition = Position;
	m_Sprite->SetPosition(Position);
}
