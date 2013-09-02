#ifndef _INGAMESTATE_H_
#define _INGAMESTATE_H_

#include "gamestate.h"
#include "stopwatch.h"
#include "level.h"
#include "camera2d.h"
#include "player.h"
#include "gateanimation.h"
#include "animationmanager.h"
#include "scoreanimation.h"


/*
Gameplay state
*/
class InGameState : public GameState
{
private :
	Stopwatch					m_Stopwatch;									// timer used for fixing the timestep
	

protected :
	// type for control keys
	enum KEYS
	{
		KEY_LEFT,
		KEY_RIGHT,
		KEY_UP,
		KEY_DOWN,
		KEY_SPACE,
		KEY_ENTER,
		NUM_OF_KEYS
	};

	// sub-states of InGame
	enum STATE
	{
		GAMEPLAY,																// default state
		ENTERING_GATE,															// the naimation after entering a gate
		LEAVING_GATE,															// the animation after coming out of a gate
		DYING																	// the player has died
	};

	Stopwatch					*m_DyingTimer;
	GateAnimation				*m_GateAnimation;
	bool						m_ScrollingEnabled;								// when true, the camera follows the player

	bool						m_Keys[NUM_OF_KEYS];							// states of the control keys
	STATE						m_State;										// current state of the game
	glm::vec2					m_Gravity;										// the gravity force
	Level						m_Level;										// the level being played
	Camera2D					m_Camera;										// camera object
	Player						*m_Player;										// the caveman

	virtual void Update();
	void UpdatePlayer();
	void UseGates();
	void PickItems();
	void KillPlayer();

	virtual void Render();
	virtual void HandleInput();


public :
	InGameState(Game *GamePtr);
	~InGameState();

	static std::string GetID();

	virtual void EnterState();
	virtual void LeaveState();
	virtual void MainLoop();
};

#endif
