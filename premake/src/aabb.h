#ifndef _AABB_H_
#define _AABB_H_

#include <glm/glm.hpp>


/*
Axis-Aligned Bounding Box
*/
class AABB
{
public :
	glm::vec2					m_Position;										// the rectangle's upper left corner
	glm::vec2					m_Size;											// the rectangle's width and height

	AABB();
	AABB(const glm::vec2 &Position, const glm::vec2 &Size);

	// check for intersection with another rectangle
	bool IntersectsWidth(const AABB &Other) const;

	glm::vec2 GetCenterPosition() const;
};

#endif
