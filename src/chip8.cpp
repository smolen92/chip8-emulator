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

void chip8::emulate_cycle2() {
// Fetch opcode
	opcode = RAM[pc] << 8 | RAM[pc + 1];
	
	// Process opcode
	switch(opcode & 0xF000)
	{		
		case 0x0000:
			switch(opcode & 0x000F)
			{
				case 0x0000: // 0x00E0: Clears the screen
					for(int i = 0; i < 2048; ++i)
						screen[i] = 0x0;
					draw_flag = true;
					pc += 2;
				break;

				case 0x000E: // 0x00EE: Returns from subroutine
					--sp;			// 16 levels of stack, decrease stack pointer to prevent overwrite
					pc = stack[sp];	// Put the stored return address from the stack back into the program counter					
					pc += 2;		// Don't forget to increase the program counter!
				break;

				default:
					printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);					
			}
		break;

		case 0x1000: // 0x1NNN: Jumps to address NNN
			pc = opcode & 0x0FFF;
		break;

		case 0x2000: // 0x2NNN: Calls subroutine at NNN.
			stack[sp] = pc;			// Store current address in stack
			++sp;					// Increment stack pointer
			pc = opcode & 0x0FFF;	// Set the program counter to the address at NNN
		break;
		
		case 0x3000: // 0x3XNN: Skips the next instruction if VX equals NN
			if(v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
		break;
		
		case 0x4000: // 0x4XNN: Skips the next instruction if VX doesn't equal NN
			if(v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
		break;
		
		case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY.
			if(v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 4])
				pc += 4;
			else
				pc += 2;
		break;
		
		case 0x6000: // 0x6XNN: Sets VX to NN.
			v[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			pc += 2;
		break;
		
		case 0x7000: // 0x7XNN: Adds NN to VX.
			v[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			pc += 2;
		break;
		
		case 0x8000:
			switch(opcode & 0x000F)
			{
				case 0x0000: // 0x8XY0: Sets VX to the value of VY
					v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0001: // 0x8XY1: Sets VX to "VX OR VY"
					v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0002: // 0x8XY2: Sets VX to "VX AND VY"
					v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0003: // 0x8XY3: Sets VX to "VX XOR VY"
					v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't					
					if(v[(opcode & 0x00F0) >> 4] > (0xFF - v[(opcode & 0x0F00) >> 8])) 
						v[0xF] = 1; //carry
					else 
						v[0xF] = 0;					
					v[(opcode & 0x0F00) >> 8] += v[(opcode & 0x00F0) >> 4];
					pc += 2;					
				break;

				case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
					if(v[(opcode & 0x00F0) >> 4] > v[(opcode & 0x0F00) >> 8]) 
						v[0xF] = 0; // there is a borrow
					else 
						v[0xF] = 1;					
					v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
					pc += 2;
				break;

				case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
					v[0xF] = v[(opcode & 0x0F00) >> 8] & 0x1;
					v[(opcode & 0x0F00) >> 8] >>= 1;
					pc += 2;
				break;

				case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
					if(v[(opcode & 0x0F00) >> 8] > v[(opcode & 0x00F0) >> 4])	// VY-VX
						v[0xF] = 0; // there is a borrow
					else
						v[0xF] = 1;
					v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];				
					pc += 2;
				break;

				case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
					v[0xF] = v[(opcode & 0x0F00) >> 8] >> 7;
					v[(opcode & 0x0F00) >> 8] <<= 1;
					pc += 2;
				break;

				default:
					printf ("Unknown opcode [0x8000]: 0x%X\n", opcode);
			}
		break;
		
		case 0x9000: // 0x9XY0: Skips the next instruction if VX doesn't equal VY
			if(v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4])
				pc += 4;
			else
				pc += 2;
		break;

		case 0xA000: // ANNN: Sets I to the address NNN
			I = opcode & 0x0FFF;
			pc += 2;
		break;
		
		case 0xB000: // BNNN: Jumps to the address NNN plus V0
			pc = (opcode & 0x0FFF) + v[0];
		break;
		
		case 0xC000: // CXNN: Sets VX to a random number and NN
			v[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
			pc += 2;
		break;
	
		case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
					 // Each row of 8 pixels is read as bit-coded starting from memory location I; 
					 // I value doesn't change after the execution of this instruction. 
					 // VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 
					 // and to 0 if that doesn't happen
		{
			unsigned short x = v[(opcode & 0x0F00) >> 8];
			unsigned short y = v[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;

			v[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = RAM[I + yline];
				for(int xline = 0; xline < 8; xline++)
				{
					if((pixel & (0x80 >> xline)) != 0)
					{
						if(screen[(x + xline + ((y + yline) * 64))] == 1)
						{
							v[0xF] = 1;                                    
						}
						screen[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}
						
			draw_flag = true;			
			pc += 2;
		}
		break;
			
		case 0xE000:
			switch(opcode & 0x00FF)
			{
				case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed
					if(keypad[v[(opcode & 0x0F00) >> 8]] != 0)
						pc += 4;
					else
						pc += 2;
				break;
				
				case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
					if(keypad[v[(opcode & 0x0F00) >> 8]] == 0)
						pc += 4;
					else
						pc += 2;
				break;

				default:
					printf ("Unknown opcode [0xE000]: 0x%X\n", opcode);
			}
		break;
		
		case 0xF000:
			switch(opcode & 0x00FF)
			{
				case 0x0007: // FX07: Sets VX to the value of the delay timer
					v[(opcode & 0x0F00) >> 8] = delay_timer;
					pc += 2;
				break;
								
				case 0x000A: // FX0A: A key press is awaited, and then stored in VX		
				{
					bool keyPress = false;

					for(int i = 0; i < 16; ++i)
					{
						if(keypad[i] != 0)
						{
							v[(opcode & 0x0F00) >> 8] = i;
							keyPress = true;
						}
					}

					// If we didn't received a keypress, skip this cycle and try again.
					if(!keyPress)						
						return;

					pc += 2;					
				}
				break;
				
				case 0x0015: // FX15: Sets the delay timer to VX
					delay_timer = v[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;

				case 0x0018: // FX18: Sets the sound timer to VX
					sound_timer = v[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;

				case 0x001E: // FX1E: Adds VX to I
					if(I + v[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
						v[0xF] = 1;
					else
						v[0xF] = 0;
					I += v[(opcode & 0x0F00) >> 8];
					pc += 2;
				break;

				case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
					I = v[(opcode & 0x0F00) >> 8] * 0x5;
					pc += 2;
				break;

				case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX at the addresses I, I plus 1, and I plus 2
					RAM[I]     = v[(opcode & 0x0F00) >> 8] / 100;
					RAM[I + 1] = (v[(opcode & 0x0F00) >> 8] / 10) % 10;
					RAM[I + 2] = (v[(opcode & 0x0F00) >> 8] % 100) % 10;					
					pc += 2;
				break;

				case 0x0055: // FX55: Stores V0 to VX in memory starting at address I					
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
						RAM[I + i] = v[i];	

					// On the original interpreter, when the operation is done, I = I + X + 1.
					I += ((opcode & 0x0F00) >> 8) + 1;
					pc += 2;
				break;

				case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I					
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
						v[i] = RAM[I + i];			

					// On the original interpreter, when the operation is done, I = I + X + 1.
					I += ((opcode & 0x0F00) >> 8) + 1;
					pc += 2;
				break;

				default:
					printf ("Unknown opcode [0xF000]: 0x%X\n", opcode);
			}
		break;

		default:
			printf ("Unknown opcode: 0x%X\n", opcode);
	}	

	// Update timers
	if(delay_timer > 0)
		--delay_timer;

	if(sound_timer > 0)
	{
		if(sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;
	}
}


void chip8::render(SDL_Renderer* renderer) {
	if( !draw_flag) return;
	
	SDL_SetRenderDrawColor(renderer, 0,0,0, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	SDL_FRect temp;
	
	temp.h = 10; //WINDOW_WIDTH/SCREEN_WIDTH
	temp.w = 10;

	for(int i=0; i < SCREEN_SIZE; i++) {
		temp.x = (float)((int)i%64)*10;
		temp.y = (float)((int)i/64)*10;
	
		(screen[i] == 1) ? SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF) : SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
		
		SDL_RenderFillRect(renderer, &temp);
		
		//if(i%64 == 0) std::cout << "\n";
		//(screen[i] == 1) ? std::cout << "1" : std::cout << "0";
	}

	//std::cout << "\n";

	SDL_RenderPresent(renderer);
	
	draw_flag = false;
}
