#pragma once
#include <string>
#include "engine\drawing\Rectangle.h"

namespace sdmg {
	namespace engine {
		namespace drawing {
			class DrawEngine;
		}
	}
	namespace model{
		class Character;
	}
	namespace helperclasses {
		using namespace engine::drawing;
		using namespace model;

		class HUD
		{
		public:
			HUD(Character &character, const int x) : _character(&character), _isInitialized(false), _rectangle(x, 10, 230, 75) {}
			virtual ~HUD();
			void draw(DrawEngine &drawEngine);
			void changeOwner(Character *owner);
		private:
			void init(DrawEngine &drawEngine);
			Character *_character = nullptr;
			DrawEngine *_drawengine = nullptr;
			bool _isInitialized;
			std::string _spriteKeyPrefix;
			Rectangle _rectangle;
		};

	}
}