#ifndef SDL3_TTF_H
#define SDL3_TTF_H

#include <SDL3/SDL.h>
#include <stdlib.h>

typedef struct {int id;} TTF_Font;

static inline void TTF_Init(){}
static inline TTF_Font *TTF_OpenFont(char *file,int size){(void)file;(void)size;TTF_Font *font = malloc(sizeof(TTF_Font));font->id = 0;return font;}
static inline SDL_Surface *TTF_RenderText_Blended(TTF_Font *font,char *text, int i, SDL_Color color){(void)font;(void)text;(void)i;(void)color;SDL_Surface *s;return s;}
static inline void TTF_CloseFont(){}
static inline void TTF_Quit(){}
#endif
