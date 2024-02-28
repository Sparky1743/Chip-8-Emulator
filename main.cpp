#include <iostream>
#include <stdio.h>
#include <SDL2/SDL.h>

class Chip8
{
    public:
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
};

int main(int argc, char* argv[]) {
    // Your main code here
    printf("hello world");
    return 0;
}

