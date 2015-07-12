#pragma once

#include <avr/io.h>

/***** ATTENTION ***** ATTENTION ***** ATTENTION *****/
/*                                                   */
/* This code has only been tested with the ATMEGA 8  */
/*        Also seems to work on ATMEGA16A            */
/*                                                   */
/***** ATTENTION ***** ATTENTION ***** ATTENTION *****/

/*
 * Description:
 *  Use master-send mode to send data to slave with addresse 'addr'.
 *
 * Parameters:
 *  addr : Addresse of slave to send data to
 *  data : Pointer to buffer that holds the data to be sent
 *  len  : no. of bytes to send
 *
 * Returnvalue:
 *  0 if something went wrong. 1 else.
 */
unsigned char twi_send_data(unsigned char addr, unsigned char* data, unsigned char len);

/*
 * Description:
 *  Use master-receive mode to receive data from the slave
 *  with addresse 'addr'.
 *
 * Parameters:
 *  addr : Addresse of slave to receive data from
 *  data : Pointer to buffer that will hold the data
 *  len  : no. of bytes to receive
 *
 * Returnvalue:
 *  0 if something went wrong. 1 else.
 */
unsigned char twi_receive_data(unsigned char addr, unsigned char* data, unsigned char len);

/*
 * Description:
 *  Sends TWI Stop-condition
 */
void twi_stop(void);


