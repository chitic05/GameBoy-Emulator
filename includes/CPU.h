#pragma once
#include <cstdint>
#include "Memory.h"

class CPU{
    private:
        uint8_t V[16]{}; //registers 0x0 to 0xF
        Memory* mem;

        uint16_t programCounter{0x200};
        uint16_t stackPointer{};
        uint16_t I{}; //register for graphics

        struct{
            bool pixels[32*64]{};        
        } display;
        struct{
            bool pressed[16]{};
        }keypad;
        struct{
            uint8_t delayTimer{};
            uint8_t soundTimer{};
        }timers;

    public:
        CPU(Memory* mem);

        const uint16_t readReg(const uint8_t id) const;
        void writeReg(const uint8_t id,const uint16_t value);

        const uint16_t pop(); //pop from stack
        void push(uint16_t value); //push to stack

        uint16_t fetch();
        void execute();

        void run();

        const bool* getPixels() const;

        void setKey(uint8_t key, bool val);

        const uint8_t getSoundTimer() const;
        void updateTimers();

};