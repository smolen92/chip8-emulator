#include <cstdint>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <fstream>

#define NEXT_INSTRUCTION(x) (x) += 2
#define SKIP_INSTRUCTION(x) (x) += 4

#define REGISTER_COUNT 16
#define RAM_SIZE 4096
#define SCREEN_SIZE 64*32
#define STACK_SIZE 16
#define PROGRAM_LOCATION 0x200

class chip8 {
	public:
		void initilalize();

		void load_game(const char* name);

		void emulate_cycle();

		bool draw_flag;

		unsigned char keypad[16];
	private:
		uint16_t opcode;
		uint8_t RAM[RAM_SIZE];
		uint8_t v[REGISTER_COUNT]; //16x 8-bit register
		uint16_t I, pc; //Index register, program counter

		uint8_t screen[SCREEN_SIZE];

		uint8_t delay_timer, sound_timer;

		uint16_t stack[STACK_SIZE];
		uint16_t sp; //stack pointer
			     //
		uint8_t chip8_fontset[80] = {
		  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		  0x20, 0x60, 0x20, 0x20, 0x70, // 1
		  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
		};

};

void chip8::initilalize() {
	pc = PROGRAM_LOCATION;
	opcode = 0;
	I = 0;
	sp = 0;

	for(int i=0; i < 80; i++) {
		RAM[i] = chip8_fontset[i];
	}
	
	std::memset(screen, 0, SCREEN_SIZE);

	srand(time(0));
}

void chip8::load_game(const char* name) {
	
	for(int i=0; i < bufferSize; i++) {
		RAM[i+PROGRAM_LOCATION] = buffer[i];
	}

}

void chip8::emulate_cycle() {
	
	opcode = RAM[pc] << 8 | RAM[pc+1];

	switch(opcode & 0xF000) {
		
		case 0x0000:
			switch(opcode & 0x000F) {
				case 0x0000:
					std::memset(screen, 0, SCREEN_SIZE);
					NEXT_INSTRUCTION(pc);
					break;
				
				case 0x000E:
					sp--;
					pc = stack[sp];
					NEXT_INSTRUCTION(pc); 
					break;

				default:
					std::cout << "Unknown opcode [0x0000]: " << opcode << "\n";
			}
			break;
		
		case 0x1000:
			pc = opcode & 0x0FFF;
			break;

		case 0x2000:
			stack[sp] = pc;
			sp++;
			pc = (opcode & 0x0FFF);
			break;
		case 0x3000: 
			( v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF) ) ? SKIP_INSTRUCTION(pc) : NEXT_INSTRUCTION(pc);
			break;

		case 0x4000: 
			( v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF) ) ? SKIP_INSTRUCTION(pc) : NEXT_INSTRUCTION(pc);
			break;

		case 0x5000: 
			( v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 4] ) ? SKIP_INSTRUCTION(pc) : NEXT_INSTRUCTION(pc);
			break;
			
		case 0x6000: 
			v[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			NEXT_INSTRUCTION(pc);
			break;

		case 0x7000:
			v[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			NEXT_INSTRUCTION(pc);
			break;

		case 0x8000:
			switch (opcode & 0x000F) {
				case 0x0000:
					v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
					NEXT_INSTRUCTION(pc);
					break;

				case 0x0001:
					v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
					NEXT_INSTRUCTION(pc);
					break;

				case 0x0002:
					v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
					NEXT_INSTRUCTION(pc);
					break;

				case 0x0003:
					v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
					NEXT_INSTRUCTION(pc);
					break;
				
				case 0x0004:
					( v[(opcode & 0x00F0) >> 4] > (0xFF - v[(opcode & 0x0F00) >> 8]) ) ? v[0xF] = 1 : v[0xF] = 0;
					v[(opcode & 0x0F00) >> 8] += v[(opcode & 0x00F0) >> 4];
					NEXT_INSTRUCTION(pc);
					break;

				case 0x0005:
					( v[(opcode & 0x0F00) >> 8] >= v[(opcode & 0x00F0) >> 4] ) ? v[0xF] = 1 : v[0xF] = 0;
					v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
					NEXT_INSTRUCTION(pc);
					break;

				case 0x0006:
					v[0xF] = v[(opcode & 0x0F00) >> 8] & 0x8;
					v[(opcode & 0x0F00) >> 8] >>= 1;
					NEXT_INSTRUCTION(pc);
					break;

				case 0x0007:
					( v[(opcode & 0x00F0) >> 4] >= v[(opcode & 0x0F00) >> 8] ) ? v[0xF] = 1 : v[0xF] = 0;
					v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];
					NEXT_INSTRUCTION(pc);
					break;

				case 0x00E:
					v[0xF] = v[(opcode & 0x0F00) >> 8] & 0x1;
					v[(opcode & 0x0F00) >> 8] <<= 1;
					NEXT_INSTRUCTION(pc);
					break;

				default:
					std::cout << "Unknown opcode [0x8000]: " << opcode << "\n";
			}

			break;

		case 0x9000:
			( v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4] ) ? SKIP_INSTRUCTION(pc) : NEXT_INSTRUCTION(pc);
			break;

		case 0xA000:
			I  = opcode & 0x0FFF;
			NEXT_INSTRUCTION(pc);
			break;

		case 0xB000:
			pc = v[0] + opcode & 0x0FFF;
			break;

		case 0xC000:
			v[(opcode & 0x0F00) >> 8] = (rand()%255) & (opcode & 0x00FF);
			NEXT_INSTRUCTION(pc);
			break;

		case 0xD000:
			{
				uint8_t x_pos = (opcode & 0x0F00) >> 8;
				uint8_t y_pos = (opcode & 0x00F0) >> 4;
				uint8_t sprite_height = opcode & 0x000F;
				uint8_t sprite_width = 8;

				v[0xF] = 0;

				for(int i=0; i < sprite_height; i++) {
					
					uint8_t pixel_row = RAM[I+i];


					for(int j=0; j < sprite_width; j++) {
						if( screen[x_pos+j + ((y_pos+i)*64) ] == 1 && (pixel_row & (0x80 >> j) == 0) ) v[0xF] = 1; 

						screen[x_pos+j + ((y_pos+i)*64) ] = pixel_row & (0x80 >> j); 
					}

				}
			}

			NEXT_INSTRUCTION(pc);
			break;

		case 0xE000:
			switch(opcode & 0x00FF) {
				case 0x009E:
					for(int i=0; i < 16; i++) {
						( keypad[i] == (v[(opcode & 0x0F00) >> 8] & 0x000F)) ? SKIP_INSTRUCTION(pc) : NEXT_INSTRUCTION(pc);
					}
					break;
				case 0x00A1:
					for(int i=0; i < 16; i++) {
						( keypad[i] != (v[(opcode & 0x0F00) >> 8] & 0x000F)) ? SKIP_INSTRUCTION(pc) : NEXT_INSTRUCTION(pc);
					}
					break;
				default:
					std::cout << "Unknown opcode [0xE000] " << opcode << "\n";
			}
			break;
		
		case 0xF000:
			switch(opcode & 0x00FF) {
				case 0x0007:
					v[(opcode & 0x0F00) >> 8] = delay_timer;
					NEXT_INSTRUCTION(pc);
					break;
				case 0x000A:
					for(int i=0; i < 16; i++) {
						if(keypad[i] == 1) {
							v[(opcode & 0x0F00) >> 8] = i;
							NEXT_INSTRUCTION(pc);
						}
					}
					break;
				case 0x0015:
					delay_timer = v[(opcode & 0x0F00) >> 8];
					NEXT_INSTRUCTION(pc);
					break;
				case 0x0018:
					sound_timer = v[(opcode & 0x0F00) >> 8];
					NEXT_INSTRUCTION(pc);
					break;
				case 0x001E:
					I += v[(opcode & 0x0F00) >> 8];
					NEXT_INSTRUCTION(pc);
					break;
				case 0x0029:
					I = v[(opcode & 0x0F00) >> 8] * 0x05;
					NEXT_INSTRUCTION(pc);
					break;
				case 0x0033:
					RAM[I]   = v[(opcode & 0xF00) >> 8] / 100;
					RAM[I+1] = (v[(opcode & 0xF00) >> 8] % 100)/10;
					RAM[I+2] = (v[(opcode & 0xF00) >> 8] % 100)%10;
					NEXT_INSTRUCTION(pc);
					break;
				case 0x0055:
					for(int v_index=0; v_index < REGISTER_COUNT; v_index++) {
						RAM[I+v_index] = v[v_index];
					}
					NEXT_INSTRUCTION(pc);
					break;
				case 0x0065:
					for(int v_index=0; v_index < REGISTER_COUNT; v_index++) {
						v[v_index] = RAM[I+v_index];  
					}
					NEXT_INSTRUCTION(pc);
					break;
				default:
					std::cout << "Unkwon opcode [0xF000]" << opcode << "\n";
				}
			break;

		default:
			std::cout << "Unknown opcode: " << opcode << "\n";
	}

	if(delay_timer > 0) delay_timer--;

	if(sound_timer > 0) {
		if(sound_timer == 1) {
			std::cout << "BEEP\n";
			sound_timer--;
		}
	}

}

int main(int argc, char **argv) {

	if(argc < 2) std::cout << "Usage chip8.out <program name>\n";

	chip8 Chip;

	Chip.initilalize();
	Chip.load_game(argv[1]);

	bool running = true;

	while(running) {

		Chip.emulate_cycle();

		if(Chip.draw_flag) {

		}

	}


	return 0;
}

