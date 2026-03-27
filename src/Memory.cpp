#include "Memory.h"
#include <iostream>

Memory::Memory(){
    std::fill(this->mem+0, this->mem+MEM_SIZE, 0);
    std::cout << "Memory initialized!\n";
}

uint8_t Memory::readMemory8(uint16_t address){
    if(address < MEM_SIZE)
        return this->mem[address];
    else throw std::runtime_error("Invalid address");
}
void Memory::writeMemory8(uint16_t address, uint8_t value){
    if(address < MEM_SIZE)
        this->mem[address] = value;
    else throw std::runtime_error("Invalid address");
}
uint16_t Memory::readMemory16(uint16_t address){
    try{
        uint8_t lowByte = this->readMemory8(address);
        uint8_t highByte = this->readMemory8(address+1);
        return static_cast<uint16_t>(highByte) << 8 |
                lowByte;
    }catch(const std::exception& e){
        std::cerr << "Can't read from [ " << address << " ]"<< e.what() << '\n';
        return 0;
    }
    
}
void Memory::writeMemory16(uint16_t address, uint16_t value){
    try{
       this->writeMemory8(address, static_cast<uint8_t>(value));
        this->writeMemory8(address+1, static_cast<uint8_t>(value >> 8)); 
    }catch(const std::exception& e){
        std::cerr << "Can't write to [ " << address << " ]"<< e.what() << '\n'; 
    }
}