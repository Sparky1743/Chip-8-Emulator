#include <fstream>
#include <cstdint>
#include <iostream>
#include <SDL2/SDL.h> // Used for graphics
#include <cstring>
#include <chrono>
#include <stdio.h>
#include <random>

const unsigned int FONTSET_SIZE = 80;
const unsigned int START_ADDRESS = 0x200; // First instruction address
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int KEY_COUNT = 16;
const unsigned int MEMORY_SIZE = 4096;
const unsigned int REGISTER_COUNT = 16;
const unsigned int STACK_LEVELS = 16;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

uint8_t fontset[FONTSET_SIZE] = // Fonts to be displayed. There are 16 characters at 5 bytes each, so we need an array of 80 bytes.
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, // represents 0
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

class Chip8
{
    public:
	Chip8();
    void LoadROM(char const *filename);
	void Cycle();

	uint8_t keypad[KEY_COUNT]{};
	uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};


	private:
	uint8_t fontset[FONTSET_SIZE];
    uint8_t regs[16]{};            // there are sixteen 8bit (1 byte) registers for storing 1byte in each reg.
    uint8_t memory[4096]{};        // 4kb of memory
    uint16_t Program_Counter{};    // 16 bit special register -- each pc element stores two instructions (two 8-bit instructions)
    uint16_t index{};              // a special register used to point locations in memory (max memory address 0xFFF - too big for 8bit register)
    uint16_t stack[16]{};          // used to hold pc's
    uint8_t stack_pointer{};       // keeps record of last inserted pc value in stack
    uint8_t delay_timer{}; 
    uint8_t sound_timer{};
    uint32_t display[64 * 32]{};   // uint32_t used for compatability with modern devices
    uint8_t keypad[4 * 4]{};       // used to take input
    uint16_t opcode{};             // operational codes -- 2 bytes long -- at a time, one op code is only processed

	// Instructions:
	
	// Do nothing
	void OP_NULL();

	// CLS
	void OP_00E0();

	// RET
	void OP_00EE();

	// JP address
	void OP_1nnn();

	// CALL address
	void OP_2nnn();

	// SE Vx, byte
	void OP_3xkk();

	// SNE Vx, byte
	void OP_4xkk();

	// SE Vx, Vy
	void OP_5xy0();

	// LD Vx, byte
	void OP_6xkk();

	// ADD Vx, byte
	void OP_7xkk();

	// LD Vx, Vy
	void OP_8xy0();

	// OR Vx, Vy
	void OP_8xy1();

	// AND Vx, Vy
	void OP_8xy2();

	// XOR Vx, Vy
	void OP_8xy3();

	// ADD Vx, Vy
	void OP_8xy4();

	// SUB Vx, Vy
	void OP_8xy5();

	// SHR Vx
	void OP_8xy6();

	// SUBN Vx, Vy
	void OP_8xy7();

	// SHL Vx
	void OP_8xyE();

	// SNE Vx, Vy
	void OP_9xy0();

	// LD I, address
	void OP_Annn();

	// JP V0, address
	void OP_Bnnn();

	// RND Vx, byte
	void OP_Cxkk();

	// DRW Vx, Vy, height
	void OP_Dxyn();

	// SKP Vx
	void OP_Ex9E();

	// SKNP Vx
	void OP_ExA1();

	// LD Vx, DT
	void OP_Fx07();

	// LD Vx, K
	void OP_Fx0A();

	// LD DT, Vx
	void OP_Fx15();

	// LD ST, Vx
	void OP_Fx18();

	// ADD I, Vx
	void OP_Fx1E();

	// LD F, Vx
	void OP_Fx29();

	// LD B, Vx
	void OP_Fx33();

	// LD [I], Vx
	void OP_Fx55();

	// LD Vx, [I]
	void OP_Fx65();
	
	std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randByte;

};

void Chip8::LoadROM(char const* filename)
{
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[START_ADDRESS + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}
}

void Chip8::OP_NULL()
{}

void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
	--stack_pointer;
	Program_Counter = stack[stack_pointer];
}

void Chip8::OP_1nnn()
{
	uint16_t address = opcode & 0x0FFFu;

	Program_Counter = address;
}

void Chip8::OP_2nnn()
{
	uint16_t address = opcode & 0x0FFFu;

	stack[stack_pointer] = Program_Counter;
	++stack_pointer;
	Program_Counter = address;
}

void Chip8::OP_3xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (regs[Vx] == byte)
	{
		Program_Counter += 2;
	}
}

void Chip8::OP_4xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (regs[Vx] != byte)
	{
		Program_Counter += 2;
	}
}

void Chip8::OP_5xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (regs[Vx] == regs[Vy])
	{
		Program_Counter += 2;
	}
}

void Chip8::OP_6xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	regs[Vx] = byte;
}

void Chip8::OP_7xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	regs[Vx] += byte;
}

void Chip8::OP_8xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	regs[Vx] = regs[Vy];
}

void Chip8::OP_8xy1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	regs[Vx] |= regs[Vy];
}

void Chip8::OP_8xy2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	regs[Vx] &= regs[Vy];
}

void Chip8::OP_8xy3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	regs[Vx] ^= regs[Vy];
}

void Chip8::OP_8xy4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	uint16_t sum = regs[Vx] + regs[Vy];

	if (sum > 255U)
	{
		regs[0xF] = 1;
	}
	else
	{
		regs[0xF] = 0;
	}

	regs[Vx] = sum & 0xFFu;
}

void Chip8::OP_8xy5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (regs[Vx] > regs[Vy])
	{
		regs[0xF] = 1;
	}
	else
	{
		regs[0xF] = 0;
	}

	regs[Vx] -= regs[Vy];
}

void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save LSB in VF
	regs[0xF] = (regs[Vx] & 0x1u);

	regs[Vx] >>= 1;
}

void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (regs[Vy] > regs[Vx])
	{
		regs[0xF] = 1;
	}
	else
	{
		regs[0xF] = 0;
	}

	regs[Vx] = regs[Vy] - regs[Vx];
}

void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	// Save MSB in VF
	regs[0xF] = (regs[Vx] & 0x80u) >> 7u;

	regs[Vx] <<= 1;
}

void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (regs[Vx] != regs[Vy])
	{
		Program_Counter += 2;
	}
}

void Chip8::OP_Annn()
{
	uint16_t address = opcode & 0x0FFFu;

	index = address;
}

void Chip8::OP_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;

	Program_Counter = regs[0] + address;
}

void Chip8::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	regs[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = regs[Vx] % VIDEO_WIDTH;
	uint8_t yPos = regs[Vy] % VIDEO_HEIGHT;

	regs[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					regs[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = regs[Vx];

	if (keypad[key])
	{
		Program_Counter += 2;
	}
}

void Chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = regs[Vx];

	if (!keypad[key])
	{
		Program_Counter += 2;
	}
}

void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	regs[Vx] = delay_timer;
}

void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0])
	{
		regs[Vx] = 0;
	}
	else if (keypad[1])
	{
		regs[Vx] = 1;
	}
	else if (keypad[2])
	{
		regs[Vx] = 2;
	}
	else if (keypad[3])
	{
		regs[Vx] = 3;
	}
	else if (keypad[4])
	{
		regs[Vx] = 4;
	}
	else if (keypad[5])
	{
		regs[Vx] = 5;
	}
	else if (keypad[6])
	{
		regs[Vx] = 6;
	}
	else if (keypad[7])
	{
		regs[Vx] = 7;
	}
	else if (keypad[8])
	{
		regs[Vx] = 8;
	}
	else if (keypad[9])
	{
		regs[Vx] = 9;
	}
	else if (keypad[10])
	{
		regs[Vx] = 10;
	}
	else if (keypad[11])
	{
		regs[Vx] = 11;
	}
	else if (keypad[12])
	{
		regs[Vx] = 12;
	}
	else if (keypad[13])
	{
		regs[Vx] = 13;
	}
	else if (keypad[14])
	{
		regs[Vx] = 14;
	}
	else if (keypad[15])
	{
		regs[Vx] = 15;
	}
	else
	{
		Program_Counter -= 2;
	}
}

void Chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	delay_timer = regs[Vx];
}

void Chip8::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	sound_timer = regs[Vx];
}

void Chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	index += regs[Vx];
}

void Chip8::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = regs[Vx];

	index = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = regs[Vx];

	// Ones-place
	memory[index + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory[index + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory[index] = value % 10;
}

void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index + i] = regs[i];
	}
}

void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		regs[i] = memory[index + i];
	}
}

Chip8::Chip8(): randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    // Initialize PC
    Program_Counter = START_ADDRESS;

	// Initialize fonts into memory
	for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}

	// Initialize RNG
	randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}


int main(int argc, char* argv[]) {
    // Your main code here
    printf("hello world");
    return 0;
}

