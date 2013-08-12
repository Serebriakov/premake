#include <iostream>
#include <sstream>

#include "game.h"
#include "logger.h"
#include "ingamestate.h"


Game::Game()
{
	Running = true;
	Initialized = false;
	Active = true;
	CurrentState = NULL;
}

Game::~Game()
{
	GLESRenderer::Instance()->Destroy();
	SDL_Quit();
}

void Game::Init()
{
	int err;

	// initialize SDL
	err = SDL_Init(SDL_INIT_EVERYTHING);
	if( err )
	{
		Logger::Instance().Log("FATAL ERROR: initializing SDL failed");
		Logger::Instance().Log(SDL_GetError());
		return;
	}
	err = SDL_VideoInit(NULL);

	// initialize the rendering subsystem
	err = GLESRenderer::Init(CONFIG_PATH);
	Logger::Instance().Log(err ? "FATAL ERROR: initializing the rendering subsystem failed" : "Rendering subsystem initialized");
	if( err )
	{
		return;
	}
	GLESRenderer::Instance()->LogBasicInfo();
	GLESRenderer::Instance()->QueryErrors();

	// create the gamestate objects
	// this should happen AFTER the rendering context has been created...
	Logger::Instance().Log("Creating gamestates...");
	States.Insert(InGameState::GetID(), new InGameState(this));
	Logger::Instance().Log("Gamestates created");
	
	// activate gamestate
	ChangeGameState(InGameState::GetID());
			
	// initialization completed
	Initialized = true;
}

void Game::Run()
{
	if( !Initialized )
	{
		Logger::Instance().Log("FATAL ERROR: The game cannot run without being initialized.");
		return;
	}

	while( Running )
	{
		CurrentState->MainLoop();
	}
}

void Game::ChangeGameState(const std::string &StateID)
{
	Dictionary<GameState>::iterator gs = States.Find(StateID);
	if( gs == States.IteratorEnd() )
	{
		std::string error_str = "ERROR: Invalid StateID passed to ChangeGameState: ";
		error_str += StateID;
		Logger::Instance().Log(error_str.c_str());
	}
	else
	{
		if( CurrentState )
		{
			CurrentState->LeaveState();
		}
		CurrentState = gs->second;
		CurrentState->EnterState();
	}
}

void Game::SetActiveness(bool Activeness)
{
	Active = Activeness;
}

bool Game::IsActive()
{
	return Active;
}

void Game::ExitGame()
{
	Running = false;
}
