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
};

struct VisualEffects {
  SDL_Window* window{};
  SDL_Renderer* renderer{};
};

class Multimedia
{
  public:
    Multimedia(char const* title);
    ~Multimedia();
    void UpdateDisplay(uint8_t* buffer);
    bool ProcessInput(uint8_t* port1, uint8_t* port2);
    void ToggleMusic();
    void PlayPortSounds(uint8_t value, uint8_t prevValue, int port);
  private:
    VisualEffects vfx;
    SoundEffects sfx;
    int ufoChannel;
    bool InitVideo(char const* title);
    bool InitAudio();
};