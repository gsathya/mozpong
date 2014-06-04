#include <stdio.h>
#include <sdl2/SDL.h>

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

 //Get window surface
 screenSurface = SDL_GetWindowSurface( window );
 
 //Fill the surface white
 SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
      
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
