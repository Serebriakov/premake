#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "game.h"


/*
Abstract base class for gamestates
*/
class GameState
{
protected :
	Game						*m_Game;										// pointer to the Game object

	virtual void HandleInput();


public :
	GameState(Game *GamePtr);

	virtual ~GameState()
	{
	}

	virtual void EnterState() = 0;
	virtual void LeaveState() = 0;
	virtual void MainLoop() = 0;
};

#endif
