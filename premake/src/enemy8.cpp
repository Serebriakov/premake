#include "enemy8.h"
#include "animationmanager.h"
#include "dyinganimation.h"
#include "constants.h"
#include "level.h"


Enemy8::Enemy8(const Level *LevelPtr) : EnemyBase(8, LevelPtr, true)
{
}

void Enemy8::Update(const Player *PlayerPtr)
{
	if( m_Active )
	{
		m_Sprite->Animate();

		const glm::vec2 &player_pos = PlayerPtr->GetHitbox().GetCenterPosition();
		glm::vec2 center_pos = m_Sprite->GetPosition() + (m_Sprite->GetSize() / 2.f);

		// set sprite position & animate sprite
		
		AABB hitbox(glm::vec2(m_Position.x - m_Sprite->GetSize().x / 2.f, m_Position.y - m_Sprite->GetSize().y), m_Sprite->GetSize());

		switch( m_CurrentState )
		{
			case IDLE :
			{
				// if the player is close enough
				if( (fabs(center_pos.x - player_pos.x) <= m_ActivationDistance.x) && (fabs(center_pos.y - player_pos.y) <= m_ActivationDistance.y) )
				{
					if( m_Timer.GetElapsedTime() >= (Uint32)m_Pause * 500 )
					{
						SetState(MOVING);
					}
				}

				break;
			}

			case MOVING :
			{
				// if it's on the ground, it can jump
				if( (m_CDResult.ObstacleTileY != NULL) || (m_CDResult.ObstaclePlatform != NULL) )
				{
					SetDirection((player_pos.x >= center_pos.x) ? (RIGHT) : (LEFT));

					if( m_Timer.GetElapsedTime() >= (Uint32)m_Pause * 500 )
					{
						m_Velocity.y = -1 * m_JumpPower.y / 2.f;
						m_Velocity.x = m_CurrentDirection * m_JumpPower.x / 4.f;
						m_Timer.Reset();
					}
					else
					{
						m_Velocity.x = 0.f;
						SetState(IDLE);
					}
				}
				else
				{
					m_Velocity.y += 8 * (TIMESTEP / 1000.f);

					int last_frame = m_Sprite->GetCurrentImageSequence()->GetNumberOfImages() - 1;
					if( m_Velocity.y > 0)
					{
						m_Sprite->JumpToFrame(last_frame);
					}
					else if( m_Sprite->GetCurrentFrameNumber() >= (Uint32)last_frame )
					{
						m_Sprite->JumpToFrame((last_frame >= 1) ? (last_frame - 1) : (0));
					}
				}

				m_Level->MoveAABB(hitbox, m_Velocity, m_CDResult, true, false, true, false);
				hitbox.m_Position = m_CDResult.NewPosition;

				m_Position.x = hitbox.m_Position.x + hitbox.m_Size.x / 2.f;
				m_Position.y = hitbox.m_Position.y + hitbox.m_Size.y;

				break;
			}
		}
		
		m_Sprite->SetPosition(glm::vec2(m_Position.x - m_Sprite->GetSize().x / 2.f, m_Position.y - m_Sprite->GetSize().y));
	}
}

void Enemy8::Die()
{
	// add dying animation
	Sprite *sprite = new Sprite(*m_Sprite);
	sprite->JumpToSequence(m_AnimationIndex[DYING]);
	DyingAnimation *animation = new DyingAnimation(2000, sprite, glm::vec2(14.f, 20.f));
	AnimationManager::Instance().AddAnimation(animation);

	// add score animation
	if( m_ScoreAnimation != NULL )
	{
		m_ScoreAnimation->SetStartPosition(m_Sprite->GetPosition());
		AnimationManager::Instance().AddAnimation(new ScoreAnimation(*m_ScoreAnimation));
	}

	// switch state
	Deactivate();
	SetState(DYING);
}

void Enemy8::Initialize()
{
	SetState(IDLE);
	Activate();
	m_Timer.Reset();
}

void Enemy8::SetActivationDistance(const glm::vec2 &Distance)
{
	m_ActivationDistance = Distance;
}

const glm::vec2 &Enemy8::GetActivationDistance() const
{
	return m_ActivationDistance;
}

void Enemy8::SetJumpPower(const glm::vec2 &Power)
{
	m_JumpPower = Power;
}

const glm::vec2 &Enemy8::GetJumpPower() const
{
	return m_JumpPower;
}

void Enemy8::SetPosition(const glm::vec2 &Position)
{
	m_Position = Position;
}

const glm::vec2 &Enemy8::GetPosition() const
{
	return m_Position;
}

void Enemy8::SetDirection(DIRECTION Direction)
{
	m_CurrentDirection = Direction;
	m_Sprite->FlipHorizontally(m_CurrentDirection == LEFT);
}
