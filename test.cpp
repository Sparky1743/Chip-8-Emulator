#include <stdio.h>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) { 
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error: %s\n", SDL_GetError());
        return 0;
    }
    
    printf("no error in importing SDL\n");
    
    SDL_Quit();
    return 0;
}
