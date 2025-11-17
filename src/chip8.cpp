#include "chip8.h"

void chip8::initilalize() {
	pc = PROGRAM_LOCATION;
	opcode = 0;
	I = 0;
	sp = 0;

	for(int i=0; i < 80; i++) {
		RAM[i] = chip8_fontset[i];
	}
	
	std::memset(screen, 0, SCREEN_SIZE);

	std::memset(keypad, 0, 16);

	draw_flag = true;

	srand(time(0));
}

bool chip8::load_game(const char* name) {
	
	std::fstream file;

	file.open(name, std::ios::binary|std::ios::in);

	if(!file.is_open()) {
		std::cout << "Cannot open file\n";
		return false;
	}

	file.seekg(0,std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0);

	if(size > RAM_SIZE-PROGRAM_LOCATION) {
		std::cout << "File too large\n";
		return false;
	}

	for(int i=PROGRAM_LOCATION; i < RAM_SIZE; i++) {
		char c;
		file.get(c);
		RAM[i] = c;
	}
	
	return true;
}

void chip8::emulate_cycle() {
	
	opcode = RAM[pc] << 8 | RAM[pc+1];
	
	//std::cout << std::dec << pc << " " << std::showbase << std::hex << opcode << std::endl;

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
					std::cout << "Unknown opcode [0x0000]: " << std::showbase << std::hex << opcode << "\n";
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
					v[0xF] = v[(opcode & 0x0F00) >> 8] & 0x1;
					v[(opcode & 0x0F00) >> 8] >>= 1;
					NEXT_INSTRUCTION(pc);
					break;

				case 0x0007:
					( v[(opcode & 0x00F0) >> 4] >= v[(opcode & 0x0F00) >> 8] ) ? v[0xF] = 1 : v[0xF] = 0;
					v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];
					NEXT_INSTRUCTION(pc);
					break;

				case 0x00E:
					v[0xF] = v[(opcode & 0x0F00) >> 8] & 0x7;
					v[(opcode & 0x0F00) >> 8] <<= 1;
					NEXT_INSTRUCTION(pc);
					break;

				default:
					std::cout << "Unknown opcode [0x8000]: " << std::showbase << std::hex << opcode << "\n";
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
			pc = v[0] + (opcode & 0x0FFF);
			break;

		case 0xC000:
			v[(opcode & 0x0F00) >> 8] = (rand()%255) & (opcode & 0x00FF);
			NEXT_INSTRUCTION(pc);
			break;

		case 0xD000: //check
			{
				draw_flag = true;
				uint8_t x_pos = (opcode & 0x0F00) >> 8;
				uint8_t y_pos = (opcode & 0x00F0) >> 4;
				uint8_t sprite_height = opcode & 0x000F;
				uint8_t sprite_width = 8;

				v[0xF] = 0;

				for(int i=0; i < sprite_height; i++) {
					
					uint8_t pixel_row = RAM[I+i];

					for(int j=0; j < sprite_width; j++) {
						if( (screen[x_pos+j + ((y_pos+i)*64) ] == 1) && ( (pixel_row & (0x80 >> j)) == 0) ) v[0xF] = 1; 

						screen[x_pos+j + ((y_pos+i)*64) ] = pixel_row & (0x80 >> j); 
					}

				}
			}

			NEXT_INSTRUCTION(pc);
			break;

		case 0xE000:
			switch(opcode & 0x00FF) {
				case 0x009E: //check
					for(int i=0; i < 16; i++) {
						( keypad[i] == (v[(opcode & 0x0F00) >> 8] & 0x000F)) ? SKIP_INSTRUCTION(pc) : NEXT_INSTRUCTION(pc);
					}
					break;
				case 0x00A1: //check
					for(int i=0; i < 16; i++) {
						( keypad[i] != (v[(opcode & 0x0F00) >> 8] & 0x000F)) ? SKIP_INSTRUCTION(pc) : NEXT_INSTRUCTION(pc);
					}
					break;
				default:
					std::cout << "Unknown opcode [0xE000] " << std::showbase << std::hex << opcode << "\n";
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
					for(int v_index=0; v_index <= ( (opcode & 0x0F00) >> 8); v_index++) {
						RAM[I+v_index] = v[v_index];
					}
					NEXT_INSTRUCTION(pc);
					break;
				case 0x0065: 
					for(int v_index=0; v_index <= ( (opcode & 0x0F00) >> 8); v_index++) {
						v[v_index] = RAM[I+v_index];  
					}
					NEXT_INSTRUCTION(pc);
					break;
				default:
					std::cout << "Unkwon opcode [0xF000]: " << std::showbase << std::hex << opcode << "\n";
				}
			break;

		default:
			std::cout << "Unknown opcode: " << std::showbase << std::hex << opcode << "\n";
	}

	if(delay_timer > 0) delay_timer--;

	if(sound_timer > 0) {
		if(sound_timer == 1) {
			std::cout << "BEEP\n";
			sound_timer--;
		}
	}

}

void chip8::render(SDL_Renderer* renderer) {
	if( !draw_flag) return;
	
	SDL_SetRenderDrawColor(renderer, 0,0,0, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	SDL_FRect temp;
	
	temp.h = 10; //WINDOW_WIDTH/SCREEN_WIDTH
	temp.y = 10;

	for(int i=0; i < SCREEN_SIZE; i++) {
		temp.x = (float)((int)i%64)*10;
		temp.y = (float)((int)i/64)*10;
	
		(screen[i] == 1) ? SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF) : SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

		SDL_RenderFillRect(renderer, &temp);
		
		//if(i%64 == 0) std::cout << "\n";
		//std::cout << screen[i] << " ";
	}

	//std::cout << "\n";

	SDL_RenderPresent(renderer);
	
	//draw_flag = false;
}
