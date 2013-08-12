#ifndef _DYING_ANIMATION_H_
#define _DYING_ANIMATION_H_

#include "animationbase.h"


class DyingAnimation : public AnimationBase
{
protected :
	Sprite						*m_Sprite;										// the sprite to be drawn
	glm::vec2					m_StartPosition;								// start position of the sprite
	glm::vec2					m_InitialVelocity;
	float						m_Angle;


public :
	DyingAnimation(Uint32 Lifetime, Sprite *SpritePtr, const glm::vec2 &InitialVelocity, float Angle = 60);
	DyingAnimation(const DyingAnimation &Other);

	virtual ~DyingAnimation();

	virtual void Animate();
	virtual const Sprite *GetSprite() const;

	void SetStartPosition(const glm::vec2 &Position);
};

#endif
