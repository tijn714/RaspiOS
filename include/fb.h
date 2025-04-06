#ifndef FB_H
#define FB_H

void fb_init();
void put_pixel(int x, int y, unsigned int attribute);
void putchar(int x, int y, char c, unsigned int attribute);

#endif // FB_H