#include <json/json.h>
#include <fstream>

#include "weapon.h"
#include "glesrenderer.h"
#include "logger.h"


Weapon::Weapon()
{
	m_Sprite = new Sprite();
	m_Sprite->UseSizeOfCurrentImage(true);
	m_Sprite->SetRenderState(GLESRenderer::Instance()->AddRenderState(RenderState(GLESRenderer::Instance()->GetShader("tiles"), RenderState::BLENDING_ENABLED)));
	m_CurrentState = ATTACKING;
}

Weapon::~Weapon()
{
	delete m_Sprite;
}

void Weapon::LoadConfigFromFile(const std::string &FilePath)
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

	const Json::Value &states = root["states"];
	for( size_t i = 0, size = states.size(); i < size; ++i )
	{
		const Json::Value &state = states[i];
		const std::string &img_seq_str = state["image_sequence"].asString();
		const std::string &w_state_str = state["state"].asString();
		WEAPON_STATE w_state;
		if( w_state_str == "attacking" )
		{
			w_state = ATTACKING;
		}
		else if( w_state_str == "attacking_up" )
		{
			w_state = ATTACKING_UP;
		}
		else if( w_state_str == "attacking_down" )
		{
			w_state = ATTACKING_DOWN;
		}

		GLESRenderer::Instance()->LoadImageSequence(img_seq_str);
		ImageSequence *img_seq = GLESRenderer::Instance()->GetImageSequence(img_seq_str);
		m_Sprite->AddImageSequence(img_seq);
		size_t img_seq_index = m_Sprite->GetNumberOfSequences() - 1;

		m_AnimationIndex[w_state] = img_seq_index;

		const Json::Value &offsets = state["offsets"];
		for( size_t i = 0, size = offsets.size(); i < size; ++i )
		{
			const Json::Value &offset = offsets[i];
			glm::vec2 delta_pos((float)(offset["x"].asDouble()), (float)(offset["y"].asDouble()));
			m_Offsets[w_state].push_back(delta_pos);
		}
	}
}

const Sprite *Weapon::GetSprite() const
{
	return m_Sprite;
}

void Weapon::SetState(WEAPON_STATE State)
{
	m_CurrentState = State;

	m_Sprite->JumpToSequence(m_AnimationIndex[m_CurrentState]);
}

Weapon::WEAPON_STATE Weapon::GetState() const
{
	return m_CurrentState;
}

const glm::vec2 &Weapon::GetPositionOffset(size_t FrameNumber)
{
	return m_Offsets[m_CurrentState][FrameNumber];
}

void Weapon::UseFrame(const Sprite *PlayerSprite)
{
	m_Sprite->JumpToFrame(PlayerSprite->GetCurrentFrameNumber());
	if( PlayerSprite->IsFlippedHorizontally() )
	{
		float pos_x = PlayerSprite->GetPosition().x + PlayerSprite->GetSize().x - GetPositionOffset(m_Sprite->GetCurrentFrameNumber()).x - m_Sprite->GetSize().x;
		float pos_y = PlayerSprite->GetPosition().y + GetPositionOffset(m_Sprite->GetCurrentFrameNumber()).y;
		m_Sprite->SetPosition(glm::vec2(pos_x, pos_y));
		m_Sprite->FlipHorizontally(true);
	}
	else
	{
		m_Sprite->SetPosition(PlayerSprite->GetPosition() + GetPositionOffset(m_Sprite->GetCurrentFrameNumber()));
		m_Sprite->FlipHorizontally(false);
	}
}
