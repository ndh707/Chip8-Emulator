#include <SDL2/SDL.h>
#include <stdio.h>
#include "cpu.hpp"
#include <string>
#include <iostream>

std::string TEST_FILEPATH = "/Users/alexnoh/repos/Chip-8/ROM/3-corax+.ch8";

//int argc, char* argv[]
int main()
{
	
	std::cout << "Started Main \n";
	
	cpu chip8;
	std::cout << "Instantiated cpu \n";
	
	chip8.loadROM(TEST_FILEPATH);
	
	SDL_Init(SDL_INIT_EVERYTHING);              // Initialize SDL2

	// Create an application window with the following settings:
	SDL_Window *window = SDL_CreateWindow
	("SDL2 Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
 
	// A basic main loop to prevent blocking
	bool is_running = true;
	SDL_Event event;
	while (is_running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				is_running = false;
			}
		}
		SDL_Delay(16);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		chip8.cycle();
		SDL_UpdateTexture(texture, NULL, chip8.video, 64 * sizeof(Uint32));
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
 
	}
 
	SDL_DestroyRenderer(renderer);
	// Close and destroy the window
	SDL_DestroyWindow(window);

	// Clean up
	SDL_Quit();

	return 0;
}
