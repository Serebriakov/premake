#ifndef _ENEMY_BASE_H_
#define _ENEMY_BASE_H_

#include "scoreanimation.h"
#include "aabb.h"
#include "player.h"

class Level;


/*
Base class for enemy types
*/
class EnemyBase
{
public :
	enum ENEMY_STATE
	{
		SPAWNING,
		IDLE,
		MOVING,
		DYING,
		DISAPPEARING,
		NUMBER_OF_STATES
	};
	

protected :
	unsigned char				m_Type;											// type of enemy [0..12]
	bool						m_ExpertOnly;									// if true, the enemy is used only in expert mode
	bool						m_DecorationOnly;								// if true, the enemy is basically just a sprite, doesn't move, cannot die...
	unsigned char				m_Hitpoints;									// the enemy's "life"
	unsigned char				m_Pause;										// delay before action
	int							m_Score;										// the score the enemy gives when killed
	ScoreAnimation				*m_ScoreAnimation;								// if not NULL, this animation should be cloned and added to the animationmanager when the enemy is killed

	ENEMY_STATE					m_CurrentState;
	Sprite						*m_Sprite;
	size_t						m_AnimationIndex[NUMBER_OF_STATES];
	bool						m_Active;
	bool						m_Damaging;
	int							m_ComboMultiplier;

	const Level					*m_Level;


public :
	EnemyBase(unsigned char Type, const Level *LevelPtr, bool Damaging = true, bool ExpertOnly = false, bool DecorationOnly = false, unsigned char Hitpoints = 255, unsigned char Pause = 0, int Score = 0);
	virtual ~EnemyBase();

	void LoadConfigFromFile(const std::string &FilePath);

	unsigned char GetType() const;
	void SetDamaging(bool Damaging);
	bool IsDamaging() const;
	void SetExpertOnly(bool ExpertOnly);
	bool IsExpertOnly() const;
	void SetDecorationOnly(bool DecorationOnly);
	bool IsDecorationOnly() const;
	void SetHitpoints(unsigned char Hitpoints);
	unsigned char GetHitpoints() const;
	void SetPauseDelay(unsigned char Pause);
	unsigned char GetPauseDelay() const;
	void SetScore(int Score);
	int GetScore() const;
	void SetScoreAnimation(ScoreAnimation *Animation);
	ScoreAnimation *GetScoreAnimation();

	void SetState(ENEMY_STATE State);
	ENEMY_STATE GetCurrentState() const;
	const Sprite *GetSprite() const;
	void Activate();
	void Deactivate();
	bool IsActive() const;

	virtual void Update(const Player *PlayerPtr) = 0;
	virtual void Die() = 0;
	virtual void Initialize() = 0;
};

#endif
