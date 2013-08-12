#ifndef _CAMERA_2D_H_
#define _CAMERA_2D_H_

#include <glm/glm.hpp>
#include "aabb.h"


/*
Camera type for 2D rendering
*/
class Camera2D
{
protected :
	AABB						m_ViewArea;										// the AABB representing the view area in world coordinates (upper left corner & width and height of the rectangle)


public :
	Camera2D(const glm::vec2 &Position = glm::vec2(0,0), const glm::vec2 &ViewAreaSize = glm::vec2(320, 200));

	void SetPosition(const glm::vec2 &Position);
	void SetCenterPosition(const glm::vec2 &Position);
	void SetViewAreaSize(const glm::vec2 &ViewAreaSize);
	const glm::vec2 &GetPosition() const;
	const AABB &GetViewArea() const;
};

#endif
