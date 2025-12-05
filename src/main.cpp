#include <SDL3/SDL.h>

#include "chip8.h"

#define WINDOW_WIDHT 640
#define WINDOW_HEIHGT 320


int main(int argc, char **argv) {

	if(argc < 2) {
		std::cout << "Usage chip8.out <program name>\n";
		return 1;
	}
	
	if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) == false) {
		std::cout << "Error: " << SDL_GetError() << "\n";
		return 1;
	}

	SDL_Window* window;
	SDL_Renderer* renderer;

	window = SDL_CreateWindow("Chip 8 Emulator", WINDOW_WIDHT, WINDOW_HEIHGT, SDL_WINDOW_VULKAN);
	if(window == NULL) {
		std::cout << "Error: " << SDL_GetError() << "\n";
		return 1;
	}

	renderer = SDL_CreateRenderer(window, NULL);
	if(renderer == NULL) {
		std::cout << "Error: " << SDL_GetError() << "\n";
		return 1;
	}

	if( !SDL_SetRenderVSync(renderer, 1) ) {
		std::cout << "Unable to turn Vsync on\n";
	}

	chip8 Chip;

	Chip.initilalize();
	if( !Chip.load_game(argv[1]) ) return 1;

	bool running = true;

	while(running) {

		SDL_Event input;
		while(SDL_PollEvent(&input)) {
			if(input.type == SDL_EVENT_QUIT) {
				running = false;
			}

		}

		Chip.emulate_cycle();

		Chip.render(renderer);
	}


	SDL_DestroyRenderer(renderer);
	renderer = NULL;

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();


	return 0;
}

