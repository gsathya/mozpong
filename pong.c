#include <stdio.h>
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include "pong.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const char *img_path = "x.png";

int init(SDL_Window **window) {
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    return 1;
  }

  //Create window
  *window = SDL_CreateWindow( "Ping pong",
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

  return 0;
}

int load_surface(SDL_Surface **png_surface, SDL_Surface *screen_surface) {
  SDL_Surface* loadedSurface = IMG_Load(img_path);
  if(loadedSurface == NULL) {
    printf( "Unable to load image %s! SDL_image Error: %s\n", img_path, IMG_GetError() );
    return 1;
  }

  *png_surface = SDL_ConvertSurface( loadedSurface, screen_surface->format, NULL );
  if( png_surface == NULL ) {
    printf( "Unable to optimize image %s! SDL Error: %s\n", img_path, SDL_GetError() );
    return 1;
  }

  //Get rid of old loaded surface
  SDL_FreeSurface( loadedSurface );
  return 0;
}

int main(int argc, char *args[]){
  SDL_Window* window = NULL;
  SDL_Surface* screen_surface = NULL;
  SDL_Surface* png_surface = NULL;

  if (init(&window) == 1){
    printf("Failed to init system\n");
    return 0;
  }

  //Get window surface
  screen_surface = SDL_GetWindowSurface( window );

  if (load_surface(&png_surface, screen_surface) == 1){
    printf("Failed to load surface\n");
    return 0;
  }

  SDL_BlitSurface(png_surface, NULL, screen_surface, NULL);
  //Update the surface
  SDL_UpdateWindowSurface( window );

  //Wait two seconds
  SDL_Delay( 2000 );

  SDL_FreeSurface(png_surface);
  png_surface = NULL;

  SDL_DestroyWindow(window);
  window = NULL;

  IMG_Quit();
  SDL_Quit();

  return 0;
}
