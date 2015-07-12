#pragma once

/***** ATTENTION ***** ATTENTION ***** ATTENTION *****/
/*                                                   */
/* This code has only been tested with the ATMEGA 8  */
/*        Also seems to work on ATMEGA16A            */
/*                                                   */
/***** ATTENTION ***** ATTENTION ***** ATTENTION *****/

#include <stdint.h>

#include "bit_tools.h"

#ifndef TW_SCL
    #error you have to define TW_SCL before including this file
#endif

// define every possible value of TWPS
#define _TWBR_0 ((F_CPU - 16ul * TW_SCL) / (2ul * 1ul * TW_SCL))
#define _TWBR_1 ((F_CPU - 16ul * TW_SCL) / (2ul * 4ul * TW_SCL))
#define _TWBR_2 ((F_CPU - 16ul * TW_SCL) / (2ul * 16ul * TW_SCL))
#define _TWBR_3 ((F_CPU - 16ul * TW_SCL) / (2ul * 64ul * TW_SCL))

// now check if any TWPS leads to a possible value for TWBR
#if ((_TWBR_0 < 0xff) && (_TWBR_0 > 0))
    #define _TWBR _TWBR_0
    #define _TWPS 0
#elif ((_TWBR_1 < 0xff) && (_TWBR_1 > 0))
    #define _TWBR _TWBR_1
    #define _TWPS 1
#elif ((_TWBR_2 < 0xff) && (_TWBR_2 > 0))
    #define _TWBR _TWBR_2
    #define _TWPS 2
#elif ((_TWBR_3 < 0xff) && (_TWBR_3 > 0))
    #define _TWBR _TWBR_3
    #define _TWPS 3
#else
    #error TWI frequency is not possible
#endif

/*
 * Description:
 *  This macro sets the twi-speed to the value set in TW_SCL
 */
#define twi_init() { TWBR = _TWBR; SET_BIT_VALUE(TWSR, 0, (1 & _TWPS)); SET_BIT_VALUE(TWSR, 1, (2 & _TWPS) >> 1); }

