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

    void run();
    void loadRom(const std::string& path);
        
    CPU& getCPU();
    Memory& getMemory();
    
};
