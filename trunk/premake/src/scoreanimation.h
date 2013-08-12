#ifndef _SCORE_ANIMATION_H_
#define _SCORE_ANIMATION_H_

#include "animationbase.h"


class ScoreAnimation : public AnimationBase
{
protected :
	Sprite						*m_Sprite;
	float						m_MaxDistance;
	float						m_StartY;


public :
	ScoreAnimation(Uint32 Lifetime, Sprite *ScoreSprite, float MaxDistance = 80);
	ScoreAnimation(const ScoreAnimation &Other);

	virtual ~ScoreAnimation();

	virtual void Animate();
	virtual const Sprite *GetSprite() const;

	void SetStartPosition(const glm::vec2 &Position);
};

#endif
