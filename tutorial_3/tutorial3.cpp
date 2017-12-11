// tutorial3.cpp : Defines the entry point for the console application.
// http://www.willusher.io/sdl2%20tutorials/2013/08/18/lesson-3-sdl-extension-libraries

#include "stdafx.h"
#include <iostream>
#include "SDL.h"
#include "res_path.h"
#include "SDL_image.h"


//eigen implementatie van cleanup
template <class T>
void cleanup(T *t) { }  // in general do nothing 

template <>
void cleanup(SDL_Texture *t) { SDL_DestroyTexture(t); }

template <>
void cleanup(SDL_Renderer *t) { SDL_DestroyRenderer(t); }

template <>
void cleanup(SDL_Window *t) { SDL_DestroyWindow(t); }


/**
* Log an SDL error with some error message to the output stream of our choice
* @param os The output stream to write the message to
* @param msg The error message to write, format will be msg error: SDL_GetError()
*/
void logSDLError(std::ostream &os, const std::string &msg) {
	os << msg << " error: " << SDL_GetError() << std::endl;
	std::cin.get();
}


/**
* Loads a BMP image into a texture on the rendering device
* @param file The BMP image file to load
* @param ren The renderer to load the texture onto
* @return the loaded texture, or nullptr if something went wrong.
*/
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren) {
	SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
	if (texture == nullptr) {
		logSDLError(std::cout, "LoadTexture");
	}
	return texture;
}
/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param tex The source texture we want to draw
* @param ren The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	//Query the texture to get its width and height to use
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
* width and height
* @param tex The source texture we want to draw
* @param ren The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
* @param w The width of the texture to draw
* @param h The height of the texture to draw
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h) {
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
}





int main(int argc, char* argv[])
{
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;
	const int TILE_SIZE = 40;

	//try to start SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}

	//already init PNG subsystem (so the first PNG images will load faster )
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		logSDLError(std::cout, "IMG_Init");
		SDL_Quit();
		return 1;
	}

	//try opening a window
	SDL_Window *window = SDL_CreateWindow("Lesson 2", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		logSDLError(std::cout, "SDL_CreateWindow");
		SDL_Quit();
		return 1;
	}

	//try creating the renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		logSDLError(std::cout, "CreateRenderer");
		cleanup(window);
		SDL_Quit();
		return 1;
	}



	const std::string resPath = getResourcePath("Lesson3");
	SDL_Texture *background = loadTexture(resPath + "background.png", renderer);
	SDL_Texture *image = loadTexture(resPath + "image.png", renderer);
	if (background == nullptr || image == nullptr) {
		cleanup(background);
		cleanup(renderer);
		cleanup(window);
		cleanup(image);
		SDL_Quit();
		return 1;
	}


	//render loop
	//A sleepy rendering loop, wait for 3 seconds and render and present the screen each time

	for (int i = 0; i < 3; ++i) {
		//clear screen
		SDL_RenderClear(renderer);
		//render background

		//Determine how many tiles we'll need to fill the screen
		int xTiles = SCREEN_WIDTH / TILE_SIZE;
		int yTiles = SCREEN_HEIGHT / TILE_SIZE;

		//Draw the tiles by calculating their positions
		for (int i = 0; i < xTiles * yTiles; ++i) {
			int x = i % xTiles;
			int y = i / xTiles;
			renderTexture(background, renderer, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE,
				TILE_SIZE);
		}
	

		//render image
		int iW, iH;
		SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
		int x = SCREEN_WIDTH / 2 - iW / 2;
		int y = SCREEN_HEIGHT / 2 - iH / 2;
		renderTexture(image, renderer, x, y);

		//present 
		SDL_RenderPresent(renderer);
		SDL_Delay(1000);
	}

	std::cout << "Resource path is: " << getResourcePath() << std::endl;


	//cleanup
	cleanup(background);
	cleanup(renderer);
	cleanup(window);
	cleanup(image);
	SDL_Quit();

	//std::cin.get();
	SDL_Quit();
	return 0;


}


