//
//
//  @ Project : SDMG
//  @ File Name : JumpAction.cpp
//  @ Date : 26-9-2014
//  @ Author : 42IN13SAd
//
//


#include "JumpAction.h"
#include "model\Character.h"
#include "engine\MovableGameObject.h"
#include "engine\GameBase.h"
#include "engine\physics\PhysicsEngine.h"
#include "engine\Engine.h"

namespace sdmg {
	namespace actions {
		JumpAction::JumpAction(Character *character) : CharacterAction(character, "JumpAction") {}
		JumpAction::JumpAction(Character *character, SDL_Event event) : CharacterAction(character, event, "JumpAction") {}

		bool JumpAction::run(engine::GameBase &game) {
			CharacterAction::run(game);

			if (_character->stateIsInterruptible())
			{
				if (_event.type == SDL_KEYDOWN) {
					if (!_character->getIsJumping())
					{
						if (_character->getState() != MovableGameObject::State::JUMPING)
						{
							if (_character->getState() == MovableGameObject::State::WALKING)
							{
								if (_character->getDirection() == MovableGameObject::Direction::LEFT)
									_character->setState(MovableGameObject::State::JUMPINGLEFT);
								else if (_character->getDirection() == MovableGameObject::Direction::RIGHT)
									_character->setState(MovableGameObject::State::JUMPINGRIGHT);
							}
							else
								_character->setState(MovableGameObject::State::JUMPING);

							_character->setIsJumping(true);
							game.getEngine()->getPhysicsEngine()->doAction(_character, PhysicsEngine::Action::JUMP);
						}
					}
				}
			}
			return true;
		}
		
		Action* JumpAction::create(SDL_Event &event) {
			return new JumpAction(_character, event);
		}
	}
}
