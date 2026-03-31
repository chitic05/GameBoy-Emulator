#pragma once

#include <cstddef>
#include "CPU.h"
#include "Memory.h"
#include <string>
#include <SDL3/SDL.h>

class Emulator {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_AudioStream* audioStream = nullptr;

    const bool* keyState = nullptr;
    Memory memory;
    CPU cpu;

public:
    Emulator();
    ~Emulator();
    void run();
    void loadRom(const std::string& path);
        
    CPU& getCPU();
    Memory& getMemory();
    
};
