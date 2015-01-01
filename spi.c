#include <xc.h>
#include "spi.h"
#include "constants.h"
#include "definitions.h"


void spi_wait(void){
    __delay_us(1);
}

void spi_push(void){
    __delay_us(15); //Change this value if spi doesn't work (unknown reason)
    SPI_LATCH = HIGH;
    spi_wait();
    SPI_LATCH = LOW;
    spi_wait();
}

void spi_send(uint8 val) { //spi needed because pic18F4550 doesn't allow I2C and SPI simultaneously
    uint8 cmp = 0b10000000;
    //char isPush = 0;
    for(char i=0; i<8; i++) {
        if(cmp & val) {
            SPI_DATA = HIGH;
            spi_wait();
            SPI_CLK = HIGH;
            spi_wait();
            SPI_CLK = LOW;
            spi_wait();
        } else {
            SPI_DATA = LOW;
            spi_wait();
            SPI_CLK = HIGH;
            spi_wait();
            SPI_CLK = LOW;
            spi_wait();
        }
        cmp = cmp>>1;
    }
}

void spi_send2(uint8 val){
    SSPBUF = val;
    while(!SSPSTATbits.BF);
    //__delay_us(10);
}

void spi_write(uint8 val){
    spi_send2(val);
    spi_push();
}
