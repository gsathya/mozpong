Sample game to test SDL2 port to javascript using emscripten

To build this -
* Install emscripten
* Clone and build sdl2 - https://github.com/gsathya/SDL-mirror/
* Clone and build sdl2_image - https://github.com/gsathya/sdl2-image

Run -

emcc -O2 --js-opts 0 -g4 pong.c -I/path/to/SDL-mirror/include -I/path/to/SDL_image/include /path/to/SDL-mirror/build/.libs/libSDL2.a /path/to/SDL_image/lib/libSDL2_image.a -o index.html --preload-file x.png --preload-file dot.bmp
