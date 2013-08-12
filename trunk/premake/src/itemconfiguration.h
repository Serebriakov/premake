#ifndef _ITEM_CONFIGURATION_H_
#define _ITEM_CONFIGURATION_H_

#include <map>
#include <string>

#include "item.h"
#include "sprite.h"


/*
Stores an instance of each item, mapped to its sprite name.
The properties of these instances can be copied to the Item objects created while loading a level file.
*/
class ItemConfiguration
{
protected :
	void RemoveEffectSprites();


public :
	enum ERROR_CODE
	{
		OK,
		PARSING_FAILED
	};

	static const unsigned		SCORE_EFFECT_LIFETIME = 1200;
	std::map<std::string, Item>	Items;
	std::map<std::string, Sprite *> EffectSprites;							// effect name --> sprite to be used when creating ScoreAnimation

	~ItemConfiguration();

	int LoadFromFile(const std::string &ItemConfigPath, const std::string &EffectsConfigPath);
};

#endif
