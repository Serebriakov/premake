#include <fstream>
#include <sstream>
#include <iomanip>
#include <json/json.h>

#include "player.h"
#include "glesrenderer.h"
#include "constants.h"
#include "logger.h"
#include "animationmanager.h"
#include "dyinganimation.h"


Player::Player()
{
	m_Sprite = new Sprite();
	m_Sprite->UseSizeOfCurrentImage(true);
	m_InvulnerabilityTimer.SetCycleInterval(3000);

	m_HUD = new HUD();
}

Player::~Player()
{
	delete m_Sprite;
	delete m_HUD;
}

void Player::LoadConfigFromFile(const std::string &FilePath, const std::string &HUDConfigPath)
{
	// try to read the contents of the file
	std::ifstream file(FilePath.c_str());
	Json::Reader reader;
	Json::Value root;
	bool read_successful = reader.parse(file, root, false);
	if( read_successful == false )
	{
		Logger::Instance().Log("ERROR: Couldn't parse player config file: " + FilePath);
		Logger::Instance().Log(reader.getFormatedErrorMessages());
	}
	file.close();

	// hud
	m_HUD->LoadConfigFromFile(HUDConfigPath);

	// set hitbox
	m_Hitbox.m_Size = glm::vec2(root["hitbox"]["width"].asUInt(), root["hitbox"]["height"].asUInt());

	// set speed
	m_Speed = glm::vec2((float)(root["speed"]["horizontal"].asDouble()), (float)(root["speed"]["vertical"].asDouble()));

	// load animations
	const Json::Value &animations = root["animations"];
	for( unsigned i = 0, size = animations.size(); i < size; ++i )
	{
		const Json::Value &animation = animations[i];

		const std::string &img_seq_str = animation["image_sequence"].asString();
		const std::string &state_str = animation["state"].asString();

		PLAYER_STATE state;
		if( state_str == "idle" )
		{
			state = IDLE;
		}
		else if( state_str == "jumping" )
		{
			state = JUMPING;
		}
		else if( state_str == "falling" )
		{
			state = FALLING;
		}
		else if( state_str == "taunting" )
		{
			state = TAUNTING;
		}
		else if( state_str == "running" )
		{
			state = RUNNING;
		}
		else if( state_str == "sliding" )
		{
			state = SLIDING;
		}
		else if( state_str == "dying" )
		{
			state = DYING;
		}
		else if( state_str == "attacking_up" )
		{
			state = ATTACKING_UP;
		}
		else if( state_str == "attacking" )
		{
			state = ATTACKING;
		}
		else if( state_str == "attacking_down" )
		{
			state = ATTACKING_DOWN;
		}

		GLESRenderer::Instance()->LoadImageSequence(img_seq_str);
		ImageSequence *img_seq = GLESRenderer::Instance()->GetImageSequence(img_seq_str);
		m_Sprite->AddImageSequence(img_seq);
		size_t img_seq_index = m_Sprite->GetNumberOfSequences() - 1;

		m_AnimationIndex[state] = img_seq_index;
	}
}

void Player::Initialize(const glm::vec2 &StartPosition)
{
	if( m_Sprite->GetRenderState() == NULL )
	{
		m_Sprite->SetRenderState(GLESRenderer::Instance()->AddRenderState(RenderState(GLESRenderer::Instance()->GetShader("tiles"), RenderState::BLENDING_ENABLED)));
	}

	m_Hitbox.m_Position = StartPosition - glm::vec2(m_Hitbox.m_Size.x / 2.f, m_Hitbox.m_Size.y);
	m_Velocity = glm::vec2(0, 0);
	m_IsOnGround = false;
	m_IsPressingDown = false;

	ReleaseWeapons();
	LoadWeapon("data/weapons/1/weapon1.json");
	LoadWeapon("data/weapons/2/weapon2.json");

	m_CurrentWeaponIndex = 0;

	SetState(IDLE);

	SetScore(0);
	SetHealth(3);
	m_Energy = 3;
	for( size_t i = 0; i < HUD::NUMBER_OF_LETTERS; ++i )
	{
		m_BonusLetters[i] = false;
	}
}

void Player::Update()
{
	// animate sprite
	m_Sprite->Animate();

	// move the sprite with the hitbox
	glm::vec2 pos;
	pos.x = m_Hitbox.m_Position.x + (m_Hitbox.m_Size.x / 2.f) - (m_Sprite->GetSize().x / 2.f);
	pos.y = m_Hitbox.m_Position.y + m_Hitbox.m_Size.y - m_Sprite->GetSize().y;
	m_Sprite->SetPosition(pos);

	// update the weapon...
	if( IsAttacking() )
	{
		Weapon::WEAPON_STATE w_state;
		switch( GetCurrentState() )
		{
			case ATTACKING : w_state = Weapon::ATTACKING; break;
			case ATTACKING_UP : w_state = Weapon::ATTACKING_UP; break;
			case ATTACKING_DOWN : w_state = Weapon::ATTACKING_DOWN; break;
		}

		m_Weapons[m_CurrentWeaponIndex]->SetState(w_state);
		m_Weapons[m_CurrentWeaponIndex]->UseFrame(m_Sprite);
	}
}

void Player::EnterGate(Gate *GatePtr)
{
	m_Gate = GatePtr;
}

void Player::LeaveGate()
{
	// change position
	m_Hitbox.m_Position = m_Gate->GetExitPosition() - glm::vec2(m_Hitbox.m_Size.x / 2.f, m_Hitbox.m_Size.y);
	// reset gravity force
	m_Velocity.y = 0;
	// update sprite position
	Update();
}

const Gate *Player::GetGate()
{
	return m_Gate;
}

void Player::SetVelocity(const glm::vec2 &Velocity)
{
	m_Velocity = Velocity;
}

void Player::SetVelocityX(float X)
{
	m_Velocity.x = X;
}

void Player::SetVelocityY(float Y)
{
	m_Velocity.y = Y;
}

void Player::AddVelocity(const glm::vec2 &Velocity)
{
	m_Velocity += Velocity;
}

void Player::AddVelocityX(float X)
{
	m_Velocity.x += X;
}

void Player::AddVelocityY(float Y)
{
	m_Velocity.y += Y;
}

const glm::vec2 &Player::GetVelocity() const
{
	return m_Velocity;
}


void Player::MoveLeft()
{
	m_Sprite->FlipHorizontally(true);
	m_Velocity.x = (-1) * m_Speed.x * (TIMESTEP / 1000.f);
}

void Player::MoveRight()
{
	m_Sprite->FlipHorizontally(false);
	m_Velocity.x = m_Speed.x * (TIMESTEP / 1000.f);
}

void Player::Jump()
{
	m_Velocity.y = (-1) * m_Speed.y;
}


void Player::SetOnGroundFlag(bool OnGround)
{
	m_IsOnGround = OnGround;
}

bool Player::IsOnGround() const
{
	return m_IsOnGround;
}

void Player::ChangePositionTo(const glm::vec2 &Position)
{
	m_Hitbox.m_Position = Position;

	if( m_Hitbox.m_Position.x < 0 )
	{
		m_Hitbox.m_Position.x = 0;
	}
}

void Player::ChangePositionWith(const glm::vec2 &DeltaPosition)
{
	m_Hitbox.m_Position += DeltaPosition;
}

const AABB &Player::GetHitbox() const
{
	return m_Hitbox;
}

const Weapon *Player::GetWeapon() const
{
	return m_Weapons[m_CurrentWeaponIndex];
}

const Sprite *Player::GetSprite() const
{
	return m_Sprite;
}

void Player::SetPressingDownFlag(bool State)
{
	m_IsPressingDown = State;
}

bool Player::IsPressingDown() const
{
	return m_IsPressingDown;
}

void Player::SetState(PLAYER_STATE State)
{
	m_CurrentState = State;
	m_Sprite->JumpToSequence(m_AnimationIndex[m_CurrentState]);
}

Player::PLAYER_STATE Player::GetCurrentState() const
{
	return m_CurrentState;
}

bool Player::IsAttacking() const
{
	return (m_CurrentState == ATTACKING) || (m_CurrentState == ATTACKING_UP) || (m_CurrentState == ATTACKING_DOWN);
}

void Player::Draw()
{
	if( m_CurrentState != DYING )
	{
		GLESRenderer::Instance()->AddSprite2D(m_Sprite);
		GLESRenderer::Instance()->FlushSprites();

		if( IsAttacking() )
		{
			GLESRenderer::Instance()->AddSprite2D(m_Weapons[m_CurrentWeaponIndex]->GetSprite());
			GLESRenderer::Instance()->FlushSprites();
		}
	}
}

void Player::DrawHUD()
{
	m_HUD->Draw(m_HealthStr, m_ScoreStr, m_Energy, m_BonusLetters);
}

void Player::ReleaseWeapons()
{
	for( std::vector<Weapon *>::iterator it = m_Weapons.begin(), end = m_Weapons.end(); it != end; ++it )
	{
		delete (*it);
	}
	m_Weapons.clear();
}

void Player::LoadWeapon(const std::string &ConfigPath)
{
	Weapon *w = new Weapon();
	w->LoadConfigFromFile(ConfigPath);
	m_Weapons.push_back(w);
}

void Player::AddScore(int Score)
{
	SetScore(m_Score + Score);
}

void Player::SetScore(int Score)
{
	m_Score = Score;
	std::stringstream ss;
	ss << std::setw(7) << std::setfill('0') << m_Score;
	m_ScoreStr = ss.str();
}

int Player::GetScore() const
{
	return m_Score;
}

void Player::SetEnergyLevel(int EnergyLevel)
{
	m_Energy = EnergyLevel;
}

int Player::GetEnergyLevel() const
{
	return m_Energy;
}

void Player::SetHealth(int Health)
{
	m_Health = Health;
	std::stringstream ss;
	ss << m_Health;
	m_HealthStr = ss.str();
}

int Player::GetHealth() const
{
	return m_Health;
}

void Player::TakeDamage()
{
	if( m_Energy > 0 )
	{
		m_Energy--;
		m_InvulnerabilityTimer.Reset();
		Jump();
	}
}

bool Player::IsInvulnerable() const
{
	return (m_InvulnerabilityTimer.GetCycleState() < 1.f);
}

void Player::AddDyingAnimation()
{
	Sprite *sprite = new Sprite(*m_Sprite);
	sprite->JumpToSequence(m_AnimationIndex[DYING]);
	DyingAnimation *animation = new DyingAnimation(2000, sprite, glm::vec2(14.f, 20.f));
	AnimationManager::Instance().AddAnimation(animation);
}

void Player::SwitchWeapon(size_t WeaponIndex)
{
	if( WeaponIndex < m_Weapons.size() )
	{
		m_CurrentWeaponIndex = WeaponIndex;
	}
}

void Player::PickUpBonusLetter(HUD::BONUS_LETTERS Letter)
{
	m_BonusLetters[Letter] = true;
}

bool Player::HasBonusLetter(HUD::BONUS_LETTERS Letter) const
{
	return m_BonusLetters[Letter];
}
