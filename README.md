# Chip-8-Emulator-in-C
CHIP-8 is an interpretted programming language developed by Joseph Weisbecker in the mid 70s and was initally used on the COSMAC VIP and Telmac 1800 8-bit microcomputers to make game programming easier. CHIP-8 programs are run using a CHIP-8 virtual machine.

This emulator supports a debug menu , enable disable audio , video go to full screen mode ,Pause mode 'emulation,loading games or menu with hotkeys

For reference there are excellent guides example : 
https://austinmorlan.com/posts/chip8_emulator/
Or
https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

to compile the project: 
install library Sdl2
open terminal digit: gcc -o chip8 main.c chip8.c -std=c18 -lSDL2 -Wall -g
