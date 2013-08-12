#ifndef _GAME_H_
#define _GAME_H_


#include <cstdio>

#include "platformspecific.h"
#include "constants.h"
#include "dictionary.hpp"
#include "glesrenderer.h"


class GameState;


/*
Main class of the application
*/
class Game
{
protected :
	// app state
	bool							Running;									// while true, the program remains in the main loop
	bool							Initialized;								// flag that indicates whether Init() has been called
	bool							Active;										// the game is active if its window is in focus
	// game state
	Dictionary<GameState>			States;										// gamestates stored in an associative array
	GameState						*CurrentState;								// pointer to the currently active gamestate


public :
	Game();
	virtual ~Game();

	virtual void Init();

	virtual void Run();

	void ChangeGameState(const std::string &StateID);
	void SetActiveness(bool Activeness);
	bool IsActive();

	void ExitGame();	
};


#endif
