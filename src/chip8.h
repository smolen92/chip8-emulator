#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <cstdint>
#include <cstring> //memset
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <SDL3/SDL.h>

#define NEXT_INSTRUCTION(x) (x) += 2
#define SKIP_INSTRUCTION(x) (x) += 4

#define REGISTER_COUNT 16
#define RAM_SIZE 4096
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define STACK_SIZE 16
#define PROGRAM_LOCATION 0x200

struct quirks {
	bool vf_reset;
	bool memory;
	bool display_wait;
	bool clipping;
	bool shifting;
	bool jumping;
};

class chip8 {
	public:
		void initilalize();

		bool load_game(const char* name);

		void emulate_cycle();
		
		void emulate_timer();

		uint8_t keypad[16];

		void render(SDL_Renderer* renderer);
	private:
		uint16_t opcode;
		uint8_t RAM[RAM_SIZE];
		uint8_t v[REGISTER_COUNT]; //16x 8-bit register
		uint16_t I, pc; //Index register, program counter

		uint8_t screen[SCREEN_WIDTH][SCREEN_HEIGHT];

		uint8_t delay_timer, sound_timer;

		uint16_t stack[STACK_SIZE];
		uint16_t sp; //stack pointer

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

		quirks settings;

		bool draw_flag, vsync;

		uint64_t instruction_counter;

		uint64_t instruction_per_frame;
};

#endif

