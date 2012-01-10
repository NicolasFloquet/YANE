#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

void fb_init();
void fb_update();
void fb_put_pixel(unsigned short int x,unsigned short int y,unsigned int color, unsigned char scale);

#endif /* _FRAMEBUFFER_H */
