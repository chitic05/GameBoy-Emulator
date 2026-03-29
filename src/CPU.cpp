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
        uint8_t family = (opcode & 0xF000)>>12;
        switch(family){ //first byte
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
            case 0x6://Store/Set immediate value
                {
                    uint8_t reg = (opcode&0x0F00)>>8;
                    uint8_t value = (opcode&0x00FF);
                    this->V[reg] = value;
                }
                break;
            case 0x7://Add to register(silent no flag) immediate value
                {
                    uint8_t reg = (opcode&0x0F00)>>8;
                    uint8_t value = (opcode&0x00FF);
                    this->V[reg] += value;
                }
                break;
            case 0x8: // ALU
                {
                    uint8_t operation = (opcode&0x000F);
                    switch(operation){
                        case 0x0:
                            {
                                uint8_t reg1 = (opcode & 0x0F00) >> 8;
                                uint8_t reg2 = (opcode & 0x00F0) >> 4;
                                this->V[reg1] = this->V[reg2];
                            }
                            break;
                        case 0x1:
                            {
                                uint8_t reg1 = (opcode & 0x0F00) >> 8;
                                uint8_t reg2 = (opcode & 0x00F0) >> 4;
                                this->V[reg1] = this->V[reg1] | this->V[reg2];
                            }
                            break;
                        case 0x2:
                            {
                                uint8_t reg1 = (opcode & 0x0F00) >> 8;
                                uint8_t reg2 = (opcode & 0x00F0) >> 4;
                                this->V[reg1] = this->V[reg1] & this->V[reg2];
                            }
                            break;
                        case 0x3:
                            {
                                uint8_t reg1 = (opcode & 0x0F00) >> 8;
                                uint8_t reg2 = (opcode & 0x00F0) >> 4;
                                this->V[reg1] = this->V[reg1] ^ this->V[reg2];
                            }
                            break;
                        case 0x4:
                            {
                                uint8_t reg1 = (opcode & 0x0F00) >> 8;
                                uint8_t reg2 = (opcode & 0x00F0) >> 4;

                                //Check if it carries and update the flags
                                uint8_t temp_flag;
                                uint16_t result = this->V[reg1] + this->V[reg2];                              
                                if(result > 0xFF)
                                    temp_flag = 1;
                                else temp_flag= 0;

                                this->V[reg1] = this->V[reg1] + this->V[reg2];
                                
                                this->V[0XF] = temp_flag;
                            }
                            break;
                        case 0x5:
                            {
                                uint8_t reg1 = (opcode & 0x0F00) >> 8;
                                uint8_t reg2 = (opcode & 0x00F0) >> 4;
                                
                                //Check if it borrows
                                uint8_t temp_flag;
                                if(this->V[reg1] >= this->V[reg2])
                                    temp_flag = 1;
                                else temp_flag = 0;

                                this->V[reg1] = this->V[reg1] - this->V[reg2];

                                this->V[0XF] = temp_flag;
                            }
                            break;
                        case 0x6:
                            {
                                uint8_t reg1 = (opcode & 0x0F00) >> 8;
                                uint8_t reg2 = (opcode & 0x00F0) >> 4;

                                uint8_t temp_flag;
                                temp_flag = this->V[reg1] % 2;

                                this->V[reg1] = this->V[reg1] >> 1;

                                this->V[0xF] = temp_flag;
                            }
                            break;
                        case 0x7:
                            {
                                uint8_t reg1 = (opcode & 0x0F00) >> 8;
                                uint8_t reg2 = (opcode & 0x00F0) >> 4;

                                //Check if it borrows
                                uint8_t temp_flag;
                                if(this->V[reg2] >= this->V[reg1])
                                    temp_flag = 1;
                                else temp_flag = 0;
                                this->V[reg1] = this->V[reg2] - this->V[reg1];

                                this->V[0XF] = temp_flag;
                            }
                            break;
                        case 0xE:
                            {
                                uint8_t reg1 = (opcode & 0x0F00) >> 8;
                                uint8_t reg2 = (opcode & 0x00F0) >> 4;

                                //set the flag register with the bit that gets deleted;
                                uint8_t temp_flag;
                                temp_flag= (this->V[reg1] >> 7) % 2;

                                this->V[reg1] = this->V[reg1] << 1;
                                
                                this->V[0xF] = temp_flag;
                            }
                            break;
                    }
                }
                break;
            case 0x9:
                {

                }
                break;
            case 0xA:
                {

                }
                break;
            case 0xB:
                {

                }
                break;
            case 0xC:
                {

                }
                break;
            //Complete other families 
        }
    }catch(std::exception& e){
        std::cerr << "Error in execute cycle: " << e.what() << '\n';
    }
}