//Lesson 6: True Type Fonts with SDL_ttf
//http://www.willusher.io/sdl2%20tutorials/2013/12/18/lesson-6-true-type-fonts-with-sdl_ttf

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
/**
* Draw an SDL_Texture to an SDL_Renderer at some destination rect
* taking a clip of the texture if desired
* @param tex The source texture we want to draw
* @param ren The renderer we want to draw to
* @param dst The destination rectangle to render the texture to
* @param clip The sub-section of the texture to draw (clipping rect)
*		default of nullptr draws the entire texture
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, SDL_Rect dst,
	SDL_Rect *clip = nullptr)
{
	SDL_RenderCopy(ren, tex, clip, &dst);
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height and taking a clip of the texture if desired
* If a clip is passed, the clip's width and height will be used instead of
*	the texture's
* @param tex The source texture we want to draw
* @param ren The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
* @param clip The sub-section of the texture to draw (clipping rect)
*		default of nullptr draws the entire texture
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y,
	SDL_Rect *clip = nullptr)
{
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	if (clip != nullptr) {
		dst.w = clip->w;
		dst.h = clip->h;
	}
	else {
		SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	}
	renderTexture(tex, ren, dst, clip);
}

/**
* Render the message we want to display to a texture for drawing
* @param message The message we want to display
* @param fontFile The font we want to use to render the text
* @param color The color we want the text to be
* @param fontSize The size we want the font to be
* @param renderer The renderer to load the texture in
* @return An SDL_Texture containing the rendered message, or nullptr if something went wrong
*/
SDL_Texture* renderText(const std::string &message, const std::string &fontFile,
	SDL_Color color, int fontSize, SDL_Renderer *renderer)
{
	//Open the font
	TTF_Font *font = TTF_OpenFont(fontFile.c_str(), fontSize);
	if (font == nullptr) {
		logSDLError(std::cout, "TTF_OpenFont");
		return nullptr;
	}
	//We need to first render to a surface as that's what TTF_RenderText
	//returns, then load that surface into a texture
	SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), color);
	if (surf == nullptr) {
		TTF_CloseFont(font);
		logSDLError(std::cout, "TTF_RenderText");
		return nullptr;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surf);
	if (texture == nullptr) {
		logSDLError(std::cout, "CreateTexture");
	}
	//Clean up the surface and font
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
	return texture;
}


int main(int argc, char* argv[])
{

	//iW and iH are the clip width and height
	//We'll be drawing only clips so get a center position for the w/h of a clip
	int iW = 100, iH = 100;
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;
	const int TILE_SIZE = 40;

	//Setup the clips for our image
	SDL_Rect clips[4];
	for (int i = 0; i < 4; ++i) {
		clips[i].x = i / 2 * iW;
		clips[i].y = i % 2 * iH;
		clips[i].w = iW;
		clips[i].h = iH;
	}
	//Specify a default clip to start with
	int useClip = 0;

	//try to start SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		logSDLError(std::cout, "SDL_Init");
		return 1;
	}

	//init PNG subsystem (so the first PNG images will load faster )
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		logSDLError(std::cout, "IMG_Init");
		SDL_Quit();
		return 1;
	}

	//try to init ttf font lib
	if (TTF_Init() != 0) {
		logSDLError(std::cout, "TTF_Init");
		SDL_Quit();
		return 1;
	}

	//try opening a window
	SDL_Window *window = SDL_CreateWindow("	n 4", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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



	const std::string resPath = getResourcePath("Lesson6");
	//We'll render the string "TTF fonts are cool!" in white
	//Color is in RGBA format
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Texture *image = renderText("TTF fonts are cool!", resPath + "sample.ttf",
		color, 64, renderer);
	if (image == nullptr) {
		cleanup(renderer);
		cleanup(window);
		TTF_Quit();
		SDL_Quit();
		return 1;
	}
	//Get the texture w/h so we can center it in the screen
	int iW, iH;
	SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
	int x = SCREEN_WIDTH / 2 - iW / 2;
	int y = SCREEN_HEIGHT / 2 - iH / 2;


	//real render loop
	//Our event structure
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {

			switch (e.key.keysym.sym) {
			case SDLK_1:
			case SDLK_KP_1:
				useClip = 0;
				break;
			case SDLK_2:
			case SDLK_KP_2:
				useClip = 1;
				break;
			case SDLK_3:
			case SDLK_KP_3:
				useClip = 2;
				break;
			case SDLK_4:
			case SDLK_KP_4:
				useClip = 3;
				break;
			case SDLK_ESCAPE:
				quit = true;
				break;
			default:
				break;
			}
		}

		//Render the scene
		//clear screen
		SDL_RenderClear(renderer);

	
		//Note: This is within the program's main loop
		SDL_RenderClear(renderer);
		//We can draw our message as we do any other texture, since it's been
		//rendered to a texture
		renderTexture(image, renderer, x, y);
		SDL_RenderPresent(renderer);


		//present
		SDL_RenderPresent(renderer);
	}

	std::cout << "Resource path is: " << getResourcePath() << std::endl;


	//cleanup

	cleanup(renderer);
	cleanup(window);
	cleanup(image);
	SDL_Quit();

	//std::cin.get();
	SDL_Quit();
	return 0;


}


