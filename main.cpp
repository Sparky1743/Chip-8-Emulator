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

