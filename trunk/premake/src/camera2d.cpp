#include "camera2d.h"


Camera2D::Camera2D(const glm::vec2 &Position, const glm::vec2 &ViewAreaSize)
{
	m_ViewArea.m_Position = Position;
	m_ViewArea.m_Size = ViewAreaSize;
}


void Camera2D::SetPosition(const glm::vec2 &Position)
{
	m_ViewArea.m_Position = Position;
}
void Camera2D::SetCenterPosition(const glm::vec2 &Position)
{
	m_ViewArea.m_Position = Position - (m_ViewArea.m_Size / 2.f);
}


void Camera2D::SetViewAreaSize(const glm::vec2 &ViewAreaSize)
{
	m_ViewArea.m_Size = ViewAreaSize;
}

const glm::vec2 &Camera2D::GetPosition() const
{
	return m_ViewArea.m_Position;
}

const AABB &Camera2D::GetViewArea() const
{
	return m_ViewArea;
}
