#ifndef _ITEM_H_
#define _ITEM_H_

#include "sprite.h"
#include "camera2d.h"
#include "scoreanimation.h"


/*
Class of item objects that can be picked up by the player
*/
class Item
{
public :
	enum ITEM_TYPE
	{
		NONE,																	// these items cannot be picked up
		SCORE,																	// these items give some score
		BONUS,																	// items of this type give a letter of the word "bonus"
		DEATH,																	// when picked up, kills the player
		EXPLOSIVE,																// causes some kind of explosion
		ENERGY,																	// gives an energy to the player
		TYPE_1UP,																// gives 1 health to the player
		LIGHT_ON,																// switches the light ON
		LIGHT_OFF,																// switches the light OFF
		WEAPON_1,																// default
		WEAPON_2,																// big hammer weapon
		CHECKPOINT,																// when reached, activates a checkpoint on the level
		END_OF_LEVEL															// when reached, the player finishes the level
	};


protected :
	float						m_SinOffset;									// parameter of sin(...) used for moving the item
	float						m_MaxPositionOffset;							// maximal distance from original position
	glm::vec2					m_StartPosition;								// original position of the item's sprite
	bool						m_Visible;										// true if the item is in the visible area
	Sprite						*m_Sprite;										// pointer to the item's sprite
	ITEM_TYPE					m_Type;											// type of the item
	int							m_Score;										// if the type is SCORE, the player gains this when picking up the item
	ScoreAnimation				*m_ScoreAnimation;								// if not NULL, this animation should be cloned and added to the animationmanager when picked up
	char						m_Letter;										// if the type is BONUS, this is the letter
	bool						m_PickedUp;										// indicates if the item has already been picked up


public :
	Item(Sprite *SpritePtr = NULL, ITEM_TYPE Type = NONE, float SinOffset = 0, float MaxPosOffset = 6);
	~Item();

	void CheckVisibility(const Camera2D &Camera);
	bool IsVisible() const;

	void Animate();
	const Sprite *GetSprite() const;

	void SetScoreAnimation(ScoreAnimation *Animation);
	ScoreAnimation *GetScoreAnimation();

	void SetType(ITEM_TYPE Type);
	ITEM_TYPE GetType() const;
	void SetScore(int Score);
	int GetScore() const;
	void SetLetter(char Letter);
	char GetLetter() const;

	void MarkAsPickedUp();
	bool IsPickedUp() const;
};

#endif
