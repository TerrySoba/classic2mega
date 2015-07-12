#include "twi_func.h"

#include <avr/io.h>
#include <util/delay.h>     /* for _delay_ms() */
#include <avr/interrupt.h>

#include "bit_tools.h"

#define WAIT_FOR_TWI(TIMEOUT) {\
                                 long int tmp_n = 0;\
                                 while (!(TWCR & (1<<TWINT)) && (tmp_n < 100000)) tmp_n++;\
                                 if (!(TWCR & (1<<TWINT))) { SET_BIT(TWCR, TWINT); goto fend; }\
                              }

// #define WAIT_FOR_TWI(TIMEOUT) while (!(TWCR & (1<<TWINT)));

unsigned char twi_send_data(unsigned char addr, unsigned char* data, unsigned char len) {
    unsigned char i;
    // long int n;
    // volatile unsigned char timeout = 0;
    
    // enable TWI and send start condition
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

    // wait until it has been transmited
    // while (!(TWCR & (1<<TWINT)));
    WAIT_FOR_TWI(10);

    // check if start was sent
    if (((TWSR & 0xf8) != 0x08) && ((TWSR & 0xf8) != 0x10)) {
        goto fend;
    }

    // enable master write mode
    TWDR = (addr<<1) + 0; // WRITE MODE
    TWCR = (1<<TWINT) | (1<<TWEN);

    // wait until it has been transmited
    WAIT_FOR_TWI(10);
    
    if ((TWSR & 0xf8) != 0x18) {
        goto fend;
    }

    // _delay_us(20);

    // now send data
    for (i = 0; i < len; i++) {
        TWDR = data[i];
        TWCR = (1<<TWINT) | (1<<TWEN);

        // wait until it has been transmited
        // while (!(TWCR & (1<<TWINT)));
        WAIT_FOR_TWI(10);

        // check if data was acked by slave
        if ((TWSR & 0xf8) != 0x28) {
            goto fend;
        }
    }

    // send stop
    twi_stop();

    return 1;


    fend:
    // if an error occurs send stop and return 0
    twi_stop();
    return 0;
    
}

unsigned char twi_receive_data(unsigned char addr, unsigned char* data, unsigned char len) {
    unsigned char i;
    // long int n;
    // volatile unsigned char timeout = 0;
    
    // enable TWI and send start condition
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

    // wait until it has been transmited
    // while (!(TWCR & (1<<TWINT)));
    WAIT_FOR_TWI(10);
    
    // check if start was sent
    if (((TWSR & 0xf8) != 0x08) && ((TWSR & 0xf8) != 0x10)) {
        goto fend;
    }

    TWDR = (addr << 1) + 1; // READ MODE
    TWCR = (1<<TWINT) | (1<<TWEN);

    // wait until it has been transmited
    // while (!(TWCR & (1<<TWINT)));
    WAIT_FOR_TWI(10);

    
    // check if start was sent
    if ((TWSR & 0xf8) != 0x40) {
        goto fend;
    }

    // now get data
    for (i = 0; i < len; i++) {

        // get data
        TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);

        // wait until it has been transmited
        // while (!(TWCR & (1<<TWINT)));
        WAIT_FOR_TWI(10);

        // check if data was received
        if ((TWSR & 0xf8) != 0x50) {
            goto fend;
        }

        data[i] = TWDR; // get data
    }

    twi_stop();

    return 1;

    fend:

    twi_stop();
    return 0;
}


void twi_stop(void) {
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    _delay_us(10);
}
