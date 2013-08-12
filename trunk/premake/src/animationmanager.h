#ifndef _ANIMATION_MANAGER_H_
#define _ANIMATION_MANAGER_H_

#include <list>
#include "animationbase.h"


/*
Animation manager: stores, updates and draws animations, and deletes expired ones.
*/
class AnimationManager
{
private :
	AnimationManager() {}
	AnimationManager(const AnimationManager &Other);
	const AnimationManager &operator=(const AnimationManager &Other);


protected :
	std::list<AnimationBase *>	m_Animations;									// list of managed animation objects


public :
	static AnimationManager &Instance();
	~AnimationManager();

	void AddAnimation(AnimationBase *NewAnimation);
	void RemoveAnimations();

	void UpdateAnimations();

	void DrawAnimations();
};

#endif
