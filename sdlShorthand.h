#include <SDL.h>
#include <math.h>

// Default window to 800 by 600

#define WWIDTH 800
#define WHEIGHT 600

#define SDL_8BITCOLOUR 256

// Ensure there is a representation of Pi
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

// All info required for windows / renderer & event loop
struct SDL_Simplewin {
   SDL_bool finished;
   SDL_Window *win;
   SDL_Renderer *renderer;
};

typedef struct SDL_Simplewin SDL_Simplewin;

void Shorthand_SDL_Init(SDL_Simplewin *sw);
void Shorthand_SDL_Events(SDL_Simplewin *sw);
void Shorthand_SDL_SetDrawColour(SDL_Simplewin *sw, Uint8 r, Uint8 g, Uint8 b);
void Shorthand_SDL_RenderFillCircle(SDL_Renderer *rend, int cx, int cy, int r);
void Shorthand_SDL_RenderDrawCircle(SDL_Renderer *rend, int cx, int cy, int r);
