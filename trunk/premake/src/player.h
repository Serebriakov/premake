#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <glm/glm.hpp>

#include "sprite.h"
#include "aabb.h"
#include "gate.h"
#include "hud.h"
#include "weapon.h"
#include "stopwatch.h"
#include "collisiondetection.h"


/*
Class of the player's character
*/
class Player
{
public :
	enum PLAYER_STATE
	{
		IDLE,
		JUMPING,
		FALLING,
		TAUNTING,
		RUNNING,
		SLIDING,
		DYING,
		ATTACKING_UP,
		ATTACKING,
		ATTACKING_DOWN,
		NUMBER_OF_STATES
	};


protected :
	Gate						*m_Gate;										// pointer to the gate the player entered
	Sprite						*m_Sprite;										// the character's sprite
	AABB						m_Hitbox;										// position and size of the player's hitbox
	glm::vec2					m_Speed;										// moving and jumping speed of the player
	glm::vec2					m_Velocity;										// the vector the player wants to move with
	size_t						m_AnimationIndex[NUMBER_OF_STATES];				// m_AnimationIndex[m_CurrentState] always gives the index of the image sequence used in that state
	PLAYER_STATE				m_CurrentState;									// the player's current state
	bool						m_IsOnGround;									// true if the player is standing on something, false if it's in the air
	bool						m_IsPressingDown;								// indicates if the down arrow is being pressed
	std::vector<Weapon *>		m_Weapons;										// holds 1 instance of each weapon type
	size_t						m_CurrentWeaponIndex;							// index into m_Weapons, selects the weapon currently in use
	HUD							*m_HUD;
	int							m_Score;
	std::string					m_ScoreStr;
	int							m_Energy;
	int							m_Health;
	Stopwatch					m_InvulnerabilityTimer;
	std::string					m_HealthStr;
	bool						m_BonusLetters[HUD::NUMBER_OF_LETTERS];


public :
	CollisionDetectionResult	CDResult;										// results of moving the player around

	Player();
	~Player();

	void LoadConfigFromFile(const std::string &FilePath, const std::string &HUDConfigPath);

	void Initialize(const glm::vec2 &StartPosition);

	void Update();

	void EnterGate(Gate *GatePtr);
	void LeaveGate();
	const Gate *GetGate();

	void SetVelocity(const glm::vec2 &Velocity);
	void SetVelocityX(float X);
	void SetVelocityY(float Y);
	void AddVelocity(const glm::vec2 &Velocity);
	void AddVelocityX(float X);
	void AddVelocityY(float Y);
	const glm::vec2 &GetVelocity() const;

	void MoveLeft();
	void MoveRight();
	void Jump();

	void SetOnGroundFlag(bool OnGround);
	bool IsOnGround() const;

	void ChangePositionTo(const glm::vec2 &Position);
	void ChangePositionWith(const glm::vec2 &DeltaPosition);
	const AABB &GetHitbox() const;

	const Weapon *GetWeapon() const;

	const Sprite *GetSprite() const;

	void SetPressingDownFlag(bool State);
	bool IsPressingDown() const;

	void SetState(PLAYER_STATE State);
	PLAYER_STATE GetCurrentState() const;

	bool IsAttacking() const;

	void Draw();
	void DrawHUD();

	void ReleaseWeapons();
	void LoadWeapon(const std::string &ConfigPath);

	void AddScore(int Score);
	void SetScore(int Score);
	int GetScore() const;

	void SetEnergyLevel(int EnergyLevel);
	int GetEnergyLevel() const;

	void SetHealth(int Health);
	int GetHealth() const;

	void TakeDamage();

	bool IsInvulnerable() const;

	void AddDyingAnimation();

	void SwitchWeapon(size_t WeaponIndex);

	void PickUpBonusLetter(HUD::BONUS_LETTERS Letter);
	bool HasBonusLetter(HUD::BONUS_LETTERS Letter) const;
};

#endif
