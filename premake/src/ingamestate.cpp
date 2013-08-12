#include "ingamestate.h"
#include "renderstate.h"
#include "tools.h"


/*
Handle SDL events
*/
void InGameState::HandleInput()
{
	SDL_Event event;
	while( SDL_PollEvent(&event) )
	{
		switch(event.type)
		{
			case SDL_QUIT :
			{
				m_Game->ExitGame();

				break;
			}

			case SDL_KEYDOWN :
			{
				switch( event.key.keysym.sym )
				{
					case SDLK_ESCAPE :
					{
						m_Game->ExitGame();
						break;
					}

					case SDLK_RIGHT :
					{
						m_Keys[KEY_RIGHT] = true;
						break;
					}

					case SDLK_LEFT :
					{
						m_Keys[KEY_LEFT] = true;
						break;
					}

					case SDLK_UP :
					{
						m_Keys[KEY_UP] = true;
						break;
					}

					case SDLK_DOWN :
					{
						m_Keys[KEY_DOWN] = true;
						break;
					}

					case SDLK_SPACE :
					{
						m_Keys[KEY_SPACE] = true;
						break;
					}

					case SDLK_RETURN :
					{
						m_Keys[KEY_ENTER] = true;
						break;
					}
				}

				break;
			}

			case SDL_KEYUP :
			{
				switch( event.key.keysym.sym )
				{
					case SDLK_RIGHT :
					{
						m_Keys[KEY_RIGHT] = false;
						break;
					}

					case SDLK_LEFT :
					{
						m_Keys[KEY_LEFT] = false;
						break;
					}

					case SDLK_UP :
					{
						m_Keys[KEY_UP] = false;
						break;
					}

					case SDLK_DOWN :
					{
						m_Keys[KEY_DOWN] = false;
						break;
					}

					case SDLK_SPACE :
					{
						m_Keys[KEY_SPACE] = false;
						break;
					}

					case SDLK_RETURN :
					{
						m_Keys[KEY_ENTER] = false;
						break;
					}
				}
				break;
			}
		}
	}
}

/*
Update the world:
animate objects, use AI, etc.
*/
void InGameState::Update()
{
	if( m_State == ENTERING_GATE )
	{
		m_GateAnimation->Update();

		// entering animation finished
		if( m_GateAnimation->GetAnimationState() >= 0.5f )
		{
			// the player leaves the gate
			m_Player->LeaveGate();

			// place the camera
			const Gate *gate = m_Player->GetGate();
			// m_ScrollingEnabled = gate->IsScrollingEnabled();
			if( m_ScrollingEnabled == false )
			{
				m_Camera.SetPosition(gate->GetCameraPosition());
				GLESRenderer::Instance()->SetCameraPosition(m_Camera.GetPosition());
			}

			// change the state
			m_State = LEAVING_GATE;
		}
	}
	else if( m_State == LEAVING_GATE )
	{
		m_GateAnimation->Update();

		// leaving animation finished
		if( m_GateAnimation->GetAnimationState() >= 1.f )
		{
			// change the state
			m_State = GAMEPLAY;
		}
	}
	else if( m_State == GAMEPLAY )
	{
		UpdatePlayer();

		if( GetAsyncKeyState(VK_F2) )
		{
			KillPlayer();
		}
	}
	else if( m_State == DYING )
	{
		if( m_DyingTimer->GetCycleState() >= 1.f )
		{
			m_ScrollingEnabled = true;
			int health = m_Player->GetHealth();
			m_Player->Initialize(m_Level.GetStartPosition());
			m_Player->SetHealth(health - 1);
			m_State = GAMEPLAY;
		}
	}

	// place the camera to the player
	if( m_ScrollingEnabled )
	{
		m_Camera.SetCenterPosition(m_Player->GetHitbox().GetCenterPosition());
		glm::vec2 camera_pos = m_Camera.GetPosition();
		Tools::ClampValue<float>(camera_pos.x, 0, m_Level.GetWidth() * m_Level.GetTileSize().x - 320);
		Tools::ClampValue<float>(camera_pos.y, 0, m_Level.GetHeight() * m_Level.GetTileSize().y - 200 + 24);
		m_Camera.SetPosition(camera_pos);
		GLESRenderer::Instance()->SetCameraPosition(camera_pos);
	}

	// update enemies
	m_Level.UpdateEnemies(m_Player);

	// animate items and check their visibility
	m_Level.UpdateItems(m_Camera);

	// animate background tiles
	m_Level.AnimateTiles();

	// update animations
	AnimationManager::Instance().UpdateAnimations();
}

/*
Update the player object
*/
void InGameState::UpdatePlayer()
{
	//
	// update platforms
	//

	glm::vec2 platform_pos0 = glm::vec2(0.f, 0.f);
	if( m_Player->CDResult.ObstaclePlatform != NULL )
	{
		platform_pos0 = m_Player->CDResult.ObstaclePlatform->GetAABB().m_Position;
	}
	m_Level.UpdatePlatforms();

	//
	// state transitions
	//

	Player::PLAYER_STATE player_state = Player::IDLE;

	// --> IDLE
	if( m_Player->IsOnGround() )
	{
		player_state = Player::IDLE;
	}

	// --> TAUNTING
	if( player_state == Player::IDLE )
	{
		if( m_Keys[KEY_DOWN] )
		{
			player_state = Player::TAUNTING;
		}
	}

	// --> RUNNING
	if( player_state == Player::IDLE )
	{
		if( m_Keys[KEY_LEFT] != m_Keys[KEY_RIGHT] )
		{
			player_state = Player::RUNNING;
		}
	}

	// --> JUMPING
	if( (player_state == Player::IDLE) || (player_state == Player::RUNNING) )
	{
		if( m_Keys[KEY_UP] )
		{
			player_state = Player::JUMPING;
		}
	}

	// --> FALLING
	if( (m_Player->IsOnGround() == false) && (m_Player->GetVelocity().y > 0) )
	{
		// m_PlayerPtr->SetState(Player::FALLING);
		player_state = Player::FALLING;
	}

	// --> ATTACKING
	if( (m_Keys[KEY_ENTER]) || ( (m_Player->IsAttacking()) && (m_Player->GetSprite()->FinishedCurrentSequence() == false) ) )
	{
		if( m_Keys[KEY_UP] )
		{
			player_state = Player::ATTACKING_UP;
		}
		else if( m_Keys[KEY_DOWN] )
		{
			player_state = Player::ATTACKING_DOWN;
		}
		else
		{
			player_state = Player::ATTACKING;
		}
	}

	m_Player->SetState(player_state);


	//
	// apply forces & set velocity
	//

	// X
	if( (player_state == Player::IDLE) || (player_state == Player::TAUNTING) || (m_Player->IsAttacking()) )
	{
		m_Player->SetVelocityX(0);
	}
	else
	{
		if( m_Keys[KEY_LEFT] == m_Keys[KEY_RIGHT] )
		{
			m_Player->SetVelocityX(0);
		}
		else if( m_Keys[KEY_LEFT] )
		{
			m_Player->MoveLeft();
		}
		else
		{
			m_Player->MoveRight();
		}
	}

	// Y
	// jumping
	if( player_state == Player::JUMPING )
	{
		if( m_Player->IsOnGround() )
		{
			m_Player->Jump();
		}
	}
	// applying gravity
	if( m_Player->IsOnGround() == false )
	{
		m_Player->AddVelocity(m_Gravity * (TIMESTEP / 1000.f));
	}
	// attacking causes some very little jumping as well
	if( (m_Player->IsAttacking()) && (m_Player->GetSprite()->FinishedCurrentSequence()) )
	{
		m_Player->SetVelocityY(m_Player->GetVelocity().y - 1.f);
	}

	// down arrow state
	m_Player->SetPressingDownFlag(m_Keys[KEY_DOWN]);
	

	//
	// move the player with the platform it's standing on
	//

	if( m_Player->CDResult.ObstaclePlatform )
	{
		glm::vec2 platform_delta_pos = m_Player->CDResult.ObstaclePlatform->GetAABB().m_Position - platform_pos0;
		m_Player->ChangePositionTo(glm::vec2(platform_delta_pos.x + m_Player->GetHitbox().m_Position.x, m_Player->CDResult.ObstaclePlatform->GetAABB().m_Position.y - m_Player->GetHitbox().m_Size.y - 0.5f));

		m_Player->CDResult.ObstaclePlatform->SetUnderPlayer(true);
		m_Player->CDResult.ObstaclePlatform->SetCycleFinished(false);
	}


	//
	// move the player
	//

	m_Player->ChangePositionTo(m_Level.MovePlayer(m_Player, m_Player->CDResult));

	
	//
	// use collision detection results
	//
	
	// check if the player hit the ground
	m_Player->SetOnGroundFlag((m_Player->GetVelocity().y > 0) && ((m_Player->CDResult.ObstacleTileY != NULL) || (m_Player->CDResult.ObstaclePlatform != NULL) || (m_Player->CDResult.ObstacleEnemy != NULL)));

	// if the player collided with the bottom side of something or is on the ground...
	if( (m_Player->CDResult.ObstacleTileY != NULL) && (m_Player->GetVelocity().y < 0) )
	{
		m_Player->SetVelocityY(0);
	}
	if( (m_Player->IsOnGround()) && (m_Player->CDResult.ObstaclePlatform == NULL) )
	{
		m_Player->SetVelocityY(0);
	}
	
	// if the player jumped on an enemy...
	if( m_Player->CDResult.ObstacleEnemy )
	{
		// jump from enemy
		if( m_Keys[KEY_UP] )
		{
			m_Player->SetState(Player::JUMPING);
			m_Player->Jump();
			m_Player->SetVelocityY(m_Player->GetVelocity().y * 1.5f);
			m_Player->SetOnGroundFlag(false);
		}
		// bounce from enemy
		else
		{
			m_Player->SetVelocityY(m_Player->GetVelocity().y - 2.f);
		}
	}


	//
	// walk through gates
	//

	UseGates();

	//
	// pick up items
	//

	PickItems();

	// update the player's sprite
	m_Player->Update();


	//
	// enemies
	//

	// kill enemies
	if( m_Player->IsAttacking() )
	{
		const EnemyBase *enemy = m_Level.KillEnemies(m_Player->GetWeapon());
		if( enemy )
		{
			m_Player->AddScore(enemy->GetScore());
		}
	}

	// damage player
	const EnemyBase *enemy = m_Level.GetDamagingEnemy(m_Player);
	if( m_Player->IsInvulnerable() == false )
	{
		if( enemy )
		{
			if( m_Player->GetEnergyLevel() == 0 )
			{
				KillPlayer();
			}
			else
			{
				m_Player->TakeDamage();
			}
		}
	}

	// jumping on deadly tiles kills...
	if( (m_Player->CDResult.ObstacleTileY != NULL) && (m_Player->CDResult.ObstacleTileY->GetSideProperty(Tile::TOP) == Tile::DEADLY) )
	{
		KillPlayer();
	}

	// falling out of the level is deadly
	const AABB &player_hitbox = m_Player->GetHitbox();
	if( player_hitbox.m_Position.y + player_hitbox.m_Size.y >= m_Level.GetHeight() * m_Level.GetTileSize().y )
	{
		KillPlayer();
	}
	if( (m_ScrollingEnabled == false) && ((player_hitbox.m_Position.x < 0) || (player_hitbox.m_Position.x + player_hitbox.m_Size.x >= m_Camera.GetViewArea().m_Position.x + m_Camera.GetViewArea().m_Size.x)) )
	{
		KillPlayer();
	}
}

void InGameState::UseGates()
{
	if( m_Keys[KEY_DOWN] )
	{
		Gate *gate = m_Level.TeleportPlayer(m_Player);

		if( gate != NULL )
		{
			m_Player->EnterGate(gate);
			m_GateAnimation->StartAnimation();
			m_State = ENTERING_GATE;
		}
	}
}

void InGameState::PickItems()
{
	Item *item = m_Level.PickItem(m_Player);
	if( item )
	{
		switch( item->GetType() )
		{
			case Item::SCORE :
			{
				item->MarkAsPickedUp();
				m_Player->AddScore(item->GetScore());
				break;
			}

			case Item::BONUS :
			{
				item->MarkAsPickedUp();
				switch( item->GetLetter() )
				{
					case 'b' : case 'B' : m_Player->PickUpBonusLetter(HUD::LETTER_B); break;
					case 'o' : case 'O' : m_Player->PickUpBonusLetter(HUD::LETTER_O); break;
					case 'n' : case 'N' : m_Player->PickUpBonusLetter(HUD::LETTER_N); break;
					case 'u' : case 'U' : m_Player->PickUpBonusLetter(HUD::LETTER_U); break;
					case 's' : case 'S' : m_Player->PickUpBonusLetter(HUD::LETTER_S); break;
				}
				break;
			}

			case Item::DEATH :
			{
				item->MarkAsPickedUp();
				KillPlayer();
				break;
			}

			case Item::EXPLOSIVE :
			{
				item->MarkAsPickedUp();
				//
				// TODO
				//
				break;
			}

			case Item::ENERGY :
			{
				if( m_Player->GetEnergyLevel() < 3 )
				{
					item->MarkAsPickedUp();
					m_Player->SetEnergyLevel(m_Player->GetEnergyLevel() + 1);
				}
				break;
			}

			case Item::TYPE_1UP :
			{
				item->MarkAsPickedUp();
				m_Player->SetHealth(m_Player->GetHealth() + 1);
				break;
			}

			case Item::LIGHT_ON :
			{
				item->MarkAsPickedUp();
				//
				// TODO
				//
				break;
			}

			case Item::LIGHT_OFF :
			{
				item->MarkAsPickedUp();
				//
				// TODO
				//
				break;
			}

			case Item::WEAPON_1 :
			{
				item->MarkAsPickedUp();
				m_Player->SwitchWeapon(0);
				break;
			}

			case Item::WEAPON_2 :
			{
				item->MarkAsPickedUp();
				m_Player->SwitchWeapon(1);
				break;
			}

			case Item::CHECKPOINT :
			{
				item->MarkAsPickedUp();
				//
				// TODO
				//
				break;
			}

			case Item::END_OF_LEVEL :
			{
				item->MarkAsPickedUp();
				//
				// TODO
				//
				break;
			}
		}

		// if the item has a score-animation, clone it and add to the animation manager
		ScoreAnimation *score_anim = item->GetScoreAnimation();
		if( score_anim != NULL )
		{
			AnimationManager::Instance().AddAnimation(new ScoreAnimation(*score_anim));
		}
	}
}

/*
The player dies...
*/
void InGameState::KillPlayer()
{
	m_ScrollingEnabled = false;
	m_Player->SetState(Player::DYING);
	m_State = DYING;
	m_Player->AddDyingAnimation();
	m_DyingTimer->Reset();
}

/*
Render the visible objects
*/
void InGameState::Render()
{
	GLESRenderer::Instance()->ClearBuffers();

	m_Level.DrawBackground(m_Camera);
	m_Level.DrawPlatforms();
	m_Level.DrawItems();

	AnimationManager::Instance().DrawAnimations();

	m_Level.DrawEnemies();
	m_Player->Draw();

	m_Level.DrawOverlayingSprites(m_Camera);

	m_Player->DrawHUD();

	if( (m_State == ENTERING_GATE) || (m_State == LEAVING_GATE) )
	{
		m_GateAnimation->Draw();
	}

	GLESRenderer::Instance()->SwapBuffers();
}


InGameState::InGameState(Game *GamePointer) : GameState(GamePointer)
{
	m_GateAnimation = new GateAnimation();
	m_Player = new Player();
	m_Gravity = glm::vec2(0, 11.f);
	m_State = GAMEPLAY;
	m_ScrollingEnabled = true;

	m_DyingTimer = new Stopwatch(3000);
}

InGameState::~InGameState()
{
	m_Level.ReleaseResources();
	delete m_Player;
	m_Player = NULL;
	delete m_GateAnimation;
	m_GateAnimation = NULL;
	delete m_DyingTimer;
	m_DyingTimer = NULL;
}

std::string InGameState::GetID()
{
	return "INGAMESTATE";
}

void InGameState::EnterState()
{
	// set the camera
	m_Camera.SetPosition(glm::vec2(0, 0));
	m_Camera.SetViewAreaSize(glm::vec2(320, 200));

	// renderer settings
	GLESRenderer::Instance()->SetClearColor(0.3f, 0.4f, 0.5f);
	GLESRenderer::Instance()->SetProjection2D(0, 320, 200, 0);

	// load shaders
	int res_loading_result;
	res_loading_result = GLESRenderer::Instance()->LoadShaderFromFile("data/shaders/tiles.vert", "data/shaders/tiles.frag", "tiles");
	res_loading_result = GLESRenderer::Instance()->LoadShaderFromFile("data/shaders/background.vert", "data/shaders/background.frag", "background");
	res_loading_result = GLESRenderer::Instance()->LoadShaderFromFile("data/shaders/color.vert", "data/shaders/color.frag", "color");

	// create renderstates
	RenderState *rs_tiles = GLESRenderer::Instance()->AddRenderState(RenderState(GLESRenderer::Instance()->GetShader("tiles"), RenderState::BLENDING_ENABLED));
	RenderState *rs_background = GLESRenderer::Instance()->AddRenderState(RenderState(GLESRenderer::Instance()->GetShader("background"), RenderState::BLENDING_DISABLED));
	RenderState *rs_font = GLESRenderer::Instance()->AddRenderState(RenderState(GLESRenderer::Instance()->GetShader("background"), RenderState::BLENDING_ENABLED));

	// load fonts
	GLESRenderer::Instance()->LoadFont("data/fonts/font.json", rs_font, "font_ingame");

	// load level file
	ItemConfiguration item_configuration;
	item_configuration.LoadFromFile("data/items/items.json", "data/items/effects.json");
	m_Level.LoadFromFile("data/levels/level2.json", "data/enemies/enemy_types.json", item_configuration, rs_background, rs_tiles);

	// initialize the player
	m_Player->LoadConfigFromFile("data/player/player.json", "data/player/hud.json");
	m_Player->Initialize(m_Level.GetStartPosition());

	// initialize control
	for( unsigned short i = 0; i < NUM_OF_KEYS; ++i )
	{
		m_Keys[i] = false;
	}

	// initialize game logic
	m_State = GAMEPLAY;
	m_GateAnimation->Initialize();

	// timer initialization - this should be the last thing in this function
	m_SumTime = 0;
	m_PrevTime = SDL_GetTicks();
}

void InGameState::LeaveState()
{
	m_Level.ReleaseResources();
	delete m_Player;
	m_Player = NULL;
}

void InGameState::MainLoop()
{
	// the mainloop should run at a fixed rate
	Uint32 t = SDL_GetTicks();
	m_SumTime += t - m_PrevTime;
	m_PrevTime = t;

	if( m_SumTime <= (TIMESTEP - 5) )
	{
		SDL_Delay(TIMESTEP - m_SumTime);
	}
	
	while( m_SumTime >= TIMESTEP )
	{
		m_SumTime -= TIMESTEP;

		if( m_Game->IsActive() )
		{
			HandleInput();
			Update();
			Render();
		}
	}
}
