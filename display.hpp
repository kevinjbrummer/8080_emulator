#include <SDL2/SDL.h>
#include <stdint.h>
#include <SDL2/SDL_mixer.h>

class Display
{
  public:
    int screenWidth;
    int screenHeight;
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};

    Mix_Chunk* ufoSound{};
    int ufoChannel;
    Mix_Chunk* playerShoot{};
    Mix_Chunk* playerDeath{};
    Mix_Chunk* invaderDeath{};
    Mix_Chunk* fleetMovement1{};
    Mix_Chunk* fleetMovement2{};
    Mix_Chunk* fleetMovement3{};
    Mix_Chunk* fleetMovement4{};
    Mix_Chunk* ufoHit{};

    Mix_Music* bgMusic{};

    Display(char const* title);
    ~Display();
    void Update(uint8_t* buffer);
    bool ProcessInput(uint8_t* port1, uint8_t* port2);
    void ToggleMusic();
    void PlayPort3Sounds(uint8_t port3, uint8_t prevPort3);
    void PlayPort5Sounds(uint8_t port5, uint8_t prevPort5);
};