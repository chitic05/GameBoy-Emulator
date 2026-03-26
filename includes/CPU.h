#pragma once
#include <cstdint>
#include "Memory.h"
enum class RegsID : uint8_t{
    A=0,
    B=1,
    C=2,
    D=3,
    E=4,
    F=5,
    H=6,
    L=7,
    AF=8,
    BC=9,
    DE=10,
    HL=11,
    SP=12,
    PC=13
};
class CPU{
    private:
        uint8_t registers[8];
        uint8_t* mem;
        uint16_t programCounter;
        uint16_t stackPointer;
        struct Flags{
            bool carry;
            bool halfCarry;
            bool subtraction;
            bool zero;
        }flags;

    public:
        CPU(uint8_t* mem);
        const uint16_t readRegister(const RegsID id) const;
        void writeRegister(const RegsID id,const uint16_t value);

};