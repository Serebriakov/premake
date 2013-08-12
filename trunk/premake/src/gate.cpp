#include "gate.h"


Gate::Gate(const AABB &Entrance, const glm::vec2 &ExitPosition, bool Scrolling, const glm::vec2 &CameraPosition) : m_EntranceHitbox(Entrance), m_ExitPosition(ExitPosition), m_ScrollingEnabled(Scrolling), m_CameraPosition(CameraPosition)
{
}

const AABB &Gate::GetEntranceHitbox() const
{
	return m_EntranceHitbox;
}

const glm::vec2 &Gate::GetExitPosition() const
{
	return m_ExitPosition;
}

bool Gate::IsScrollingEnabled() const
{
	return m_ScrollingEnabled;
}

const glm::vec2 &Gate::GetCameraPosition() const
{
	return m_CameraPosition;
}
