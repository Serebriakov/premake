#include "animationmanager.h"
#include "glesrenderer.h"


AnimationManager &AnimationManager::Instance()
{
	static AnimationManager instance;
	return instance;
}

AnimationManager::~AnimationManager()
{
	RemoveAnimations();
}

void AnimationManager::AddAnimation(AnimationBase *NewAnimation)
{
	NewAnimation->ResetTimer();
	m_Animations.push_back(NewAnimation);
}

void AnimationManager::RemoveAnimations()
{
	for( std::list<AnimationBase *>::iterator it = m_Animations.begin(), end = m_Animations.end(); it != end; ++it )
	{
		delete (*it);
	}
	m_Animations.clear();
}

void AnimationManager::UpdateAnimations()
{
	for( std::list<AnimationBase *>::iterator it = m_Animations.begin(), end = m_Animations.end(); it != end; )
	{
		(*it)->Animate();

		if( (*it)->Expired() )
		{
			delete (*it);
			it = m_Animations.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void AnimationManager::DrawAnimations()
{
	for( std::list<AnimationBase *>::iterator it = m_Animations.begin(), end = m_Animations.end(); it != end; ++it )
	{
		GLESRenderer::Instance()->AddSprite2D((*it)->GetSprite());
	}
	GLESRenderer::Instance()->FlushSprites();
}
