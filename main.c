#include "chip8.h"

#define W 640
#define H 480
extern void *PtrPIXEL;
extern bool_t DEBUGMODE;

int main(int argc, char *argv[])
{

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    return -1;
  }else
  {

  SDL_Window *window = SDL_CreateWindow("Chip8-Emulator in C",
                                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H,
                                        SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

  SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
   
  SDL_Event event;

  if (strcmp(argv[0],"-"))
  {
    char *pathGame = argv[1];
    Reset();
    LoadFile(pathGame);
    printf("PRESS F10 FOR ENTER DEBUG MODE \n");
  }



while (TRUE)
{

      if(!DEBUGMODE)
      {
       ExecuteCpu();
       ExecuteVideo();
       ExecuteKeys(event);
      }else
      {
       DebugInstr(event);
       ExecuteVideo();
       ExecuteKeys(event);
      }


    // Update
    SDL_UpdateTexture(texture, NULL, PtrPIXEL, 64 * sizeof(U32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_Delay(10);
  }

}
  exit(EXIT_SUCCESS);
}