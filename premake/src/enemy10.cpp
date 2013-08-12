#include "enemy10.h"
#include "animationmanager.h"
#include "dyinganimation.h"
#include "tools.h"
#include "level.h"


Enemy10::Enemy10(const Level *LevelPtr) : EnemyBase(10, LevelPtr, true)
{
	m_Timer.Reset();
}

void Enemy10::Update(const Player *PlayerPtr)
{
	// if the enemy has been inactive, activate it
	if( m_Active == false )
	{
		if( m_Timer.GetElapsedTime() >= (Uint32)(m_Pause * 500) )
		{
			Spawn();
		}
	}
	else
	{
		m_Sprite->Animate();

		AABB hitbox(glm::vec2(m_Position.x - m_Sprite->GetSize().x / 2.f, m_Position.y - m_Sprite->GetSize().y), m_Sprite->GetSize());

		// animation transitions
		switch( m_CurrentState )
		{
			// spawning --> moving
			case SPAWNING :
			{
				if( m_Sprite->FinishedCurrentSequence() )
				{
					SetState(MOVING);
					m_Timer.Reset();
				}
				
				// just to place the sprite on top of the ground
				m_Level->MoveAABB(hitbox, glm::vec2(0.f, 1.f), m_CDResult, true, false, false, false);
				hitbox.m_Position = m_CDResult.NewPosition;
				m_Position.x = hitbox.m_Position.x + hitbox.m_Size.x / 2.f;
				m_Position.y = hitbox.m_Position.y + hitbox.m_Size.y;

				break;
			}
			// moving --> disappearing
			case MOVING :
			{
				if( m_Timer.GetElapsedTime() >= (Uint32)(m_Pause * 500) )
				{
					SetState(DISAPPEARING);
					break;
				}
				
				// move around
				m_Level->MoveAABB(hitbox, glm::vec2(0.5f * m_Direction, 1.f), m_CDResult, true, false, false, false);
				hitbox.m_Position = m_CDResult.NewPosition;
				m_Position.x = hitbox.m_Position.x + hitbox.m_Size.x / 2.f;
				m_Position.y = hitbox.m_Position.y + hitbox.m_Size.y;

				// change direction when hitting wall
				if( m_CDResult.ObstacleTileX )
				{
					SetDirection((m_Direction == LEFT) ? (RIGHT) : (LEFT));
				}

				break;
			}
			// disappearing --> deactivated
			case DISAPPEARING :
			{
				if( m_Sprite->FinishedCurrentSequence() )
				{
					Deactivate();
					m_Timer.Reset();
				}
				
				// pull down the sprite to make it disappear downwards
				m_Level->MoveAABB(hitbox, glm::vec2(0.f, 50.f), m_CDResult, true, false, false, false);
				hitbox.m_Position = m_CDResult.NewPosition;
				m_Position.x = hitbox.m_Position.x + hitbox.m_Size.x / 2.f;
				m_Position.y = hitbox.m_Position.y + hitbox.m_Size.y;

				break;
			}
		}

		m_Sprite->SetPosition(glm::vec2(m_Position.x - m_Sprite->GetSize().x / 2.f, m_Position.y - m_Sprite->GetSize().y));
	}
}

void Enemy10::Spawn()
{
	Activate();
	SetState(SPAWNING);

	// randomize start location
	float pos_x = Tools::GetRandBetween(m_Area.m_Position.x, m_Area.m_Position.x + m_Area.m_Size.x);
	float pos_y = m_Area.m_Position.y + m_Area.m_Size.y - m_Sprite->GetSize().y;
	glm::vec2 pos_tile = m_Level->WorldToTileCoordinates(glm::vec2(pos_x, pos_y + 1));
	glm::vec2 ground_tile = m_Level->FindFirstGroundTileAbove(pos_tile);
	ground_tile.y += 1;

	m_Position = m_Level->TileToWorldCoordinates(ground_tile);

	// set direction according to player position
	const Tile *tile_under_player = m_Level->GetTileUnderPlayer();
	if( tile_under_player )
	{
		const glm::vec2 &player_pos = tile_under_player->GetSprite(0)->GetPosition();
		SetDirection((player_pos.x >= pos_x) ? (RIGHT) : (LEFT));
	}
	else
	{
		SetDirection((Tools::GetRandBetween(1, 2) >= 1) ? (RIGHT) : (LEFT));
	}

	m_Timer.Reset();
}

void Enemy10::Die()
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
	m_Timer.Reset();
}

void Enemy10::Initialize()
{
	Deactivate();
}

void Enemy10::Disappear()
{
	SetState(DISAPPEARING);
}

void Enemy10::SetArea(const AABB &Area)
{
	m_Area = Area;
}

const AABB &Enemy10::GetArea() const
{
	return m_Area;
}

void Enemy10::SetSpeed(unsigned char Speed)
{
	m_Speed = Speed;
}

unsigned char Enemy10::GetSpeed() const
{
	return m_Speed;
}

void Enemy10::SetDirection(DIRECTION Direction)
{
	m_Direction = Direction;
	m_Sprite->FlipHorizontally(m_Direction == LEFT);
}
