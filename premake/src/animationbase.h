#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "stopwatch.h"
#include "sprite.h"


/*
Base class for animations
*/
class AnimationBase
{
protected :
	Stopwatch					*m_Timer;


public :
	AnimationBase(Uint32 Lifetime);
	AnimationBase(const AnimationBase &Other);
	virtual ~AnimationBase();

	void ResetTimer();

	bool Expired() const;

	virtual void Animate() = 0;
	virtual const Sprite *GetSprite() const = 0;
};

#endif
