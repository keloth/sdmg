//
//
//  @ Project : SDMG
//  @ File Name : LoadingPlayBackState.cpp
//  @ Date : 18-12-2014
//  @ Author : 42IN13SAd
//
//


#include "LoadingPlayBackState.h"
#include "MainMenuState.h"
#include "engine\Engine.h"
#include "engine\drawing\DrawEngine.h"
#include <SDL.h>
#include "engine\physics\PhysicsEngine.h"
#include "model\Platform.h"
#include "model\MovablePlatform.h"
#include "model\Character.h"
#include "factories\CharacterFactory.h"
#include "engine\input\InputEngine.h"
#include "actions\Actions.h"
#include "engine\World.h"
#include "engine\audio\AudioEngine.h"
#include "helperclasses\HUD.h"
#include "helperclasses\ConfigManager.h"
#include <array>

#include "engine\util\FileManager.h"

#include <random>

namespace sdmg {
	namespace gamestates {

		void LoadingPlayBackState::init(GameBase &game)
		{
			_game = &game;
			_game->getWorld()->clearWorld();

			// LoadingBar
			_loadingValue = 0;
			_marginInner = 3;
			_marginValue = 1;
			_totalWidth = 300;
			_totalHeight = 23;
			_loadingBarX = game.getEngine()->getDrawEngine()->getWindowWidth() / 2 - _totalWidth / 2;
			_loadingBarY = 565;

			_isLoaded = false;
			_isError = false;
			_isAdvertisement = false;
			_recordQueue = new std::queue<PlayBackState::RecordStep*>();

			game.getEngine()->getDrawEngine()->load("loading", "assets\\screens\\loadingscreen");

			_game->getStateManager()->draw();

			game.getEngine()->getAudioEngine()->load("hurt_punch", "assets\\sounds\\effects\\hurt_punch.ogg", AUDIOTYPE::SOUND_EFFECT);
			game.getEngine()->getAudioEngine()->load("hurt_shoot", "assets\\sounds\\effects\\hurt_shoot.ogg", AUDIOTYPE::SOUND_EFFECT);
			load();
			game.getEngine()->getAudioEngine()->unload("main_menu_bgm");

			_game->getEngine()->getPhysicsEngine()->resume();

			_game->getStateManager()->update();
		}

		void LoadingPlayBackState::cleanup(GameBase &game)
		{
			delete _level;
			_level = nullptr;
			delete _fileName;
			_fileName = nullptr;

			if (_characters) {
				for (auto it : *_characters)
					delete it;
				_characters->clear();
			}
			delete _characters;
			_characters = nullptr;

			_recordQueue = nullptr;
			
			if (_recordMap && _recordMap->size() > 0) {
				std::unordered_map<std::string, Action*>::iterator itr = _recordMap->begin();
				while (itr != _recordMap->end()) {
					delete itr->second;
					_recordMap->erase(itr++);
				}
			}
			delete _recordMap;
			_recordMap = nullptr;

			delete _characterObjects;
			_characterObjects = nullptr;

			game.getEngine()->getDrawEngine()->unload("loading");
		}

		void LoadingPlayBackState::pause(GameBase &game)
		{
		}

		void LoadingPlayBackState::resume(GameBase &game)
		{
		}

		void LoadingPlayBackState::handleEvents(GameBase &game, GameTime &gameTime)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event));
		}

		void LoadingPlayBackState::update(GameBase &game, GameTime &gameTime)
		{
			if (_isLoaded)
			{
				PlayBackState::getInstance().setHUDs(_huds);
				changeState(game, PlayBackState::getInstance());
			}
			if (_isError) {
				// Clean uppen
				changeState(game, MainMenuState::getInstance());
			}
		}
		
		void LoadingPlayBackState::draw(GameBase &game, GameTime &gameTime)
		{
			game.getEngine()->getDrawEngine()->prepareForDraw();
			game.getEngine()->getDrawEngine()->draw("loading");

			if (_isAdvertisement)
				game.getEngine()->getDrawEngine()->draw("advertisement", _advertisementX, _advertisementY);
			
			game.getEngine()->getDrawEngine()->drawRectangle(Rectangle(_loadingBarX, _loadingBarY, _totalWidth, _totalHeight), 200, 200, 200);
			game.getEngine()->getDrawEngine()->drawRectangle(Rectangle(_loadingBarX + _marginInner, _loadingBarY + _marginInner, _totalWidth - (_marginInner * 2), _totalHeight - (_marginInner * 2)), 255, 255, 255);
			game.getEngine()->getDrawEngine()->drawRectangle(Rectangle(_loadingBarX + _marginInner + _marginValue, _loadingBarY + _marginInner + _marginValue, _loadingValue, _totalHeight - (_marginInner * 2) - (_marginValue * 2)), 50, 50, 50);

			game.getEngine()->getDrawEngine()->render();
		}

		void LoadingPlayBackState::setPlaybackFileName(std::string fileName)
		{
			_fileName = new std::string(fileName);
		}

		void LoadingPlayBackState::load() {
			try
			{
				int maxLoadingValue = _totalWidth - (_marginInner * 2) - (_marginValue * 2);
				_loadingStep = maxLoadingValue / 3;

				loadAdvertisement();
				loadKeybindings();
				loadPlaybackSteps();

				_loadingValue = maxLoadingValue;

				_isLoaded = true;
				clearEventQueue();
			}
			catch (...)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Cannot load recording", "Cannot read the recording file.", nullptr);
				changeState(*_game, MainMenuState::getInstance());
			}
		}

		void LoadingPlayBackState::loadPlaybackSteps()
		{
			try
			{
				auto json = engine::util::FileManager::getInstance().loadFileContentsAsJson("assets/playbacks/" + *_fileName);

				_level = new std::string(json["level"].get<std::string>());

				auto jsonCharacterArray = json["characters"].get<nlohmann::json>();
				auto numberOfCharacters = jsonCharacterArray.size();

				_characters = new std::vector<std::string*>(numberOfCharacters);
				for (auto& jsonCharacter : jsonCharacterArray)
				{
					(*_characters)[jsonCharacter["index"].get<int>()] = new std::string(jsonCharacter["key"].get<std::string>());
				}
				loadLevel();

				// load steps
				_recordQueue = new std::queue<PlayBackState::RecordStep*>();

				auto jsonStepsArray = json["steps"].get<nlohmann::json>();
				auto numberOfSteps = jsonStepsArray.size();
				for (auto& jsonStepObj : jsonStepsArray)
				{
					auto actionName = jsonStepObj["action"].get<std::string>();
					auto timeStamp = jsonStepObj["timestamp"].get<int>();

					PlayBackState::RecordStep* step;

					if (actionName != "GameOver")
					{
						auto characterIndex = jsonStepObj["character"].get<int>();
						auto characterAction = actionName + std::to_string(characterIndex);

						// Fake SDL Event
						auto keyState = jsonStepObj["keyDown"].get<bool>();

						SDL_Event event;
						event.type = keyState ? SDL_KEYDOWN : SDL_KEYUP;

						// create and run action
						auto action = (*_recordMap)[characterAction]->create(event);

						auto player = (*_characterObjects)[characterIndex];
						step = new PlayBackState::RecordStep(action, timeStamp, player);
					}
					else
					{
						// Make step for GameOver
						step = new PlayBackState::RecordStep(nullptr, timeStamp, nullptr);
					}

					auto jsonStepCharacters = jsonStepObj["characters"].get<std::vector<nlohmann::json>>();

					for (auto& stepCharacter : jsonStepCharacters)
					{
						auto stepCharacterIndex = stepCharacter["character"].get<int>();
						auto character = (*_characterObjects)[stepCharacterIndex];
						step->_playerData.push_back({
														character,
														stepCharacter["hp"].get<int>(),
														stepCharacter["lives"].get<int>(),
														stepCharacter["pp"].get<int>(),
														stepCharacter["x"].get<float>(),
														stepCharacter["y"].get<float>(),
														stepCharacter["velocityx"].get<float>(),
														stepCharacter["velocityy"].get<float>(),
														static_cast<MovableGameObject::Direction>(stepCharacter["direction"].get<int>())
													});
					}

					_recordQueue->push(step);
				}

				PlayBackState::getInstance().setPlaybackSteps(_recordQueue);
			}
			catch (...)
			{
				throw;
			}
		}

		void LoadingPlayBackState::clearEventQueue() {
			SDL_Event event;
			while (SDL_PollEvent(&event));
		}

		void LoadingPlayBackState::loadLevel() {

			_game->getStateManager()->draw();

			auto json = engine::util::FileManager::getInstance().loadFileContentsAsJson("assets/levels/" + (*_level) + "/data");

			PhysicsEngine *pe = _game->getEngine()->getPhysicsEngine();
			DrawEngine *de = _game->getEngine()->getDrawEngine();

			auto jsonGravityObject = json["gravity"].get<nlohmann::json>();

			pe->setWorldGravity(jsonGravityObject["left"].get<float>(), jsonGravityObject["down"].get<float>());

			auto jsonPlatforms = json["platforms"].get<std::vector<nlohmann::json>>();

			if (jsonPlatforms.size() > 0)
			{
				int platformStep = (_loadingStep / 3) / jsonPlatforms.size();

				for (auto& jsonPlatform : jsonPlatforms)
				{
					auto jsonSizeObject = jsonPlatform["size"].get<nlohmann::json>();
					auto jsonLocationObject = jsonPlatform["location"].get<nlohmann::json>();
					auto jsonBodyPaddingObject = jsonPlatform["bodyPadding"].get<nlohmann::json>();

					Platform* platform = new model::Platform();
					platform->setSize(jsonSizeObject["width"].get<float>(), jsonSizeObject["height"].get<float>());
					platform->setLocation(jsonLocationObject["x"].get<float>(), jsonLocationObject["y"].get<float>());
					pe->addBody(platform, jsonBodyPaddingObject["x"].get<float>(), jsonBodyPaddingObject["y"].get<float>());

					if (jsonPlatform.contains("canMoveThroughIt"))
						platform->setCanMoveThroughIt(false);
					else
						platform->setCanMoveThroughIt(true);

					_game->getWorld()->addPlatform(platform);

					if (jsonPlatform.contains("image"))
						de->load(platform, "assets/levels/" + (*_level) + "/" + jsonPlatform["image"].get<std::string>());

					_loadingValue += platformStep;
					_game->getStateManager()->draw();
				}
			}
			else {
				_loadingValue += _loadingStep / 3;
			}

			de->load("overlay", "assets/levels/" + (*_level) + "/overlay");
			de->load("background", "assets/levels/" + (*_level) + "/background");
			_game->getEngine()->getAudioEngine()->load("bgm", "assets/levels/" + (*_level) + "/bgm.mp3", AUDIOTYPE::MUSIC);

			loadCharacters(json["startingPositions"].get<std::vector<nlohmann::json>>());

			if (json.contains("bobs"))
				loadBulletBobs(json["bobs"].get<std::vector<nlohmann::json>>());

			// Load fps text
			de->loadDynamicText("fps", { 255, 255, 255 }, "arial", 18);
			de->loadDynamicText("speed", { 255, 255, 255 }, "arial", 18);
		}

		void LoadingPlayBackState::loadCharacters(std::vector<nlohmann::json>& startingPositions) {

			delete _recordMap;
			_recordMap = new std::unordered_map<std::string, Action*>();

			_characterObjects = new std::vector<Character*>(_characters->size());
			
			int characterStep = (_loadingStep / 3) / (_characters->size() + 1);

			for (int i = 0; i < _characters->size(); i++) {

				_game->getStateManager()->draw();

				int retries = 0;
				do{
					(*_characterObjects)[i] = factories::CharacterFactory::create(*(*_characters)[i], *_game, startingPositions[i]["x"].get<float>(), startingPositions[i]["y"].get<float>());
					_game->getWorld()->addPlayer((*_characterObjects)[i]);
					if (retries++ > 10){
						_isError = true;
						return;
					}

					_recordMap->insert({ "BlockAction" + std::to_string(i), new actions::BlockAction((*_characterObjects)[i]) });
					_recordMap->insert({ "JumpAction" + std::to_string(i), new actions::JumpAction((*_characterObjects)[i]) });
					_recordMap->insert({ "LeftWalkAction" + std::to_string(i), new actions::LeftWalkAction((*_characterObjects)[i]) });
					_recordMap->insert({ "LongRangeAttackAction" + std::to_string(i), new actions::LongRangeAttackAction((*_characterObjects)[i]) });
					_recordMap->insert({ "MidRangeAttackAction" + std::to_string(i), new actions::MidRangeAttackAction((*_characterObjects)[i]) });
					_recordMap->insert({ "RespawnAction" + std::to_string(i), new actions::RespawnAction((*_characterObjects)[i]) });
					_recordMap->insert({ "RightWalkAction" + std::to_string(i), new actions::RightWalkAction((*_characterObjects)[i]) });
					_recordMap->insert({ "RollAction" + std::to_string(i), new actions::RollAction((*_characterObjects)[i]) });
					
				} while ((*_characterObjects)[i] == nullptr);

				_loadingValue += characterStep;
				_game->getStateManager()->draw();
			}

			(*_characterObjects)[1]->setDirection(MovableGameObject::Direction::LEFT);
			(*_characterObjects)[1]->setSpawnDirection(MovableGameObject::Direction::LEFT);

			_game->getStateManager()->draw();

			// Create a HUD for each player
			_huds = new std::vector<helperclasses::HUD*>();

			HUD *hud1 = new HUD(*(*_characterObjects)[0], 10);
			_huds->push_back(hud1);

			HUD *hud2 = new HUD(*(*_characterObjects)[1], _game->getEngine()->getDrawEngine()->getWindowWidth() - 230 - 10);
			_huds->push_back(hud2);

			if ((*_characterObjects).size() >= 3)
			{
				HUD *hud3 = new HUD(*(*_characterObjects)[2], 20 + 230);
				_huds->push_back(hud3);
			}
			if ((*_characterObjects).size() >= 4)
			{
				HUD *hud4 = new HUD(*(*_characterObjects)[3], _game->getEngine()->getDrawEngine()->getWindowWidth() - (230 * 2) - 20);
				_huds->push_back(hud4);
			}

			_loadingValue += characterStep;
		}

		void LoadingPlayBackState::loadBulletBobs(std::vector<nlohmann::json>& bobs) {

			_game->getStateManager()->draw();

			int bobStep = 0;
			if (bobStep <= 0)
				_loadingValue += (_loadingStep / 3);
			else
				bobStep = (_loadingStep / 3) / bobs.size();

			for (auto& jsonBobObj : bobs)
			{
				auto jsonSizeObject = jsonBobObj["size"].get<nlohmann::json>();
				auto jsonLocationObject = jsonBobObj["location"].get<nlohmann::json>();
				auto jsonEndLocationObject = jsonBobObj["endLocation"].get<nlohmann::json>();
				auto jsonSpeedObject = jsonBobObj["speed"].get<nlohmann::json>();

				auto direction = static_cast<MovableGameObject::Direction>(jsonBobObj["direction"].get<int>());
				auto locationX = jsonLocationObject["x"].get<float>();
				auto locationY = jsonLocationObject["x"].get<float>();

				MovablePlatform* platform = new MovablePlatform();
				platform->setSize(jsonSizeObject["width"].get<float>(), jsonSizeObject["height"].get<float>());
				platform->setLocation(locationX, locationY);
				platform->setStartLocation(b2Vec2(locationX, locationY));
				platform->setEndLocation(b2Vec2(jsonEndLocationObject["x"].get<float>(), jsonEndLocationObject["y"].get<float>()));
				platform->setDirection(direction);
				platform->setSpeed(jsonSpeedObject["horizontal"].get<float>(), jsonSpeedObject["vertical"].get<float>());
				platform->setDamageOnImpact(100);

				_game->getWorld()->addPlatform(platform);
				_game->getEngine()->getPhysicsEngine()->addKinematicBody(platform);
				_game->getEngine()->getDrawEngine()->loadMap(platform, MovableGameObject::State::IDLE, R"(assets\levels\level2\bullet.sprite)", 1097, 494, 0.1);

				_loadingValue += bobStep;
				_game->getStateManager()->draw();

			}
		}

		void LoadingPlayBackState::loadKeybindings() {

			_loadingValue += _loadingStep;
			_game->getStateManager()->draw();
		}

		void LoadingPlayBackState::loadAdvertisement()
		{

			_game->getStateManager()->draw();

			std::vector<std::string> advertismentList = util::FileManager::getInstance().getFiles("assets/advertisements/");

			if (advertismentList.size() > 0)
			{
				_isAdvertisement = true;

				std::random_device dev;
				std::default_random_engine dre(dev());
				std::uniform_int_distribution<int> randomAdvertisement(0, advertismentList.size() - 1);

				_game->getEngine()->getDrawEngine()->load("advertisement", "assets\\advertisements\\" + advertismentList[randomAdvertisement(dre)]);

				const std::array<int, 2> size = _game->getEngine()->getDrawEngine()->getImageSize("advertisement");
				_advertisementX = _game->getEngine()->getDrawEngine()->getWindowWidth() - size[0] - 10;
				_advertisementY = _game->getEngine()->getDrawEngine()->getWindowHeight() - size[1] - 10;

			}

			_loadingValue += _loadingStep;
		}
	}
}