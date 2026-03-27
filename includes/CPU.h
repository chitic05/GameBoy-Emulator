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
        Memory* mem;
        uint16_t programCounter;
        uint16_t stackPointer;
        struct Flags{
            bool carry;
            bool halfCarry;
            bool subtraction;
            bool zero;
        }flags;

    public:
        CPU(Memory* mem);
        const uint16_t readRegister(const RegsID id) const;
        void writeRegister(const RegsID id,const uint16_t value);
        void push(const RegsID id);
        void pop(const RegsID id);

        uint8_t fetch();
        void execute(uint8_t opcode);
        void executeCB(uint8_t opcode);

        // 8-Bit Arithmetic (Matches against Register A)
        void add8(uint8_t value); // Add
        void adc8(uint8_t value); // Add + the current Carry flag
        void sub8(uint8_t value); // Subtract
        void sbc8(uint8_t value); // Subtract + the current Carry flag

        // 8-Bit Logical Operations
        void and8(uint8_t value); // Bitwise AND
        void or8(uint8_t value);  // Bitwise OR
        void xor8(uint8_t value); // Bitwise XOR
        void cp8(uint8_t value);  // Compare (Exactly like sub8, but doesn't save the result!)

        // Single Register Modification
        void inc8(const RegsID id); // Increment a register by 1
        void dec8(const RegsID id); // Decrement a register by 1

        void add16(uint16_t value);  // Adds value specifically to HL
        void inc16(const RegsID id); // Increment 16-bit register pair (e.g., BC++)
        void dec16(const RegsID id); // Decrement 16-bit register pair

        void jump(bool condition); // Changes PC to a new 16-bit address
        void call(bool condition); // Pushes current PC to stack, then jumps
        void ret(bool condition);  // Pops 16-bit address from stack into PC
};