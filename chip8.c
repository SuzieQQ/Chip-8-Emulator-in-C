#include "chip8.h"

#define N   (currOp & 0x000F) // N:   The fourth nibble. A 4-bit number.
#define NN  (currOp & 0x00FF) // NN:  The second byte (third and fourth nibbles). An 8-bit immediate number.
#define NNN (currOp & 0x0FFF) // NNN: The second, third and fourth nibbles. A 12-bit immediate memory address.

U16 pc = 0x0000;        // Progmemory Counter
U16 regI = 0x0000;      // Register Index
U16 sp = 0x0000;        // Stack  Pointer
U16 currOp = 0x0000;    // Opcode
U8 delayTimer = 0x00;   // Delay Timer
U8 soundTimer = 0x00;   // Sound Timer
U8 regV[16];            // Register VXXX
U16 stackLevel[16];     // Level Stack
U8 memory[0x1000];      // Memory Ram 4 Kilobyte
U32 pixel[0x800];       // Pixels
U8 gpu[0x800];          // 64x32 Monochrome Display Memory
U16 keys[16];           // Input Keys
void *PtrPIXEL = pixel; // For  Help
bool_t keyPressed = FALSE;

U8 fontSet[0x50] = // FontSet
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
/*
U32 Keymap[] = {

    SDLK_m,
    SDLK_RIGHT,
    SDLK_DOWN,
    SDLK_3,
    SDLK_LEFT,
    SDLK_a,
    SDLK_RIGHT,
    SDLK_w,
    SDLK_UP,
    SDLK_d,
    SDLK_n,
    SDLK_c,
    SDLK_z,
    SDLK_x,
    SDLK_f,
    SDLK_v,
};
*/

void Reset()
{
   pc = 0x200; // Set Progmemory Counter;

   currOp = 0x0000;

   regI = 0x00; // Reset Register I;

   sp = 0x00; // Reset Register SP;

   delayTimer = 0x00; // Reset Timer;

   soundTimer = 0x00; // Reset Sound;

   memset(regV, 0, sizeof(regV)); // Reset Register V

   memset(keys, 0, sizeof(keys)); // Reset Input Keys

   memset(memory, 0, sizeof(memory)); // Reset memory

   memset(stackLevel, 0, sizeof(stackLevel)); // Reset Level Stack Pointer

   memcpy(memory, fontSet, 0x50); // Load Fontset
}

bool_t LoadFile(const char *file) // Load Rom
{

   FILE *fp = fopen(file, "rb"); // Open Rom in binary;

   if (fp == NULL) // if fp is NULL error
   {
      perror("Rom not open check your path game ... ");
      exit(EXIT_FAILURE);
      return FALSE;
   }

   fseek(fp, 0, SEEK_END); // Set Position File BYTE 0

   long size = ftell(fp); // Size Rom;

   fseek(fp, 0, SEEK_SET); // Set Position File BYTE 0

   // rewind(fp);

   U8 *buf = malloc(sizeof(U8) * size); // Allocated Memory for Buffer;

   if (buf == NULL) // if buf is NULL error
   {
      perror("Memory error ...");
      return FALSE;
   }

   fread(buf, sizeof(U8), size, fp); // Read Rom

   memcpy(memory + 0x200, buf, size); // Load in memory

   fclose(fp); // Close Rom;
   free(buf);  // Clear Buffer;

   return TRUE;
}

void Op00e0() // Clear the screen
{
   memset(gpu, 0, sizeof(gpu));
}

void Op00ee() // Return from a subroutine
{
   --sp;
   pc = stackLevel[sp];
}

void Op0nnn() // Execute machine language subroutine at address NNN
{
   // NOp
}

void Op1nnn() // Jump to address NNN
{
   pc = NNN;
}

void Op2nnn() // Execute subroutine starting at address NNN
{
   stackLevel[sp] = pc;
   ++sp;
   pc = NNN;
}

void Op3xnn() // Skip the following instruction if the value of register VX equals NN
{
   if (regV[(currOp & 0xF00) >> 8] == NN)
      pc += 2;
}

void Op4xnn() // Skip the following instruction if the value of register VX is not equal to NN
{
   if (regV[(currOp & 0xF00) >> 8] != NN)
      pc += 2;
}

void Op5xy0() // Skip the following instruction if the value of register VX is equal to the value of register VY
{
   if (regV[(currOp & 0xF00) >> 8] == regV[(currOp & 0x0F0) >> 4])
      pc += 2;
}

void Op6xnn() // Store number NN in register VX
{
   regV[(currOp & 0xF00) >> 8] = NN;
}

void Op7xnn() // Add the value NN to register VX
{

   regV[(currOp & 0xF00) >> 8] += NN;
}

void Op8xy0() // Store the value of register VY in register VX
{
   regV[(currOp & 0xF00) >> 8] = regV[(currOp & 0x0F0) >> 4];
}

void Op8xy1() // Set VX to VX OR VY
{
   regV[(currOp & 0xF00) >> 8] |= regV[(currOp & 0x0F0) >> 4];
}

void Op8xy2() // Set VX to VX AND VY
{
   regV[(currOp & 0xF00) >> 8] &= regV[(currOp & 0x0F0) >> 4];
}

void Op8xy3() // Set VX to VX XOR VY
{
   regV[(currOp & 0xF00) >> 8] ^= regV[(currOp & 0x0F0) >> 4];
}

void Op8xy4() // Add the value of register VY to register VX Set VF to 01 if a carry occurs Set VF to 00 if a carry does not occur
{
    regV[(currOp & 0xF00) >> 8] += regV[(currOp & 0x0F0) >> 4];

   U16 C = regV[(currOp & 0x0F0) >> 4] + regV[(currOp & 0xF00) >> 8];

   if (C > 255)
   {
      regV[0x0F] = 1;
   }
   else
   {
      regV[0x0F] = 0;
   }
    

}

void Op8xy5() // Subtract the value of register VY from register VX Set VF to 00 if a borrow occurs Set VF to 01 if a borrow does not occur
{

  
   if (regV[(currOp & 0xF00) >> 8] > regV[(currOp & 0x0F0) >> 4])
   {
      regV[0x0F] = 1;
   }
   else
   {
      regV[0x0F] = 0;
   }

   regV[(currOp & 0xF00) >> 8] -= regV[(currOp & 0x0F0) >> 4];


}

void Op8xy6() // Store the value of register VY shifted right one bit in register VX¹ Set register VF to the least significant bit prior to the shift VY is unchanged
{
   regV[(currOp & 0xF00) >> 8] = regV[(currOp & 0x0F0) >> 4];
   regV[0x0F] = regV[(currOp & 0xF00) >> 8] & 0x01;
   regV[(currOp & 0xF00) >> 8] >>= 1;
}

void Op8xy7() // Set register VX to the value of VY minus VX Set VF to 00 if a borrow occurs Set VF to 01 if a borrow does not occur
{

   if (regV[(currOp & 0xF00) >> 8] > regV[(currOp & 0x0F0) >> 4])
   {
      regV[0x0F] = 1;
   }
   else
   {
      regV[0x0F] = 0;
   }

   regV[(currOp & 0xF00) >> 8] -= regV[(currOp & 0x0F0) >> 4];
}

void Op8xye() // Store the value of register VY shifted left one bit in register VX¹ Set register VF to the most significant bit prior to the shift VY is unchanged
{
   regV[(currOp & 0xF00) >> 8] = regV[(currOp & 0x0F0) >> 4];
   regV[0x0F] = regV[(currOp & 0xF00) >> 8] & 0x80;
   regV[(currOp & 0xF00) >> 8] <<= 1;

}

void Op9xy0() // Skip the following instruction if the value of register VX is not equal to the value of register VY
{
   if (regV[(currOp & 0xF00) >> 8] != regV[(currOp & 0x0F0) >> 4])
      pc += 2;

}

void Opannn() // Store memory address NNN in register I
{
   regI = NNN;
}

void Opbnnn() // Jump to address NNN + V0
{
   pc = NNN + regV[0];
}

void Opcxnn() // Set VX to a random number with a mask of NN
{
   int random = rand() % 0x100;
   regV[(currOp & 0xF00) >> 8] = random & NN;
}

void Opdxyn() // Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
{

   U8 vX = regV[(currOp & 0xF00) >> 8] % 64;
   U8 vY = regV[(currOp & 0x0F0) >> 4] % 32;
   U8 spriteData = N;
   regV[0x0F] = 0x00;

   for (int rows = 0; rows < spriteData; rows++)
   {
      U8 Pixel = memory[regI + rows];

      for (int col = 0; col < 8; col++)
      {
         U16 index = (col + vX + (rows + vY) * 64);

         if (Pixel & (0x80 >> col))
         {
            if (gpu[index] == 1)
            {
               regV[0x0F] = 1;
            }

            gpu[index] ^= 1;
         }
      }
   }
   
}

void Opex9e() // Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
{
   U8 regVX = regV[(currOp & 0xF00) >> 8];

   if (keys[regVX] != 0)
   {
      pc += 2;
   }
}

void Opexa1() // Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
{
   U8 regVX = regV[(currOp & 0xF00) >> 8];

   if (keys[regVX] == 0)
   {
      pc += 2;
   }
}

void Opfx07() // Store the current value of the delay timer in register VX
{
   regV[(currOp & 0xF00) >> 8] = delayTimer;
}

void Opfx0a() // Wait for a keypress and store the result in register VX
{
   for (int i = 0; i < 16; i++)
   {
      regV[(currOp & 0xF00) >> 8] = i;
      keyPressed = TRUE;
   }
}

void Opfx15() // Set the delay timer to the value of register VX
{
   delayTimer = regV[(currOp & 0xF00) >> 8];
}

void Opfx18() // Set the sound timer to the value of register VX
{
   soundTimer = regV[(currOp & 0xF00) >> 8];
}

void Opfx1e() // Add the value stored in register VX to register I
{
   regI += regV[(currOp & 0xF00) >> 8];
}

void Opfx29() // Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX
{
   regI = regV[(currOp & 0xF00) >> 8] * 0x05;
}

void Opfx33() // Store the binary-coded decimal equivalent of the value stored in register VX at addresses I, I + 1, and I + 2
{
   U8 regVX = regV[(currOp & 0xF00) >> 8]; // VX binary % 10 for number binary
   memory[regI]     = (regVX % 1000 / 100);
   memory[regI + 1] = (regVX % 100 / 10);
   memory[regI + 2] = (regVX % 10);
}

void Opfx55() // Store the values of registers V0 to VX inclusive in memory starting at address I I is set to I + X + 1 after Operation²
{
   U8 regVX = regV[(currOp & 0xF00) >> 8];

   for (int i = 0; i <= regVX; i++)
   {
      memory[regI + i] = regV[i];
   }
}

void Opfx65() // Fill registers V0 to VX inclusive with the values stored in memory starting at address I I is set to I + X + 1 after Operation²
{
   U8 regVX = regV[(currOp & 0xF00) >> 8];

   for (int j = 0; j <= regVX; j++)
   {
      regV[j] = memory[regI + j];
   }
}

U16 FetchOpcode() // Fetch Instruction
{
   U8 lsb = memory[pc];     // lsb
   U8 msb = memory[pc + 1]; // msb
   return lsb << 8 | msb;
}

void ExecuteCpu() // Execute Instruction
{
   currOp = FetchOpcode();
   pc += 2;

   switch (currOp & 0xF000)
   {

   // 00E_
   case 0x0000:

      switch (currOp & 0x000F)
      {
      case 0x0000:
         Op00e0();
         break;
      case 0x000E:
         Op00ee();
         break;
      default:
         printf("\nUnknown Op code 1: %.4X\n", currOp);
         exit(EXIT_FAILURE);
      }
      break;

   case 0x1000:
      Op1nnn();
      break;
   case 0x2000:
      Op2nnn();
      break;
   case 0x3000:
      Op3xnn();
      break;
   case 0x4000:
      Op4xnn();
      break;
   case 0x5000:
      Op5xy0();
      break;
   case 0x6000:
      Op6xnn();
      break;
   case 0x7000:
      Op7xnn();
      break;

   // 8XY_
   case 0x8000:
      switch (currOp & 0x000F)
      {

      case 0x0000:
         Op8xy0();
         break;

      case 0x0001:
         Op8xy1();
         break;
      case 0x0002:
         Op8xy2();
         break;
      case 0x0003:
         Op8xy3();
         break;
      case 0x0004:
         Op8xy4();
         break;
      case 0x0005:
         Op8xy5();
         break;
      case 0x0006:
         Op8xy6();
         break;
      case 0x0007:
         Op8xy7();
         break;
      case 0x000E:
         Op8xye();
         break;
      default:
         printf("\nUnknown Op code 2: %.4X\n", currOp);
         exit(EXIT_FAILURE);
      }
      break;

   case 0x9000:
      Op9xy0();
      break;
   case 0xA000:
      Opannn();
      break;
   case 0xB000:
      Opbnnn();
      break;
   case 0xC000:
      Opcxnn();
      break;
   case 0xD000:
      Opdxyn();
      break;

   // EX__
   case 0xE000:

      switch (currOp & 0x00FF)
      {
      case 0x00A1:
         Opexa1();
         break;
      case 0x009E:
         Opex9e();
         break;

      default:
         printf("\nUnknown Op code 3: %.4X\n", currOp);
         exit(3);
      }
      break;

   // FX__
   case 0xF000:
      switch (currOp & 0x00FF)
      {
      case 0x0007:
         Opfx07();
         break;
      case 0x000A:
         Opfx0a();
         break;
      case 0x0015:
         Opfx15();
         break;
      case 0x0018:
         Opfx18();
         break;
      case 0x001E:
         Opfx1e();
         break;
      case 0x0029:
         Opfx29();
         break;
      case 0x0033:
         Opfx33();
         break;
      case 0x0055:
         Opfx55();
         break;
      case 0x0065:
         Opfx65();
         break;
      default:
         printf("Unknown Opcode [0xF000]: 0x%X\n", currOp);
      }
      break;

   default:
      printf("\nUnimplemented Op code 4: %.4X\n", currOp);
      exit(3);
   }

   if (delayTimer > 0)
   {
      delayTimer--;
   }

   if (soundTimer > 0)
   {
      if (soundTimer == 1)
      {
         //  playSound = true;
      }

      soundTimer--;
   }
}

void ExecuteVideo()
{
   // write value in to pixels
   for (int i = 0; i < 0x800; ++i)
   {
      pixel[i] = (0x00FFFFFF * gpu[i]) | 0xFF000000;
   }
}

void ExecuteKeys(SDL_Event event)
{
   SDL_PollEvent(&event);

   if (event.type == SDL_QUIT)
   {
      exit(EXIT_SUCCESS);
   }

   switch (event.key.keysym.sym)
   {
   case SDLK_m:

      if (event.key.type == SDL_KEYUP)
      {
         keys[0] = 0;
      }
      else
      {
         keys[0] = 1;
      }

      break;
   case SDLK_RIGHT:

      if (event.key.type == SDL_KEYUP)
      {
         keys[1] = 0;
      }
      else
      {
         keys[1] = 1;
      }

      break;
   case SDLK_DOWN:
      if (event.key.type == SDL_KEYUP)
      {
         keys[2] = 0;
      }
      else
      {
         keys[2] = 1;
      }

      break;
   case SDLK_3:
      if (event.key.type == SDL_KEYUP)
      {
         keys[3] = 0;
      }
      else
      {
         keys[3] = 1;
      }

      break;
   case SDLK_LEFT:
      if (event.key.type == SDL_KEYUP)
      {
         keys[4] = 0;
      }
      else
      {
         keys[4] = 1;
      }
      break;
   case SDLK_a:
      if (event.key.type == SDL_KEYUP)
      {
         keys[5] = 0;
      }
      else
      {
         keys[5] = 1;
      }
      break;
   case SDLK_w:
      if (event.key.type == SDL_KEYUP)
      {
         keys[6] = 0;
      }
      else
      {
         keys[6] = 1;
      }
      break;
   case SDLK_UP:
      
      if (event.key.type == SDL_KEYUP)
      {
         keys[7] = 0;
      }
      else
      {
         keys[7] = 1;
      }
      break;
   case SDLK_d:
      if (event.key.type == SDL_KEYUP)
      {
         keys[8] = 0;
      }
      else
      {
         keys[8] = 1;
      }

      break;
   case SDLK_n:
      
      if (event.key.type == SDL_KEYUP)
      {
         keys[9] = 0;
      }
      else
      {
         keys[9] = 1;
      }
      break;
   case SDLK_c:
      if (event.key.type == SDL_KEYUP)
      {
         keys[10] = 0;
      }
      else
      {
         keys[10] = 1;
      }
      break;
   case SDLK_z:
      if (event.key.type == SDL_KEYUP)
      {
         keys[11] = 0;
      }
      else
      {
         keys[11] = 1;
      }
      break;
   case SDLK_x:
         if (event.key.type == SDL_KEYUP)
      {
         keys[12] = 0;
      }
      else
      {
         keys[12] = 1;
      }
      break;
   case SDLK_f:
     if (event.key.type == SDL_KEYUP)
      {
         keys[13] = 0;
      }
      else
      {
         keys[13] = 1;
      }
      break;
   case SDLK_v:
         if (event.key.type == SDL_KEYUP)
      {
         keys[14] = 0;
      }
      else
      {
         keys[14] = 1;
      }
      break;
         case SDLK_RETURN:
         if (event.key.type == SDL_KEYUP)
      {
         keys[15] = 0;
      }
      else
      {
         keys[15] = 1;
      }
      break;
   default:
      break;
   }

   /*

        for (int i = 0; i < 16; ++i)
       {
         if (event.key.keysym.sym == Keymap[i])
         {
             keys[i] = 1;
         }
       }

       if (event.key.type == SDL_KEYUP)
       {
         for (int i = 0; i < 16; ++i)
         {
           if (event.key.keysym.sym == Keymap[i])
           {
             keys[i] = 0;
           }
         }
       }

   */
}

void printreg()
{
   // FILE* fp = fOpen("registers.log","a+");
   // fprintf(fp,"pc: 0x%X I:0x%X\n ",pc,I);
   //  printf("pc : 0x%X I : 0x%X\n ",pc,I);

   int j = 0;
   int x = 0;

   for (int i = 0; i < sizeof(memory); i++)
   {
      printf("%x ", memory[i]);

      if (j == 32)
      {
         j = 0;

         puts(" ");

         x += 1;

         if (x == 16)
         {
            system("clear");
         }
      }

      j++;
   }

   /*
   for (int i = 0; i < 16; i++)
   {
         printf(" v[%d] %x ",i,REGV[i]);

          if( j == 4)
          {
             j = 0;
             puts(" ");
            // system("clear");
          }

          j++;
   }
 */
}
