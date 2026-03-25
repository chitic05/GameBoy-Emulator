#pragma once

#include <cstdint>
#include <cstddef>

constexpr size_t MEM_SIZE = 0x10000;

class Memory{
    private:
        uint8_t mem[MEM_SIZE];
    public:
        Memory();
};