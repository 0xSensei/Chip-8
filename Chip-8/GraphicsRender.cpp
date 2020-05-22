#include "pch.h"
#include "GraphicsRender.h"
#include "stdio.h"

#pragma comment(lib,"SDL2.lib")


GraphicsRender::GraphicsRender()
{
}


GraphicsRender::~GraphicsRender()
{
	
}

void GraphicsRender::init()
{

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		window = SDL_CreateWindow("Aziz Chip-8",
			0, 0
		, 1600, 900, SDL_WINDOW_RESIZABLE);
		if (window) {
			// print created
			printf("SDL Window have been created :)\n");
		}
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (window) {
			printf("SDL Renderer have been created :)\n");

		}
	}
}

void GraphicsRender::draw()
{
}

void GraphicsRender::clear()
{
}
