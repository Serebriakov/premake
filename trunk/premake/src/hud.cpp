#include <fstream>
#include <json/json.h>

#include "hud.h"
#include "glesrenderer.h"
#include "logger.h"


HUD::HUD() : m_BackgroundSprite(new Sprite()), m_EnergySprite(new Sprite())
{
	for( int i = 0; i < NUMBER_OF_LETTERS; ++i )
	{
		m_LetterSprites[i] = new Sprite();
	}
}

HUD::~HUD()
{
	ReleaseSprites();
}

void HUD::ReleaseSprites()
{
	delete m_BackgroundSprite;
	m_BackgroundSprite = NULL;
	delete m_EnergySprite;
	m_EnergySprite = NULL;
	for( int i = 0; i < NUMBER_OF_LETTERS; ++i )
	{
		delete m_LetterSprites[i];
		m_LetterSprites[i] = NULL;
	}
}

void HUD::LoadConfigFromFile(const std::string &FilePath)
{
	// try to read the contents of the file
	std::ifstream file(FilePath.c_str());
	Json::Reader reader;
	Json::Value root;
	if( reader.parse(file, root, false) == false )
	{
		Logger::Instance().Log("ERROR: Couldn't parse HUD config file: " + FilePath);
		Logger::Instance().Log(reader.getFormatedErrorMessages());
	}
	file.close();

	RenderState *rs_background = GLESRenderer::Instance()->AddRenderState(RenderState(GLESRenderer::Instance()->GetShader("background"), RenderState::BLENDING_DISABLED));
	RenderState *rs_hud_elements = GLESRenderer::Instance()->AddRenderState(RenderState(GLESRenderer::Instance()->GetShader("background"), RenderState::BLENDING_ENABLED));

	m_Font = root["font"].asString();

	glm::vec2 background_pos((float)(root["background"]["x"].asDouble()), (float)(root["background"]["y"].asDouble()));
	m_BackgroundSprite->InitializeStaticSprite(GLESRenderer::Instance()->GetImage(root["background"]["sprite"].asString()), rs_background, AABB(background_pos, glm::vec2()));
	m_BackgroundSprite->UseSizeOfCurrentImage(true);

	m_HealthPosition = glm::vec2((float)(root["health"]["x"].asDouble()), (float)(root["health"]["y"].asDouble()));
	m_ScorePosition = glm::vec2((float)(root["score"]["x"].asDouble()), (float)(root["score"]["y"].asDouble()));

	glm::vec2 energy_pos((float)(root["energy"]["x"].asDouble()), (float)(root["energy"]["y"].asDouble()));
	m_EnergySprite->InitializeStaticSprite(GLESRenderer::Instance()->GetImage(root["energy"]["sprite"].asString()), rs_hud_elements, AABB(energy_pos, glm::vec2()));
	m_EnergySprite->UseSizeOfCurrentImage(true);

	for( int i = 0; i < NUMBER_OF_LETTERS; ++i )
	{
		Json::Value v;
		switch( i )
		{
			case LETTER_B : v = root["letter_b"]; break;
			case LETTER_O : v = root["letter_o"]; break;
			case LETTER_N : v = root["letter_n"]; break;
			case LETTER_U : v = root["letter_u"]; break;
			case LETTER_S : v = root["letter_s"]; break;
		}

		glm::vec2 pos((float)(v["x"].asDouble()), (float)(v["y"].asDouble()));
		m_LetterSprites[i]->InitializeStaticSprite(GLESRenderer::Instance()->GetImage(v["sprite"].asString()), rs_hud_elements, AABB(pos, glm::vec2()));
		m_LetterSprites[i]->UseSizeOfCurrentImage(true);
	}
}

void HUD::Draw(const std::string &Health, const std::string &Score, int Energy, const bool *BonusLetters)
{
	GLESRenderer::Instance()->AddSprite2D(m_BackgroundSprite);
	GLESRenderer::Instance()->FlushSprites();

	for( int i = 0; i < Energy; ++i )
	{
		float offset_x = i * m_EnergySprite->GetSize().x;
		GLESRenderer::Instance()->AddSprite2D(m_EnergySprite, m_EnergySprite->GetPosition() + glm::vec2(offset_x, 0));
	}
	for( int i = 0; i < NUMBER_OF_LETTERS; ++i )
	{
		if( BonusLetters[i] )
		{
			GLESRenderer::Instance()->AddSprite2D(m_LetterSprites[i]);
		}
	}
	GLESRenderer::Instance()->FlushSprites();

	GLESRenderer::Instance()->AddText(m_Font, Health, m_HealthPosition);
	GLESRenderer::Instance()->AddText(m_Font, Score, m_ScorePosition);
	GLESRenderer::Instance()->FlushSprites();
}
