#include "sdlShorthand.h"

// Set up a simple (WIDTH, HEIGHT) window.
// Attempt to hide the renderer etc. from user.
void Shorthand_SDL_Init(SDL_Simplewin *sw)
{


   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   }

   sw->finished = 0;

   sw->win= SDL_CreateWindow("Logo Interpreter",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          WWIDTH, WHEIGHT,
                          SDL_WINDOW_SHOWN);
   if(sw->win == NULL){
      fprintf(stderr, "\nUnable to initialize SDL Window:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   }

   sw->renderer = SDL_CreateRenderer(sw->win, -1, 0);
   if(sw->renderer == NULL){
      fprintf(stderr, "\nUnable to initialize SDL Renderer:  %s\n", SDL_GetError());
      SDL_Quit();
      exit(1);
   }

   // Set screen to black
   Shorthand_SDL_SetDrawColour(sw, 0, 0, 0);
   SDL_RenderClear(sw->renderer);
   SDL_RenderPresent(sw->renderer);

}

// Gobble all events & ignore most
void Shorthand_SDL_Events(SDL_Simplewin *sw)
{
   SDL_Event event;
   while(SDL_PollEvent(&event))
   {
       switch (event.type){
          case SDL_QUIT:
          case SDL_MOUSEBUTTONDOWN:
       case SDL_KEYDOWN:
         sw->finished = 1;
       }
    }
}


// Trivial wrapper to avoid complexities of renderer & alpha channels
void Shorthand_SDL_SetDrawColour(SDL_Simplewin *sw, Uint8 r, Uint8 g, Uint8 b)
{

   SDL_SetRenderDrawColor(sw->renderer, r, g, b, SDL_ALPHA_OPAQUE);

}

// Filled Circle centred at (cx,cy) of radius r, see :
// http://content.gpwiki.org/index.php/SDL:Tutorials:Drawing_and_Filling_Circles
void Shorthand_SDL_RenderFillCircle(SDL_Renderer *rend, int cx, int cy, int r)
{

   for (double dy = 1; dy <= r; dy += 1.0) {
        double dx = floor(sqrt((2.0 * r * dy) - (dy * dy)));
        SDL_RenderDrawLine(rend, cx-dx, cy+r-dy, cx+dx, cy+r-dy);
        SDL_RenderDrawLine(rend, cx-dx, cy-r+dy, cx+dx, cy-r+dy);
   }

}

// Circle centred at (cx,cy) of radius r, see :
// http://content.gpwiki.org/index.php/SDL:Tutorials:Drawing_and_Filling_Circles
void Shorthand_SDL_RenderDrawCircle(SDL_Renderer *rend, int cx, int cy, int r)
{

   double dx, dy;
   dx = floor(sqrt((2.0 * r ) ));
   SDL_RenderDrawLine(rend, cx-dx, cy+r, cx+dx, cy+r);
   SDL_RenderDrawLine(rend, cx-dx, cy-r, cx+dx, cy-r);
   for (dy = 1; dy <= r; dy += 1.0) {
        dx = floor(sqrt((2.0 * r * dy) - (dy * dy)));
        SDL_RenderDrawPoint(rend, cx+dx, cy+r-dy);
        SDL_RenderDrawPoint(rend, cx+dx, cy-r+dy);
        SDL_RenderDrawPoint(rend, cx-dx, cy+r-dy);
        SDL_RenderDrawPoint(rend, cx-dx, cy-r+dy);
   }

}
