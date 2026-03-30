# CHIP-8 Emulator

A high-fidelity CHIP-8 interpreter and emulator written in C++. This project implements a fully functional CHIP-8 virtual machine capable of running classic 8-bit games and programs from the 1970s and 1980s.

## About CHIP-8

CHIP-8 is an interpreted programming language designed in the 1970s for the COSMAC VIP computer. It runs on a simple virtual machine with 4KB of memory and a 64x32 pixel display. Despite its simplicity, CHIP-8 has become a popular learning platform for understanding emulation and interpreter design.

## Features

- **Full CHIP-8 Instruction Set**: Complete implementation of all 35 CHIP-8 opcodes
- **Graphics Rendering**: SDL3-based display with pixel-perfect rendering
- **Input Handling**: Hexadecimal keypad input (0-F) mapped to standard keyboard
- **Memory Management**: 4KB RAM with proper memory management
- **CPU Emulation**: Accurate CPU cycle timing and register management
- **Extensive ROM Library**: Includes 50+ classic CHIP-8 games and demos

## Building

### Prerequisites

- CMake 3.22 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)
- SDL3 library (included in externals/)

### Build Instructions

```bash
# Navigate to project directory
cd /home/david/Altele/Proiecte/cpp/GameBoy-Emulator

# Create build directory
mkdir -p build && cd build

# Configure with CMake
cmake .. -G Ninja

# Build the project
cmake --build .
```

The compiled executable will be located at `build/Emulator`.

## Usage

### First Run

On first execution, the emulator will create a `roms/` directory in the project root:

```bash
./build/Emulator
```

This will output:
```
Roms directory created. Please copy your roms there!
```

### Running a Game

1. Copy your CHIP-8 ROM files (`.ch8` format) to the `roms/` directory
2. Run the emulator again:

```bash
./build/Emulator
```

3. The emulator will list all available ROMs and prompt you to select one by entering its index number

### Example

After copying ROMs, running the emulator will display:

```
1.Pong.ch8
2.Breakout.ch8
3.Tetris.ch8
Enter the index of the rom you want to use!
```

Enter `1`, `2`, or `3` to select your game.

## Controls

The CHIP-8 hexadecimal keypad is mapped to your keyboard as follows:

```
CHIP-8 Keypad    Keyboard Mapping
    1 2 3 C          1 2 3 4
    4 5 6 D          Q W E R
    7 8 9 E          A S D F
    A 0 B F          Z X C V
```

## Project Structure

```
├── src/
│   ├── main.cpp          # Entry point
│   ├── Emulator.cpp      # Main emulator implementation
│   ├── CPU.cpp           # CPU core and instruction execution
│   └── Memory.cpp        # Memory management
├── includes/
│   ├── Emulator.h        # Emulator class definition
│   ├── CPU.h             # CPU class definition
│   └── Memory.h          # Memory class definition
├── chip8-roms/           # Collection of CHIP-8 ROMs
│   ├── games/            # Playable games
│   ├── demos/            # Technical demos
│   ├── programs/         # Utility programs
│   └── hires/            # High-resolution games (CHIP-8E)
├── externals/
│   └── SDL3/             # SDL3 graphics library
├── CMakeLists.txt        # Build configuration
└── README.md             # This file
```

## Emulator Components

### CPU
Implements the CHIP-8 virtual CPU with:
- 16 general-purpose 8-bit registers (V0-VF)
- 16-bit program counter (PC)
- 8-bit stack pointer
- 16-bit index register (I)
- Delay and sound timers

### Memory
Manages the 4KB (4096 bytes) address space:
- 0x000-0x1FF: Reserved for interpreter
- 0x200-0xFFF: Program ROM space

### Display
64x32 pixel monochrome display rendered using SDL3

## Available ROMs

The emulator includes a comprehensive collection of CHIP-8 software sourced from the [chip8-roms GitHub repository](https://github.com/chip-8/chip-8-archive):

- **Games**: Pong, Breakout, Snake, Tetris, and many more
- **Demos**: Visual demonstrations and technical showcases
- **Programs**: Utilities and educational software
- **High-Resolution**: Games using CHIP-8E extended graphics mode

All ROMs are properly categorized in the `chip8-roms/` directory with documentation.

## Keyboard Shortcuts

- **Close Window**: Quit the emulator

## Performance

The emulator runs at approximately 700 CPU cycles per second with proper timer synchronization to maintain authentic gameplay speeds.

## Known Issues & Limitations

- Display only supports monochrome mode (compatible games work perfectly)
- Audio is not currently implemented
- Some edge-case games may require minor timing adjustments

## Future Enhancements

- [ ] Debugger mode with breakpoints
- [ ] Save/load game states
- [ ] Configurable clock speed
- [ ] CHIP-8E and CHIP-16 extensions
- [ ] ROM library browser UI

## License

This project is provided as-is for educational purposes.

## Contributing

Contributions and bug reports are welcome! Feel free to submit issues or pull requests.

---

**Enjoy playing classic CHIP-8 games!** 🎮
