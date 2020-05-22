#pragma once
#include "SDL.h"

class GraphicsRender
{
public:
	GraphicsRender();
	~GraphicsRender();


	void init();
	void draw();
	void clear();

private:
	SDL_Window * window;
	SDL_Renderer * renderer;

};

