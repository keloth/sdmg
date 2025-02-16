//
//
//  @ Project : SDMG
//  @ File Name : DrawEngine.h
//  @ Date : 26-9-2014
//  @ Author : 42IN13SAd
//
//

#pragma once
#include <string>
#include <map>
#include <SDL.h>
#include <SDL_ttf.h>
#include "Rectangle.h"
#include <vector>
#include "engine\GameObject.h"
#include "engine\MovableGameObject.h"
#include "Surface.h"
#include <chrono>
#include <array>
#include "engine\input\Mouse.h"

class b2Body;

namespace sdmg {
	namespace engine {
		class Engine;

		namespace drawing {
			class Surface;
			class TextSurface;
			class DynamicTextSurface;

			class DrawEngine {

			public:
				DrawEngine();
				virtual ~DrawEngine();
				void load(std::string key, std::string path);
				void load(GameObject *gameObject, std::string path);
				void load(GameObject *gameObject, SDL_Surface *srcSurface);
				void loadText(std::string key, std::string text, SDL_Color fgColor, std::string fontName, int fontSize);
				void loadDynamicText(std::string key, SDL_Color fgColor, std::string fontName, int fontSize);
				void loadMap(std::string key, std::string path, int sliceWidth, int sliceHeight);
				void loadMap(GameObject *gameObject, std::string path, int sliceWidth, int sliceHeight);
				void loadMap(MovableGameObject *gameObject, MovableGameObject::State state, std::string path, int sliceWidth, int sliceHeight);
				void loadMap(MovableGameObject *gameObject, MovableGameObject::State state, std::string path, int sliceWidth, int sliceHeight, float scale);
				void loadMap(MovableGameObject *gameObject, MovableGameObject::State state, std::string path, int sliceWidth, int sliceHeight, float scale, Surface::AnimationType animationType);
				void loadMap(MovableGameObject *gameObject, MovableGameObject::State state, std::string path, int sliceWidth, int sliceHeight, int renderWidth, int renderHeight);
				void loadMap(MovableGameObject *gameObject, MovableGameObject::State state, std::string path, int sliceWidth, int sliceHeight, int renderWidth, int renderHeight, Surface::AnimationType animationType);
				void copyMap(MovableGameObject *gameObject, MovableGameObject::State copyFrom, MovableGameObject::State copyTo);
				void copyMap(std::string str, MovableGameObject *gameObject);
				void unload(std::string key);
				void unloadCopy(MovableGameObject *obj);
				void unloadText(std::string key);
				void unloadAll();
				void clear();
				
				void draw(std::string key);
				void draw(std::string key, int x, int y);
				void draw(GameObject *gameObject);
				void draw(MovableGameObject *gameObject);
				void draw(std::string key, int x, int y, int slice);
				//void draw(GameObject *gameObject, GameObject::State state, GameObject::Direction direction, float x, float y, int slice);
				void drawText(std::string key, int x, int y);
				void drawDynamicText(std::string key, std::string text, int x, int y);
				const std::array<int, 2> DrawEngine::getTextSize(std::string key);
				const std::array<int, 2> DrawEngine::getImageSize(std::string key);
				void draw(MovableGameObject *gameObject, int slice);
				void draw(MovableGameObject *gameObject, MovableGameObject::State state, MovableGameObject::Direction direction, int slice);
				void destroyText(std::string key);
				void destroyDynamicText(std::string key);
				void drawBodies(b2Body *body);
				void drawHitBoxes(std::vector<input::Mouse::Hitbox*> &boxes);
				void drawRectangle(Rectangle rect, const Uint8 r, const Uint8 g, const Uint8 b);
				void drawRectangle(Rectangle rect, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a);
				void drawParticle(SDL_Surface *surface, int x, int y);
				void refreshSurface(SDL_Surface *surface);
				void prepareForDraw();
				void render();
				void calcXY(GameObject *gameObject, Surface *surface, int &x, int &y);
				void update();
				void resetStep(GameObject *gameObject);
				void gameObjectStateChanged(MovableGameObject *gameObject);
				void createStep(GameObject *gameObject);
				void createStep(MovableGameObject *gameObject);
				void setSpeed(float speed);
				float getSpeed();
				int getWindowHeight();
				int getWindowWidth();
				int getWindowId() { return SDL_GetWindowID(_window); }
				std::string getImagePath(GameObject *gameObject) { return _objectSurfaces[gameObject]->getPath(); }
				int getDynamicTextWidth(std::string key);
				void saveScreenshot(std::string path);
				void setFullscreen(bool fullscreen);

			private:
				Engine *_engine = nullptr;
				SDL_Window *_window = nullptr;
				SDL_Renderer *_renderer = nullptr;
				int _curRenderer;
				
				std::map<std::string, Surface*> _surfaces;
				std::map<std::string, TextSurface*> _textSurfaces;
				std::map<GameObject*, Surface*> _objectSurfaces;
				std::map<MovableGameObject*, std::map<MovableGameObject::State, Surface*>*> _objectStateSurfaces;
				std::map<std::string, DynamicTextSurface*> _dynTextSurfaces;
				
				void initialize();
				int _windowHeight;
				int _windowWidth;
				std::map<GameObject*, int> _steps;

				std::chrono::high_resolution_clock::time_point _lastUpdate;
				float _step, _accumulator = 0, _speed;
				const float _standardspeed = 500.0f;
				bool _preparing;

				void drawSlice(GameObject *gameObject);
				void drawSlice(MovableGameObject *gameObject, MovableGameObject::State state, MovableGameObject::Direction direction);
			};
		}
	}
}