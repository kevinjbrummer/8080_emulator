#include "display.hpp"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_mixer.h>

Display::Display(char const* title)
{
  screenWidth = 224;
  screenHeight = 256;
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth * 3, screenHeight * 3, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

  bgMusic = Mix_LoadMUS("./sound/bgMusic02.mp3");
  ufoSound = Mix_LoadWAV("./sound/0.wav");
  playerShoot = Mix_LoadWAV("./sound/1.wav");
  playerDeath = Mix_LoadWAV("./sound/2.wav");
  invaderDeath = Mix_LoadWAV("./sound/3.wav");
  fleetMovement1 = Mix_LoadWAV("./sound/4.wav");
  fleetMovement2 = Mix_LoadWAV("./sound/5.wav");
  fleetMovement3 = Mix_LoadWAV("./sound/6.wav");
  fleetMovement4 = Mix_LoadWAV("./sound/7.wav");
  ufoHit = Mix_LoadWAV("./sound/8.wav");
  insertCoin = Mix_LoadWAV("./sound/insertCoin.wav");


}

Display::~Display()
{
  Mix_FreeMusic(bgMusic);
  Mix_FreeChunk(ufoSound);
  Mix_FreeChunk(playerShoot);
  Mix_FreeChunk(playerDeath);
  Mix_FreeChunk(invaderDeath);
  Mix_FreeChunk(fleetMovement1);
  Mix_FreeChunk(fleetMovement2);
  Mix_FreeChunk(fleetMovement3);
  Mix_FreeChunk(fleetMovement4);
  Mix_FreeChunk(insertCoin);
  Mix_FreeChunk(ufoHit);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  Mix_Quit();
  SDL_Quit();
}

void Display::Update(uint8_t* buffer)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
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
          if (y*8 >= 10 && y*8 <= 60)
          {
            SDL_SetRenderDrawColor(renderer, 57, 255, 20, 255); //green
          }
          else
          {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //white
          }
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

bool Display::ProcessInput(uint8_t* port1, uint8_t* port2)
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
            case SDLK_RETURN:
              *port1 |= 0x1;
              Mix_PlayChannel(-1, insertCoin, 0);
              break;
            case SDLK_2: *port1 |= 0x2; break;
            case SDLK_1: *port1 |= 0x4; break;
            case SDLK_w: *port1 |= 0x10; break;
            case SDLK_a: *port1 |= 0x20; break;
            case SDLK_d: *port1 |= 0x40; break;

            case SDLK_UP: *port2 |= 0x10; break;
            case SDLK_LEFT: *port2 |= 0x20; break;
            case SDLK_RIGHT: *port2 |= 0x40; break;
            default: break;
          }
        }
        break;
      case SDL_KEYUP:
        {
          switch (e.key.keysym.sym)
          {
            case SDLK_RETURN: *port1 &= ~0x1; break;
            case SDLK_2: *port1 &= ~0x2; break;
            case SDLK_1: *port1 &= ~0x4; break;
            case SDLK_w: *port1 &= ~0x10; break;
            case SDLK_a: *port1 &= ~0x20; break;
            case SDLK_d: *port1 &= ~0x40; break;

            case SDLK_UP: *port2 &= ~0x10; break;
            case SDLK_LEFT: *port2 &= ~0x20; break;
            case SDLK_RIGHT: *port2 &= ~0x40; break;
            default: break;
          }
        }
        break;
    }
  }

  return quit;
}

void Display::ToggleMusic()
{
  if (!Mix_PlayingMusic())
  {
    Mix_PlayMusic(bgMusic, -1);
  }
  else
  {
    if (Mix_PausedMusic())
    {
      Mix_ResumeMusic();
    }
    else
    {
      Mix_PauseMusic();
    }
  }
}

void Display::PlayPort3Sounds(uint8_t port3, uint8_t prevPort3)
{

  if ((port3 & 0x1) && !(prevPort3 & 0x1))
  {
    ufoChannel = Mix_PlayChannel(-1, ufoSound, -1);
  }
  else if (!(port3 & 0x1) && (prevPort3 & 0x1))
  {
    Mix_HaltChannel(ufoChannel);
  }

  if ((port3 & 0x2) && !(prevPort3 & 0x2))
  {
    Mix_PlayChannel(-1, playerShoot, 0);
  }

  if ((port3 & 0x4) && !(prevPort3 & 0x4))
  {
    Mix_PlayChannel(-1, playerDeath, 0);
  }

  if ((port3 & 0x8) && !(prevPort3 & 0x8))
  {
    Mix_PlayChannel(-1, invaderDeath, 0);
  }
}

void Display::PlayPort5Sounds(uint8_t port5, uint8_t prevPort5)
{
  if ((port5 & 0x1) && !(prevPort5 & 0x1))
  {
    Mix_PlayChannel(-1, fleetMovement1, 0);
  }

  if ((port5 & 0x2) && !(prevPort5 & 0x2))
  {
    Mix_PlayChannel(-1, fleetMovement2, 0);
  }

  if ((port5 & 0x4) && !(prevPort5 & 0x4))
  {
    Mix_PlayChannel(-1, fleetMovement3, 0);
  }

  if ((port5 & 0x8) && !(prevPort5 & 0x8))
  {
    Mix_PlayChannel(-1, fleetMovement4, 0);
  }

  if ((port5 & 0x10) && !(prevPort5 & 0x10))
  {
    Mix_PlayChannel(-1, ufoHit, 0);
  }
}