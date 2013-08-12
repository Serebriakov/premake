#include "light.h"


Light::Light(LIGHT_STATE State) : m_State(State)
{
}

void Light::Switch(bool On)
{
	if( On )
	{
		m_State = LIGHT_BEING_TURNED_ON;
	}
	else
	{
		m_State = LIGHT_BEING_TURNED_OFF;
	}
}

Light::LIGHT_STATE Light::GetState() const
{
	return m_State;
}
