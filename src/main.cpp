#include "Emulator.h"
#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {
    const char* romPath = "../gb-test-roms/cpu_instrs/cpu_instrs.gb";
    
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) {
        }
    }
    
    Emulator emulator;
    emulator.loadRom(romPath);
    
    int stepCount = 0;
    const int maxSteps = 100000000; // Increased to allow full test execution
    
    while(stepCount < maxSteps) {
        emulator.run();
        stepCount++;
    }
    
    std::cout << "\n=== Emulation completed: "<< std::endl;
    
    return 0;
}