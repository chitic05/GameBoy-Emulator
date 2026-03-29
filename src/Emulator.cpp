#include "Emulator.h"
#include <fstream>
#include <stdexcept>

Emulator::Emulator()
    : memory(), cpu(&memory){}



void Emulator::run() {
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
    if (romSize > static_cast<std::streamsize>(MEM_SIZE)) {
        throw std::runtime_error("ROM is larger than addressable memory");
    }

    romFile.seekg(0, std::ios::beg);

    for (std::streamsize i = 0; i < romSize; ++i) {
        char byte = 0;
        romFile.read(&byte, 1);
        if (!romFile) {
            throw std::runtime_error("Failed while reading ROM file: " + path);
        }
        this->memory.write(static_cast<uint16_t>(i), static_cast<uint8_t>(byte));
    }
}

CPU& Emulator::getCPU() {
    return cpu;
}

Memory& Emulator::getMemory() {
    return memory;
}

