#include "Multimedia.hpp"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_mixer.h>

const int SCREEN_WIDTH_BASE = 224;
const int SCREEN_HEIGHT_BASE = 256;

Multimedia::Multimedia(char const* title)
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
  {
    printf("Could not initialize. Error: %s\n", SDL_GetError());
    exit(1);
  }

  if (!InitVideo(title))
  {
    exit(1);
  }

  if (!InitAudio())
  {
    exit(1);
  }

}

bool Multimedia::InitVideo(char const* title)
{
  vfx.window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH_BASE * 3, SCREEN_HEIGHT_BASE * 3, SDL_WINDOW_SHOWN);

  if (vfx.window == NULL)
  {
    printf("Could not create Window. Error: %s\n", SDL_GetError());
    return false;
  }

  vfx.renderer = SDL_CreateRenderer(vfx.window, -1, SDL_RENDERER_ACCELERATED);
  if (vfx.renderer == NULL)
  {
    printf("Could not create renderer. Error: %s\n", SDL_GetError());
    return false;
  }

  return true;
}

bool Multimedia::InitAudio()
{
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
  {
    printf("Could not open audio. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.bgMusic = Mix_LoadMUS("./sound/bgMusic02.mp3");
  if (sfx.bgMusic == NULL)
  {
    printf("Could not Load background music. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.ufoSound = Mix_LoadWAV("./sound/0.wav");
  if (sfx.ufoSound == NULL)
  {
    printf("Could not Load 0.wav. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.playerShoot = Mix_LoadWAV("./sound/1.wav");
  if (sfx.playerShoot == NULL)
  {
    printf("Could not Load 1.wav. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.playerDeath = Mix_LoadWAV("./sound/2.wav");
  if (sfx.playerDeath == NULL)
  {
    printf("Could not Load 2.wav. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.invaderDeath = Mix_LoadWAV("./sound/3.wav");
  if (sfx.invaderDeath == NULL)
  {
    printf("Could not Load 3.wav. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.fleetMovement1 = Mix_LoadWAV("./sound/4.wav");
  if (sfx.fleetMovement1 == NULL)
  {
    printf("Could not Load 4.wav. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.fleetMovement2 = Mix_LoadWAV("./sound/5.wav");
  if (sfx.fleetMovement2 == NULL)
  {
    printf("Could not Load 5.wav. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.fleetMovement3 = Mix_LoadWAV("./sound/6.wav");
  if (sfx.fleetMovement3 == NULL)
  {
    printf("Could not Load 6.wav. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.fleetMovement4 = Mix_LoadWAV("./sound/7.wav");
  if (sfx.fleetMovement4 == NULL)
  {
    printf("Could not Load 7.wav. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.ufoHit = Mix_LoadWAV("./sound/8.wav");
  if (sfx.ufoHit == NULL)
  {
    printf("Could not Load 8.wav. Error: %s\n", SDL_GetError());
    return false;
  }

  sfx.insertCoin = Mix_LoadWAV("./sound/insertCoin.wav");
  if (sfx.insertCoin == NULL)
  {
    printf("Could not Load insertCoin.wav. Error: %s\n", SDL_GetError());
    return false;
  }

  return true;
}

Multimedia::~Multimedia()
{
  Mix_FreeMusic(sfx.bgMusic);
  Mix_FreeChunk(sfx.ufoSound);
  Mix_FreeChunk(sfx.playerShoot);
  Mix_FreeChunk(sfx.playerDeath);
  Mix_FreeChunk(sfx.invaderDeath);
  Mix_FreeChunk(sfx.fleetMovement1);
  Mix_FreeChunk(sfx.fleetMovement2);
  Mix_FreeChunk(sfx.fleetMovement3);
  Mix_FreeChunk(sfx.fleetMovement4);
  Mix_FreeChunk(sfx.insertCoin);
  Mix_FreeChunk(sfx.ufoHit);
  SDL_DestroyRenderer(vfx.renderer);
  SDL_DestroyWindow(vfx.window);
  Mix_Quit();
  SDL_Quit();
}

void Multimedia::Update(uint8_t* buffer)
{
  SDL_SetRenderDrawColor(vfx.renderer, 0, 0, 0, 255);
  SDL_RenderClear(vfx.renderer);
  SDL_Rect* destRect = new SDL_Rect;
  destRect->x = 0;
  destRect->y = 0;
  destRect->w = 3;
  destRect->h = 3;


  for (int x = 0; x < SCREEN_WIDTH_BASE; x++)
  {
    for (int y = 0; y < (SCREEN_HEIGHT_BASE/8); y++)
    {
      int offset = (x * (SCREEN_HEIGHT_BASE/8) + y);
      uint8_t byte = buffer[offset];
      for (int i = 7; i >= 0; i--)
      {
        uint8_t bit = (byte >> i) & 0x1;
        if (bit == 1)
        {
          if (y*8 >= 10 && y*8 <= 60)
          {
            SDL_SetRenderDrawColor(vfx.renderer, 57, 255, 20, 255); //green
          }
          else
          {
            SDL_SetRenderDrawColor(vfx.renderer, 255, 255, 255, 255); //white
          }
          destRect->y = ((SCREEN_HEIGHT_BASE - (y*8) + (7 - i)))  * 3;
          destRect->x = x * 3;
          SDL_RenderFillRect(vfx.renderer, destRect);

        }
      }
    }
  }

  delete destRect;

  SDL_RenderPresent(vfx.renderer);
}

bool Multimedia::ProcessInput(uint8_t* port1, uint8_t* port2)
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
              Mix_PlayChannel(-1, sfx.insertCoin, 0);
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

void Multimedia::ToggleMusic()
{
  if (!Mix_PlayingMusic())
  {
    Mix_PlayMusic(sfx.bgMusic, -1);
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

void Multimedia::PlayPortSounds(uint8_t value, uint8_t prevValue, int port)
{
  if (port == 3)
  {

    if ((value & 0x1) && !(prevValue & 0x1))
    {
      ufoChannel = Mix_PlayChannel(-1, sfx.ufoSound, -1);
    }
    else if (!(value & 0x1) && (prevValue & 0x1))
    {
      Mix_HaltChannel(ufoChannel);
    }

    if ((value & 0x2) && !(prevValue & 0x2))
    {
      Mix_PlayChannel(-1, sfx.playerShoot, 0);
    }

    if ((value & 0x4) && !(prevValue & 0x4))
    {
      Mix_PlayChannel(-1, sfx.playerDeath, 0);
    }

    if ((value & 0x8) && !(prevValue & 0x8))
    {
      Mix_PlayChannel(-1, sfx.invaderDeath, 0);
    }
  }
  else if (port == 5)
  {

    if ((value & 0x1) && !(prevValue & 0x1))
    {
      Mix_PlayChannel(-1, sfx.fleetMovement1, 0);
    }

    if ((value & 0x2) && !(prevValue & 0x2))
    {
      Mix_PlayChannel(-1, sfx.fleetMovement2, 0);
    }

    if ((value & 0x4) && !(prevValue & 0x4))
    {
      Mix_PlayChannel(-1, sfx.fleetMovement3, 0);
    }

    if ((value & 0x8) && !(prevValue & 0x8))
    {
      Mix_PlayChannel(-1, sfx.fleetMovement4, 0);
    }

    if ((value & 0x10) && !(prevValue & 0x10))
    {
      Mix_PlayChannel(-1, sfx.ufoHit, 0);
    }
  }
}