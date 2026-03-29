#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
constexpr size_t MEM_SIZE = 4096;

class Memory{
        
    private:
        uint8_t mem[MEM_SIZE]{};
        uint16_t stack[16]{};
    public:
        Memory();

        const uint8_t read(const uint16_t address) const;
        void write(const uint16_t address,const uint8_t value);

        uint16_t* getStack();
};