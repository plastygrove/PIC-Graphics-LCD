#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include "definitions.h"
#include "constants.h"
#include "glcd.h"
#include "spi.h"
#include "i2c.h"

void init_spireg(void) {
    BACKLEDPORT = OUTPUT;
    LEDPORT = OUTPUT;
    //Set all LCD pins as OUTPUT
    DATAPORT = OUTPUT;
    RSPORT = OUTPUT;
    RWPORT = OUTPUT;
    EPORT = OUTPUT;
    CS1PORT = OUTPUT;
    CS2PORT = OUTPUT;
    RSTPORT = OUTPUT;
    RST = HIGH;
    SSPSTATbits.SMP = 0;
    SSPSTATbits.CKE = 1;
    SSPCON1 = 0b00100000;

    SPI_CLKPORT = OUTPUT;
    SPI_LATCHPORT = OUTPUT;
    SPI_DATAPORT = OUTPUT;
    SPI_CLK = LOW;
    SPI_LATCH = LOW;
    SPI_DATA = LOW;

    I2C_CLKPORT = OUTPUT;
    I2C_DATAPORT = OUTPUT;
    ADCON1 |= (0b00001111); //Needed if we're using PORTA as digital inputs Pg254 in datasheet
    ADJBUTTONPORT = INPUT;
    CHANGEBUTTONPORT = INPUT;
}

void init_bitbang(void) {
    BACKLEDPORT = OUTPUT;
    LEDPORT = OUTPUT;
    //Set all LCD pins as OUTPUT
    DATAPORT = OUTPUT;
    RSPORT = OUTPUT;
    RWPORT = OUTPUT;
    EPORT = OUTPUT;
    CS1PORT = OUTPUT;
    CS2PORT = OUTPUT;
    RSTPORT = OUTPUT;
    RST = HIGH;
    //init_spi();

    SPI_CLKPORT = OUTPUT;
    SPI_LATCHPORT = OUTPUT;
    SPI_DATAPORT = OUTPUT;
    SPI_CLK = LOW;
    SPI_LATCH = LOW;
    SPI_DATA = LOW;
}

void init_interrupt() {
    RCONbits.IPEN = 1; //Enable interrupt priority

    INTCON2bits.INTEDG2 = 0; //trigger on rising edge;
    INTCON3bits.INT2IP = 1; //Set to high priority

    INTCON3bits.INT2IE = 1; //Enable Interrupt 2
    INTCON3bits.INT2IF = 0; //Clear flag

    //    INTCON2bits.INTEDG1 = 0; //trigger on rising edge;
    //    INTCON3bits.INT1IP = 1; //Set to high priority
    //
    //    INTCON3bits.INT1IE = 1; //Enable Interrupt 1
    //    INTCON3bits.INT1IF = 0; //CLear flag;
    INTCON2bits.INTEDG0 = 0; //trigger on rising edge;//Pin0 always high priority

    INTCONbits.INT0IE = 1; //Enable Interrupt 0
    INTCONbits.INT0IF = 0; //Clear flag to enable

    INTCONbits.GIEH = 1; //Enable global high interrupts
    INTCONbits.GIEL = 1; //Enable global low interrupts
}

void init_timer0() {
    RCONbits.IPEN = 1; //Enable interrupt priority
    T0CONbits.T08BIT = 1; //Set 8bit
    T0CONbits.T0CS = 0; //Set to internal clock
    T0CONbits.T0SE = 0; //Doesn't matter in this case
    T0CONbits.PSA = 0; //Prescaler enable
    T0CONbits.T0PS2 = 1; //Prescaler value
    T0CONbits.T0PS1 = 1;
    T0CONbits.T0PS0 = 0;
    TMR0H = 0xF8; //Initialize counter value, just to speed things up
    TMR0L = 0x30;

    INTCONbits.TMR0IF = 0; //Clear overflow flag
    INTCONbits.TMR0IE = 1; //Enable timer0 interrupt

    INTCONbits.GIEH = 1; //Enable global high interrupts
    INTCONbits.GIEL = 1; //Enable global low interrupts


    T0CONbits.TMR0ON = 1; //Turn on timer
    INTCON2bits.TMR0IP = 0; //Set timer0 to high priority
}

void init_timer2() {
    T2CONbits.T2OUTPS3 = 1; //Post-scaler
    T2CONbits.T2OUTPS2 = 1;
    T2CONbits.T2OUTPS1 = 1;
    T2CONbits.T2OUTPS0 = 1;

    T2CONbits.T2CKPS1 = 1; //Pre-scaler
    T2CONbits.T2CKPS0 = 1;

    IPR1bits.TMR2IP = 0; //1=High priority, 0=Low priority

    PIR1bits.TMR2IF = 0; //Clear overflow flag
    PIE1bits.TMR2IE = 1; //Enable timer 1 interrupt

}

void displaytime(uint8 xloc, uint8 yloc, uint8 *str, uint8 *clrarr) {
    uint8 hourstr[3];
    uint8 minstr[3];
    uint8 secstr[3];
    uint8 sep[] = ":";

    hourstr[0] = str[0];
    hourstr[1] = str[1];
    hourstr[2] = '\0';

    minstr[0] = str[3];
    minstr[1] = str[4];
    minstr[2] = '\0';

    secstr[0] = str[6];
    secstr[1] = str[7];
    secstr[2] = '\0';

    write_inv(xloc, yloc, hourstr, clrarr[0], 1);
    write_inv(xloc, yloc + 25, sep, 0, 1);
    write_inv(xloc, yloc + 30, minstr, clrarr[1], 1);
    write_inv(xloc, yloc + 55, sep, 0, 1);
    write_inv(xloc, yloc + 60, secstr, clrarr[2], 1);

}

void displaydate(uint8 xloc, uint8 yloc, uint8 *str, uint8 *clrarr) {
    uint8 monthstr[10];
    uint8 datestr[3];
    uint8 yearstr[5];
    uint8 commastr[] = ",";

    uint8 i = 0;
    while (str[i] != ' ') { //Till space is month
        monthstr[i] = str[i];
        i++;
    }
    monthstr[i] = '\0'; //Put end of string char, very imp

    datestr[0] = str[++i];
    datestr[1] = str[++i];
    datestr[2] = '\0';
    i += 2;
    yearstr[0] = str[++i];
    yearstr[1] = str[++i];
    yearstr[2] = str[++i];
    yearstr[3] = str[++i];
    yearstr[4] = '\0';

    write_small_inv(xloc, yloc, monthstr, clrarr[0], 1);
    write_small_inv(xloc, yloc + 53, datestr, clrarr[1], 1);
    write_small_inv(xloc, yloc + 66, commastr, 0, 1);
    write_small_inv(xloc, yloc + 73, yearstr, clrarr[2], 1);
}

volatile uint8 stillPressed = 0;
volatile uint8 isBlink = 1;
uint8 isAdjust = 0;
uint8 selAdjIndex = 0;

void showtime(void) {
    int waitTime = 0;
    const int LONG_CLICK_INTERVAL = 15000;

    if (!ADJBUTTON) {
        if (!stillPressed) {
            __delay_ms(10);
            waitTime += 10;
            while (1) {

                if (waitTime > LONG_CLICK_INTERVAL) {
                    for (char j = 0; j < 20; j++) {
                        LEDPIN ^= HIGH;
                        __delay_ms(500);
                    }
                    if (isAdjust) { //Toggle adjust status
                        isAdjust = 0;
                        T2CONbits.TMR2ON = 0; //Turn off the timer
                        selAdjIndex = 0; //Reset this value
                    } else {
                        isAdjust = 1;
                        T2CONbits.TMR2ON = 1; //Turn on the timer
                    }
                    //Start timer 1
                    waitTime = 0;
                    stillPressed = 1;
                    break;
                }
                if (!ADJBUTTON) {
                    __delay_ms(10);
                    waitTime += 10;
                } else {
                    LEDPIN = HIGH;
                    __delay_ms(500);
                    LEDPIN = LOW;
                    __delay_ms(500);

                    waitTime = 0;
                    if (isAdjust) { //If in adjust mode, increment the change value
                        selAdjIndex++;
                    } else { //Otherwise refresh the screen to remove artifacts
                        glcd_clearscreen();
                    }

                    break;
                }
            }
        }
    } else {
        stillPressed = 0;
    }
    uint8 timearr[7];
    uint8 timestr[9]; //Don't forget end of string character!
    uint8 datestr[20];
    uint8 daystr[10];

    uint8 showday = 0;
    uint8 showtime[] = {0, 0, 0}; //Hour - minute - second, 0=show, 1=clear
    uint8 showdate[] = {0, 0, 0}; //Month - date - year, 0=show, 1=clear

    i2c_gettime(timearr);

    if (!CHANGEBUTTON) { //change button pressed
        switch (selAdjIndex) {
            case 0: //Set seconds to 0
                timearr[SECONDS] = 0;
                i2c_settime_targeted(timearr[SECONDS], SECONDS);
                break;
            case 1: //Increment hour                
                if (timearr[HOURS] == 23) {
                    timearr[HOURS] = 0;
                } else {
                    timearr[HOURS]++;
                }
                i2c_settime_targeted(timearr[HOURS], HOURS);
                break;
            case 2: //Increment min
                if (timearr[MINUTES] == 59) {
                    timearr[MINUTES] = 0;
                } else {
                    timearr[MINUTES]++;
                }
                i2c_settime_targeted(timearr[MINUTES], MINUTES);
                break;
            case 3: //Increment day
                if (timearr[DAY] == 7) {
                    timearr[DAY] = 1;
                } else {
                    timearr[DAY]++;
                }
                i2c_settime_targeted(timearr[DAY], DAY);
                break;
            case 4: //Increment month (1-12)
                if (timearr[MONTH] == 12) {
                    timearr[MONTH] = 1;
                } else {
                    timearr[MONTH]++;
                }
                i2c_settime_targeted(timearr[MONTH], MONTH);
                break;
            case 5: //Increment date (1-28/29, 1-30, 1-31)
            {
                uint8 month = timearr[MONTH];
                uint8 year = timearr[YEAR];
                uint8 maxdate = 31;
                if (year % 4 == 0) { //leap year
                    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
                        maxdate = 31;
                    } else if (month == 2) {
                        maxdate = 29;
                    } else {
                        maxdate = 30;
                    }
                } else { //Non-leap year
                    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
                        maxdate = 31;
                    } else if (month == 2) {
                        maxdate = 28;
                    } else {
                        maxdate = 30;
                    }
                }

                if (timearr[DATE] == maxdate) {
                    timearr[DATE] = 1;
                } else {
                    timearr[DATE]++;
                }
                i2c_settime_targeted(timearr[DATE], DATE);
            }
                break;
            case 6: //Increment year (0-99)
                if (timearr[YEAR] == 99) {
                    timearr[YEAR] = 0;
                } else {
                    timearr[YEAR]++;
                }
                i2c_settime_targeted(timearr[YEAR], YEAR);
                break;
            default:
                break;
        }

        isBlink = 0; //Don't blink when the numbers are changing
        __delay_ms(80); //Wait a bit before moving on so that we can have some control
    }

    gettimestring(timestr, timearr);
    getdatestring(datestr, timearr);
    getdaystring(daystr, timearr);




    if (isAdjust) {//Make blink
        if (isBlink) {//Timer1 sets the isBlink value
            switch (selAdjIndex) {
                case 0: //Adjusting seconds
                    showtime[2] = 1;
                    break;
                case 1: //Adjusting hours
                    showtime[0] = 1;
                    break;
                case 2: //Adjusting minutes
                    showtime[1] = 1;
                    break;
                case 3:
                    showday = 1;
                    break;
                case 4:
                    showdate[0] = 1;
                    break;
                case 5:
                    showdate[1] = 1;
                    break;
                case 6:
                    showdate[2] = 1;
                    break;
                default:
                    selAdjIndex = 0; //Reset it if it comes here!
                    break;
            }
        }
    }
    //write_inv(15, 25, timestr, 0, 1);
    displaytime(15, 25, timestr, showtime);
    write_small_inv(0, 38, daystr, showday, 1);
    //write_small_inv(40, 20, datestr, 0, 1);
    displaydate(40, 20, datestr, showdate);
}




volatile uint8 backled_changed = 0;
volatile const uint8 *art = swosti;
volatile uint8 rotate_counter = 0;
volatile uint8 displaymode = 0;
volatile uint8 paddlecount = 0;
volatile uint8 paddleswitchdir = 0;

void interrupt low_priority isr_low() {//Green LED

    //Check if the interrupt is from Timer0. This is because there could be other things causing interrupts
    if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        //TMR0H=0xF8;//Reinitialize starting values for the timer counter
        //TMR0L=0x30;
        //LEDPIN = 1^LEDPIN;//For testing only

        showtime();


        INTCONbits.TMR0IF = 0;
    }

    if (PIE1bits.TMR2IE && PIR1bits.TMR2IF) { //TImer 2 triggered (adjust mode)
        isBlink ^= 1;
        PIR1bits.TMR2IF = 0;
    }
}

void interrupt high_priority isr_high() {
    if (BUTTON1) {
        rotate_counter++;
        if (rotate_counter > 6) {
            rotate_counter = 0;
        }
        displaymode = rotate_counter;
        ismodechanged = TRUE;
        BUTTON1 = 0; //Re-enable
    } else if (BUTTON2) {
        BACKLED = 1^BACKLED;
        BUTTON2 = 0; //Re-enable
    }
}

void pong(void) {
    glcd_clearscreen();
    uint8 timearr[7];

    uint8 ballxposition = 10, ballyposition = 4;
    uint8 xstep = 2, ystep = 4; //ystep=3 seems to produce the best results
    uint8 isxdirectionforward = 1, isydirectionforward = 1;

    uint8 paddle1position = 25, paddle2position = 25;
    uint8 tstr[6];
    uint8 delayval = 100;
    i2c_gettime(timearr);
    uint8 currmin = timearr[1];
    uint8 currhour = timearr[2];
    uint8 minchange = 0, hourchange = 0;
    getscorestring(tstr, timearr);
    putfigure(ballxposition, ballyposition, ball, 0);
    while (1) {
        if (ismodechanged) break;
        write_inv_pong(0, 35, tstr, 0, 1);
        putpongdivider();
        putfigure(ballxposition, ballyposition, ball, 1);
        putfigure(paddle1position - 6, 0, paddle, 1);
        putfigure(paddle2position - 6, 124, paddle, 1);

        if (ballxposition < 4 && isxdirectionforward == 0)
            isxdirectionforward = TRUE;
        else if (ballxposition >= 56 && isxdirectionforward == 1)
            isxdirectionforward = FALSE;

        if (ballyposition <= 4 && isydirectionforward == 0) {
            if (hourchange) {
                getscorestring(tstr, timearr);
                write_inv_pong(0, 35, tstr, 0, 1);
                putpongdivider();
                for (int waitmin = 0; waitmin < 40; waitmin++) {
                    __delay_ms(500);
                }
                hourchange = FALSE;
                //Reset positions
                ballxposition = 10;
                ballyposition = 4;
                paddle1position = 25;
                paddle2position = 25;
                delayval = 100;
                currhour = timearr[2];
                currmin = timearr[1];
            } else {
                isydirectionforward = TRUE;
                if (delayval > 10) delayval -= 10;
                i2c_gettime(timearr);
                if (currhour == timearr[2] && currmin != timearr[1]) { //Minute is up, this side should win
                    minchange = TRUE;
                } else if (currhour != timearr[2] && currmin != timearr[1]) { //Hour is up, this side should win
                    hourchange = TRUE;
                }
                if (!minchange && !hourchange) {
                    getscorestring(tstr, timearr);
                }
                /* The step below is to introduce minor variations to xstep
                 * so that the angle of trajectory changes
                 * By using remainder when the seconds value is divided by 2, we
                 * have a poor man's random number generator
                 * Adding 1 so that the xstep value never becomes 0, then it'll keep
                 * going in a horizontal line
                 */
                xstep = (timearr[0]) % 2 + 1;
            }
        } else if (ballyposition >= 116 && isydirectionforward == 1) {
            if (minchange) {
                getscorestring(tstr, timearr);
                write_inv_pong(0, 35, tstr, 0, 1);
                putpongdivider();
                for (int waitmin = 0; waitmin < 40; waitmin++) {
                    __delay_ms(500);
                }
                minchange = FALSE;
                //Reset positions
                ballxposition = 10;
                ballyposition = 4;
                paddle1position = 25;
                paddle2position = 25;
                delayval = 50;
                currhour = timearr[2];
                currmin = timearr[1];

            } else {
                isydirectionforward = FALSE;
                if (delayval > 10) delayval -= 10;
                i2c_gettime(timearr);
                if (currhour == timearr[2] && currmin != timearr[1]) { //Minute is up, this side should win
                    minchange = TRUE;
                } else if (currhour != timearr[2] && currmin != timearr[1]) { //Hour is up, this side should win
                    hourchange = TRUE;
                }
                if (!minchange && !hourchange) {
                    getscorestring(tstr, timearr);
                }
                xstep = (timearr[0]) % 2 + 1;
            }
        }

        if (isxdirectionforward) {
            ballxposition += xstep;
        } else {
            ballxposition -= xstep;
        }

        if (isydirectionforward) {
            ballyposition += ystep;
            if (minchange) { //If end of minute, move in opposite direction to ball
                if (ballxposition > paddle2position && paddle2position > 8) {
                    paddle2position -= 5;
                } else if (ballxposition < paddle2position && paddle2position <= 53) {
                    paddle2position += 5;
                }
            } else {
                if (ballxposition > paddle2position && paddle2position <= 56) {
                    if (ballxposition - paddle2position > 6) {
                        paddle2position += 3; //if paddle is far from ball then move it faster
                    } else {
                        paddle2position++; //if paddle is not too far, move it slowly
                    }
                } else if (ballxposition < paddle2position && paddle2position > 8) {
                    if (paddle2position - ballxposition > 6) {
                        paddle2position -= 3;
                    } else {
                        paddle2position--;
                    }
                }
            }
        } else {
            ballyposition -= ystep;
            if (hourchange) { //If end of hour, move in opposite direction to ball
                if (ballxposition > paddle1position && paddle1position > 8) {
                    paddle1position -= 5;
                } else if (ballxposition < paddle1position && paddle1position <= 53) {
                    paddle1position += 5;
                }
            } else {
                if (ballxposition > paddle1position && paddle1position <= 56) {
                    if (ballxposition - paddle1position > 6) {
                        paddle1position += 3;
                    } else {
                        paddle1position++;
                    }
                } else if (ballxposition < paddle1position && paddle1position > 8) {
                    if (paddle1position - ballxposition > 6) {
                        paddle1position -= 3;
                    } else {
                        paddle1position--;
                    }
                }
            }
        }


        putfigure(ballxposition, ballyposition, ball, 0);
        putfigure(paddle1position - 6, 0, paddle, 0);
        putfigure(paddle2position - 6, 124, paddle, 0);
        for (int w = 0; w < delayval; w++) { //Need to use the loop here because __delay_ms(some variable) isn't working
            __delay_ms(6);
        }

    }
}

void main(void) {
    //init_bitbang();
    init_spireg();
    init_interrupt();
    LEDPIN = LOW;
    __delay_ms(500); //Wait for good measure!

    glcd_on();
    glcd_clearscreen();
    glcd_gotoy(5);
    glcd_gotox(1);

    i2c_init();
    uint8 led_toggle = 1;
    BACKLED = HIGH;
    //uint8 timearr[] = {0, 14, 19, 1, 23, 11, 14};
    //i2c_settime(timearr);
    init_timer0();
    init_timer2();


    //circle(32,64,31);
    //line(10,20,32,64);

    while (1) {
        //i2c_gettimestring(timestr);

        if (ismodechanged) {
            ismodechanged = FALSE;
            switch (displaymode) {
                case 0:
                    INTCONbits.TMR0IE = 0;
                    draw_inverted(ic);
                    break;
                case 1:
                    INTCONbits.TMR0IE = 0;
                    draw_inverted(truck);
                    break;
                case 2:
                    INTCONbits.TMR0IE = 0;
                    draw_inverted(goku);
                    break;
                case 3:
                    INTCONbits.TMR0IE = 0;
                    draw_inverted(mario);
                    break;
                case 4:
                    INTCONbits.TMR0IE = 0;
                    draw_inverted(swosti);
                    break;
                case 5:
                    INTCONbits.TMR0IE = 0;
                    pong();
                    break;
                case 6:
                    glcd_clearscreen();
                    INTCONbits.TMR0IE = 1;
                    break;
                default:
                    draw_inverted(swosti);
                    break;
            }

        }
    }
}