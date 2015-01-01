#include <xc.h>
#include "glcd.h"
#include "spi.h"


volatile uint8 mmap[1024]; //Unused, but space blocked for later use as a memory map

void init_mmap(void) {
    for (int i = 0; i < 1024; i++) {
        mmap[i] = 0;
    }
}

void putmmap(uint8 x, uint8 y, uint8 mbyte) {
    mmap[128 * x + y] = mbyte;
}

uint8 getmmap(uint8 x, uint8 y) {
    return mmap[128 * x + y];
}

void glcd_exec() {
    E = HIGH;
    __delay_us(1);
    E = LOW;
    __delay_us(1);
}

void glcd_reset(void) {
    RST = LOW; //Reset display
    __delay_ms(100);
    RST = HIGH;
    __delay_ms(100);
}

void glcd_on(void) {
    init_mmap();
    CS1 = HIGH; //Don't know why but these are needed otherwise display doesn't turn on
    CS2 = HIGH;
    E = LOW; //Set enable to low to turn on display
    __delay_ms(1);

    glcd_reset();
    RS = LOW; //Display on
    RW = LOW;
    //DATA = 0b00111111;
    spi_write(0b00111111);
    glcd_exec();
}

void glcd_gotoy(uint8 y) {
    RS = LOW; //Set Y Address (0-63)
    RW = LOW;
    //DATA = 0b01000000 | y;
    spi_write(0b01000000 | y);
    glcd_exec();
}

void glcd_gotox(uint8 x) {
    RS = LOW; //Set X Address (0-7)
    RW = LOW;
    //DATA = 0b10111000 | x;
    spi_write(0b10111000 | x);
    glcd_exec();
}

void glcd_putbyte(uint8 x) {
    RS = HIGH;
    RW = LOW;
    //DATA = x;
    spi_write(x);
    glcd_exec();
}

void glcd_clearscreen(void) {
    CS1 = HIGH;
    CS2 = HIGH;
    glcd_gotoy(0);
    for (uint8 x = 0; x < 8; x++) {
        glcd_gotox(x);
        for (uint8 y = 0; y < 64; y++) {
            glcd_putbyte(0);
        }
    }
    glcd_gotoy(0);
    glcd_gotox(0);
}

void draw(const volatile uint8 *matrix) { //Can accept const uint8* or volatile const uint8*
    for (int i = 0; i < 8; i++) {
        CS1 = HIGH;
        CS2 = LOW;
        glcd_gotox(i);
        glcd_gotoy(0);

        for (int j = 0; j < 64; j++) {
            glcd_putbyte(matrix[128 * i + j]);
        }
        CS1 = LOW;
        CS2 = HIGH;
        glcd_gotox(i);
        glcd_gotoy(0);
        for (int j = 64; j < 128; j++) {
            glcd_putbyte(matrix[128 * i + j]);
        }

    }
}

//http://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
//Using this to flip the bits

uint8 flip(uint8 n) {
    //This should be just as fast and it is easier to understand.
    //return (lookup[n%16] << 4) | lookup[n/16];
    return (lookup[n & 0x0F] << 4) | lookup[n >> 4];
}

volatile uint8 ismodechanged = 0;

void draw_inverted(const volatile uint8 *matrix) {
    for (int i = 0; i < 8; i++) {
        CS1 = HIGH;
        CS2 = LOW;
        glcd_gotox(i);
        glcd_gotoy(0);

        for (int j = 0; j < 64; j++) {
            glcd_putbyte(flip(matrix[128 * (8 - i)-(j + 1)]));
        }
        CS1 = LOW;
        CS2 = HIGH;
        glcd_gotox(i);
        glcd_gotoy(0);
        for (int j = 64; j < 128; j++) {
            glcd_putbyte(flip(matrix[128 * (8 - i)-(j + 1)]));
        }
        if (ismodechanged) break;

    }
}

void text_test() {
    uint8 x = 0;
    uint8 y = 0;
    int start = ('b' - 32)*17;
    for (int i = 0; i < font1[start]*2; i += 2) {
        glcd_gotox(0);
        glcd_gotoy(y);
        glcd_putbyte(font1[start + i + 1]);
        glcd_gotox(1);
        glcd_gotoy(y);
        glcd_putbyte(font1[start + i + 2]);
        y += 1;
    }
}

void text_test2(uint8 ch, uint8 x, uint8 y) {
    uint8 sx = 0;
    uint8 sy = 0;
    int start = (ch - 32)*55;
    for (int i = 0; i < bigfont[start]*3; i += 3) {
        if (y >= 64 && y < 128) {
            sy = y - 64;
            CS1 = LOW;
            CS2 = HIGH;
        } else if (y < 64) {
            sy = y;
            CS1 = HIGH;
            CS2 = LOW;
        } else if (y > 128) {
            break;
        }

        if (x >= 8) {
            break;
        }
        glcd_gotox(x);
        glcd_gotoy(sy);
        glcd_putbyte(bigfont[start + i + 1]);
        glcd_gotox(x + 1);
        glcd_gotoy(sy);
        glcd_putbyte(bigfont[start + i + 2]);
        glcd_gotox(x + 2);
        glcd_gotoy(sy);
        glcd_putbyte(bigfont[start + i + 3]);
        y++;
    }
}

//http://stackoverflow.com/questions/12304273/retrieve-last-6-bits-from-an-integer - interesting article on rightmost bits

/**
 * Function to put a character anywhere on the GLCD
 * @param ch - Character ASCII
 * @param x - vertical coordinate, range:0-63
 * @param y - horizontal coordinate, range: 0-127
 * @return final y value where it ends
 */
uint8 glcd_putchar(uint8 ch, uint8 x, uint8 y, uint8 clrflag) { //x: 0-63, y:0-127
    uint8 font_height = 3;
    uint8 sx = 0;
    uint8 sy = 0;
    int start = (ch - 32)*55;
    uint8 vshift = x % 8;
    uint8 thebyte = 0;
    uint8 xloc = x / 8;
    uint8 xval = 0;
    uint8 mask = (0xFF << (8 - vshift)); //mask for leftmost bits
    uint8 lbits = 0;

    for (int i = 0; i < bigfont[start]*3; i += 3) {
        if (y >= 64 && y < 128) {
            sy = y - 64;
            CS1 = LOW;
            CS2 = HIGH;
        } else if (y < 64) {
            sy = y;
            CS1 = HIGH;
            CS2 = LOW;
        } else if (y > 128) {
            break;
        }

        xval = xloc;
        thebyte = bigfont[start + i + 1] << vshift;


        if (xval < 8 && thebyte > 0) {
            glcd_gotox(xval);
            glcd_gotoy(sy);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }
        xval++;

        /* This check below is needed because otherwise xval < 8 won't work in the
         * step below. We are using an unsigned char.
         * If 0 is decreased by 1, it becomes 255 which then makes 255/8 = 31.
         * So incrementing 31 will give 32 which will never satisfy x<8.
         * So we rotate it to 0 so that text can disappear at the top of the
         * screen*/
        if (xval == 32) xval = 0;

        lbits = bigfont[start + i + 1] & mask;
        lbits = lbits >> (8 - vshift);
        thebyte = (bigfont[start + i + 2] << vshift) | lbits;


        if (xval < 8 && thebyte > 0) {
            glcd_gotox(xval);
            glcd_gotoy(sy);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }
        xval++;
        if (xval == 32) xval = 0;

        lbits = bigfont[start + i + 2] & mask;
        lbits = lbits >> (8 - vshift);
        thebyte = (bigfont[start + i + 3] << vshift) | lbits;


        if (xval < 8 && thebyte > 0) {
            glcd_gotox(xval);
            glcd_gotoy(sy);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }
        xval++;
        if (xval == 32) xval = 0;

        lbits = bigfont[start + i + 3] & mask;
        lbits = lbits >> (8 - vshift);
        thebyte = lbits;


        if (xval < 8 && thebyte > 0) {
            glcd_gotox(xval);
            glcd_gotoy(sy);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        y++;
    }
    return y;
}

/**
 * Function to write one inverted character - required if you need the display inverted
 * @param ch - valid displayable ASCII character (this isn't unicode)
 * @param x
 * @param y
 * @return
 */
uint8 glcd_putchar_inv(uint8 ch, uint8 x, uint8 y) {
    //x:0-7
    //y:0-127
    int start = (ch - 32)*55;

    uint8 gx = 7 - x;
    uint8 gy = 127 - y;
    uint8 gyloc = 0;


    for (int i = 0; i < bigfont[start]*3; i += 3) {

        if (y < 64) {
            gyloc = 127 - y - 64;
            CS1 = LOW;
            CS2 = HIGH;
        } else if (y >= 64 && y < 128) {
            gyloc = 127 - y;
            CS1 = HIGH;
            CS2 = LOW;
        } else {
            break;
        }

        uint8 gxloc = gx;

        glcd_gotoy(gyloc);
        glcd_gotox(gxloc--);
        glcd_putbyte(flip(bigfont[start + i + 1]));

        glcd_gotox(gxloc--);
        glcd_gotoy(gyloc);
        glcd_putbyte(flip(bigfont[start + i + 2]));

        glcd_gotox(gxloc);
        glcd_gotoy(gyloc);
        glcd_putbyte(flip(bigfont[start + i + 3]));
        y++;
    }

    return y;

}

uint8 glcd_putchar_inv2(uint8 ch, uint8 x, uint8 y, uint8 clrflag, uint8 blankflag) {
    //x:0-7
    //y:0-127
    int start = (ch - 32)*55;
    //x=1;
    uint8 gx = 63 - x;
    uint8 gy = 127 - y;
    uint8 gyloc = 0;
    uint8 vshift = x % 8;
    uint8 gxactual = gx / 8;
    uint8 mask = 0xFF >> (8 - vshift);
    uint8 thebyte = 0;

    for (int i = 0; i < bigfont[start]*3 + 6; i += 3) {
        if (y < 64) {
            gyloc = 127 - y - 64;
            CS1 = LOW;
            CS2 = HIGH;
        } else if (y >= 64 && y < 128) {
            gyloc = 127 - y;
            CS1 = HIGH;
            CS2 = LOW;
        } else {
            break;
        }

        uint8 gxloc = gxactual;

        thebyte = flip(bigfont[start + i + 1]) >> vshift;
        if (gxloc < 8 && (thebyte > 0 || blankflag)) {
            glcd_gotoy(gyloc);
            glcd_gotox(gxloc);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        gxloc--;


        uint8 rbits = flip(bigfont[start + i + 1]) & mask;
        rbits = rbits << (8 - vshift);

        thebyte = (flip(bigfont[start + i + 2]) >> vshift) | rbits;
        if (gxloc < 8 && (thebyte > 0 || blankflag)) {
            glcd_gotox(gxloc);
            glcd_gotoy(gyloc);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        gxloc--;

        rbits = flip(bigfont[start + i + 2]) & mask;
        rbits = rbits << (8 - vshift);
        thebyte = (flip(bigfont[start + i + 3]) >> vshift) | rbits;
        if (gxloc < 8 && (thebyte > 0 || blankflag)) {
            glcd_gotox(gxloc);
            glcd_gotoy(gyloc);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        gxloc--;


        rbits = flip(bigfont[start + i + 3]) & mask;
        rbits = rbits << (8 - vshift);
        thebyte = rbits;

        if (gxloc < 8 && (thebyte > 0 || blankflag)) {
            glcd_gotox(gxloc);
            glcd_gotoy(gyloc);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        y++;
    }

    return y;

}

uint8 glcd_putchar_inv2_pong(uint8 ch, uint8 x, uint8 y, uint8 clrflag, uint8 blankflag) {
    //x:0-7
    //y:0-127
    int start = (ch - 48)*58; //Starts with 48 because our font file starts with 0
    //x=1;
    uint8 gx = 63 - x;
    uint8 gy = 127 - y;
    uint8 gyloc = 0;
    uint8 vshift = x % 8;
    uint8 gxactual = gx / 8;
    uint8 mask = 0xFF >> (8 - vshift);
    uint8 thebyte = 0;

    for (int i = 0; i < pongfont[start]*3 + 6; i += 3) {
        if (y < 64) {
            gyloc = 127 - y - 64;
            CS1 = LOW;
            CS2 = HIGH;
        } else if (y >= 64 && y < 128) {
            gyloc = 127 - y;
            CS1 = HIGH;
            CS2 = LOW;
        } else {
            break;
        }

        uint8 gxloc = gxactual;

        thebyte = flip(pongfont[start + i + 1]) >> vshift;
        if (gxloc < 8 && (thebyte > 0 || blankflag)) {
            glcd_gotoy(gyloc);
            glcd_gotox(gxloc);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        gxloc--;


        uint8 rbits = flip(pongfont[start + i + 1]) & mask;
        rbits = rbits << (8 - vshift);

        thebyte = (flip(pongfont[start + i + 2]) >> vshift) | rbits;
        if (gxloc < 8 && (thebyte > 0 || blankflag)) {
            glcd_gotox(gxloc);
            glcd_gotoy(gyloc);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        gxloc--;

        rbits = flip(pongfont[start + i + 2]) & mask;
        rbits = rbits << (8 - vshift);
        thebyte = (flip(pongfont[start + i + 3]) >> vshift) | rbits;
        if (gxloc < 8 && (thebyte > 0 || blankflag)) {
            glcd_gotox(gxloc);
            glcd_gotoy(gyloc);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        gxloc--;


        rbits = flip(pongfont[start + i + 3]) & mask;
        rbits = rbits << (8 - vshift);
        thebyte = rbits;

        if (gxloc < 8 && (thebyte > 0 || blankflag)) {
            glcd_gotox(gxloc);
            glcd_gotoy(gyloc);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        y++;
    }

    return y;

}

uint8 glcd_putchar_inv2_pong2(uint8 ch, uint8 x, uint8 y, uint8 clrflag, uint8 blankflag) {
    //x:0-7
    //y:0-127
    int start = (ch - 48)*33; //Starts with 48 because our font file starts with 0
    //x=1;
    uint8 gx = 63 - x;
    uint8 gy = 127 - y;
    uint8 gyloc = 0;
    uint8 vshift = x % 8;
    uint8 gxactual = gx / 8;
    uint8 mask = 0xFF >> (8 - vshift);
    uint8 thebyte = 0;

    if (vshift != 0) {
        for (int i = 0; i < pongfont2[start]*2; i += 2) { //pongfont[start]*3 + 6 ==> the 6 appears to be for adding additional bytes for spacing
            if (y < 64) {
                gyloc = 127 - y - 64;
                CS1 = LOW;
                CS2 = HIGH;
            } else if (y >= 64 && y < 128) {
                gyloc = 127 - y;
                CS1 = HIGH;
                CS2 = LOW;
            } else {
                break;
            }

            uint8 gxloc = gxactual;

            thebyte = flip(pongfont2[start + i + 1]) >> vshift;
            if (gxloc < 8 && (thebyte > 0 || blankflag)) {
                glcd_gotoy(gyloc);
                glcd_gotox(gxloc);
                glcd_putbyte(clrflag ? 0 : thebyte);
            }

            gxloc--;


            uint8 rbits = flip(pongfont2[start + i + 1]) & mask;
            rbits = rbits << (8 - vshift);

            thebyte = (flip(pongfont2[start + i + 2]) >> vshift) | rbits;
            if (gxloc < 8 && (thebyte > 0 || blankflag)) {
                glcd_gotox(gxloc);
                glcd_gotoy(gyloc);
                glcd_putbyte(clrflag ? 0 : thebyte);
            }

            gxloc--;

            rbits = flip(pongfont2[start + i + 2]) & mask;
            rbits = rbits << (8 - vshift);

            thebyte = rbits;

            if (gxloc < 8 && (thebyte > 0 || blankflag)) {
                glcd_gotox(gxloc);
                glcd_gotoy(gyloc);
                glcd_putbyte(clrflag ? 0 : thebyte);
            }

            y++;
        }
    } else {
        for (int i = 0; i < pongfont2[start]*2; i += 2) { //pongfont[start]*3 + 6 ==> the 6 appears to be for adding additional bytes for spacing
            if (y < 64) {
                gyloc = 127 - y - 64;
                CS1 = LOW;
                CS2 = HIGH;
            } else if (y >= 64 && y < 128) {
                gyloc = 127 - y;
                CS1 = HIGH;
                CS2 = LOW;
            } else {
                break;
            }
            uint8 gxloc = gxactual;

            thebyte = flip(pongfont2[start + i + 1]);
            if (gxloc < 8 && (thebyte > 0 || blankflag)) {
                glcd_gotoy(gyloc);
                glcd_gotox(gxloc);
                glcd_putbyte(clrflag ? 0 : thebyte);
            }

            gxloc--;

            thebyte = flip(pongfont2[start + i + 2]);
            if (gxloc < 8 && (thebyte > 0 || blankflag)) {
                glcd_gotox(gxloc);
                glcd_gotoy(gyloc);
                glcd_putbyte(clrflag ? 0 : thebyte);
            }

            y++;
        }
    }

    return y;

}

uint8 glcd_putchar_small_inv2(uint8 ch, uint8 x, uint8 y, uint8 clrflag, uint8 blankflag) {
    //x:0-7
    //y:0-127
    int start = (ch - 32)*7;
    //x=1;
    uint8 gx = 63 - x;
    uint8 gy = 127 - y;
    uint8 gyloc = 0;
    uint8 vshift = x % 8;
    uint8 gxactual = gx / 8;
    uint8 mask = 0xFF >> (8 - vshift);
    uint8 thebyte = 0;

    for (int i = 0; i < smallfont[start]; i += 1) {
        if (y < 64) {
            gyloc = 127 - y - 64;
            CS1 = LOW;
            CS2 = HIGH;
        } else if (y >= 64 && y < 128) {
            gyloc = 127 - y;
            CS1 = HIGH;
            CS2 = LOW;
        } else {
            break;
        }

        uint8 gxloc = gxactual;

        thebyte = flip(smallfont[start + i + 1]) >> vshift;
        if (gxloc < 8 && (thebyte > 0 || blankflag)) {
            glcd_gotoy(gyloc);
            glcd_gotox(gxloc);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        gxloc--;

        uint8 rbits = flip(smallfont[start + i + 1]) & mask;
        rbits = rbits << (8 - vshift);
        thebyte = rbits;

        if (gxloc < 8 && (thebyte > 0 || blankflag)) {
            glcd_gotox(gxloc);
            glcd_gotoy(gyloc);
            glcd_putbyte(clrflag ? 0 : thebyte);
        }

        y++;
    }

    return y;

}

uint8 write(uint8 x, uint8 y, uint8 *txt, uint8 clrflag) {
    int i = 0;
    while (*(txt + i) != '\0') {
        y = glcd_putchar(*(txt + i), x, y, clrflag);
        i++;
    }
    return y;
}

uint8 write_inv(uint8 x, uint8 y, uint8 *txt, uint8 clrflag, uint8 blankflag) {
    int i = 0;
    while (*(txt + i) != '\0') {
        y = glcd_putchar_inv2(*(txt + i), x, y, clrflag, blankflag);
        i++;
    }
    return y;
}

uint8 write_inv_pong(uint8 x, uint8 y, uint8 *txt, uint8 clrflag, uint8 blankflag) {
    int i = 0;
    while (*(txt + i) != '\0') {
        y = glcd_putchar_inv2_pong2(*(txt + i), x, y, clrflag, blankflag);
        i++;
    }
    return y;
}

uint8 write_small_inv(uint8 x, uint8 y, uint8 *txt, uint8 clrflag, uint8 blankflag) {
    int i = 0;
    while (*(txt + i) != '\0') {
        y = glcd_putchar_small_inv2(*(txt + i), x, y, clrflag, blankflag);
        i++;
    }
    return y;
}

void point(uint8 x, uint8 y) { //x:0-63, y:0-127
    uint8 gx = x / 8;
    uint8 shf = x % 8;
    uint8 gy;
    if (y < 64) {
        CS1 = HIGH;
        CS2 = LOW;
        gy = y;
    } else if (y >= 64 && y < 128) {
        CS1 = LOW;
        CS2 = HIGH;
        gy = y - 64;
    }

    glcd_gotox(gx);
    glcd_gotoy(gy);
    uint8 thebyte = getmmap(gx, y) | (1 << shf);
    glcd_putbyte(thebyte);
    putmmap(gx, y, thebyte);
}

/**
 * Algo: http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 * @param x0
 * @param y0
 * @param x1
 * @param y1
 */
void line(uint8 x0, uint8 y0, uint8 x1, uint8 y1) { //x:0-63, y:0-127
    float dx = x1 - x0;
    float dy = y1 - y0;
    float err = 0.0;
    float derr;
    if (dy == 0) { //horizontal line
        for (int x = x0; x <= x1; x++) {
            point(x, y0);
        }
    } else if (dx == 0) { //Vertical line
        for (int y = y0; y <= y1; y++) {
            point(x0, y);
        }
    } else if (dy > 0) {
        if (dy > dx) {
            derr = dx / dy;
            int x = x0;
            for (int y = y0; y <= y1; y++) {
                point(x, y);
                err = err + derr;
                if (err >= 0.5) {
                    x = x + 1;
                    err = err - 1.0;
                }
            }
        } else {
            derr = dy / dx;
            int y = y0;
            for (int x = x0; x <= x1; x++) {
                point(x, y);
                err = err + derr;
                if (err >= 0.5) {
                    y = y + 1;
                    err = err - 1.0;
                }
            }
        }
    } else {
        if (-dy > dx) {
            derr = -dx / dy;
            int x = x0;
            for (int y = y0; y >= y1; y--) {
                point(x, y);
                err = err + derr;
                if (err >= 0.5) {
                    x = x + 1;
                    err = err - 1.0;
                }
            }
        } else {
            derr = -dy / dx;
            int y = y0;
            for (int x = x0; x <= x1; x++) {
                point(x, y);
                err = err + derr;
                if (err >= 0.5) {
                    y = y - 1;
                    err = err - 1.0;
                }
            }
        }
    }

}

void circle(uint8 x0, uint8 y0, uint8 r) {
    int x = r;
    int y = 0;
    int rerr = 1 - x;
    while (x >= y) {
        point(x + x0, y + y0);
        point(y + x0, x + y0);
        point(-x + x0, y + y0);
        point(-y + x0, x + y0);
        point(-x + x0, -y + y0);
        point(-y + x0, -x + y0);
        point(x + x0, -y + y0);
        point(y + x0, -x + y0);
        y++;
        if (rerr < 0) {
            rerr += 2 * y + 1;
        } else {
            x--;
            rerr += 2 * (y - x + 1);
        }
    }
}

void putfigure(uint8 x, uint8 y, const uint8 *arr, uint8 clr) {//x:0-63, y:0-127
    uint8 gx = x / 8;
    uint8 shf = x % 8;
    uint8 gy = 0;
    uint8 mask = 0xFF << (8 - shf);
    uint8 maptopmask = 0xFF >> (8 - shf);
    uint8 mapbottommask = 0xFF << shf;
    uint8 w = arr[0];
    uint8 h = arr[1];
    if (shf != 0) {
        for (uint8 i = 2; i < (2 + w * h); i += h) {
            if (y < 64) {
                CS1 = HIGH;
                CS2 = LOW;
                gy = y;
            } else {
                CS1 = LOW;
                CS2 = HIGH;
                gy = y - 64;
            }
            uint8 gxloc = gx;
            glcd_gotox(gxloc);
            glcd_gotoy(gy);

            uint8 thebyte = (getmmap(gxloc, y) & maptopmask) | clr ? 0 : (arr[i] << shf);
            glcd_putbyte(thebyte);
            putmmap(gxloc, gy, thebyte);

            uint8 lbits = (arr[i] & mask);
            lbits = lbits >> (8 - shf);
            gxloc++;

            for (uint8 j = 1; j < h; j++) {
                glcd_gotox(gxloc);
                glcd_gotoy(gy);

                uint8 thebyte = (clr ? 0 : (arr[i + j] << shf) | lbits);
                glcd_putbyte(thebyte);
                putmmap(gxloc, gy, thebyte);

                lbits = (arr[i + j] & mask);
                lbits = lbits >> (8 - shf);
                gxloc++;
            }

            //Put code here to handle objects bigger than "ball"
            glcd_gotox(gxloc);
            glcd_gotoy(gy);
            uint8 thebyte = (getmmap(gxloc, y) & mapbottommask) | clr ? 0 : lbits;
            glcd_putbyte(thebyte);
            putmmap(gxloc, gy, thebyte);
            y++;

        }
    } else {
        for (uint8 i = 2; i < (2 + w * h); i += h) {
            if (y < 64) {
                CS1 = HIGH;
                CS2 = LOW;
                gy = y;
            } else {
                CS1 = LOW;
                CS2 = HIGH;
                gy = y - 64;
            }
            uint8 gxloc = gx;

            for (uint8 j = 0; j < h; j++) {
                glcd_gotox(gxloc);
                glcd_gotoy(gy);

                uint8 thebyte = clr ? 0 : (arr[i + j]);
                glcd_putbyte(thebyte);
                putmmap(gxloc, gy, thebyte);

                gxloc++;
            }
            y++;
        }
    }
}

void putpongdivider(void) {
    CS1 = HIGH;
    CS2 = LOW;
    for (int i = 0; i < 8; i++) {
        glcd_gotox(i);
        glcd_gotoy(63);
        glcd_putbyte(0b11101110);
    }
}
