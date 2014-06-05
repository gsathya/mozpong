#include <stdio.h>
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include "pong.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char *args[]){
  SDL_Window* window = NULL; 
  SDL_Surface* screenSurface = NULL;

  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    return 1;
  } 

  //Create window
  window = SDL_CreateWindow( "Ping pong",
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             SCREEN_WIDTH,
                             SCREEN_HEIGHT,
                             SDL_WINDOW_SHOWN );
  if( window == NULL ) {
    printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    return 1;
  } 

  int imgFlags = IMG_INIT_PNG;
  if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
    return 1;
  }
  
  //Get window surface
  screenSurface = SDL_GetWindowSurface( window );

  SDL_Surface* optimizedSurface = NULL;
  SDL_Surface* loadedSurface = IMG_Load("x.png");
  if(loadedSurface == NULL) {
    printf( "Unable to load image x.png! SDL_image Error: %s\n", IMG_GetError() );
    return 1;
  }

  optimizedSurface = SDL_ConvertSurface( loadedSurface, screenSurface->format, NULL );
  if( optimizedSurface == NULL ) {
    printf( "Unable to optimize image x.png! SDL Error: %s\n", SDL_GetError() );
    return 1;
  }

  //Get rid of old loaded surface
  SDL_FreeSurface( loadedSurface ); 

  SDL_BlitSurface(optimizedSurface, NULL, screenSurface, NULL);
  //Update the surface
  SDL_UpdateWindowSurface( window );

  //Wait two seconds
  SDL_Delay( 2000 );
 
  //Destroy window
  SDL_DestroyWindow( window );
 
  //Quit SDL subsystems
  SDL_Quit();
 
  return 0;
}
