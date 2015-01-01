/* 
 * File:   i2c.h
 * Author: ryder
 *
 * Created on October 29, 2014, 9:16 PM
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "definitions.h"
void iput(uint8 val);
void i2c_init(void);
void i2c_start(void);
void i2c_send(uint8 val);
void i2c_stop(void);
uint8 i2c_read(uint8 send_ack);
uint8 bcd_to_dec(uint8 val);
uint8 dec_to_bcd(uint8 val);
void i2c_settime(uint8 *timearr);
void i2c_settime_targeted(uint8 val, uint8 address);
void i2c_gettimestring(uint8 *timestr);
void gettimestring(uint8 *timestr, uint8 *timearr);
void i2c_gettime(uint8 *timearr);
void getdatestring(uint8 *datestr, uint8 *timearr);
void getdaystring(uint8 *daystr, uint8 *timearr);
void getscorestring(uint8 *scorestr, uint8 *timearr);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

