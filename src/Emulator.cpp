#include "Emulator.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

Emulator::Emulator()
    : memory(), cpu(&memory){

        if(SDL_Init(SDL_INIT_VIDEO) < 0){
            std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
            std::exit(1);
        }
        window = SDL_CreateWindow("CHIP-8", 1280, 640, 0);
        if(!window){
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            SDL_Quit();
            std::exit(1);
        }
        renderer = SDL_CreateRenderer(window, NULL);
        if(!renderer){
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(window);
            SDL_Quit();
            std::exit(1);
        }

        keyState = SDL_GetKeyboardState(nullptr);
        if (!keyState)
            std::cerr << "Couldn't get the keyboard state\n";
}

Emulator::~Emulator(){
    if(this->renderer){
        SDL_DestroyRenderer(this->renderer);
    }
    if(this->window){
        SDL_DestroyWindow(this->window);
    }
}


void Emulator::run() {

    const double targetFPS = 60.0;
    const double targetFrameTime = 1.0 / targetFPS;

    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = SDL_GetPerformanceCounter();

    bool isRunning = true;
    SDL_Event event;
    while(isRunning){
        try{
            while(SDL_PollEvent(&event)){
            if(event.type == SDL_EVENT_QUIT)
                isRunning = false;
            }
            now = SDL_GetPerformanceCounter();

            {//update keys
                cpu.setKey(0x1, keyState[SDL_SCANCODE_1]);
                cpu.setKey(0x2, keyState[SDL_SCANCODE_2]);
                cpu.setKey(0x3, keyState[SDL_SCANCODE_3]);
                cpu.setKey(0xC, keyState[SDL_SCANCODE_4]);
                
                cpu.setKey(0x4, keyState[SDL_SCANCODE_Q]);
                cpu.setKey(0x5, keyState[SDL_SCANCODE_W]);
                cpu.setKey(0x6, keyState[SDL_SCANCODE_E]);
                cpu.setKey(0xD, keyState[SDL_SCANCODE_R]);
                
                cpu.setKey(0x7, keyState[SDL_SCANCODE_A]);
                cpu.setKey(0x8, keyState[SDL_SCANCODE_S]);
                cpu.setKey(0x9, keyState[SDL_SCANCODE_D]);
                cpu.setKey(0xE, keyState[SDL_SCANCODE_F]);
                
                cpu.setKey(0xA, keyState[SDL_SCANCODE_Z]);
                cpu.setKey(0x0, keyState[SDL_SCANCODE_X]);
                cpu.setKey(0xB, keyState[SDL_SCANCODE_C]);
                cpu.setKey(0xF, keyState[SDL_SCANCODE_V]);
            }      
            //run cpu for more cycles
            for(size_t i = 0;i<10;++i)
                cpu.run();
            
            cpu.updateTimers();

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            for(size_t i = 0; i<32; ++i){
                for(size_t j = 0; j<64; ++j){
                    if(cpu.getPixels()[i*64+j]){
                        SDL_FRect pixelRect = {j * 20.0f, i * 20.0f, 20.0f, 20.0f};
                        SDL_RenderFillRect(renderer, &pixelRect);
                    }
                }
            }
            SDL_RenderPresent(renderer);

            // FRAME TIMING

            double frameTime = static_cast<double>(SDL_GetPerformanceCounter()-now)/SDL_GetPerformanceFrequency(); // how much second this while loop took(frame)
            double delayTime = targetFrameTime-frameTime;
            if(delayTime > 0){
                SDL_Delay(static_cast<uint32_t>(delayTime*1000)); // transform to seconds;
            }
        }catch(std::exception& e){
            std::cerr << e.what()<<'\n';
            isRunning = false;
        }
    }

}
void Emulator::loadRom(const std::string& path){
    std::ifstream romFile(path, std::ios::binary | std::ios::ate);
    if (!romFile.is_open()) {
        throw std::runtime_error("Failed to open ROM file: " + path);
    }

    const std::streamsize romSize = romFile.tellg();
    if (romSize < 0) {
        throw std::runtime_error("Failed to determine ROM size: " + path);
    }
    if (romSize > static_cast<std::streamsize>(MEM_SIZE-0x200)) {
        throw std::runtime_error("ROM is larger than addressable memory");
    }

    romFile.seekg(0, std::ios::beg);

    for (std::streamsize i = 0; i < romSize; ++i) {
        char byte = 0;
        romFile.read(&byte, 1);
        if (!romFile) {
            throw std::runtime_error("Failed while reading ROM file: " + path);
        }
        this->memory.write(static_cast<uint16_t>(i+0x200), static_cast<uint8_t>(byte));
    }
}

CPU& Emulator::getCPU() {
    return cpu;
}

Memory& Emulator::getMemory() {
    return memory;
}

