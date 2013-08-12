#include "platform.h"
#include "constants.h"


Platform::Platform(Sprite *SpritePtr, const glm::vec2 &Position) : m_Sprite(SpritePtr)
{
	m_StartPosition = Position - glm::vec2(SpritePtr->GetSize().x / 2.f, SpritePtr->GetSize().y);
	m_AABB.m_Position = m_StartPosition;
	m_AABB.m_Size = m_Sprite->GetSize();

	m_IsUnderPlayer = false;
	m_CurrentDistance = 0.f;
	m_DistanceGrowing = true;
	m_CycleFinished = true;
	m_ElapsedTime = 0.f;
}

Platform::~Platform()
{
	delete m_Sprite;
}

void Platform::Update()
{
	glm::vec2 direction_vector;
	switch( m_DirectionOfMoving )
	{
		case UP : direction_vector = glm::vec2(0, -1); break;
		case UPRIGHT : direction_vector = glm::vec2(1, -1); break;
		case RIGHT : direction_vector = glm::vec2(1, 0); break;
		case DOWNRIGHT : direction_vector = glm::vec2(1, 1); break;
		case DOWN : direction_vector = glm::vec2(0, 1); break;
		case DOWNLEFT : direction_vector = glm::vec2(-1, 1); break;
		case LEFT : direction_vector = glm::vec2(-1, 0); break;
		case UPLEFT : direction_vector = glm::vec2(-1, -1); break;
	}

	const float dt = TIMESTEP / 100.f;

	// if the platform moves all the time...
	if( m_TriggeredByPlayer == false )
	{
		// going towards its destination
		if( m_DistanceGrowing )
		{
			m_CurrentDistance += dt;

			if( m_CurrentDistance >= m_MaxDistance )
			{
				m_CurrentDistance = m_MaxDistance;
				m_DistanceGrowing = false;
			}
		}
		// going back to the start position
		else
		{
			m_CurrentDistance -= dt;

			if( m_CurrentDistance <= 0 )
			{
				m_CurrentDistance = 0;
				m_DistanceGrowing = true;
			}
		}

		// Distance of moving: speed*(time+speed-1)
		m_AABB.m_Position = m_StartPosition + ((m_MaxSpeed * (m_CurrentDistance/* + m_MaxSpeed - 1*/)) * direction_vector);
	}
	// the platform is triggered by the player
	else
	{
		// dropdown platforms
		if( m_Dropdown )
		{
			// is the player is standing on it
			if( m_IsUnderPlayer )
			{
				m_ElapsedTime += dt * 100;

				if( m_ElapsedTime >= m_DropDelay * 10 )
				{
					m_MaxSpeed++;
					m_CurrentDistance += dt;
					m_AABB.m_Position = m_StartPosition - ((m_MaxSpeed * (m_CurrentDistance/* + m_MaxSpeed - 1*/)) * direction_vector);
				}
			}
		}
		// regular (not-dropdown) platforms
		else if( (m_IsUnderPlayer) || (m_CycleFinished == false) )
		{
			// going towards its destination
			if( m_DistanceGrowing )
			{
				m_CurrentDistance += dt * 2;

				if( m_CurrentDistance >= m_MaxDistance )
				{
					m_CurrentDistance = m_MaxDistance;
					m_DistanceGrowing = false;
				}
			}
			// going back to the start position
			else
			{
				m_CurrentDistance -= dt * 2;

				if( m_CurrentDistance <= 0 )
				{
					m_CurrentDistance = 0;
					m_DistanceGrowing = true;
					m_CycleFinished = true;
				}
			}

			// Distance of moving: speed*(time+speed-1)
			m_AABB.m_Position = m_StartPosition + ((m_MaxSpeed * (m_CurrentDistance/* + m_MaxSpeed - 1*/)) * direction_vector);

			m_IsUnderPlayer = false;
		}
	}

	// move the sprite with the hitbox
	m_Sprite->SetPosition(m_AABB.m_Position);
}

void Platform::SetDirectionOfMoving(Platform::DIRECTION Direction)
{
	m_DirectionOfMoving = Direction;
}

Platform::DIRECTION Platform::GetDirectionOfMoving() const
{
	return m_DirectionOfMoving;
}

void Platform::SetDropdown(bool Dropdown)
{
	m_Dropdown = Dropdown;
}

bool Platform::IsDropdown() const
{
	return m_Dropdown;
}

void Platform::SetDropDelay(unsigned char DropDelay)
{
	m_DropDelay = DropDelay;
}

unsigned char Platform::GetDropDelay() const
{
	return m_DropDelay;
}

void Platform::SetTriggeredByPlayer(bool TriggeredByPlayer)
{
	m_TriggeredByPlayer = TriggeredByPlayer;
}

bool Platform::IsTriggeredByPlayer() const
{
	return m_TriggeredByPlayer;
}

void Platform::SetMaxSpeed(unsigned short MaxSpeed)
{
	m_MaxSpeed = MaxSpeed;
}

unsigned short Platform::GetMaxSpeed() const
{
	return m_MaxSpeed;
}

void Platform::SetMaxDistance(unsigned short MaxDistance)
{
	m_MaxDistance = MaxDistance;
}

unsigned short Platform::GetMaxDistance() const
{
	return m_MaxDistance;
}

void Platform::SetUnderPlayer(bool UnderPlayer)
{
	m_IsUnderPlayer = UnderPlayer;
}

bool Platform::IsUnderPlayer() const
{
	return m_IsUnderPlayer;
}

void Platform::SetCycleFinished(bool CycleFinished)
{
	m_CycleFinished = CycleFinished;
}

bool Platform::IsCycleFinished() const
{
	return m_CycleFinished;
}

const Sprite *Platform::GetSprite() const
{
	return m_Sprite;
}

const AABB &Platform::GetAABB() const
{
	return m_AABB;
}
