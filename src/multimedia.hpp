#pragma once
#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_mixer.h>

struct SoundEffects {
    Mix_Music* bgMusic{};
    Mix_Chunk* ufoSound{};
    Mix_Chunk* playerShoot{};
    Mix_Chunk* playerDeath{};
    Mix_Chunk* invaderDeath{};
    Mix_Chunk* fleetMovement1{};
    Mix_Chunk* fleetMovement2{};
    Mix_Chunk* fleetMovement3{};
    Mix_Chunk* fleetMovement4{};
    Mix_Chunk* ufoHit{};
    Mix_Chunk* insertCoin{};
    Mix_Chunk* gameStart{};
};

struct VisualEffects {
  SDL_Window* window{};
  SDL_Renderer* renderer{};
};

class Multimedia
{
  public:
    bool isPaused;
    Multimedia(char const* title);
    ~Multimedia();
    void UpdateDisplay(uint8_t* buffer);
    bool HandleEvents(uint8_t* port1, uint8_t* port2);
    void ToggleMusic();
    void PlayPortSounds(uint8_t value, uint8_t prevValue, int port);
  private:
    struct VisualEffects vfx;
    struct SoundEffects sfx;
    int ufoChannel;
    bool isFullscreen;
    bool isMinimized;
    bool InitVideo(char const* title);
    bool InitAudio();
};