//
//
//  @ Project : SDMG
//  @ File Name : InputDeviceBinding.cpp
//  @ Date : 26-9-2014
//  @ Author : 42IN13SAd
//
//


#include "InputDeviceBinding.h"

namespace sdmg {
	namespace engine {
		namespace input {
			InputDeviceBinding::InputDeviceBinding() {
				_keyBindings = new std::map<const int, Action*>;
			}

			InputDeviceBinding::~InputDeviceBinding() {

				clearBindings();

				if (_keyBindings != nullptr){
					delete _keyBindings;
					_keyBindings = nullptr;
				}
			}

			bool InputDeviceBinding::hasBinding(SDL_Event &event) {
				if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
					return _keyBindings->count(event.key.keysym.sym);
				}

				return false;
			}

			void InputDeviceBinding::setKeyBinding(const int keyCode, Action *action) {
				// check if binding exists
				if (_keyBindings->count(keyCode)) { delete (*_keyBindings)[keyCode]; (*_keyBindings)[keyCode] = action; } // replace
				else _keyBindings->insert(std::pair<const int, Action*>(keyCode, action)); // insert
			}
			
			void InputDeviceBinding::clearBindings() {

				if (_keyBindings != nullptr) {
					for (auto it : *_keyBindings)
					{
						delete it.second;
					}

					_keyBindings->clear();
				}
			}
			
			Action* InputDeviceBinding::createAction(SDL_Event &action) {
				// check if keycode exists, create and return the action
				if (_keyBindings->count(action.key.keysym.sym)) {
					return (*_keyBindings)[action.key.keysym.sym]->create(action);
				}
				return nullptr;
			}
		}
	}
}
