// tutorial1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "SDL.h"
#include "res_path.h"



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
}


/**
* Loads a BMP image into a texture on the rendering device
* @param file The BMP image file to load
* @param ren The renderer to load the texture onto
* @return the loaded texture, or nullptr if something went wrong.
*/
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren) {
	//Initialize to nullptr to avoid dangling pointer issues
	SDL_Texture *texture = nullptr;
	//Load the image
	SDL_Surface *loadedImage = SDL_LoadBMP(file.c_str());
	//If the loading went ok, convert to texture and return the texture
	if (loadedImage != nullptr) {
		texture = SDL_CreateTextureFromSurface(ren, loadedImage);
		SDL_FreeSurface(loadedImage);
		//Make sure converting went ok too
		if (texture == nullptr) {
			logSDLError(std::cout, "CreateTextureFromSurface");
		}
	}
	else {
		logSDLError(std::cout, "LoadBMP");
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



int main(int argc, char* argv[])
{
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	//try to start SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError(std::cout, "SDL_Init");
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



	const std::string resPath = getResourcePath("Lesson2");
	SDL_Texture *background = loadTexture(resPath + "background.bmp", renderer);
	SDL_Texture *image = loadTexture(resPath + "image.bmp", renderer);
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
		SDL_RenderClear(renderer);

		int bW, bH;
		SDL_QueryTexture(background, NULL, NULL, &bW, &bH);
		renderTexture(background, renderer, 0, 0);
		renderTexture(background, renderer, bW, 0);
		renderTexture(background, renderer, 0, bH);
		renderTexture(background, renderer, bW, bH);


		int iW, iH;
		SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
		int x = SCREEN_WIDTH / 2 - iW / 2;
		int y = SCREEN_HEIGHT / 2 - iH / 2;
		renderTexture(image, renderer, x, y);

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

