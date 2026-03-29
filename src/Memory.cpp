#include "Memory.h"
#include <iostream>

Memory::Memory(){
    std::cout << "Memory initialized!\n";
}

const uint8_t Memory::read(const uint16_t address) const{
    if(address >= MEM_SIZE){
        throw std::runtime_error("Memory address " + std::to_string(address) + " doesn't exist!\n");
        return 0;
}
    return mem[address];
}

void Memory::write(const uint16_t address,const uint8_t value){
    if(address > MEM_SIZE){
        throw std::runtime_error("Memory address " + std::to_string(address) + " doesn't exist!\n");
        return;
    }
    mem[address] = value;
}

uint16_t* Memory::getStack(){
    return this->stack;
}
