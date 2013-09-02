#include <fstream>
#include <sstream>
#include <algorithm>
#include <json/json.h>

#include "glesrenderer.h"
#include "level.h"
#include "logger.h"
#include "tools.h"
#include "enemy10.h"
#include "enemy1.h"
#include "enemy2.h"
#include "enemy4.h"
#include "enemy9.h"
#include "enemy8.h"


Level::Level()
{
	m_Width = 0;
	m_Height = 0;
	m_BackgroundSprite = NULL;
	m_AnimatedTileFrameNumber = 0;
	m_BackgroundAnimTimer.SetCycleInterval(200);
	m_TileUnderPlayer = NULL;
	m_HasOverlayingSprites = false;
}

Level::~Level()
{
	ReleaseResources();
}

int Level::GetWidth() const
{
	return m_Width;
}

int Level::GetHeight() const
{
	return m_Height;
}

const glm::vec2 &Level::GetTileSize() const
{
	return m_TileSize;
}

/*
Load level from file
Path: path of the level descriptor file
*/
Level::ERROR_CODE Level::LoadFromFile(const std::string &Path, const std::string &EnemyConfigPath, ItemConfiguration &ItemConfig, RenderState *RSBackgroundPtr, RenderState *RSTilesPtr)
{
	// release previously loaded stuff
	ReleaseResources();

	Json::Reader reader;

	// parse enemy configuration file
	std::ifstream enemy_config_file(EnemyConfigPath.c_str());
	Json::Value enemy_config_root;
	if( reader.parse(enemy_config_file, enemy_config_root, false) == false )
	{
		Logger::Instance().Log("ERROR: Couldn't parse enemy types file: " + EnemyConfigPath);
		Logger::Instance().Log(reader.getFormatedErrorMessages());
		return PARSING_FAILED;
	}
	enemy_config_file.close();
	std::map<std::string, std::string> enemy_config_files;
	for( unsigned i = 0, size = enemy_config_root["enemies"].size(); i < size; ++i )
	{
		const Json::Value &v = enemy_config_root["enemies"][i];
		enemy_config_files[v["sprite"].asString()] = v["configuration"].asString();
	}

	// open and read the contents of level file
	std::ifstream file(Path.c_str());
	Json::Value root;
	if( reader.parse(file, root, false) == false )
	{
		Logger::Instance().Log("ERROR: Couldn't parse level file: " + Path);
		Logger::Instance().Log(reader.getFormatedErrorMessages());
		return PARSING_FAILED;
	}
	file.close();

	// get the size of the level
	m_Width = root.get("width", 0).asInt();
	m_Height = root.get("height", 0).asInt();

	// get the tiles' size
	m_TileSize.x = (float)root.get("tile_resolution_x", 16).asInt();
	m_TileSize.y = (float)root.get("tile_resolution_y", 16).asInt();

	// load the images required by the level
	const Json::Value &images = root["images"];
	for( unsigned i = 0, size = images.size(); i < size; ++i )
	{
		const Json::Value &v = images[i];
		GLESRenderer::ERROR_CODE err = GLESRenderer::Instance()->LoadImages(v["map_path"].asString(), v["atlas_path"].asString());
		if( err != GLESRenderer::OK )
		{
			Logger::Instance().Log("Loading images failed; paths: " + v["map_path"].asString() + ", " + v["atlas_path"].asString());
		}
	}

	// add background image
	Sprite *background_sprite = new Sprite();
	background_sprite->InitializeStaticSprite(GLESRenderer::Instance()->GetImage(root["background_image"].asString()), RSBackgroundPtr, AABB(glm::vec2(0, 0), glm::vec2(320,  200)));
	m_BackgroundSprite = background_sprite;

	// get the starting position
	m_StartPosition.x = (float)root["start_position"]["x"].asUInt();
	m_StartPosition.y = (float)root["start_position"]["y"].asUInt();

	// create tiles
	m_HasOverlayingSprites = false;
	m_Tiles.reserve(m_Width * m_Height);
	const Json::Value &tiles = root["tiles"];
	std::map<std::string, Tile::SIDE_PROPERTY> side_prop;
	side_prop["traversable"] = Tile::TRAVERSABLE;
	side_prop["traversable_when_pressing_down"] = Tile::TRAVERSABLE_WHEN_PRESSING_DOWN;
	side_prop["solid"] = Tile::SOLID;
	side_prop["deadly"] = Tile::DEADLY;

	std::map<std::string, Tile::SLOPE_TYPE> slope_type;
	slope_type["horizontal_floor"] = Tile::HORIZONTAL_FLOOR;
	slope_type["slope_downright"] = Tile::SLOPE_DOWNRIGHT;
	slope_type["slope_upright"] = Tile::SLOPE_UPRIGHT;
	for( int y = 0; y < m_Height; ++y )
	{
		for( int x = 0; x < m_Width; ++x )
		{
			int i = (y * m_Width) + x;
			const Json::Value &v = tiles[i];

			Sprite *sprite0 = NULL;
			Sprite *sprite1 = NULL;
			Sprite *sprite2 = NULL;
			Sprite *overlaying_sprite = NULL;

			// if the tile has sprite(s)
			if( v["sprite_0"] != "" )
			{
				sprite0 = new Sprite();
				sprite0->InitializeStaticSprite(GLESRenderer::Instance()->GetImage(v["sprite_0"].asString()), RSTilesPtr, AABB(glm::vec2(x * m_TileSize.x, y * m_TileSize.y), m_TileSize));

				// if the tile has a 2nd sprite
				if( v["sprite_1"].isNull() == false )
				{
					sprite1 = new Sprite();
					sprite1->InitializeStaticSprite(GLESRenderer::Instance()->GetImage(v["sprite_1"].asString()), RSTilesPtr, AABB(glm::vec2(x * m_TileSize.x, y * m_TileSize.y), m_TileSize));
				}

				// if the tile has a 3rd sprite
				if( v["sprite_2"].isNull() == false )
				{
					sprite2 = new Sprite();
					sprite2->InitializeStaticSprite(GLESRenderer::Instance()->GetImage(v["sprite_2"].asString()), RSTilesPtr, AABB(glm::vec2(x * m_TileSize.x, y * m_TileSize.y), m_TileSize));
				}
			}

			// overlaying sprite
			if( v["overlaying_sprite"].isNull() == false )
			{
				m_HasOverlayingSprites = true;
				overlaying_sprite = new Sprite();
				overlaying_sprite->InitializeStaticSprite(GLESRenderer::Instance()->GetImage(v["overlaying_sprite"].asString()), RSTilesPtr, AABB(glm::vec2(x * m_TileSize.x, y * m_TileSize.y), m_TileSize));
			}

			// create a new tile objects nad set its properties
			Tile *tile = new Tile(sprite0, sprite1, sprite2, overlaying_sprite);

			// sides
			tile->SetSideProperty(Tile::BOTTOM, side_prop[v["side_property"][Tile::BOTTOM].asString()]);
			tile->SetSideProperty(Tile::TOP, side_prop[v["side_property"][Tile::TOP].asString()]);
			tile->SetSideProperty(Tile::LEFT, side_prop[v["side_property"][Tile::LEFT].asString()]);
			tile->SetSideProperty(Tile::RIGHT, side_prop[v["side_property"][Tile::RIGHT].asString()]);

			// slope type
			tile->SetSlopeType(slope_type[v["slope_type"].asString()]);

			// reversed heights
			if( tile->GetSlopeType() != Tile::HORIZONTAL_FLOOR )
			{
				tile->SetReverseHeights(v["reversed_height_left"].asUInt(), v["reversed_height_right"].asUInt());
			}
			else
			{
				tile->SetReverseHeights(0, 0);
			}
			
			m_Tiles.push_back(tile);
		}
	}

	// create and place items
	const Json::Value &items = root["items"];
	std::map<std::string, Item>::iterator item_it;
	for( unsigned i = 0, size = items.size(); i < size; ++i )
	{
		const Json::Value &v = items[i];
		const std::string &sprite_str = v["sprite"].asString();
		Image *image_ptr = GLESRenderer::Instance()->GetImage(sprite_str);
		Sprite *item_sprite = new Sprite();
		item_sprite->InitializeStaticSprite(image_ptr, RSTilesPtr, AABB(glm::vec2(v["pos_x"].asInt(), v["pos_y"].asInt()), image_ptr->GetSize()));

		Item *item = new Item(item_sprite, Item::NONE, i * 15.f);

		// copy item's properties from the ItemConfiguration collection
		item_it = ItemConfig.Items.find(sprite_str);
		if( item_it != ItemConfig.Items.end() )
		{
			item->SetType(item_it->second.GetType());
			item->SetLetter(item_it->second.GetLetter());
			item->SetScore(item_it->second.GetScore());

			if( item_it->second.GetScoreAnimation() != NULL )
			{
				ScoreAnimation *score_anim = new ScoreAnimation(*(item_it->second.GetScoreAnimation()));
				score_anim->SetStartPosition(item->GetSprite()->GetPosition());
				item->SetScoreAnimation(score_anim);
			}
		}

		m_Items.push_back(item);
	}

	// create and place platforms
	const Json::Value &platforms = root["platforms"];
	std::map<std::string, Platform::DIRECTION> dir;
	dir["up"] = Platform::UP;
	dir["upright"] = Platform::UPRIGHT;
	dir["right"] = Platform::RIGHT;
	dir["downright"] = Platform::DOWNRIGHT;
	dir["down"] = Platform::DOWN;
	dir["downleft"] = Platform::DOWNLEFT;
	dir["left"] = Platform::LEFT;
	dir["upleft"] = Platform::UPLEFT;
	for( unsigned i = 0, size = platforms.size(); i < size; ++i )
	{
		const Json::Value &v = platforms[i];
		Image *image_ptr = GLESRenderer::Instance()->GetImage(v["sprite"].asString());
		Sprite *platform_sprite = new Sprite();
		platform_sprite->InitializeStaticSprite(image_ptr, RSTilesPtr, AABB(glm::vec2(), image_ptr->GetSize()));

		Platform *platform = new Platform(platform_sprite, glm::vec2(v["pos_x"].asInt(), v["pos_y"].asInt()));
		platform->SetDirectionOfMoving(dir[v["direction"].asString()]);
		platform->SetDropdown(v["dropdown"].asBool());
		platform->SetDropDelay((platform->IsDropdown()) ? ((unsigned char)(v["drop_delay"].asUInt())) : (0));
		platform->SetTriggeredByPlayer(v["triggered_by_player"].asBool());
		platform->SetMaxSpeed((unsigned char)(v["speed"].asUInt()));
		platform->SetMaxDistance((unsigned short)(v["distance"].asUInt()));

		m_Platforms.push_back(platform);
	}

	// create and place gates
	const Json::Value &gates = root["gates"];
	for( unsigned i = 0, size = gates.size(); i < size; ++i )
	{
		const Json::Value &v = gates[i];

		AABB entrance;
		// originally, the entrance tile stored in the level file is the one the player has to stand on, in order to enter the gate...
		// ...but use the one above it, to be able to use AABB intersection tests
		entrance.m_Position = glm::vec2(v["in"]["tile_x"].asInt(), v["in"]["tile_y"].asInt() - 1) * m_TileSize;
		entrance.m_Size = m_TileSize;
		// the exit position should be a little bit above the original one
		glm::vec2 exit_pos(v["out"]["tile_x"].asInt() * m_TileSize.x, v["out"]["tile_y"].asInt() * m_TileSize.y);
		// scrolling behavior
		bool scrolling_enabled = v["out"]["scrolling_enabled"].asBool();
		// camera position
		glm::vec2 camera_position = glm::vec2(v["out"]["screen_x"].asInt() * m_TileSize.x, v["out"]["screen_y"].asInt() * m_TileSize.y);
		Gate *gate = new Gate(entrance, exit_pos, scrolling_enabled, camera_position);

		m_Gates.push_back(gate);
	}

	// create enemies
	const unsigned score_values[] = { 100, 200, 300, 500, 600, 700, 750, 800, 1000, 2000, 5000, 8000, 10000, 20000, 30000, 60000, 100000 };
	const char *score_effects[] = { "effect_score_100", "effect_score_200", "effect_score_300", "effect_score_500", "effect_score_600", \
									"effect_score_700", "effect_score_750", "effect_score_800", "effect_score_1000", "effect_score_2000", \
									"effect_score_5000", "effect_score_8000", "effect_score_10000", "effect_score_20000", "effect_score_30000", "effect_score_60000", "effect_score_100000"};
	const Json::Value &enemies = root["enemies"];
	for( unsigned i = 0, size = enemies.size(); i < size; ++i )
	{
		const Json::Value &v = enemies[i];
		EnemyBase *new_enemy = NULL;

		unsigned type = v["type"].asUInt();

		switch( type )
		{
			case 10 :
			{
				new_enemy = new Enemy10(this);
				Enemy10 *enemy_ptr = static_cast<Enemy10*>(new_enemy);

				enemy_ptr->SetSpeed(v["speed"].asUInt());
				glm::vec2 pos((float)(v["area"]["x"].asDouble()) * m_TileSize.x, (float)(v["area"]["y"].asDouble()) * m_TileSize.y);
				glm::vec2 size((float)(v["area"]["width"].asDouble()) * m_TileSize.x, (float)(v["area"]["height"].asDouble()) * m_TileSize.y);
				enemy_ptr->SetArea(AABB(pos, size));

				break;
			}

			case 1 :
			{
				new_enemy = new Enemy1(this);
				static_cast<Enemy1*>(new_enemy)->SetPosition(glm::vec2(v["x"].asUInt(), v["y"].asUInt()));

				break;
			}

			case 2 :
			{
				new_enemy = new Enemy2(this);
				Enemy2 *enemy_ptr = static_cast<Enemy2*>(new_enemy);
				enemy_ptr->SetSpeed((float)(v["speed"].asUInt()));
				enemy_ptr->SetStartPosition(glm::vec2(v["x"].asUInt(), v["y"].asUInt()));
				enemy_ptr->SetMaxWebLength((float)(v["max_web_length"].asUInt()));

				break;
			}

			case 4 :
			{
				new_enemy = new Enemy4(this);
				Enemy4 *enemy_ptr = static_cast<Enemy4*>(new_enemy);

				enemy_ptr->SetRadius((float)(v["radius"].asUInt()));
				enemy_ptr->SetMaxAngle((float)(v["angle"].asInt()));
				enemy_ptr->SetOrigoPosition(glm::vec2(v["x"].asUInt(), v["y"].asUInt()));

				break;
			}

			case 9 :
			{
				new_enemy = new Enemy9(this);
				Enemy9 *enemy_ptr = static_cast<Enemy9*>(new_enemy);

				enemy_ptr->SetCurrentPosition(glm::vec2((float)(v["x"].asInt()), (float)(v["y"].asInt())));
				enemy_ptr->SetSpeed(v["speed"].asUInt());
				enemy_ptr->SetBorders((float)(v["border_left"].asUInt()), (float)(v["border_right"].asUInt()));

				break;
			}

			case 8 :
			{
				new_enemy = new Enemy8(this);
				Enemy8 *enemy_ptr = static_cast<Enemy8*>(new_enemy);

				enemy_ptr->SetActivationDistance(glm::vec2((float)(v["distance_x"].asUInt()) * m_TileSize.x, (float)(v["distance_y"].asUInt()) * m_TileSize.y));
				enemy_ptr->SetJumpPower(glm::vec2((float)(v["jump_power_x"].asInt()), (float)(v["jump_power_y"].asInt())));
				enemy_ptr->SetPosition(glm::vec2((float)(v["x"].asInt()), (float)(v["y"].asInt())));

				break;
			}
		}

		if( new_enemy )
		{
			new_enemy->SetExpertOnly(v["expert_only"].asBool());
			new_enemy->SetDecorationOnly(v["decoration_only"].asBool());
			new_enemy->SetHitpoints(v["hitpoints"].asUInt());
			new_enemy->SetPauseDelay(v["pause"].asUInt());
			unsigned score_index = v["score"].asUInt();

			if( new_enemy->IsDecorationOnly() == false )
			{
				new_enemy->SetScore(score_values[score_index]);
				ScoreAnimation *score_anim = new ScoreAnimation(2000, new Sprite(*(ItemConfig.EffectSprites[score_effects[score_index]])));
				new_enemy->SetScoreAnimation(score_anim);
			}

			std::map<std::string, std::string>::iterator it = enemy_config_files.find(v["sprite"].asString());
			if( it != enemy_config_files.end() )
			{
				new_enemy->LoadConfigFromFile(it->second);
				new_enemy->Initialize();
				m_Enemies.push_back(new_enemy);
			}
			else
			{
				Logger::Instance().Log("WARNING: No enemy type found for sprite " + v["sprite"].asString());
				delete new_enemy;
			}
		}
	}

	// initialize stuff
	m_BackgroundAnimTimer.Reset();

	return OK;
}


void Level::ReleaseResources()
{
	delete m_BackgroundSprite;
	m_BackgroundSprite = NULL;

	m_TileUnderPlayer = NULL;

	for( std::vector<Tile *>::iterator it = m_Tiles.begin(), end = m_Tiles.end(); it != end; ++it )
	{
		delete *it;
		*it = NULL;
	}
	m_Tiles.clear();

	for( std::vector<Item *>::iterator it = m_Items.begin(), end = m_Items.end(); it != end; ++it )
	{
		delete *it;
		*it = NULL;
	}
	m_Items.clear();

	for( std::vector<Platform *>::iterator it = m_Platforms.begin(), end = m_Platforms.end(); it != end; ++it )
	{
		delete *it;
		*it = NULL;
	}
	m_Platforms.clear();

	for( std::vector<Gate *>::iterator it = m_Gates.begin(), end = m_Gates.end(); it != end; ++it )
	{
		delete *it;
		*it = NULL;
	}
	m_Gates.clear();

	for( std::vector<EnemyBase *>::iterator it = m_Enemies.begin(), end = m_Enemies.end(); it != end; ++it )
	{
		delete *it;
		*it = NULL;
	}
	m_Enemies.clear();
}


void Level::DrawBackground(const Camera2D &Camera)
{
	const AABB &cam_aabb = Camera.GetViewArea();
	int from_x = (int)(cam_aabb.m_Position.x / m_TileSize.x);
	int to_x = from_x + (int)(cam_aabb.m_Size.x / m_TileSize.x) + 1;
	int from_y = (int)(cam_aabb.m_Position.y  / m_TileSize.y);
	int to_y = from_y + (int)(cam_aabb.m_Size.y / m_TileSize.y) + 2;

	// background image
	GLESRenderer::Instance()->AddSprite2D(m_BackgroundSprite);
	GLESRenderer::Instance()->FlushSprites();

	// background tiles
	Tools::ClampValue<int>(from_x, 0, m_Width);
	Tools::ClampValue<int>(to_x, 0, m_Width);
	Tools::ClampValue<int>(from_y, 0, m_Height);
	Tools::ClampValue<int>(to_y, 0, m_Height);
	
	int i;
	for( int y = from_y; y < to_y; ++y )
	{
		for( int x = from_x; x < to_x; ++x )
		{
			i = (y * m_Width) + x;

			// select a frame according to animation state / player position
			const Sprite *sprite_ptr = m_Tiles[i]->GetSprite(0);
			if( m_Tiles[i]->IsAnimated() )
			{
				sprite_ptr = m_Tiles[i]->GetSprite(m_AnimatedTileFrameNumber);
			}
			else if( (m_Tiles[i]->IsChangedWhenSteppedOn()) && (m_TileUnderPlayer == m_Tiles[i]) )
			{
				sprite_ptr = m_Tiles[i]->GetSprite(1);
			}

			GLESRenderer::Instance()->AddSprite2D(sprite_ptr);
		}
	}
	GLESRenderer::Instance()->FlushSprites();
}


void Level::DrawOverlayingSprites(const Camera2D &Camera)
{
	if( m_HasOverlayingSprites == false )
	{
		return;
	}

	const AABB &cam_aabb = Camera.GetViewArea();
	int from_x = (int)(cam_aabb.m_Position.x / m_TileSize.x);
	int to_x = from_x + (int)(cam_aabb.m_Size.x / m_TileSize.x) + 1;
	int from_y = (int)(cam_aabb.m_Position.y  / m_TileSize.y);
	int to_y = from_y + (int)(cam_aabb.m_Size.y / m_TileSize.y) + 2;

	Tools::ClampValue<int>(from_x, 0, m_Width);
	Tools::ClampValue<int>(to_x, 0, m_Width);
	Tools::ClampValue<int>(from_y, 0, m_Height);
	Tools::ClampValue<int>(to_y, 0, m_Height);
	
	int i;
	for( int y = from_y; y < to_y; ++y )
	{
		for( int x = from_x; x < to_x; ++x )
		{
			i = (y * m_Width) + x;
			GLESRenderer::Instance()->AddSprite2D(m_Tiles[i]->GetOverlayingSprite());
		}
	}
	GLESRenderer::Instance()->FlushSprites();
}

void Level::UpdatePlatforms()
{
	for( std::vector<Platform *>::iterator it = m_Platforms.begin(), end = m_Platforms.end(); it != end; ++it )
	{
		(*it)->Update();
	}
}

void Level::DrawPlatforms()
{
	for( std::vector<Platform *>::iterator it = m_Platforms.begin(), end = m_Platforms.end(); it != end; ++it )
	{
		GLESRenderer::Instance()->AddSprite2D((*it)->GetSprite());
	}
	GLESRenderer::Instance()->FlushSprites();
}

/*
Update items
Calculate visibility and animate if necessary
*/
void Level::UpdateItems(const Camera2D &Camera)
{
	for( std::vector<Item *>::iterator it = m_Items.begin(), end = m_Items.end(); it != end; ++it )
	{
		if( (*it)->IsPickedUp() == false )
		{
			(*it)->CheckVisibility(Camera);
			(*it)->Animate();
		}
	}
}

void Level::DrawItems()
{
	for( std::vector<Item *>::iterator it = m_Items.begin(), end = m_Items.end(); it != end; ++it )
	{
		if( ((*it)->IsVisible()) && ((*it)->IsPickedUp() == false) )
		{
			GLESRenderer::Instance()->AddSprite2D((*it)->GetSprite());
		}
	}
	GLESRenderer::Instance()->FlushSprites();
}

/*
Check the visible items if the player intersects with one, and if so,
mark it as picked up and return a pointer to it
*/
Item *Level::PickItem(const Player *PlayerPtr)
{
	for( std::vector<Item *>::iterator it = m_Items.begin(), end = m_Items.end(); it != end; ++it )
	{
		if( ((*it)->IsVisible()) && ((*it)->IsPickedUp() == false) )
		{
			if( (*it)->GetSprite()->GetAABB().IntersectsWidth(PlayerPtr->GetHitbox()) )
			{
				return *it;
			}
		}
	}

	return NULL;
}

void Level::UpdateEnemies(const Player *PlayerPtr)
{
	for( std::vector<EnemyBase *>::iterator it = m_Enemies.begin(), end = m_Enemies.end(); it != end; ++it )
	{
		(*it)->Update(PlayerPtr);
	}
}

void Level::DrawEnemies()
{
	for( std::vector<EnemyBase *>::iterator it = m_Enemies.begin(), end = m_Enemies.end(); it != end; ++it )
	{
		if( (*it)->IsActive() )
		{
			GLESRenderer::Instance()->AddSprite2D((*it)->GetSprite());
		}
	}
	GLESRenderer::Instance()->FlushSprites();
}

const EnemyBase *Level::KillEnemies(const Weapon *WeaponPtr)
{
	AABB weapon_aabb(WeaponPtr->GetSprite()->GetPosition(), WeaponPtr->GetSprite()->GetSize());
	for( std::vector<EnemyBase *>::iterator it = m_Enemies.begin(), end = m_Enemies.end(); it != end; ++it )
	{
		if( ((*it)->IsActive()) && ((*it)->IsDecorationOnly() == false) )
		{
			AABB enemy_aabb((*it)->GetSprite()->GetPosition(), (*it)->GetSprite()->GetSize());
			if( enemy_aabb.IntersectsWidth(weapon_aabb) )
			{
				(*it)->Die();
				return (*it);
			}
		}
	}

	return NULL;
}

const EnemyBase *Level::GetDamagingEnemy(const Player *PlayerPtr)
{
	for( std::vector<EnemyBase *>::iterator it = m_Enemies.begin(), end = m_Enemies.end(); it != end; ++it )
	{
		if( ((*it)->IsActive()) && ((*it)->IsDamaging()) )
		{
			AABB enemy_aabb((*it)->GetSprite()->GetPosition(), (*it)->GetSprite()->GetSize());
			if( enemy_aabb.IntersectsWidth(PlayerPtr->GetHitbox()) )
			{
				return (*it);
			}
		}
	}

	return NULL;
}


const glm::vec2 &Level::GetStartPosition() const
{
	return m_StartPosition;
}

/*
Calculate the new position of a given AABB (collision detection with tiles)
*/
void Level::MoveAABB(const AABB &Hitbox, const glm::vec2 &DeltaPosition, CollisionDetectionResult &Result, bool CheckTiles, bool PassThroughHatches, bool CheckPlatforms, bool CheckEnemies) const
{
	AABB new_aabb = Hitbox;

	int row_first;
	int row_last;
	int column_first;
	int column_last;

	Result.ObstaclePlatform = NULL;
	Result.ObstacleEnemy = NULL;
	Result.Collided = false;

	// --- move along axis X ---

	if( DeltaPosition.x != 0 )
	{
		// rows of tiles to be checked...
		row_first = (int)((Hitbox.m_Position.y) / m_TileSize.y);
		row_last = (int)((Hitbox.m_Position.y + Hitbox.m_Size.y - 1) / m_TileSize.y);
		bool skip_last_row = false;
		Tools::ClampValue<int>(row_first, 0, m_Height - 1);
		Tools::ClampValue<int>(row_last, 0, m_Height - 1);

		// moving left
		if( DeltaPosition.x < 0 )
		{
			column_first = (int)(Hitbox.m_Position.x / m_TileSize.x);
			skip_last_row = ((Result.ObstacleTileY)  && (Result.ObstacleTileY->GetSlopeType() == Tile::SLOPE_DOWNRIGHT));
			Tools::ClampValue<int>(column_first, 0, m_Width - 1);

			// find the closest obstacle tile
			int delta_column = m_Width;
			if( CheckTiles )
			{
				for( int row = row_first; row <= ( (skip_last_row) ? (row_last - 1) : (row_last) ); ++row )
				{
					for( int column = column_first; (abs(column - column_first) < delta_column) && (column >= 0); --column )
					{
						int i = (row * m_Width) + column;

						if( m_Tiles[i]->GetSideProperty(Tile::RIGHT) == Tile::SOLID )
						{
							delta_column = abs(column - column_first);
							Result.ObstacleTileX = m_Tiles[i];
						}
					}
				}
			}

			// set the new x coordinate
			float obstacle_x = (column_first - delta_column + 1) * m_TileSize.x;
			float new_x = Hitbox.m_Position.x + DeltaPosition.x;
			// the object collided with a tile
			if( obstacle_x >= new_x )
			{
				new_aabb.m_Position.x = obstacle_x;
				Result.Collided = true;
			}
			// the object didn't collide
			else
			{
				new_aabb.m_Position.x = new_x;
				Result.ObstacleTileX = NULL;
			}

			// don't pass through platforms
			if( CheckPlatforms )
			{
				for( std::vector<Platform *>::const_iterator it = m_Platforms.begin(), end = m_Platforms.end(); it != end; ++it )
				{
					if( new_aabb.IntersectsWidth((*it)->GetAABB()) )
					{
						new_aabb.m_Position.x = (*it)->GetAABB().m_Position.x + (*it)->GetAABB().m_Size.x;
						Result.Collided = true;
						break;
					}
				}
			}
		}
		// moving right
		else
		{
			column_first = (int)((Hitbox.m_Position.x + Hitbox.m_Size.x) / m_TileSize.x);
			skip_last_row = ((Result.ObstacleTileY)  && (Result.ObstacleTileY->GetSlopeType() == Tile::SLOPE_UPRIGHT));
			Tools::ClampValue<int>(column_first, 0, m_Width - 1);

			// find the closest obstacle tile
			int delta_column = m_Width;
			if( CheckTiles )
			{
				for( int row = row_first; row <= ( (skip_last_row) ? (row_last - 1) : (row_last) ); ++row )
				{
					for( int column = column_first; (abs(column - column_first) < delta_column) && (column < m_Width); ++column )
					{
						int i = (row * m_Width) + column;

						if( m_Tiles[i]->GetSideProperty(Tile::LEFT) == Tile::SOLID )
						{
							delta_column = abs(column - column_first);
							Result.ObstacleTileX = m_Tiles[i];
						}
					}
				}
			}

			// set the new x coordinate
			float obstacle_x = (column_first + delta_column) * m_TileSize.x - Hitbox.m_Size.x;
			float new_x = Hitbox.m_Position.x + DeltaPosition.x;
			// the object collided with a tile
			if( obstacle_x <= new_x )
			{
				new_aabb.m_Position.x = obstacle_x;
				Result.Collided = true;
			}
			// the object didn't collide
			else
			{
				new_aabb.m_Position.x = new_x;
				Result.ObstacleTileX = NULL;
			}

			// don't pass through platforms
			if( CheckPlatforms )
			{
				for( std::vector<Platform *>::const_iterator it = m_Platforms.begin(), end = m_Platforms.end(); it != end; ++it )
				{
					if( new_aabb.IntersectsWidth((*it)->GetAABB()) )
					{
						new_aabb.m_Position.x = (*it)->GetAABB().m_Position.x - Hitbox.m_Size.x;
						Result.Collided = true;
						break;
					}
				}
			}
		}
	}
	else
	{
		Result.ObstacleTileX = NULL;
	}

	// --- move along axis Y ---
	if( DeltaPosition.y != 0 )
	{
		// columns of tiles to be checked...
		column_first = (int)((new_aabb.m_Position.x + 1) / m_TileSize.x);
		column_last = (int)((new_aabb.m_Position.x + Hitbox.m_Size.x - 1) / m_TileSize.x);
		int column_middle = (int)((new_aabb.m_Position.x + (Hitbox.m_Size.x / 2.f) - 1) / m_TileSize.x);
		Tools::ClampValue<int>(column_first, 0, m_Width - 1);
		Tools::ClampValue<int>(column_last, 0, m_Width - 1);
		Tools::ClampValue<int>(column_middle, 0, m_Width - 1);

		// moving upwards
		if( DeltaPosition.y < 0 )
		{
			// where to start checking tiles
			row_first = (int)(Hitbox.m_Position.y / m_TileSize.y);
			Tools::ClampValue<int>(row_first, 0, m_Height - 1);

			// find the closest obstacle tile
			int delta_row = m_Height;
			if( CheckTiles )
			{
				for( int column = column_first; column <= column_last; ++column )
				{
					for( int row = row_first; (abs(row - row_first) < delta_row) && (row >= 0); --row )
					{
						int i = (row * m_Width) + column;

						if( m_Tiles[i]->GetSideProperty(Tile::BOTTOM) == Tile::SOLID )
						{
							delta_row = abs(row - row_first);
							Result.ObstacleTileY = m_Tiles[i];
						}
					}
				}
			}

			// set the new y coordinate
			float obstacle_y = (row_first - delta_row + 1) * m_TileSize.y;
			float new_y = Hitbox.m_Position.y + DeltaPosition.y;
			// the object collided with a tile
			if( obstacle_y >= new_y )
			{
				new_aabb.m_Position.y = obstacle_y;
				Result.Collided = true;
			}
			// the object didn't collide
			else
			{
				new_aabb.m_Position.y = new_y;
				Result.ObstacleTileY = NULL;
			}

			// don't pass through platforms
			if( CheckPlatforms )
			{
				for( std::vector<Platform *>::const_iterator it = m_Platforms.begin(), end = m_Platforms.end(); it != end; ++it )
				{
					if( new_aabb.IntersectsWidth((*it)->GetAABB()) )
					{
						new_aabb.m_Position.y = (*it)->GetAABB().m_Position.y + (*it)->GetAABB().m_Size.y;
						Result.Collided = true;
						break;
					}
				}
			}
		}
		// moving downwards
		else
		{
			// where to start checking tiles
			row_first = (int)((Hitbox.m_Position.y + Hitbox.m_Size.y - 1) / m_TileSize.y);
			Tools::ClampValue<int>(row_first, 0, m_Height - 1);

			// find the closest obstacle tile
			int delta_row = m_Height;
			if( CheckTiles )
			{
				for( int column = column_first; column <= column_last; ++column )
				{
					for( int row = row_first; (abs(row - row_first) < delta_row) && (row < m_Height); ++row )
					{
						int i = (row * m_Width) + column;

						// the tile is an obstacle if its top is solid OR the player could fall through it if arrow down were pressed
						Tile::SIDE_PROPERTY top_prop = m_Tiles[i]->GetSideProperty(Tile::TOP);
						if( ((top_prop == Tile::SOLID || (top_prop == Tile::DEADLY) && (row > row_first) ) && (m_Tiles[i]->GetSlopeType() == Tile::HORIZONTAL_FLOOR)) || ((top_prop == Tile::TRAVERSABLE_WHEN_PRESSING_DOWN) && (PassThroughHatches == false)) )
						{
							delta_row = abs(row - row_first);
							Result.ObstacleTileY = m_Tiles[i];
						}
					}
				}
			}

			// slopes
			int slope_row;
			int slope_col = column_middle;
			bool slope_flag = false;
			float h = 0.f;
			if( CheckTiles )
			{
				for( slope_row = (row_first > 0) ? (row_first - 1) : (row_first); (slope_row <= row_first + 1) && (slope_row < m_Height); ++slope_row )
				{
					int slope_i = (slope_row * m_Width) + slope_col;

					if( m_Tiles[slope_i]->GetSlopeType() != Tile::HORIZONTAL_FLOOR )
					{
						slope_flag = true;

						Result.ObstacleTileY = m_Tiles[slope_i];

						float t = ((new_aabb.m_Position.x + (Hitbox.m_Size.x / 2)) - (slope_col * m_TileSize.x)) / m_TileSize.x;
						float left = (float)m_Tiles[slope_i]->GetReverseHeightLeft();
						float right = (float)m_Tiles[slope_i]->GetReverseHeightRight();
						if( (m_Tiles[slope_i]->GetSlopeType() == Tile::SLOPE_DOWNRIGHT) && (m_Tiles[slope_i]->GetReverseHeightRight() == 0) )
						{
							right = m_TileSize.y - 1;
						}
						h = Tools::Interpolate(left, right, t);

						break;
					}
				}
			}

			// set the new y coordinate
			float obstacle_y;			
			if( slope_flag )
			{
				obstacle_y = (slope_row) * m_TileSize.y - Hitbox.m_Size.y + h;
			}
			else
			{
				obstacle_y = (row_first + delta_row) * m_TileSize.y - Hitbox.m_Size.y;
			}
			
			float new_y = Hitbox.m_Position.y + DeltaPosition.y;

			// the object collided with a tile
			if( obstacle_y <= new_y )
			{
				new_aabb.m_Position.y = obstacle_y;
				Result.Collided = true;
			}
			// the object didn't collide
			else
			{
				new_aabb.m_Position.y = new_y;
				Result.ObstacleTileY = NULL;
			}

			// don't pass through platforms
			if( CheckPlatforms )
			{
				for( std::vector<Platform *>::const_iterator it = m_Platforms.begin(), end = m_Platforms.end(); it != end; ++it )
				{
					if( new_aabb.IntersectsWidth((*it)->GetAABB()) )
					{
						new_aabb.m_Position.y = (*it)->GetAABB().m_Position.y - Hitbox.m_Size.y;
						Result.Collided = true;
						Result.ObstaclePlatform = *it;
						break;
					}
				}
			}

			// jump on enemies
			if( CheckEnemies )
			{
				for( std::vector<EnemyBase *>::const_iterator it = m_Enemies.begin(), end = m_Enemies.end(); it != end; ++it )
				{
					if( ((*it)->IsActive()) && ((*it)->IsDecorationOnly() == false) )
					{
						AABB enemy_aabb((*it)->GetSprite()->GetPosition(), (*it)->GetSprite()->GetSize());
						if( (Hitbox.m_Position.y + Hitbox.m_Size.y <= enemy_aabb.m_Position.y) && (new_aabb.IntersectsWidth(enemy_aabb)) )
						{
							new_aabb.m_Position.y = enemy_aabb.m_Position.y - Hitbox.m_Size.y;
							Result.Collided = true;
							Result.ObstacleEnemy = *it;
						}
					}
				}
			}
		}
	}
	else
	{
		Result.ObstacleTileY = NULL;
	}

	Result.NewPosition = new_aabb.m_Position;
}

glm::vec2 Level::MovePlayer(const Player *PlayerPtr, CollisionDetectionResult &Result)
{
	MoveAABB(PlayerPtr->GetHitbox(), PlayerPtr->GetVelocity(), Result, true, PlayerPtr->IsPressingDown(), true, true);
	if( fabs(PlayerPtr->GetVelocity().y) >= 0.1f )
	{
		m_TileUnderPlayer = Result.ObstacleTileY;
	}
	return Result.NewPosition;
}

Gate *Level::TeleportPlayer(const Player *PlayerPtr)
{
	// check if the player's hitbox intersects with a gate's hitbox
	for( std::vector<Gate *>::iterator it = m_Gates.begin(), end = m_Gates.end(); it != end; ++it )
	{
		if( (*it)->GetEntranceHitbox().IntersectsWidth(PlayerPtr->GetHitbox()) )
		{
			return (*it);
		}
	}

	return NULL;
}

const Tile &Level::GetTile(int Row, int Column) const
{
	return *(m_Tiles[Row * m_Width + Column]);
}

void Level::AnimateTiles()
{
	m_AnimatedTileFrameNumber += (unsigned char)(m_BackgroundAnimTimer.GetCycles());
	m_AnimatedTileFrameNumber %= 3;
}

glm::vec2 Level::WorldToTileCoordinates(const glm::vec2 &WorldPos) const
{
	return glm::vec2((int)(WorldPos.x / m_TileSize.x), (int)(WorldPos.y / m_TileSize.y));
}

glm::vec2 Level::TileToWorldCoordinates(const glm::vec2 &TileIndices) const
{
	return TileIndices * m_TileSize;
}

/*
Find the first ground tile, starting from a given tile
*/
glm::vec2 Level::FindFirstGroundTileAbove(const glm::vec2 &StartingTileIndices) const
{
	int column = (int)(StartingTileIndices.x);
	int row_first = (int)(StartingTileIndices.y + 1);
	Tools::ClampValue<int>(row_first, 0, m_Height);

	for( int row = row_first; row > 0; --row )
	{
		int i = (row * m_Width) + column;
		int below = ((row + 1) * m_Width) + column;
		
		// if the tile is solid from the top and the tile above it is traversable...
		if( (m_Tiles[i]->GetSideProperty(Tile::TOP) == Tile::TRAVERSABLE) &&
			(m_Tiles[below]->GetSideProperty(Tile::TOP) != Tile::TRAVERSABLE) )
		{
			return glm::vec2(column, row);
		}
	}

	return StartingTileIndices;
}

const Tile *Level::GetTileUnderPlayer() const
{
	return m_TileUnderPlayer;
}
