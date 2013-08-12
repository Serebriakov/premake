#include "item.h"
#include "tools.h"
#include "constants.h"


Item::Item(Sprite *SpritePtr, ITEM_TYPE Type, float SinOffset, float MaxPosOffset) : m_Sprite(SpritePtr), m_Type(Type), m_SinOffset(SinOffset), m_MaxPositionOffset(MaxPosOffset)
{
	if( SpritePtr )
	{
		m_StartPosition = m_Sprite->GetPosition() - glm::vec2(SpritePtr->GetSize().x / 2.f, SpritePtr->GetSize().y);
	}

	m_ScoreAnimation = NULL;
	m_PickedUp = false;
	m_Visible = true;
	m_Score = 0;
	m_Letter = '\0';
}

Item::~Item()
{
	delete m_Sprite;
	delete m_ScoreAnimation;
}

void Item::CheckVisibility(const Camera2D &Camera)
{
	m_Visible = m_Sprite->GetAABB().IntersectsWidth(Camera.GetViewArea());
}

bool Item::IsVisible() const
{
	return m_Visible;
}

void Item::Animate()
{
	m_SinOffset += TIMESTEP / 4.f;
	while( m_SinOffset >= 360 )
	{
		m_SinOffset -= 360;
	}

	m_Sprite->SetPosition(m_StartPosition + glm::vec2(0, sin(m_SinOffset * M_PI / 180.f) * m_MaxPositionOffset));
}

const Sprite *Item::GetSprite() const
{
	return m_Sprite;
}

void Item::SetScoreAnimation(ScoreAnimation *Animation)
{
	m_ScoreAnimation = Animation;
}

ScoreAnimation *Item::GetScoreAnimation()
{
	return m_ScoreAnimation;
}

void Item::SetType(ITEM_TYPE Type)
{
	m_Type = Type;
}

Item::ITEM_TYPE Item::GetType() const
{
	return m_Type;
}

void Item::SetScore(int Score)
{
	m_Score = Score;
}

int Item::GetScore() const
{
	return m_Score;
}

void Item::SetLetter(char Letter)
{
	m_Letter = Letter;
}

char Item::GetLetter() const
{
	return m_Letter;
}

void Item::MarkAsPickedUp()
{
	m_PickedUp = true;
}

bool Item::IsPickedUp() const
{
	return m_PickedUp;
}
