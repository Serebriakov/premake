#ifndef _WEAPON_H_
#define _WEAPON_H_

#include <glm/glm.hpp>

#include "sprite.h"


class Weapon
{
public :
	enum WEAPON_STATE
	{
		ATTACKING,
		ATTACKING_UP,
		ATTACKING_DOWN,
		NUMBER_OF_STATES
	};


protected :
	Sprite						*m_Sprite;
	WEAPON_STATE				m_CurrentState;
	size_t						m_AnimationIndex[NUMBER_OF_STATES];
	std::vector<glm::vec2>		m_Offsets[NUMBER_OF_STATES];


public :
	Weapon();
	~Weapon();

	void LoadConfigFromFile(const std::string &FilePath);

	const Sprite *GetSprite() const;

	void SetState(WEAPON_STATE State);

	WEAPON_STATE GetState() const;

	const glm::vec2 &GetPositionOffset(size_t FrameNumber);

	void UseFrame(const Sprite *PlayerSprite);
};

#endif
