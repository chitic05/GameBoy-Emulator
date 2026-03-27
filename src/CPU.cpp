#include "CPU.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
CPU::CPU(Memory* mem){
    std::cout << "CPU initialized\n";
    std::fill(registers+0, registers+8, 0);
    programCounter = 0;
    stackPointer = 0xFFFE;
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

void CPU::push(const RegsID id){
    if(id >= RegsID::A && id <= RegsID::L){
        throw std::runtime_error("Can't push this register to stack!\n");
        return;
    }
    uint16_t val = this->readRegister(id);
    this->stackPointer-=2;
    this->mem->writeMemory16(this->stackPointer, val);
}

void CPU::pop(const RegsID id){
    if(id >= RegsID::A && id <= RegsID::L){
        throw std::runtime_error("Can't pop to this register!\n");
        return;
    }
    
    uint16_t val = this->mem->readMemory16(this->stackPointer);
    this->stackPointer+=2;
    this->writeRegister(id, val);
}

uint8_t CPU::fetch(){
    uint8_t opcode = this->mem->readMemory8(this->programCounter);
    this->programCounter++;
    return opcode;
}

void CPU::execute(uint8_t opcode){
    auto readFlagZ = [this]() -> bool {
        return (this->registers[static_cast<uint8_t>(RegsID::F)] & 0x80) != 0;
    };
    auto readFlagC = [this]() -> bool {
        return (this->registers[static_cast<uint8_t>(RegsID::F)] & 0x10) != 0;
    };
    auto writeFlags = [this](bool z, bool n, bool h, bool c){
        this->flags.zero = z;
        this->flags.subtraction = n;
        this->flags.halfCarry = h;
        this->flags.carry = c;
        this->registers[static_cast<uint8_t>(RegsID::F)] =
            (static_cast<uint8_t>(z) << 7) |
            (static_cast<uint8_t>(n) << 6) |
            (static_cast<uint8_t>(h) << 5) |
            (static_cast<uint8_t>(c) << 4);
    };

    switch(opcode){
        case 0x00: // NOP
            break;

        case 0xCB: // Prefix CB
            this->executeCB(this->fetch());
            break;

        // LD r, d8
        case 0x06: this->writeRegister(RegsID::B, this->fetch()); break;
        case 0x0E: this->writeRegister(RegsID::C, this->fetch()); break;
        case 0x16: this->writeRegister(RegsID::D, this->fetch()); break;
        case 0x1E: this->writeRegister(RegsID::E, this->fetch()); break;
        case 0x26: this->writeRegister(RegsID::H, this->fetch()); break;
        case 0x2E: this->writeRegister(RegsID::L, this->fetch()); break;
        case 0x3E: this->writeRegister(RegsID::A, this->fetch()); break;

        // Flag control
        case 0x37: // SCF
            writeFlags(readFlagZ(), false, false, true);
            break;
        case 0x3F: // CCF
            writeFlags(readFlagZ(), false, false, !readFlagC());
            break;

        // LD rr, d16
        case 0x01: {
            const uint16_t value = this->mem->readMemory16(this->programCounter);
            this->programCounter += 2;
            this->writeRegister(RegsID::BC, value);
            break;
        }
        case 0x11: {
            const uint16_t value = this->mem->readMemory16(this->programCounter);
            this->programCounter += 2;
            this->writeRegister(RegsID::DE, value);
            break;
        }
        case 0x21: {
            const uint16_t value = this->mem->readMemory16(this->programCounter);
            this->programCounter += 2;
            this->writeRegister(RegsID::HL, value);
            break;
        }
        case 0x31: {
            const uint16_t value = this->mem->readMemory16(this->programCounter);
            this->programCounter += 2;
            this->writeRegister(RegsID::SP, value);
            break;
        }

        // LD A, r
        case 0x78: this->writeRegister(RegsID::A, this->readRegister(RegsID::B)); break;
        case 0x79: this->writeRegister(RegsID::A, this->readRegister(RegsID::C)); break;
        case 0x7A: this->writeRegister(RegsID::A, this->readRegister(RegsID::D)); break;
        case 0x7B: this->writeRegister(RegsID::A, this->readRegister(RegsID::E)); break;
        case 0x7C: this->writeRegister(RegsID::A, this->readRegister(RegsID::H)); break;
        case 0x7D: this->writeRegister(RegsID::A, this->readRegister(RegsID::L)); break;
        case 0x7E: this->writeRegister(RegsID::A, this->mem->readMemory8(this->readRegister(RegsID::HL))); break;

        // LD (HL), A
        case 0x77:
            this->mem->writeMemory8(this->readRegister(RegsID::HL), static_cast<uint8_t>(this->readRegister(RegsID::A)));
            break;

        // INC r
        case 0x04: this->inc8(RegsID::B); break;
        case 0x0C: this->inc8(RegsID::C); break;
        case 0x14: this->inc8(RegsID::D); break;
        case 0x1C: this->inc8(RegsID::E); break;
        case 0x24: this->inc8(RegsID::H); break;
        case 0x2C: this->inc8(RegsID::L); break;
        case 0x3C: this->inc8(RegsID::A); break;

        // DEC r
        case 0x05: this->dec8(RegsID::B); break;
        case 0x0D: this->dec8(RegsID::C); break;
        case 0x15: this->dec8(RegsID::D); break;
        case 0x1D: this->dec8(RegsID::E); break;
        case 0x25: this->dec8(RegsID::H); break;
        case 0x2D: this->dec8(RegsID::L); break;
        case 0x3D: this->dec8(RegsID::A); break;

        // ADD A, r / d8
        case 0x80: this->add8(static_cast<uint8_t>(this->readRegister(RegsID::B))); break;
        case 0x81: this->add8(static_cast<uint8_t>(this->readRegister(RegsID::C))); break;
        case 0x82: this->add8(static_cast<uint8_t>(this->readRegister(RegsID::D))); break;
        case 0x83: this->add8(static_cast<uint8_t>(this->readRegister(RegsID::E))); break;
        case 0x84: this->add8(static_cast<uint8_t>(this->readRegister(RegsID::H))); break;
        case 0x85: this->add8(static_cast<uint8_t>(this->readRegister(RegsID::L))); break;
        case 0x86: this->add8(this->mem->readMemory8(this->readRegister(RegsID::HL))); break;
        case 0x87: this->add8(static_cast<uint8_t>(this->readRegister(RegsID::A))); break;
        case 0xC6: this->add8(this->fetch()); break;

        // ADC A, r / d8
        case 0x88: this->adc8(static_cast<uint8_t>(this->readRegister(RegsID::B))); break;
        case 0x89: this->adc8(static_cast<uint8_t>(this->readRegister(RegsID::C))); break;
        case 0x8A: this->adc8(static_cast<uint8_t>(this->readRegister(RegsID::D))); break;
        case 0x8B: this->adc8(static_cast<uint8_t>(this->readRegister(RegsID::E))); break;
        case 0x8C: this->adc8(static_cast<uint8_t>(this->readRegister(RegsID::H))); break;
        case 0x8D: this->adc8(static_cast<uint8_t>(this->readRegister(RegsID::L))); break;
        case 0x8E: this->adc8(this->mem->readMemory8(this->readRegister(RegsID::HL))); break;
        case 0x8F: this->adc8(static_cast<uint8_t>(this->readRegister(RegsID::A))); break;
        case 0xCE: this->adc8(this->fetch()); break;

        // SUB A, r / d8
        case 0x90: this->sub8(static_cast<uint8_t>(this->readRegister(RegsID::B))); break;
        case 0x91: this->sub8(static_cast<uint8_t>(this->readRegister(RegsID::C))); break;
        case 0x92: this->sub8(static_cast<uint8_t>(this->readRegister(RegsID::D))); break;
        case 0x93: this->sub8(static_cast<uint8_t>(this->readRegister(RegsID::E))); break;
        case 0x94: this->sub8(static_cast<uint8_t>(this->readRegister(RegsID::H))); break;
        case 0x95: this->sub8(static_cast<uint8_t>(this->readRegister(RegsID::L))); break;
        case 0x96: this->sub8(this->mem->readMemory8(this->readRegister(RegsID::HL))); break;
        case 0x97: this->sub8(static_cast<uint8_t>(this->readRegister(RegsID::A))); break;
        case 0xD6: this->sub8(this->fetch()); break;

        // SBC A, r / d8
        case 0x98: this->sbc8(static_cast<uint8_t>(this->readRegister(RegsID::B))); break;
        case 0x99: this->sbc8(static_cast<uint8_t>(this->readRegister(RegsID::C))); break;
        case 0x9A: this->sbc8(static_cast<uint8_t>(this->readRegister(RegsID::D))); break;
        case 0x9B: this->sbc8(static_cast<uint8_t>(this->readRegister(RegsID::E))); break;
        case 0x9C: this->sbc8(static_cast<uint8_t>(this->readRegister(RegsID::H))); break;
        case 0x9D: this->sbc8(static_cast<uint8_t>(this->readRegister(RegsID::L))); break;
        case 0x9E: this->sbc8(this->mem->readMemory8(this->readRegister(RegsID::HL))); break;
        case 0x9F: this->sbc8(static_cast<uint8_t>(this->readRegister(RegsID::A))); break;
        case 0xDE: this->sbc8(this->fetch()); break;

        // AND/XOR/OR/CP with r and d8
        case 0xA0: this->and8(static_cast<uint8_t>(this->readRegister(RegsID::B))); break;
        case 0xA1: this->and8(static_cast<uint8_t>(this->readRegister(RegsID::C))); break;
        case 0xA2: this->and8(static_cast<uint8_t>(this->readRegister(RegsID::D))); break;
        case 0xA3: this->and8(static_cast<uint8_t>(this->readRegister(RegsID::E))); break;
        case 0xA4: this->and8(static_cast<uint8_t>(this->readRegister(RegsID::H))); break;
        case 0xA5: this->and8(static_cast<uint8_t>(this->readRegister(RegsID::L))); break;
        case 0xA6: this->and8(this->mem->readMemory8(this->readRegister(RegsID::HL))); break;
        case 0xA7: this->and8(static_cast<uint8_t>(this->readRegister(RegsID::A))); break;
        case 0xE6: this->and8(this->fetch()); break;

        case 0xA8: this->xor8(static_cast<uint8_t>(this->readRegister(RegsID::B))); break;
        case 0xA9: this->xor8(static_cast<uint8_t>(this->readRegister(RegsID::C))); break;
        case 0xAA: this->xor8(static_cast<uint8_t>(this->readRegister(RegsID::D))); break;
        case 0xAB: this->xor8(static_cast<uint8_t>(this->readRegister(RegsID::E))); break;
        case 0xAC: this->xor8(static_cast<uint8_t>(this->readRegister(RegsID::H))); break;
        case 0xAD: this->xor8(static_cast<uint8_t>(this->readRegister(RegsID::L))); break;
        case 0xAE: this->xor8(this->mem->readMemory8(this->readRegister(RegsID::HL))); break;
        case 0xAF: this->xor8(static_cast<uint8_t>(this->readRegister(RegsID::A))); break;
        case 0xEE: this->xor8(this->fetch()); break;

        case 0xB0: this->or8(static_cast<uint8_t>(this->readRegister(RegsID::B))); break;
        case 0xB1: this->or8(static_cast<uint8_t>(this->readRegister(RegsID::C))); break;
        case 0xB2: this->or8(static_cast<uint8_t>(this->readRegister(RegsID::D))); break;
        case 0xB3: this->or8(static_cast<uint8_t>(this->readRegister(RegsID::E))); break;
        case 0xB4: this->or8(static_cast<uint8_t>(this->readRegister(RegsID::H))); break;
        case 0xB5: this->or8(static_cast<uint8_t>(this->readRegister(RegsID::L))); break;
        case 0xB6: this->or8(this->mem->readMemory8(this->readRegister(RegsID::HL))); break;
        case 0xB7: this->or8(static_cast<uint8_t>(this->readRegister(RegsID::A))); break;
        case 0xF6: this->or8(this->fetch()); break;

        case 0xB8: this->cp8(static_cast<uint8_t>(this->readRegister(RegsID::B))); break;
        case 0xB9: this->cp8(static_cast<uint8_t>(this->readRegister(RegsID::C))); break;
        case 0xBA: this->cp8(static_cast<uint8_t>(this->readRegister(RegsID::D))); break;
        case 0xBB: this->cp8(static_cast<uint8_t>(this->readRegister(RegsID::E))); break;
        case 0xBC: this->cp8(static_cast<uint8_t>(this->readRegister(RegsID::H))); break;
        case 0xBD: this->cp8(static_cast<uint8_t>(this->readRegister(RegsID::L))); break;
        case 0xBE: this->cp8(this->mem->readMemory8(this->readRegister(RegsID::HL))); break;
        case 0xBF: this->cp8(static_cast<uint8_t>(this->readRegister(RegsID::A))); break;
        case 0xFE: this->cp8(this->fetch()); break;

        // 16-bit arithmetic
        case 0x03: this->inc16(RegsID::BC); break;
        case 0x13: this->inc16(RegsID::DE); break;
        case 0x23: this->inc16(RegsID::HL); break;
        case 0x33: this->inc16(RegsID::SP); break;
        case 0x0B: this->dec16(RegsID::BC); break;
        case 0x1B: this->dec16(RegsID::DE); break;
        case 0x2B: this->dec16(RegsID::HL); break;
        case 0x3B: this->dec16(RegsID::SP); break;

        case 0x09: this->add16(this->readRegister(RegsID::BC)); break;
        case 0x19: this->add16(this->readRegister(RegsID::DE)); break;
        case 0x29: this->add16(this->readRegister(RegsID::HL)); break;
        case 0x39: this->add16(this->readRegister(RegsID::SP)); break;

        // JR e8 / JR cc,e8
        case 0x18: {
            const int8_t offset = static_cast<int8_t>(this->fetch());
            this->programCounter = static_cast<uint16_t>(this->programCounter + offset);
            break;
        }
        case 0x20: {
            const int8_t offset = static_cast<int8_t>(this->fetch());
            if(!readFlagZ()) this->programCounter = static_cast<uint16_t>(this->programCounter + offset);
            break;
        }
        case 0x28: {
            const int8_t offset = static_cast<int8_t>(this->fetch());
            if(readFlagZ()) this->programCounter = static_cast<uint16_t>(this->programCounter + offset);
            break;
        }
        case 0x30: {
            const int8_t offset = static_cast<int8_t>(this->fetch());
            if(!readFlagC()) this->programCounter = static_cast<uint16_t>(this->programCounter + offset);
            break;
        }
        case 0x38: {
            const int8_t offset = static_cast<int8_t>(this->fetch());
            if(readFlagC()) this->programCounter = static_cast<uint16_t>(this->programCounter + offset);
            break;
        }

        // JP nn / JP cc,nn / JP (HL)
        case 0xC3: this->jump(true); break;
        case 0xC2: this->jump(!readFlagZ()); break;
        case 0xCA: this->jump(readFlagZ()); break;
        case 0xD2: this->jump(!readFlagC()); break;
        case 0xDA: this->jump(readFlagC()); break;
        case 0xE9: this->programCounter = this->readRegister(RegsID::HL); break;

        // CALL nn / CALL cc,nn
        case 0xCD: this->call(true); break;
        case 0xC4: this->call(!readFlagZ()); break;
        case 0xCC: this->call(readFlagZ()); break;
        case 0xD4: this->call(!readFlagC()); break;
        case 0xDC: this->call(readFlagC()); break;

        // RET / RET cc
        case 0xC9: this->ret(true); break;
        case 0xC0: this->ret(!readFlagZ()); break;
        case 0xC8: this->ret(readFlagZ()); break;
        case 0xD0: this->ret(!readFlagC()); break;
        case 0xD8: this->ret(readFlagC()); break;

        // PUSH/POP rr
        case 0xC5: this->push(RegsID::BC); break;
        case 0xD5: this->push(RegsID::DE); break;
        case 0xE5: this->push(RegsID::HL); break;
        case 0xF5: this->push(RegsID::AF); break;
        case 0xC1: this->pop(RegsID::BC); break;
        case 0xD1: this->pop(RegsID::DE); break;
        case 0xE1: this->pop(RegsID::HL); break;
        case 0xF1: this->pop(RegsID::AF); break;

        // HALT (placeholder: scheduling not implemented yet)
        case 0x76:
            break;

        default:
            throw std::runtime_error("Unimplemented opcode: " + std::to_string(opcode));
    }
}

void CPU::executeCB(uint8_t opcode){
    auto getReg8 = [this](uint8_t index) -> uint8_t {
        switch(index){
            case 0: return static_cast<uint8_t>(this->readRegister(RegsID::B));
            case 1: return static_cast<uint8_t>(this->readRegister(RegsID::C));
            case 2: return static_cast<uint8_t>(this->readRegister(RegsID::D));
            case 3: return static_cast<uint8_t>(this->readRegister(RegsID::E));
            case 4: return static_cast<uint8_t>(this->readRegister(RegsID::H));
            case 5: return static_cast<uint8_t>(this->readRegister(RegsID::L));
            case 6: return this->mem->readMemory8(this->readRegister(RegsID::HL));
            case 7: return static_cast<uint8_t>(this->readRegister(RegsID::A));
            default: throw std::runtime_error("Invalid register index");
        }
    };

    auto setReg8 = [this](uint8_t index, uint8_t value){
        switch(index){
            case 0: this->writeRegister(RegsID::B, value); break;
            case 1: this->writeRegister(RegsID::C, value); break;
            case 2: this->writeRegister(RegsID::D, value); break;
            case 3: this->writeRegister(RegsID::E, value); break;
            case 4: this->writeRegister(RegsID::H, value); break;
            case 5: this->writeRegister(RegsID::L, value); break;
            case 6: this->mem->writeMemory8(this->readRegister(RegsID::HL), value); break;
            case 7: this->writeRegister(RegsID::A, value); break;
            default: throw std::runtime_error("Invalid register index");
        }
    };

    auto writeFlags = [this](bool z, bool n, bool h, bool c){
        this->flags.zero = z;
        this->flags.subtraction = n;
        this->flags.halfCarry = h;
        this->flags.carry = c;
        this->registers[static_cast<uint8_t>(RegsID::F)] =
            (static_cast<uint8_t>(z) << 7) |
            (static_cast<uint8_t>(n) << 6) |
            (static_cast<uint8_t>(h) << 5) |
            (static_cast<uint8_t>(c) << 4);
    };

    const uint8_t r = opcode & 0x07;
    const uint8_t x = opcode >> 6;
    const uint8_t y = (opcode >> 3) & 0x07;

    switch(x){
        case 0: {
            uint8_t value = getReg8(r);
            bool carryOut = false;

            switch(y){
                case 0: // RLC
                    carryOut = (value & 0x80) != 0;
                    value = static_cast<uint8_t>((value << 1) | (carryOut ? 1 : 0));
                    writeFlags(value == 0, false, false, carryOut);
                    break;
                case 1: // RRC
                    carryOut = (value & 0x01) != 0;
                    value = static_cast<uint8_t>((value >> 1) | (carryOut ? 0x80 : 0));
                    writeFlags(value == 0, false, false, carryOut);
                    break;
                case 2: { // RL
                    const bool carryIn = this->flags.carry || ((this->registers[static_cast<uint8_t>(RegsID::F)] & 0x10) != 0);
                    carryOut = (value & 0x80) != 0;
                    value = static_cast<uint8_t>((value << 1) | (carryIn ? 1 : 0));
                    writeFlags(value == 0, false, false, carryOut);
                    break;
                }
                case 3: { // RR
                    const bool carryIn = this->flags.carry || ((this->registers[static_cast<uint8_t>(RegsID::F)] & 0x10) != 0);
                    carryOut = (value & 0x01) != 0;
                    value = static_cast<uint8_t>((value >> 1) | (carryIn ? 0x80 : 0));
                    writeFlags(value == 0, false, false, carryOut);
                    break;
                }
                case 4: // SLA
                    carryOut = (value & 0x80) != 0;
                    value = static_cast<uint8_t>(value << 1);
                    writeFlags(value == 0, false, false, carryOut);
                    break;
                case 5: // SRA
                    carryOut = (value & 0x01) != 0;
                    value = static_cast<uint8_t>((value >> 1) | (value & 0x80));
                    writeFlags(value == 0, false, false, carryOut);
                    break;
                case 6: // SWAP
                    value = static_cast<uint8_t>((value >> 4) | (value << 4));
                    writeFlags(value == 0, false, false, false);
                    break;
                case 7: // SRL
                    carryOut = (value & 0x01) != 0;
                    value = static_cast<uint8_t>(value >> 1);
                    writeFlags(value == 0, false, false, carryOut);
                    break;
                default:
                    throw std::runtime_error("Invalid CB operation");
            }

            setReg8(r, value);
            break;
        }
        case 1: { // BIT b, r
            const uint8_t value = getReg8(r);
            const bool bitSet = ((value >> y) & 0x01) != 0;
            const bool carry = this->flags.carry || ((this->registers[static_cast<uint8_t>(RegsID::F)] & 0x10) != 0);
            writeFlags(!bitSet, false, true, carry);
            break;
        }
        case 2: { // RES b, r
            uint8_t value = getReg8(r);
            value = static_cast<uint8_t>(value & ~(1u << y));
            setReg8(r, value);
            break;
        }
        case 3: { // SET b, r
            uint8_t value = getReg8(r);
            value = static_cast<uint8_t>(value | (1u << y));
            setReg8(r, value);
            break;
        }
        default:
            throw std::runtime_error("Invalid CB opcode group");
    }
}

// 8-Bit Arithmetic (Matches against Register A)
void CPU::add8(uint8_t value){
    const uint8_t a = this->registers[static_cast<uint8_t>(RegsID::A)];
    const uint16_t result = static_cast<uint16_t>(a) + static_cast<uint16_t>(value);

    this->registers[static_cast<uint8_t>(RegsID::A)] = static_cast<uint8_t>(result);

    this->flags.zero = (static_cast<uint8_t>(result) == 0);
    this->flags.subtraction = false;
    this->flags.halfCarry = ((a & 0x0F) + (value & 0x0F)) > 0x0F;
    this->flags.carry = result > 0xFF;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

void CPU::adc8(uint8_t value){
    const uint8_t a = this->registers[static_cast<uint8_t>(RegsID::A)];
    const uint8_t carryIn = static_cast<uint8_t>(
        this->flags.carry || ((this->registers[static_cast<uint8_t>(RegsID::F)] & 0x10) != 0)
    );
    const uint16_t result = static_cast<uint16_t>(a) + static_cast<uint16_t>(value) + carryIn;

    this->registers[static_cast<uint8_t>(RegsID::A)] = static_cast<uint8_t>(result);

    this->flags.zero = (static_cast<uint8_t>(result) == 0);
    this->flags.subtraction = false;
    this->flags.halfCarry = ((a & 0x0F) + (value & 0x0F) + carryIn) > 0x0F;
    this->flags.carry = result > 0xFF;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

void CPU::sub8(uint8_t value){
    const uint8_t a = this->registers[static_cast<uint8_t>(RegsID::A)];
    const uint8_t result = static_cast<uint8_t>(a - value);

    this->registers[static_cast<uint8_t>(RegsID::A)] = result;

    this->flags.zero = (result == 0);
    this->flags.subtraction = true;
    this->flags.halfCarry = (a & 0x0F) < (value & 0x0F);
    this->flags.carry = a < value;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

void CPU::sbc8(uint8_t value){
    const uint8_t a = this->registers[static_cast<uint8_t>(RegsID::A)];
    const uint8_t carryIn = static_cast<uint8_t>(
        this->flags.carry || ((this->registers[static_cast<uint8_t>(RegsID::F)] & 0x10) != 0)
    );
    const uint16_t rhs = static_cast<uint16_t>(value) + carryIn;
    const uint16_t result = static_cast<uint16_t>(a) - rhs;
    const uint8_t result8 = static_cast<uint8_t>(result);

    this->registers[static_cast<uint8_t>(RegsID::A)] = result8;

    this->flags.zero = (result8 == 0);
    this->flags.subtraction = true;
    this->flags.halfCarry = (a & 0x0F) < ((value & 0x0F) + carryIn);
    this->flags.carry = static_cast<uint16_t>(a) < rhs;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

// 8-Bit Logical Operations
void CPU::and8(uint8_t value){
    const uint8_t result = this->registers[static_cast<uint8_t>(RegsID::A)] & value;
    this->registers[static_cast<uint8_t>(RegsID::A)] = result;

    this->flags.zero = (result == 0);
    this->flags.subtraction = false;
    this->flags.halfCarry = true;
    this->flags.carry = false;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

void CPU::or8(uint8_t value){
    const uint8_t result = this->registers[static_cast<uint8_t>(RegsID::A)] | value;
    this->registers[static_cast<uint8_t>(RegsID::A)] = result;

    this->flags.zero = (result == 0);
    this->flags.subtraction = false;
    this->flags.halfCarry = false;
    this->flags.carry = false;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

void CPU::xor8(uint8_t value){
    const uint8_t result = this->registers[static_cast<uint8_t>(RegsID::A)] ^ value;
    this->registers[static_cast<uint8_t>(RegsID::A)] = result;

    this->flags.zero = (result == 0);
    this->flags.subtraction = false;
    this->flags.halfCarry = false;
    this->flags.carry = false;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

void CPU::cp8(uint8_t value){
    const uint8_t a = this->registers[static_cast<uint8_t>(RegsID::A)];
    const uint8_t result = static_cast<uint8_t>(a - value);

    this->flags.zero = (result == 0);
    this->flags.subtraction = true;
    this->flags.halfCarry = (a & 0x0F) < (value & 0x0F);
    this->flags.carry = a < value;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

// Single Register Modification
void CPU::inc8(const RegsID id){
    if(id < RegsID::A || id > RegsID::L){
        throw std::runtime_error("inc8 only supports 8-bit CPU registers\n");
    }

    const uint8_t index = static_cast<uint8_t>(id);
    const uint8_t oldValue = this->registers[index];
    const uint8_t result = static_cast<uint8_t>(oldValue + 1);
    const bool oldCarry = this->flags.carry;

    this->registers[index] = result;

    this->flags.zero = (result == 0);
    this->flags.subtraction = false;
    this->flags.halfCarry = ((oldValue & 0x0F) + 1) > 0x0F;
    this->flags.carry = oldCarry;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

void CPU::dec8(const RegsID id){
    if(id < RegsID::A || id > RegsID::L){
        throw std::runtime_error("dec8 only supports 8-bit CPU registers\n");
    }

    const uint8_t index = static_cast<uint8_t>(id);
    const uint8_t oldValue = this->registers[index];
    const uint8_t result = static_cast<uint8_t>(oldValue - 1);
    const bool oldCarry = this->flags.carry;

    this->registers[index] = result;

    this->flags.zero = (result == 0);
    this->flags.subtraction = true;
    this->flags.halfCarry = (oldValue & 0x0F) == 0;
    this->flags.carry = oldCarry;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

void CPU::add16(uint16_t value){
    const uint16_t hl = this->readRegister(RegsID::HL);
    const uint32_t result = static_cast<uint32_t>(hl) + static_cast<uint32_t>(value);
    const bool oldZero = this->flags.zero;

    this->writeRegister(RegsID::HL, static_cast<uint16_t>(result));

    this->flags.zero = oldZero;
    this->flags.subtraction = false;
    this->flags.halfCarry = ((hl & 0x0FFF) + (value & 0x0FFF)) > 0x0FFF;
    this->flags.carry = result > 0xFFFF;

    this->registers[static_cast<uint8_t>(RegsID::F)] =
        (static_cast<uint8_t>(this->flags.zero) << 7) |
        (static_cast<uint8_t>(this->flags.subtraction) << 6) |
        (static_cast<uint8_t>(this->flags.halfCarry) << 5) |
        (static_cast<uint8_t>(this->flags.carry) << 4);
}

void CPU::inc16(const RegsID id){
    if(id != RegsID::BC && id != RegsID::DE && id != RegsID::HL && id != RegsID::SP){
        throw std::runtime_error("inc16 only supports BC, DE, HL or SP\n");
    }

    const uint16_t oldValue = this->readRegister(id);
    this->writeRegister(id, static_cast<uint16_t>(oldValue + 1));
}

void CPU::dec16(const RegsID id){
    if(id != RegsID::BC && id != RegsID::DE && id != RegsID::HL && id != RegsID::SP){
        throw std::runtime_error("dec16 only supports BC, DE, HL or SP\n");
    }

    const uint16_t oldValue = this->readRegister(id);
    this->writeRegister(id, static_cast<uint16_t>(oldValue - 1));
}

void CPU::jump(bool condition){
    const uint16_t address = this->mem->readMemory16(this->programCounter);
    this->programCounter += 2;

    if(condition){
        this->programCounter = address;
    }
}

void CPU::call(bool condition){
    const uint16_t address = this->mem->readMemory16(this->programCounter);
    this->programCounter += 2;

    if(condition){
        this->stackPointer -= 2;
        this->mem->writeMemory16(this->stackPointer, this->programCounter);
        this->programCounter = address;
    }
}

void CPU::ret(bool condition){
    if(condition){
        this->programCounter = this->mem->readMemory16(this->stackPointer);
        this->stackPointer += 2;
    }
}
