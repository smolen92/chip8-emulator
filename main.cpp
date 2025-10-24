#include "chip8.h"

int main(int argc, char **argv) {

	if(argc < 2) std::cout << "Usage chip8.out <program name>\n";

	chip8 Chip;

	Chip.initilalize();
	Chip.load_game(argv[1]);

	bool running = true;

	while(running) {

		Chip.emulate_cycle();

	}

	return 0;
}

