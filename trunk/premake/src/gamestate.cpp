#include "gamestate.h"


GameState::GameState(Game *GamePtr) : m_Game(GamePtr)
{
}

void GameState::HandleInput()
{
	SDL_Event event;
	while( SDL_PollEvent(&event) )
	{
		;
	}
}
