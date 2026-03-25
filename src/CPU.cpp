#include "CPU.h"
#include <iostream>

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
            ret_val = (this->registers[static_cast<uint8_t>(RegsID::A)]<<8) ||
                      (this->registers[static_cast<uint8_t>(RegsID::F)]);
            return ret_val;
            break;
        case RegsID::BC:
            ret_val = (this->registers[static_cast<uint8_t>(RegsID::B)]<<8) ||
                      (this->registers[static_cast<uint8_t>(RegsID::C)]);
            return ret_val;
            break;
        case RegsID::DE:
            ret_val = (this->registers[static_cast<uint8_t>(RegsID::D)]<<8) ||
                      (this->registers[static_cast<uint8_t>(RegsID::E)]);
            return ret_val;
            break;
        case RegsID::HL:
            ret_val = (this->registers[static_cast<uint8_t>(RegsID::H)]<<8) ||
                      (this->registers[static_cast<uint8_t>(RegsID::L)]);
            return ret_val;
            break;
        default:
            return 0;
    }
}
