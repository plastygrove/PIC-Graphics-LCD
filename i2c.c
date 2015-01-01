#include <xc.h>
#include <stdio.h>
#include "definitions.h"
#include "constants.h"

void i2c_wait(void){
    __delay_ms(1);
}
void iput(uint8 val) {
    I2C_DATA = val;
    i2c_wait();
    I2C_CLK = HIGH;
    i2c_wait();
    I2C_CLK = LOW;
    i2c_wait();
}

void i2c_init(void) {
    I2C_DATA = HIGH;
    I2C_CLK = HIGH;
    i2c_wait();
}

void i2c_start(void) {
    I2C_DATA = LOW;
    i2c_wait();
    I2C_CLK = LOW;
    i2c_wait();
}

void i2c_send(uint8 val) {
    uint8 cmp = 0b10000000;
    for(int i=0; i<8; i++){
        if(cmp & val){
            iput(HIGH);
        } else {
            iput(LOW);
        }
        cmp = cmp>>1;
    }

    I2C_DATA = HIGH;
    i2c_wait();
    I2C_CLK = HIGH;
    i2c_wait();

    I2C_DATAPORT = INPUT;
    i2c_wait();
    if(I2C_DATAREAD == HIGH) {
        LEDPIN = HIGH;
        __delay_ms(500);
        LEDPIN = LOW;
        __delay_ms(500);
    } else {
        //LEDPIN = HIGH;
    }
    I2C_DATAPORT = OUTPUT;
    I2C_CLK = LOW;
}

void i2c_stop(void) {
    I2C_DATA = LOW;
    I2C_CLK = HIGH;
    I2C_DATA = HIGH;
}


uint8 i2c_read(uint8 send_ack){
    I2C_CLK = LOW;
    i2c_wait();
    I2C_DATA = HIGH;
    i2c_wait();

    uint8 retval = 0;
    I2C_DATAPORT = INPUT;
    i2c_wait();
    for(int i=0; i<8; i++) {
        I2C_CLK = HIGH;
        i2c_wait();
        uint8 inbit = I2C_DATAREAD;
        i2c_wait();
        if(inbit) {
            retval = retval | (0b10000000 >> i);
        }
        I2C_CLK = LOW;
        i2c_wait();
    }
    I2C_DATAPORT = OUTPUT;
    i2c_wait();
    if(send_ack) {
        I2C_DATA = LOW;
    } else {
        I2C_DATA = HIGH;
    }
    i2c_wait();
    I2C_CLK = HIGH;
    i2c_wait();
    I2C_CLK = LOW;
    i2c_wait();
    return retval;
}

uint8 bcd_to_dec(uint8 val){
    uint8 lower = val & 0b00001111;
    uint8 higher = (val & 0b11110000) >> 4;
    return (higher * 10 + lower);
}

uint8 dec_to_bcd(uint8 val) {
    uint8 lower = val % 10;
    uint8 higher = val/10;
    return ((higher<<4)|lower);
}

void i2c_settime(uint8 *timearr){
    i2c_start();
    i2c_send(DS1307_I2C_WRITEADDRESS);
    i2c_send(0); //Seconds address
    for(int i=0; i<7; i++){
        i2c_send(dec_to_bcd(*(timearr+i)));
    }

    i2c_stop();
    /* //Uncomment this to stop timer
    i2c_start();
    i2c_send(DS1307_I2C_WRITEADDRESS);
    i2c_send(0); //Seconds address
    i2c_send(0b10000000);
    i2c_stop();
      */
}

void i2c_settime_targeted(uint8 val, uint8 address){
    i2c_start();
    i2c_send(DS1307_I2C_WRITEADDRESS);
    i2c_send(address); //0-sec, 1-min, 2-hr, 3-day, 4-date, 5-month, 6-year
    i2c_send(dec_to_bcd(val));
    i2c_stop();
}

void i2c_gettimestring(uint8 *timestr) {
    i2c_start();
    i2c_send(DS1307_I2C_WRITEADDRESS);
    i2c_send(0); //0 = Seconds
    i2c_stop();

    i2c_start();
    i2c_send(DS1307_I2C_READADDRESS);
    uint8 time[7];
    for(int i=0; i<6; i++){
        time[i] = bcd_to_dec(i2c_read(1));
    }
    time[6] = bcd_to_dec(i2c_read(0));
    sprintf(timestr, "%02d:%02d:%02d", time[2], time[1], time[0]);
    i2c_stop();
}

void gettimestring(uint8 *timestr, uint8 *timearr) {
    sprintf(timestr, "%02d:%02d:%02d", timearr[2], timearr[1], timearr[0]);
    i2c_stop();
}


void i2c_gettime(uint8 *timearr){//timearr should be 7 bytes long
    i2c_start();
    i2c_send(DS1307_I2C_WRITEADDRESS);
    i2c_send(0); //0 = Seconds
    i2c_stop();

    i2c_start();
    i2c_send(DS1307_I2C_READADDRESS);

    for(int i=0; i<6; i++){
        *(timearr+i) = bcd_to_dec(i2c_read(1));
    }
    *(timearr+6) = bcd_to_dec(i2c_read(0));

    i2c_stop();
}

void getdatestring(uint8 *datestr, uint8 *timearr){
    switch(*(timearr+5)){
        case 1:
            sprintf(datestr, "January %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 2:
            sprintf(datestr, "February %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 3:
            sprintf(datestr, "March %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 4:
            sprintf(datestr, "April %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 5:
            sprintf(datestr, "May %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 6:
            sprintf(datestr, "June %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 7:
            sprintf(datestr, "July %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 8:
            sprintf(datestr, "August %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 9:
            sprintf(datestr, "September %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 10:
            sprintf(datestr, "October %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 11:
            sprintf(datestr, "November %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        case 12:
            sprintf(datestr, "December %02d, 20%02d", *(timearr+4), *(timearr+6));
            break;
        default:
            sprintf(datestr, "No month");
            break;
    }
}

void getdaystring(uint8 *daystr, uint8 *timearr){
    switch(*(timearr+3)){
        case 1:
            sprintf(daystr, " Sunday  ");
            break;
        case 2:
            sprintf(daystr, " Monday  ");
            break;
        case 3:
            sprintf(daystr, " Tuesday ");
            break;
        case 4:
            sprintf(daystr, "Wednesday");
            break;
        case 5:
            sprintf(daystr, "Thursday ");
            break;
        case 6:
            sprintf(daystr, " Friday  ");
            break;
        case 7:
            sprintf(daystr, "Saturday ");
            break;
        default:
            sprintf(daystr, "No day");
    }
}

void getscorestring(uint8 *scorestr, uint8 *timearr){
    sprintf(scorestr, "%02d:%02d", timearr[2], timearr[1]);
}




