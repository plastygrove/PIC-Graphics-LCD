/* 
 * File:   glcd.h
 * Author: ryder
 *
 * Created on October 25, 2014, 11:47 AM
 */

#ifndef GLCD_H
#define	GLCD_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "definitions.h"
#include "constants.h"
void init_mmap(void);
void putmmap(uint8 x, uint8 y, uint8 mbyte);
uint8 getmmap(uint8 x, uint8 y);
void glcd_exec();
void glcd_reset(void);
void glcd_on(void);
void glcd_gotoy(uint8 y);
void glcd_gotox(uint8 x);
void glcd_putbyte(uint8 x);
void glcd_clearscreen(void);
void draw(const volatile uint8 *matrix);
uint8 flip(uint8 n);
void draw_inverted(const volatile uint8 *matrix);
void text_test();
void text_test2(uint8 ch, uint8 x, uint8 y);
uint8 glcd_putchar(uint8 ch, uint8 x, uint8 y, uint8 clrflag);
uint8 glcd_putchar_inv(uint8 ch, uint8 x, uint8 y);
uint8 glcd_putchar_inv2(uint8 ch, uint8 x, uint8 y, uint8 clrflag, uint8 blankflag);
uint8 write(uint8 x, uint8 y, uint8 *txt, uint8 clrflag);
uint8 write_inv(uint8 x, uint8 y, uint8 *txt, uint8 clrflag, uint8 blankflag);
uint8 write_small_inv(uint8 x, uint8 y, uint8 *txt, uint8 clrflag, uint8 blankflag);
uint8 write_inv_pong(uint8 x, uint8 y, uint8 *txt, uint8 clrflag, uint8 blankflag);
uint8 glcd_putchar_inv2_pong(uint8 ch, uint8 x, uint8 y, uint8 clrflag, uint8 blankflag);
uint8 glcd_putchar_inv2_pong2(uint8 ch, uint8 x, uint8 y, uint8 clrflag, uint8 blankflag);

//Drawing functions
void point(uint8 px, uint8 py);
void line(uint8 fromx, uint8 fromy, uint8 tox, uint8 toy);
void circle(uint8 x, uint8 y, uint8 r);
void putfigure(uint8 x, uint8 y, const uint8 *arr, uint8 clr);
void putpongdivider(void);

extern volatile uint8 ismodechanged;
extern volatile uint8 mmap[1024];


#ifdef	__cplusplus
}
#endif

#endif	/* GLCD_H */

