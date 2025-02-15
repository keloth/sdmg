#pragma once
#include "MenuState.h"
#include "engine\GameState.h"

using namespace sdmg::engine;

namespace sdmg {

	namespace engine {
		class GameTime;
	}
	namespace helperclasses {
		class Menu;
		class MenuItem;
	}
	namespace gamestates {
		using namespace sdmg::helperclasses;
		class HelpState :
			public MenuState
		{
		public:
			void init(GameBase &game);
			void cleanup(GameBase &game);
			
			void handleEvents(GameBase &game, GameTime &gameTime);
			void update(GameBase &game, GameTime &gameTime);
			void draw(GameBase &game, GameTime &gameTime);

			static HelpState& getInstance() {
				static HelpState _instance;
				return _instance;
			}

		protected:
			HelpState() { }
		private:
			GameBase *_game = nullptr;

			void loadText(std::string key, std::string text, std::string fontName, int fontSize);

			std::vector<std::string> *_text = nullptr;

			/*std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
			std::vector<std::string> split(const std::string &s, char delim);*/
		};
	}
}