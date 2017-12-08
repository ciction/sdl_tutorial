#ifndef RES_PATH_H
#define RES_PATH_H

#include <iostream>
#include <string>
#include <SDL.h>

/*
* Get the resource path for resources located in res/subDir
* It's assumed the project directory is structured like:
* bin/ || Debug/ ||Release/  (depending on the platform ) ==> eigen toevoeging
*  the executable
* res/
*  Lesson1/
*  Lesson2/
*
* Paths returned will be Lessons/res/subDir
*/

std::string getResourcePath(const std::string &subDir = "") {
	//We need to choose the path separator properly based on which
	//platform we're running on, since Windows uses a different
	//separator than most systems

#ifdef _WIN32
	const char PATH_SEP = '\\';

#if _DEBUG
	//Code that will not be executed in RELEASE or UAT
	static std::string binFolder = "Debug";
#else
	static std::string binFolder = "Release";
#endif
#else
	static std::string binFolder = "res";
	const char PATH_SEP = '/';
#endif // _WIN32
	//This will hold the base resource path: Lessons/res/
	//We give it static lifetime so that we'll only need to call
	//SDL_GetBasePath once to get the executable path
	static std::string baseRes;

	//lazy loading
	if (baseRes.empty()) {
		//SDL_GetBasePath will return NULL if something went wrong in retrieving the path
		char *basePath = SDL_GetBasePath();
		if (basePath) {
			baseRes = basePath;
			SDL_free(basePath);
		}
		else {
			std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
			return "";
		}
		//We replace the last bin/ with res/ to get the the resource path
		size_t pos = baseRes.rfind(binFolder);
		baseRes = baseRes.substr(0, pos) + "res" + PATH_SEP;
	}
	//If we want a specific subdirectory path in the resource directory
	//append it to the base path. This would be something like Lessons/res/Lesson0
	return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;


}

#endif