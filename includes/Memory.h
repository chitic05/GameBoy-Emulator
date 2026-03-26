#pragma once

#include <cstdint>
#include <cstddef>

constexpr size_t MEM_SIZE = 0x10000;

class Memory{
    private:
        uint8_t mem[MEM_SIZE];
    public:
        Memory();
        uint8_t readMemory8(uint16_t address);
        void writeMemory8(uint16_t address, uint8_t value);
        uint16_t readMemory16(uint16_t address);
        void writeMemory16(uint16_t address, uint16_t value);
        
};