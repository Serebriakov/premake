#ifndef _GATE_H_
#define _GATE_H_

#include <glm/glm.hpp>

#include "aabb.h"


/*
Class of gates (teleports) in the game
*/
class Gate
{
protected :
	AABB						m_EntranceHitbox;								// the player can enter when its hitbox intersects with this rectangle
	glm::vec2					m_ExitPosition;									// where the player comes out, in world coordinates
	bool						m_ScrollingEnabled;								// true, if scrolling is allowed after leaving the gate
	glm::vec2					m_CameraPosition;								// the camera's position in world space, after leaving the gate


public :
	Gate(const AABB &Entrance, const glm::vec2 &ExitPosition, bool Scrolling, const glm::vec2 &CameraPosition);

	const AABB &GetEntranceHitbox() const;
	const glm::vec2 &GetExitPosition() const;
	bool IsScrollingEnabled() const;
	const glm::vec2 &GetCameraPosition() const;
};

#endif
