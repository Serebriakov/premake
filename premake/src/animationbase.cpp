#include "animationbase.h"


AnimationBase::AnimationBase(Uint32 Lifetime)
{
	m_Timer = new Stopwatch(Lifetime);
}

AnimationBase::AnimationBase(const AnimationBase &Other)
{
	m_Timer = new Stopwatch(*(Other.m_Timer));
}

AnimationBase::~AnimationBase()
{
	delete m_Timer;
}

void AnimationBase::ResetTimer()
{
	m_Timer->Reset();
}

bool AnimationBase::Expired() const
{
	return (m_Timer->GetCycleState() >= 1.f);
}
