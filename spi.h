/* 
 * File:   spi.h
 * Author: ryder
 *
 * Created on October 26, 2014, 10:18 AM
 */

#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif
#include "definitions.h"
    
void spi_wait(void);
void spi_push(void);
void spi_send(uint8 val);
void spi_send2(uint8 val);
void spi_write(uint8 val);


#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */

