#include <fstream>
#include <sstream>
#include <json/json.h>

#include "enemybase.h"
#include "glesrenderer.h"
#include "logger.h"
#include "level.h"


EnemyBase::EnemyBase(unsigned char Type, const Level *LevelPtr, bool Damaging, bool ExpertOnly, bool DecorationOnly, unsigned char Hitpoints, unsigned char Pause, int Score) : m_Level(LevelPtr)
{
	for( unsigned i = 0; i < NUMBER_OF_STATES; ++i )
	{
		m_AnimationIndex[i] = 0;
	}

	m_Type = Type;
	m_Damaging = Damaging;
	m_ExpertOnly = ExpertOnly;
	m_DecorationOnly = DecorationOnly;
	m_Hitpoints = Hitpoints;
	m_Pause = Pause;
	m_Score = Score;

	m_Active = false;
	m_ScoreAnimation = NULL;

	m_Sprite = new Sprite();
	m_Sprite->UseSizeOfCurrentImage(true);
	m_Sprite->SetRenderState(GLESRenderer::Instance()->AddRenderState(RenderState(GLESRenderer::Instance()->GetShader("tiles"), RenderState::BLENDING_ENABLED)));

	m_ComboMultiplier = 1;
}

EnemyBase::~EnemyBase()
{
	delete m_ScoreAnimation;
	delete m_Sprite;
}

void EnemyBase::LoadConfigFromFile(const std::string &FilePath)
{
	std::ifstream file(FilePath.c_str());
	Json::Reader reader;
	Json::Value root;
	if( reader.parse(file, root, false) == false )
	{
		Logger::Instance().Log("ERROR: Couldn't parse enemy config file: " + FilePath);
		Logger::Instance().Log(reader.getFormatedErrorMessages());
	}
	file.close();

	// load animations
	const Json::Value &states = root["states"];
	for( unsigned i = 0, size = states.size(); i < size; ++i )
	{
		const Json::Value &state = states[i];

		const std::string &img_seq_str = state["image_sequence"].asString();
		const std::string &state_str = state["state"].asString();

		ENEMY_STATE enemy_state;
		if( state_str == "spawning" )
		{
			enemy_state = SPAWNING;
		}
		else if( state_str == "idle" )
		{
			enemy_state = IDLE;
		}
		else if( state_str == "moving" )
		{
			enemy_state = MOVING;
		}
		else if( state_str == "dying" )
		{
			enemy_state = DYING;
		}
		else if( state_str == "disappearing" )
		{
			enemy_state = DISAPPEARING;
		}

		GLESRenderer::Instance()->LoadImageSequence(img_seq_str);
		ImageSequence *img_seq = GLESRenderer::Instance()->GetImageSequence(img_seq_str);
		m_Sprite->AddImageSequence(img_seq);
		size_t img_seq_index = m_Sprite->GetNumberOfSequences() - 1;

		m_AnimationIndex[enemy_state] = img_seq_index;
	}
}

unsigned char EnemyBase::GetType() const
{
	return m_Type;
}

void EnemyBase::SetDamaging(bool Damaging)
{
	m_Damaging = Damaging;
}

bool EnemyBase::IsDamaging() const
{
	return m_Damaging;
}

void EnemyBase::SetExpertOnly(bool ExpertOnly)
{
	m_ExpertOnly = ExpertOnly;
}

bool EnemyBase::IsExpertOnly() const
{
	return m_ExpertOnly;
}

void EnemyBase::SetDecorationOnly(bool DecorationOnly)
{
	m_DecorationOnly = DecorationOnly;
}

bool EnemyBase::IsDecorationOnly() const
{
	return m_DecorationOnly;
}

void EnemyBase::SetHitpoints(unsigned char Hitpoints)
{
	m_Hitpoints = Hitpoints;
}

unsigned char EnemyBase::GetHitpoints() const
{
	return m_Hitpoints;
}

void EnemyBase::SetPauseDelay(unsigned char Pause)
{
	m_Pause = Pause;
}

unsigned char EnemyBase::GetPauseDelay() const
{
	return m_Pause;
}

void EnemyBase::SetScore(int Score)
{
	m_Score = Score;
}

int EnemyBase::GetScore() const
{
	return m_Score;
}

void EnemyBase::SetScoreAnimation(ScoreAnimation *Animation)
{
	m_ScoreAnimation = Animation;
}

ScoreAnimation *EnemyBase::GetScoreAnimation()
{
	return m_ScoreAnimation;
}

void EnemyBase::SetState(ENEMY_STATE State)
{
	m_CurrentState = State;
	m_Sprite->JumpToSequence(m_AnimationIndex[m_CurrentState]);
}

EnemyBase::ENEMY_STATE EnemyBase::GetCurrentState() const
{
	return m_CurrentState;
}

const Sprite *EnemyBase::GetSprite() const
{
	return m_Sprite;
}

void EnemyBase::Activate()
{
	m_ComboMultiplier = 1;
	m_Active = true;
}

void EnemyBase::Deactivate()
{
	m_Active = false;
}

bool EnemyBase::IsActive() const
{
	return m_Active;
}
