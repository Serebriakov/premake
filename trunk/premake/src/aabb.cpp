#include "aabb.h"


AABB::AABB() : m_Position(0, 0), m_Size(0, 0)
{
}

AABB::AABB(const glm::vec2 &Position, const glm::vec2 &Size) : m_Position(Position), m_Size(Size)
{
}

/*
Check for intersection with another rectangle
*/
bool AABB::IntersectsWidth(const AABB &Other) const
{
	glm::vec2 mid_point1 = GetCenterPosition();
	glm::vec2 mid_point2 = Other.GetCenterPosition();

	return (fabs(mid_point1.x - mid_point2.x) < ((m_Size.x + Other.m_Size.x) / 2.f)) && (fabs(mid_point1.y - mid_point2.y) < ((m_Size.y + Other.m_Size.y) / 2.f));
}

glm::vec2 AABB::GetCenterPosition() const
{
	return m_Position + (m_Size / 2.f);
}
