//
//
//  @ Project : SDMG
//  @ File Name : Joystick.h
//  @ Date : 30-9-2014
//  @ Author : 42IN13SAd
//
//

#pragma once
#include <SDL.h>
#include <SDL_thread.h>
#include <string>
#include <map>
#include <vector>

namespace sdmg {
	namespace engine {
		class Engine;

		namespace input {
			class Joystick {
			public:
				Joystick(int id, SDL_GameController *pad, std::string name = "");
				~Joystick();

				int getID();
				int getSDLID();
				std::string getName() const;
				SDL_GameController *getController();

			private:
				int _id;
				std::string _name;
				SDL_GameController *_pad = nullptr;
				void initialize();
			};
		}
	}
}