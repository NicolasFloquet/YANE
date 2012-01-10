#include <framebuffer.h>
#include <SDL/SDL.h>

/* tmp */
static SDL_Surface * screen;

void _put_pixel(SDL_Surface * surface, Uint16 x, Uint16 y, Uint32 color)
{
    /* Nombre de bits par pixels de la surface d'écran */
    Uint8 bpp = surface->format->BytesPerPixel;
    /* Pointeur vers le pixel à remplacer (pitch correspond à la taille 
       d'une ligne d'écran, c'est à dire (longueur * bitsParPixel) 
       pour la plupart des cas) */
    Uint8 * p = ((Uint8 *)surface->pixels) + y * surface->pitch + x * bpp;
     switch(bpp)
    {
	  case 1:
		*p = (Uint8) color;
		break;
        case 2:
            *(Uint16 *)p = (Uint16) color;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            {
                *(Uint16 *)p = ((color >> 8) & 0xff00) | ((color >> 8) & 0xff);
                *(p + 2) = color & 0xff;
            }
            else
            {
                *(Uint16 *)p = color & 0xffff;
                *(p + 2) = ((color >> 16) & 0xff) ;
            }
            break;
        case 4:
            *(Uint32 *)p = color;
            break;
    }
}


void fb_put_pixel(unsigned short int x,unsigned short int y,unsigned int color, unsigned char scale) {
	int i,j;
	for(i=0; i<scale; i++) {
		for(j=0; j<scale; j++) {
			_put_pixel(screen, x*scale+i, y*scale+j, color);
		}
	}
}

void fb_init() {	
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
    }
    screen = SDL_SetVideoMode(512, 480, 16, 
                              SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_ANYFORMAT);      
}

void fb_update() {
	SDL_Flip(screen);
}
