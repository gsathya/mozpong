#include <stdio.h>
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#include "pong.h"

static const int SCREEN_WIDTH = 640;
static const int SCREEN_HEIGHT = 480;
static const char *title_img_path = "x.png";
static const char *dot_img_path = "dot.bmp";

int init(SDL_Window **window, SDL_Renderer **renderer) {
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

  *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
  if(renderer == NULL) {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);

  int imgFlags = IMG_INIT_PNG;
  if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
    return 1;
  }

  return 0;
}

int load_surface(SDL_Surface **png_surface, SDL_Surface *screen_surface) {
  SDL_Surface* loaded_surface = IMG_Load(title_img_path);
  if(loaded_surface == NULL) {
    printf( "Unable to load image %s! SDL_image Error: %s\n", title_img_path, IMG_GetError() );
    return 1;
  }

  *png_surface = SDL_ConvertSurface(loaded_surface, screen_surface->format, NULL);
  if( png_surface == NULL ) {
    printf( "Unable to optimize image %s! SDL Error: %s\n", title_img_path, SDL_GetError() );
    return 1;
  }

  //Get rid of old loaded surface
  SDL_FreeSurface(loaded_surface);
  return 0;
}

int load_texture(SDL_Texture **texture, SDL_Renderer *renderer) {
  //Load image at specified path
  SDL_Surface *loaded_surface = IMG_Load(title_img_path);
  if(loaded_surface == NULL){
    printf( "Unable to load image %s! SDL_image Error: %s\n", title_img_path, IMG_GetError());
    return 1;
  }

  //Create texture from surface pixels
  *texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
  if( texture == NULL ){
    printf( "Unable to create texture from %s! SDL Error: %s\n", title_img_path, SDL_GetError() );
    return 1;
  }

  //Get rid of old loaded surface
  SDL_FreeSurface(loaded_surface);
  return 0;
}

void update_position(SDL_Rect* player, int x, int y, int w, int h) {
  player->x = x;
  player->y = y;
  player->w = w;
  player->h = h;
}

void move(SDL_Rect* player, int velocity) {
  int y = player->y;
  int h = player->h;

  y = y + velocity;

  // don't go out of screen!!
  if (y+h > SCREEN_HEIGHT || y < 0) {
    y = y - velocity;
  }

  player->y = y;
}

int main(int argc, char *args[]){
  static int quit = FALSE;
  static int show_title = TRUE;

  static SDL_Window *window = NULL;
  static SDL_Surface *screen_surface = NULL;
  static SDL_Surface *png_surface = NULL;
  static SDL_Texture *texture = NULL;
  static SDL_Renderer *renderer = NULL;
  static SDL_Event event;

  SDL_Rect player1, player2;

  if (init(&window, &renderer) == 1){
    printf("Failed to init system\n");
    return 1;
  }

  //Get window surface
  screen_surface = SDL_GetWindowSurface(window);

  /* if (load_surface(&png_surface, screen_surface) == 1){ */
  /*   printf("Failed to load surface\n"); */
  /*   return 0; */
  /* } */

  if(load_texture(&texture, renderer) == 1){
    printf("Failed to load texture\n");
    return 1;
  }

  // set up initial positions
  update_position(&player1,
                  0,
                  SCREEN_HEIGHT / 2 -  SCREEN_HEIGHT / 12,
                  10,
                  SCREEN_HEIGHT / 6);

  // set up initial positions
  update_position(&player2,
                  SCREEN_WIDTH-10,
                  SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 12,
                  10,
                  SCREEN_HEIGHT / 6);

  while(!quit){
    while(SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT){
        quit = TRUE;
      }
    }

    const Uint8 *key_state = SDL_GetKeyboardState( NULL );
    if( key_state[ SDL_SCANCODE_UP ] ) {
      move(&player1, -1);
    }

    if( key_state[ SDL_SCANCODE_DOWN ] ) {
      move(&player1, 1);
    } 

    if( key_state[ SDL_SCANCODE_W ] ) {
      move(&player2, -1);
    }

    if( key_state[ SDL_SCANCODE_S ] ) {
      move(&player2, 1);
    }
    //Render texture to screen
    if (show_title) {
      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, NULL );
      SDL_RenderPresent(renderer);

      show_title = FALSE;
      SDL_Delay(500);
      continue;
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    //Render player1
    SDL_SetRenderDrawColor( renderer, 0xFF, 0x00, 0x00, 0xFF );
    SDL_RenderFillRect( renderer, &player1 );

    //Render player2
    SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0xFF, 0xFF );
    SDL_RenderFillRect( renderer, &player2 );

    //Update screen
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(texture);
  texture = NULL;

  SDL_DestroyRenderer(renderer);
  renderer = NULL;

  SDL_DestroyWindow(window);
  window = NULL;

  IMG_Quit();
  SDL_Quit();

  return 0;
}
