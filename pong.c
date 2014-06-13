#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#ifdef EMSCRIPTEN
#include <SDL.h>
#include <SDL_image.h>
#include <emscripten/emscripten.h>
#else
#include <sdl2/SDL.h>
#include <sdl2/SDL_image.h>
#endif

static const int SCREEN_WIDTH = 640;
static const int SCREEN_HEIGHT = 480;
static const char *title_img_path = "x.png";
static const char *ball_img_path = "dot.bmp";
#ifdef EMSCRIPTEN
static const int velocity = 10;
int ball_vel_x = 5, ball_vel_y = 0;
#else
static const int velocity = 5;
int ball_vel_x = 5, ball_vel_y = 0;
#endif

#define TRUE 1
#define FALSE 0

int collision;
int ball_w;
int ball_h;
int player1_vel = 0, player2_vel = 0;

static int quit = FALSE;
static int show_title = TRUE;

static SDL_Window *window = NULL;
static SDL_Surface *screen_surface = NULL;
static SDL_Surface *png_surface = NULL;
static SDL_Texture *texture = NULL;
static SDL_Texture *ball_texture = NULL;
static SDL_Renderer *renderer = NULL;

SDL_Rect player1, player2, ball;

SDL_Rect *clip = NULL;
SDL_Point *center = NULL;
double angle = 0.0;
SDL_RendererFlip flip = SDL_FLIP_NONE;

int init(SDL_Window **window, SDL_Renderer **renderer) {
  printf("Use <up-arrow> and <down-arrow> to control RED player\n");
  printf("Use 'w' and 's' to control BLUE player\n");

  srand(time(NULL));

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

  /* int imgFlags = IMG_INIT_PNG; */
  /* if( !( IMG_Init( imgFlags ) & imgFlags ) ) { */
  /*   printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() ); */
  /*   return 1; */
  /* } */

  return 0;
}

int load_texture(SDL_Texture **texture,
                 SDL_Texture **ball_texture,
                 SDL_Renderer *renderer) {
  //Load image at specified path
  SDL_Surface *loaded_surface = IMG_Load(title_img_path);
  if(loaded_surface == NULL){
    printf( "Unable to load image %s! SDL_image Error: %s\n",
            title_img_path,
            IMG_GetError());
    return 1;
  }

  //Create texture from surface pixels
  *texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
  if( texture == NULL ){
    printf( "Unable to create texture from %s! SDL Error: %s\n",
            title_img_path,
            SDL_GetError() );
    return 1;
  }

  //Get rid of old loaded surface
  SDL_FreeSurface(loaded_surface);

  //Load image at specified path
  loaded_surface = IMG_Load(ball_img_path);
  if(loaded_surface == NULL){
    printf( "Unable to load image %s! SDL_image Error: %s\n",
            ball_img_path,
            IMG_GetError());
    return 1;
  }

  //Create texture from surface pixels
  *ball_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
  if( texture == NULL ){
    printf( "Unable to create texture from %s! SDL Error: %s\n",
            ball_img_path,
            SDL_GetError() );
    return 1;
  }

  ball_w = loaded_surface->w;
  ball_h = loaded_surface->h;

  //Get rid of old loaded surface
  SDL_FreeSurface(loaded_surface);
  return 0;
}

void set_position(SDL_Rect* player, int x, int y, int w, int h) {
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

void move_ball(SDL_Rect* ball, int velocity_x, int velocity_y) {
  int x = ball->x;
  int y = ball->y;
  int h = ball->h;
  int w = ball->w;

  x = x + velocity_x;
  y = y + velocity_y;

  // don't go out of screen!!
  if (x+w > SCREEN_WIDTH || x < 0) {
    x = x - velocity_x;
  }

  // don't go out of screen!!
  if (y+h > SCREEN_HEIGHT || y < 0) {
    y = y - velocity_y;
  }

  ball->x = x;
  ball->y = y;
}

int check_collision(SDL_Rect A, SDL_Rect B) {
  int leftA, leftB;
  int rightA, rightB;
  int topA, topB;
  int bottomA, bottomB;

  leftA = A.x;
  rightA = A.x+A.w;
  topA = A.y;
  bottomA = A.y+A.h;

  leftB = B.x;
  rightB = B.x + B.w;
  topB = B.y;
  bottomB = B.y+B.h;

  if(bottomA <= topB) {
    return 1;
  }

  if(topA >= bottomB) {
    return 1;
  }

  if(rightA <= leftB) {
    return 1;
  }

  if(leftA >= rightB) {
    return 1;
  }

  int midA = topA + (A.h / 2);
  int midB = topB + (B.h / 2);

  if(midB >= midA ) {
    ball_vel_y = abs(ball_vel_y+1);
  } else {
    ball_vel_y = -abs(ball_vel_y+1);
  }
  return 0;
}

int hit_wall() {
  // we hit the side wall
  if ((ball.x <= 0) || ((ball.x + ball.w) >= SCREEN_WIDTH)) {
    return 1;
  }

  // we hit top and bottom
  if ((ball.y-5 <= 0) || ((ball.y+ball.h+5) >= SCREEN_HEIGHT)){
    ball_vel_y = -ball_vel_y;
  }

  return 0;
}

void loop() {
  static SDL_Event event;

  while(SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT){
      quit = TRUE;
    }
    if( event.type == SDL_KEYDOWN && event.key.repeat == 0 ) {
      switch( event.key.keysym.sym ) {
      case SDLK_UP: player1_vel -= velocity; break;
      case SDLK_DOWN: player1_vel += velocity; break;
      case SDLK_w: player2_vel -= velocity; break;
      case SDLK_s: player2_vel += velocity; break;
      case SDLK_q : quit = TRUE; break;
      default: break;
      }
    } else if( event.type == SDL_KEYUP && event.key.repeat == 0 ) {
      switch( event.key.keysym.sym ) {
      case SDLK_UP: player1_vel += velocity; break;
      case SDLK_DOWN: player1_vel -= velocity; break;
      case SDLK_w: player2_vel += velocity; break;
      case SDLK_s: player2_vel -= velocity; break;
      default: break;
      }
    }
  }

  //Render texture to screen
  if (show_title) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL );
    SDL_RenderPresent(renderer);

    show_title = FALSE;
    SDL_Delay(500);
    return;
  }

  move(&player1, player1_vel);
  move(&player2, player2_vel);
  move_ball(&ball, ball_vel_x, ball_vel_y);

  collision = check_collision(player1, ball);
  if(collision == 0){
    ball_vel_x = abs(ball_vel_x);
  }
  collision = check_collision(player2, ball);
  if(collision == 0){
    ball_vel_x = -(abs(ball_vel_x));
  }

  collision = hit_wall();
  if(collision == 1){
    ball_vel_y = 0;
    ball_vel_x = abs(ball_vel_x);    
    set_position(&ball,
                 SCREEN_WIDTH/2 - ball_w/2,
                 SCREEN_HEIGHT/2 - ball_h/2,
                 ball_w,
                 ball_h);
  }

  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(renderer);

  //Render player1
  SDL_SetRenderDrawColor( renderer, 0xFF, 0x00, 0x00, 0xFF );
  SDL_RenderFillRect( renderer, &player1 );

  //Render player2
  SDL_SetRenderDrawColor( renderer, 0x00, 0x00, 0xFF, 0xFF );
  SDL_RenderFillRect( renderer, &player2 );

  //Render the ball
  SDL_RenderCopyEx(renderer,
                   ball_texture,
                   clip,
                   &ball,
                   angle,
                   center,
                   SDL_FLIP_NONE);

  //Update screen
  SDL_RenderPresent(renderer);
}

int main(int argc, char *args[]){

  if (init(&window, &renderer) == 1){
    printf("Failed to init system\n");
    return 1;
  }

  //Get window surface
  screen_surface = SDL_GetWindowSurface(window);

  if(load_texture(&texture, &ball_texture, renderer) == 1){
    printf("Failed to load texture\n");
    return 1;
  }

  // set up player1
  set_position(&player1,
               0,
               SCREEN_HEIGHT / 2 -  SCREEN_HEIGHT / 12,
               10,
               SCREEN_HEIGHT / 6);

  // set up player2
  set_position(&player2,
               SCREEN_WIDTH - 10,
               SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 12,
               10,
               SCREEN_HEIGHT / 6);

  // set up the ball
  set_position(&ball,
               SCREEN_WIDTH/2 - ball_w/2,
               SCREEN_HEIGHT/2 - ball_h/2,
               ball_w,
               ball_h);

#ifdef EMSCRIPTEN
  emscripten_set_main_loop(loop, 0, 1);
#else
  while(!quit) {
    loop();
  }
#endif
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
