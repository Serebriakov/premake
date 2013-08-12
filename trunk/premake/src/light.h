#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "stopwatch.h"
#include "glesrenderer.h"


class Light
{
public :
	enum LIGHT_STATE
	{
		LIGHT_ON,
		LIGHT_OFF,
		LIGHT_BEING_TURNED_ON,
		LIGHT_BEING_TURNED_OFF
	};


protected :
	LIGHT_STATE					m_State;										// the light's current state


public :
	Light(LIGHT_STATE State);

	void Switch(bool On);
	LIGHT_STATE GetState() const;
};

#endif
