//
//
//  @ Project : SDMG
//  @ File Name : MidRangeAttackAction.cpp
//  @ Date : 21-10-2014
//  @ Author : 42IN13SAd
//
//

#include "MidRangeAttackAction.h"
#include "model\Character.h"
#include "engine\MovableGameObject.h"
#include "engine\GameBase.h"
#include "engine\Engine.h"
#include "engine\audio\AudioEngine.h"

namespace sdmg {
	namespace actions {
		MidRangeAttackAction::MidRangeAttackAction(Character *character) : CharacterAction(character, "MidRangeAttackAction") {}
		MidRangeAttackAction::MidRangeAttackAction(Character *character, SDL_Event event) : CharacterAction(character, event, "MidRangeAttackAction") {}

		bool MidRangeAttackAction::run(engine::GameBase &game) {
			CharacterAction::run(game);

			if (_character->stateIsInterruptible() && _character->getAttackBody() == nullptr)
			{
				if (_event.type == SDL_KEYDOWN) {
					if (_character->getPP() >= 10)
					{
						/*
						if (_character->getState() != (MovableGameObject::State::WALKING | MovableGameObject::State::MIDRANGEATTACKBEGIN) &&
							_character->getState() != (MovableGameObject::State::WALKING | MovableGameObject::State::MIDRANGEATTACK) &&
							_character->getState() != (MovableGameObject::State::WALKING | MovableGameObject::State::MIDRANGEATTACKEND))
						{
							if (_character->getState() == MovableGameObject::State::WALKING)
								_character->setState(MovableGameObject::State::WALKING | MovableGameObject::State::MIDRANGEATTACKBEGIN);
							else
								_character->setState(MovableGameObject::State::IDLE | MovableGameObject::State::MIDRANGEATTACKBEGIN);
						}
						*/

						game.getEngine()->getAudioEngine()->play("punch_" + _character->getKey(), 0);

						if (_character->getState() == MovableGameObject::State::WALKING || _character->getState() == MovableGameObject::State::FALLINGLEFT
							|| _character->getState() == MovableGameObject::State::FALLINGRIGHT || _character->getState() == MovableGameObject::State::JUMPINGLEFT
							|| _character->getState() == MovableGameObject::State::JUMPINGRIGHT)
							_character->setState(MovableGameObject::State::WALKING | MovableGameObject::State::MIDRANGEATTACKBEGIN);
						else
							_character->setState(MovableGameObject::State::IDLE | MovableGameObject::State::MIDRANGEATTACKBEGIN);
					}
				}
				else {

				}
			}
			/*
			if (_event.type == SDL_KEYDOWN) {
			_character->setDirection(MovableGameObject::Direction::LEFT);
			_character->setState(MovableGameObject::State::WALKING);
			}
			else if (_event.type == SDL_KEYUP && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_D]) {
			_character->setDirection(MovableGameObject::Direction::RIGHT);
			_character->setState(MovableGameObject::State::WALKING);
			}
			else {
			_character->setState(MovableGameObject::State::IDLE);
			}
			*/
			return true;
		}

		Action* MidRangeAttackAction::create(SDL_Event &event) {
			return new MidRangeAttackAction(_character, event);
		}
	}
}