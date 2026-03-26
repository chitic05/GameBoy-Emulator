#include "CPU.h"
#include <iostream>
#include <stdexcept>
CPU::CPU(uint8_t* mem){
    std::cout << "CPU initialized\n";
    std::fill(registers+0, registers+8, 0);
    programCounter = 0;
    flags = {
        .carry = false,
        .halfCarry = false,
        .subtraction = false,
        .zero = false
    };
    this->mem = mem;
}

const uint16_t CPU::readRegister(const RegsID id) const{
    uint16_t ret_val = 0;
    if(id >= RegsID::A && id <= RegsID::L){
        ret_val = this->registers[static_cast<uint8_t>(id)];
        return ret_val;
    }
    switch(id){
        case RegsID::AF:
            ret_val = (this->registers[static_cast<uint8_t>(RegsID::A)]<<8) |
                      (this->registers[static_cast<uint8_t>(RegsID::F)]);
            return ret_val;
            break;
        case RegsID::BC:
            ret_val = (this->registers[static_cast<uint8_t>(RegsID::B)]<<8) |
                      (this->registers[static_cast<uint8_t>(RegsID::C)]);
            return ret_val;
            break;
        case RegsID::DE:
            ret_val = (this->registers[static_cast<uint8_t>(RegsID::D)]<<8) |
                      (this->registers[static_cast<uint8_t>(RegsID::E)]);
            return ret_val;
            break;
        case RegsID::HL:
            ret_val = (this->registers[static_cast<uint8_t>(RegsID::H)]<<8) |
                      (this->registers[static_cast<uint8_t>(RegsID::L)]);
            return ret_val;
            break;
        case RegsID::SP:
            ret_val = this->stackPointer;
            return ret_val;
            break;
        case RegsID::PC:
            ret_val = this->programCounter;
            return ret_val;
            break;
        default:
            throw std::runtime_error("Register" + std::to_string(static_cast<uint8_t>(id)) + "doesn't exist!\n");
            return ret_val;
    }
}

void CPU::writeRegister(const RegsID id,const uint16_t value){
    if(id >= RegsID::A && id <= RegsID::L){
        this->registers[static_cast<uint8_t>(id)] = static_cast<uint8_t>(value);
        return;
    }
    switch(id){
        case RegsID::AF:
            this->registers[static_cast<uint8_t>(RegsID::A)] = static_cast<uint8_t>(value>>8);
            this->registers[static_cast<uint8_t>(RegsID::F)] = static_cast<uint8_t>(value);
            break;
        case RegsID::BC:
            this->registers[static_cast<uint8_t>(RegsID::B)] = static_cast<uint8_t>(value>>8);
            this->registers[static_cast<uint8_t>(RegsID::C)] = static_cast<uint8_t>(value);
            break;
        case RegsID::DE:
            this->registers[static_cast<uint8_t>(RegsID::D)] = static_cast<uint8_t>(value>>8);
            this->registers[static_cast<uint8_t>(RegsID::E)] = static_cast<uint8_t>(value);
            break;
        case RegsID::HL:
            this->registers[static_cast<uint8_t>(RegsID::H)] = static_cast<uint8_t>(value>>8);
            this->registers[static_cast<uint8_t>(RegsID::L)] = static_cast<uint8_t>(value);
            break;
        case RegsID::SP:
            this->stackPointer = value;
            break;
        case RegsID::PC:
            this->programCounter = value;
            break;
        default:
            throw std::runtime_error("Register" + std::to_string(static_cast<uint8_t>(id)) + "doesn't exist!\n");

    }
}

