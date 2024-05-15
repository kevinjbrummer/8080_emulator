#include "display.hpp"
#include <SDL2/SDL.h>
#include <stdint.h>

Display::Display(char const* title)
{
  screenWidth = 224;
  screenHeight = 256;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth * 3, screenHeight * 3, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

Display::~Display()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Display::Update(uint8_t* buffer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_Rect* destRect = new SDL_Rect;
  destRect->x = 0;
  destRect->y = 0;
  destRect->w = 3;
  destRect->h = 3;


  for (int x = 0; x < screenWidth; x++)
  {
    for (int y = 0; y < (screenHeight/8); y++)
    {
      int offset = (x * (screenHeight/8) + y);
      uint8_t byte = buffer[offset];
      for (int i = 7; i >= 0; i--)
      {
        uint8_t bit = (byte >> i) & 0x1;
        if (bit == 1)
        {
          destRect->y = ((screenHeight - (y*8) + (7 - i)))  * 3;
          destRect->x = x * 3;
          SDL_RenderFillRect(renderer, destRect);
          
        }
      }
    }
  }

  delete destRect;

  SDL_RenderPresent(renderer);
}

bool Display::ProcessInput()
{
  bool quit = false;
  SDL_Event e;

  while (SDL_PollEvent(&e))
  {
    switch(e.type)
    {
      case SDL_QUIT:
        {
          quit = true;
        }
        break;
      case SDL_KEYDOWN:
        {
          switch (e.key.keysym.sym)
          {
            case SDLK_ESCAPE: quit = true; break;
            default: break;
          }
        }
        break;
    }
  }

  return quit;
}