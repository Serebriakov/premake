#ifndef _GATE_ANIMATION_H_
#define _GATE_ANIMATION_H_

#include "sprite.h"
#include "stopwatch.h"


/*
Class of animations played when entering/leaving a gate
*/
class GateAnimation
{
protected :
	glm::vec2					m_Position[2];									// the positions where the sprite has to be drawn
	Sprite						*m_Sprite;										// the sprite used for drawing an overlay sprite when entering/leaving a gate
	Stopwatch					m_AnimationTimer;								// timer used for the animation when entering/leaving a gate


public :
	GateAnimation();
	~GateAnimation();

	void Initialize(Uint32 AnimationLength = 1000);

	float GetAnimationState() const;

	void StartAnimation();
	void Update();
	void Draw();
};

#endif
