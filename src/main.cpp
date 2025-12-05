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
			
			if(input.key.type == SDL_EVENT_KEY_DOWN) {
				if (input.key.scancode == SDL_SCANCODE_1) Chip.keypad[0] = 1;
				if (input.key.scancode == SDL_SCANCODE_2) Chip.keypad[1] = 1;
				if (input.key.scancode == SDL_SCANCODE_3) Chip.keypad[2] = 1;
				if (input.key.scancode == SDL_SCANCODE_Q) Chip.keypad[3] = 1; 
				if (input.key.scancode == SDL_SCANCODE_W) Chip.keypad[4] = 1;
				if (input.key.scancode == SDL_SCANCODE_E) Chip.keypad[5] = 1;
				if (input.key.scancode == SDL_SCANCODE_A) Chip.keypad[6] = 1;
				if (input.key.scancode == SDL_SCANCODE_S) Chip.keypad[7] = 1;
				if (input.key.scancode == SDL_SCANCODE_D) Chip.keypad[8] = 1;
				if (input.key.scancode == SDL_SCANCODE_X) Chip.keypad[9] = 1;
				if (input.key.scancode == SDL_SCANCODE_Z) Chip.keypad[10] = 1; 
				if (input.key.scancode == SDL_SCANCODE_C) Chip.keypad[11] = 1;
				if (input.key.scancode == SDL_SCANCODE_4) Chip.keypad[12] = 1; 
				if (input.key.scancode == SDL_SCANCODE_R) Chip.keypad[13] = 1;
				if (input.key.scancode == SDL_SCANCODE_F) Chip.keypad[14] = 1;
				if (input.key.scancode == SDL_SCANCODE_V) Chip.keypad[15] = 1;
			}

			if(input.key.type == SDL_EVENT_KEY_UP) {
				if (input.key.scancode == SDL_SCANCODE_1) Chip.keypad[0] = 0;
				if (input.key.scancode == SDL_SCANCODE_2) Chip.keypad[1] = 0;
				if (input.key.scancode == SDL_SCANCODE_3) Chip.keypad[2] = 0;
				if (input.key.scancode == SDL_SCANCODE_Q) Chip.keypad[3] = 0; 
				if (input.key.scancode == SDL_SCANCODE_W) Chip.keypad[4] = 0;
				if (input.key.scancode == SDL_SCANCODE_E) Chip.keypad[5] = 0;
				if (input.key.scancode == SDL_SCANCODE_A) Chip.keypad[6] = 0;
				if (input.key.scancode == SDL_SCANCODE_S) Chip.keypad[7] = 0;
				if (input.key.scancode == SDL_SCANCODE_D) Chip.keypad[8] = 0;
				if (input.key.scancode == SDL_SCANCODE_X) Chip.keypad[9] = 0;
				if (input.key.scancode == SDL_SCANCODE_Z) Chip.keypad[10] = 0; 
				if (input.key.scancode == SDL_SCANCODE_C) Chip.keypad[11] = 0;
				if (input.key.scancode == SDL_SCANCODE_4) Chip.keypad[12] = 0; 
				if (input.key.scancode == SDL_SCANCODE_R) Chip.keypad[13] = 0;
				if (input.key.scancode == SDL_SCANCODE_F) Chip.keypad[14] = 0;
				if (input.key.scancode == SDL_SCANCODE_V) Chip.keypad[15] = 0;
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

