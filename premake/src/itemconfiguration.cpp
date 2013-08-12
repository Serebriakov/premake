#include <fstream>
#include <sstream>
#include <json/json.h>

#include "itemconfiguration.h"
#include "logger.h"
#include "glesrenderer.h"


ItemConfiguration::~ItemConfiguration()
{
	RemoveEffectSprites();
	Items.clear();
}

void ItemConfiguration::RemoveEffectSprites()
{
	for( Dictionary<Sprite>::iterator it = EffectSprites.begin(), end = EffectSprites.end(); it != end; ++it )
	{
		delete it->second;
		it->second = NULL;
	}
	EffectSprites.clear();
}

int ItemConfiguration::LoadFromFile(const std::string &ItemConfigPath, const std::string &EffectsConfigPath)
{
	Items.clear();
	RemoveEffectSprites();

	// read score effects
	std::ifstream effects_file(EffectsConfigPath.c_str());
	Json::Reader reader;
	Json::Value effects_root;
	if( reader.parse(effects_file, effects_root, false) == false )
	{
		Logger::Instance().Log("ERROR: Couldn't parse item effects configuration file: " + EffectsConfigPath);
		Logger::Instance().Log(reader.getFormatedErrorMessages());
		return PARSING_FAILED;
	}
	effects_file.close();

	RenderState *rs_effects = GLESRenderer::Instance()->AddRenderState(RenderState(GLESRenderer::Instance()->GetShader("tiles"), RenderState::BLENDING_ENABLED));

	const Json::Value &effects = effects_root["effects"];
	for( size_t i = 0, size = effects.size(); i < size; ++i )
	{
		const Json::Value &effect = effects[i];
		const std::string &effect_name = effect["name"].asString();
		const std::string &sprite_str = effect["sprite"].asString();
		if( EffectSprites.find(effect_name) == EffectSprites.end() )
		{
			Sprite *sprite = new Sprite();
			sprite->InitializeStaticSprite(GLESRenderer::Instance()->GetImage(sprite_str), rs_effects, AABB());
			sprite->UseSizeOfCurrentImage(true);
			EffectSprites[effect_name] = sprite;
		}
	}

	// read items' properties
	std::ifstream items_file(ItemConfigPath.c_str());
	Json::Value items_root;
	if( reader.parse(items_file, items_root, false) == false )
	{
		Logger::Instance().Log("ERROR: Couldn't parse item configuration file: " + ItemConfigPath);
		Logger::Instance().Log(reader.getFormatedErrorMessages());
		return PARSING_FAILED;
	}
	items_file.close();

	const Json::Value &items = items_root["items"];

	for( size_t i = 0, size = items.size(); i < size; ++i )
	{
		const Json::Value &item = items[i];

		Item::ITEM_TYPE type = Item::NONE;
		const std::string &type_str = item["type"].asString();
		const std::string &sprite_str = item["sprite"].asString();

		if( type_str == "score" )
		{
			type = Item::SCORE;
		}
		else if( type_str == "bonus" )
		{
			type = Item::BONUS;
		}
		else if( type_str == "death" )
		{
			type = Item::DEATH;
		}
		else if( type_str == "explosive" )
		{
			type = Item::EXPLOSIVE;
		}
		else if( type_str == "energy" )
		{
			type = Item::ENERGY;
		}
		else if( type_str == "1up" )
		{
			type = Item::TYPE_1UP;
		}
		else if( type_str == "light_on" )
		{
			type = Item::LIGHT_ON;
		}
		else if( type_str == "light_off" )
		{
			type = Item::LIGHT_OFF;
		}
		else if( type_str == "weapon_2" )
		{
			type = Item::WEAPON_2;
		}
		else if( type_str == "checkpoint" )
		{
			type = Item::CHECKPOINT;
		}
		else if( type_str == "end_of_level" )
		{
			type = Item::END_OF_LEVEL;
		}

		Item &new_item = Items[sprite_str];
		new_item.SetType(type);
		
		switch( type )
		{
			case Item::BONUS :
			{
				new_item.SetLetter(item["letter"].asString()[0]);
				break;
			}

			case Item::SCORE :
			{
				new_item.SetScore(item["score"].asInt());
				break;
			}
		}

		if( item["effect"].isNull() == false )
		{
			Dictionary<Sprite>::iterator effect_it = EffectSprites.find(item["effect"].asString());
			if( effect_it != EffectSprites.end() )
			{
				new_item.SetScoreAnimation(new ScoreAnimation(SCORE_EFFECT_LIFETIME, new Sprite(*(effect_it->second))));
			}
		}
	}

	return OK;
}
