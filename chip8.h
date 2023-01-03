#ifndef _CHIP8_
#define _CHIP8_

#include "typedef.h"

void Reset();                        // Reset
bool_t LoadFile(const char *file);   // Load Rom
U16 FetchOpcode();                   // Fetch   Istruction
void ExecuteCpu();                   // Execute Instruction
void ExecuteVideo();                 // Execute Gpu
void ExecuteKeys(SDL_Event event);  // Execute Keys
void printreg();

void Op00e0(); // Clear the screen
void Op00ee(); // Return from a subroutine
void Op0nnn(); // Execute machine language subroutine at address NNN
void Op1nnn(); // Jump to address NNN
void Op2nnn(); // Execute subroutine starting at address NNN
void Op3xnn(); // Skip the following instruction if the value of register VX equals NN
void Op4xnn(); // Skip the following instruction if the value of register VX is not equal to NN
void Op5xy0(); // Skip the following instruction if the value of register VX is equal to the value of register VY
void Op6xnn(); // Store number NN in register VX
void Op7xnn(); // Add the value NN to register VX
void Op8xy0(); // Store the value of register VY in register VX
void Op8xy1(); // Set VX to VX OR VY
void Op8xy2(); // Set VX to VX AND VY
void Op8xy3(); // Set VX to VX XOR VY
void Op8xy4(); // Add the value of register VY to register VX Set VF to 01 if a carry occurs Set VF to 00 if a carry does not occur
void Op8xy5(); // Subtract the value of register VY from register VX Set VF to 00 if a borrow occurs Set VF to 01 if a borrow does not occur
void Op8xy6(); // Store the value of register VY shifted right one bit in register VX¹ Set register VF to the least significant bit prior to the shift VY is unchanged
void Op8xy7(); // Set register VX to the value of VY minus VX Set VF to 00 if a borrow occurs Set VF to 01 if a borrow does not occur
void Op8xye(); // Store the value of register VY shifted left one bit in register VX¹ Set register VF to the most significant bit prior to the shift VY is unchanged
void Op9xy0(); // Skip the following instruction if the value of register VX is not equal to the value of register VY
void Opannn(); // Store memory address NNN in register I
void Opbnnn(); // Jump to address NNN + V0
void Opcxnn(); // Set VX to a random number with a mask of NN
void Opdxyn(); // Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
void Opex9e(); // Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
void Opexa1(); // Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
void Opfx07(); // Store the current value of the delay timer in register VX
void Opfx0a(); // Wait for a keypress and store the result in register VX
void Opfx15(); // Set the delay timer to the value of register VX
void Opfx18(); // Set the sound timer to the value of register VX
void Opfx1e(); // Add the value stored in register VX to register I
void Opfx29(); // Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX
void Opfx33(); // Store the binary-coded decimal equivalent of the value stored in register VX at addresses I, I + 1, and I + 2
void Opfx55(); // Store the values of registers V0 to VX inclusive in memory starting at address I I is set to I + X + 1 after Operation²
void Opfx65(); // Fill registers V0 to VX inclusive with the values stored in memory starting at address I I is set to I + X + 1 after Operation²

#endif