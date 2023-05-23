#include <SDL2/SDL.h>
#include <stdio.h>
#include "cpu.hpp"
#include <string>
#include <iostream>

std::string TEST_FILEPATH = "/Users/alexnoh/repos/Chip-8/ROM/2-ibm-logo.ch8";
uint8_t keymap[16] = {
	SDLK_x,
	SDLK_1,
	SDLK_2,
	SDLK_3,
	SDLK_q,
	SDLK_w,
	SDLK_e,
	SDLK_a,
	SDLK_s,
	SDLK_d,
	SDLK_z,
	SDLK_c,
	SDLK_4,
	SDLK_r,
	SDLK_f,
	SDLK_v,
};

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
		//input handling
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				is_running = false;
			}
			if (event.type == SDL_KEYDOWN)
			{
				//quit if ESC down
				if (event.key.keysym.sym == SDL_SCANCODE_ESCAPE)
				{
					is_running = false;
				}
				//if keydown input matches matching SDL key map, set matching keypad to 1
				for(int i=0; i < sizeof(chip8.keypad); i++)
				{
					if (event.key.keysym.sym == keymap[i])
					{
						chip8.keypad[i] = 1;
					}
				}
			}
			if (event.type == SDL_KEYUP)
			{
				//if key up input matches matching SDL key map, set matching keypad to 0
				for(int i=0; i < sizeof(chip8.keypad); i++)
				{
					if (event.key.keysym.sym == keymap[i])
					{
						chip8.keypad[i] = 0;
					}
				}
			}
		}
		SDL_Delay(16);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		chip8.cycle(&chip8);
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
