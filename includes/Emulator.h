#pragma once

#include <cstddef>
#include "CPU.h"
#include "Memory.h"
#include <string>


class Emulator {
private:
    Memory memory;
    CPU cpu;

public:
    Emulator();

    void run(std::size_t steps);
    void loadRom(const std::string& path);
    
    CPU& getCPU();
    Memory& getMemory();
    
};
