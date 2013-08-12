#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "sprite.h"
#include "aabb.h"


/*
Class of moving platforms
*/
class Platform
{
public :
	// direction of moving
	/*
    7 0 1
    6 * 2
    5 4 3
	*/
	enum DIRECTION
	{
		UP,
		UPRIGHT,
		RIGHT,
		DOWNRIGHT,
		DOWN,
		DOWNLEFT,
		LEFT,
		UPLEFT
	};


protected :
	DIRECTION					m_DirectionOfMoving;							// the direction the platform is moving in when starting the cycle
	bool						m_Dropdown;										// the platform falls down when stepped on
	unsigned char				m_DropDelay;									// if dropdown, the delay before falling down
	bool						m_TriggeredByPlayer;							// if true, the platform starts moving when the player steps on it, otherwise works by itself
	unsigned short				m_MaxSpeed;										// max speed of movement
	unsigned short				m_MaxDistance;									// time of moving on max speed if not dropdown, else zero
	bool						m_IsUnderPlayer;								// the player is standing on the platform
	bool						m_CycleFinished;								// the platform has gone back to its start position
	float						m_CurrentDistance;								// current position is calculated from this...
	float						m_ElapsedTime;									// the time that has passed since the player stepped on the platform
	bool						m_DistanceGrowing;								// if true, the platform is going towards its destination, if false, it's going back to its original position
	Sprite						*m_Sprite;										// pointer to the platform's sprite
	glm::vec2					m_StartPosition;								// original position of the hitbox
	AABB						m_AABB;											// current position and size of the hitbox


public :
	Platform(Sprite *SpritePtr, const glm::vec2 &Position);
	~Platform();

	void Update();

	void SetDirectionOfMoving(DIRECTION Direction);
	DIRECTION GetDirectionOfMoving() const;

	void SetDropdown(bool Dropdown);
	bool IsDropdown() const;

	void SetDropDelay(unsigned char DropDelay);
	unsigned char GetDropDelay() const;

	void SetTriggeredByPlayer(bool TriggeredByPlayer);
	bool IsTriggeredByPlayer() const;

	void SetMaxSpeed(unsigned short MaxSpeed);
	unsigned short GetMaxSpeed() const;

	void SetUnderPlayer(bool UnderPlayer);
	bool IsUnderPlayer() const;

	void SetCycleFinished(bool CycleFinished);
	bool IsCycleFinished() const;

	void SetMaxDistance(unsigned short MaxDistance);
	unsigned short GetMaxDistance() const;

	const Sprite *GetSprite() const;
	const AABB &GetAABB() const;
};

#endif
