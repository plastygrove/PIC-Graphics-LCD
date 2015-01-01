/* 
 * File:   definitions.h
 * Author: ryder
 *
 * Created on October 25, 2014, 11:35 AM
 */

#ifndef DEFINITIONS_H
#define	DEFINITIONS_H

#ifdef	__cplusplus
extern "C" {
#endif
#define _XTAL_FREQ 1000000L //Not accurate

#define LEDPIN LATBbits.LATB4
#define LEDPORT TRISBbits.RB4
#define BACKLED LATDbits.LATD1
#define BACKLEDPORT TRISDbits.RD1
#define OUTPUT 0
#define INPUT 1
#define LOW 0
#define HIGH 1
#define TRUE 1
#define FALSE 0

/**
 Need an 8-bit integer, so using unsigned char
 */
#define uint8 unsigned char
#define int8 signed char

#define RS LATDbits.LATD5
#define RSPORT TRISDbits.RD5
#define RW LATDbits.LATD4
#define RWPORT TRISDbits.RD4
#define E LATCbits.LATC2
#define EPORT TRISCbits.RC2
#define CS1 LATEbits.LATE0
#define CS1PORT TRISEbits.RE0
#define CS2 LATEbits.LATE1
#define CS2PORT TRISEbits.RE1
#define RST LATBbits.LATB3
#define RSTPORT TRISBbits.RB3

#define DATA LATD
#define DATAPORT TRISD

#define SPI_CLK LATBbits.LATB1
#define SPI_CLKPORT TRISBbits.RB1
#define SPI_LATCH LATAbits.LA5
#define SPI_LATCHPORT TRISAbits.RA5
#define SPI_DATA LATCbits.LATC7
#define SPI_DATAPORT TRISCbits.RC7

#define I2C_DATA LATDbits.LATD2
#define I2C_DATAPORT TRISDbits.RD2
#define I2C_DATAREAD PORTDbits.RD2
#define I2C_CLK LATDbits.LATD3
#define I2C_CLKPORT TRISDbits.RD3

#define DS1307_I2C_WRITEADDRESS 0b11010000
#define DS1307_I2C_READADDRESS 0b11010001

#pragma config PLLDIV   = 5
#pragma config CPUDIV   = OSC2_PLL3
#pragma config FOSC     = HS //INTOSC_HS //For Internal clock
#pragma config WDT      = OFF
#pragma config LVP      = OFF
#pragma config BOR      = OFF
#pragma config MCLRE    = ON
#pragma config PWRT     = ON
#pragma config PBADEN   = OFF

#define byte uint8_t
#define BUTTON1 INTCON3bits.INT2IF
#define BUTTON2 INTCONbits.INT0IF

#define ADJBUTTONPORT TRISAbits.RA0
#define ADJBUTTON PORTAbits.RA0

#define CHANGEBUTTONPORT TRISAbits.RA1
#define CHANGEBUTTON PORTAbits.RA1

//Define DS1307 addresses
#define SECONDS 0x00
#define MINUTES 0x01
#define HOURS 0x02
#define DAY 0x03
#define DATE 0x04
#define MONTH 0x05
#define YEAR 0x06



#ifdef	__cplusplus
}
#endif

#endif	/* DEFINITIONS_H */

