#include "CPU.h"
#include <iostream>
#include <cstdio>
#include <stdexcept>
CPU::CPU(Memory* mem){
    std::cout << "CPU initialized\n";
    this->mem = mem;
}

const uint16_t CPU::readReg(const uint8_t id) const{
    if(id == 0x10) 
        return this->I;
    else if(id < 16) 
    return this->V[id];

    throw std::runtime_error("Register " + std::to_string(id) +" doesn't exist!\n");
}

void CPU::writeReg(const uint8_t id,const uint16_t value){
    if(id == 0x10) {
        this->I = value;
        return;
    }else if(id<16){
        this->V[id] = static_cast<uint8_t>(value);
        return;
    }
    

    throw std::runtime_error("Register " + std::to_string(id) +" doesn't exist!\n");
    
}

const uint16_t CPU::pop(){
    if(this->stackPointer>0){
        this->stackPointer--;
        return mem->getStack()[stackPointer];
    }else{
        throw std::runtime_error("Stack pointer underflowed!\n");
    }
    
}
void CPU::push(uint16_t value){
    if(this->stackPointer<=15){
        mem->getStack()[stackPointer] = value;
        this->stackPointer++;
    }else{
        throw std::runtime_error("Stack pointer overflowed!\n");
    }
}

uint16_t CPU::fetch(){
    if(programCounter >= MEM_SIZE-1){
        throw std::runtime_error("Program counter out of reach!\n");
        return 0;
    }

    
    uint16_t value = (static_cast<uint16_t>(this->mem->read(this->programCounter)) << 8) | //high byte
                     this->mem->read(this->programCounter+1); //low byte
    this->programCounter+=2;
    return value;
}

void CPU::execute(){
    try{
        uint16_t opcode = this->fetch();
        switch(opcode & 0xF000){ //first byte
            case 0x0:
                if(opcode == 0x00E0){
                    //CLEAR DISPLAY
                }else if(opcode == 0X00EE){
                    //RET to subroutine
                    this->programCounter = this->pop();
                }
                break;
            case 0x1: //JUMP
                this->programCounter = opcode & 0x0FFF;
                break;
            case 0x2://CALL
                this->push(this->programCounter);
                this->programCounter = opcode & 0x0FFF;
                break;
            case 0x3://skip next instr if ==
                {
                    uint8_t reg = (opcode&0x0F00)>>8;
                    uint8_t value = opcode&0x00FF;
                    if(this->V[reg] == value){
                        this->programCounter+=2;
                    }
                }
                
                break;
            case 0x4://skip next instr if !=
                {
                    uint8_t reg = (opcode&0x0F00)>>8;
                    uint8_t value = opcode&0x00FF;
                    if(this->V[reg] != value){
                        this->programCounter+=2;
                    }
                }
                break;
            case 0x5://skip next instr if reg1 == reg2
                {
                    uint8_t reg1 = (opcode&0x0F00)>>8;
                    uint8_t reg2 = (opcode&0x00F0)>>4;
                    if(this->V[reg1] == this->V[reg2]){
                        this->programCounter+=2;
                    }
                }
                break;
            case 0x6://Store/Set
                {
                    uint8_t reg = (opcode&0x0F00)>>8;
                    uint8_t value = (opcode&0x00FF);
                    this->V[reg] = value;
                }
                break;
            case 0x7://Add to register(silent no flag)
                {
                    uint8_t reg = (opcode&0x0F00)>>8;
                    uint8_t value = (opcode&0x00FF);
                    this->V[reg] += value;
                }
                break;   
            //Complete other families 
        }
    }catch(std::exception& e){
        std::cerr << "Error in execute cycle: " << e.what() << '\n';
    }
}