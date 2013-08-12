#ifndef _HUD_H_
#define _HUD_H_

#include <string>
#include "sprite.h"


class HUD
{
public :
	enum BONUS_LETTERS
	{
		LETTER_B,
		LETTER_O,
		LETTER_N,
		LETTER_U,
		LETTER_S,
		NUMBER_OF_LETTERS
	};

protected :
	std::string					m_Font;
	Sprite						*m_BackgroundSprite;
	Sprite						*m_EnergySprite;
	Sprite						*m_LetterSprites[NUMBER_OF_LETTERS];
	glm::vec2					m_HealthPosition;
	glm::vec2					m_ScorePosition;

	void ReleaseSprites();


public :
	HUD();
	~HUD();

	void LoadConfigFromFile(const std::string &FilePath);
	void Draw(const std::string &Health, const std::string &Score, int Energy, const bool *BonusLetters);
};

#endif
