/* Name: main.c
 * Project: classic2mega, a Wii Classic Controller to mega drive / genesis adapter
 * Author: Torsten Stremlau <torsten@stremlau.de>
 * Creation Date: 2015-07-11
 * Tabsize: 4
 *
 * License: GNU GPL v2 (see License.txt), GNU GPL v3
 */

#define TW_SCL 100000 // TWI frequency in Hz

#include "twi_speed.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include "bit_tools.h"
#include "twi_func.h"

#include <stdint.h>
#include <stdbool.h>

#define SLAVE_ADDR 0x52     /* address of classic controller and nunchuck */


typedef struct {
    uint8_t   buttons[2];
} report_t;


uint8_t rawData[6];

#define REPORT_BUFFER_COUNT 8

static report_t reportBuffer[REPORT_BUFFER_COUNT];
static int reportBufferPos = 0;

/* I2C initialization */
void myI2CInit(void) {
    twi_init(); // this is a macro from "twi_speed.h"
}

// initialize Wii controller
unsigned char myWiiInit(void) {
    unsigned char buf[2] = {0x40, 0x00};

    return twi_send_data(SLAVE_ADDR, buf, 2);
}


unsigned char fillReportWithWii(void) {
    uint8_t i;
    unsigned char buf[6];

    /* send 0x00 to the controller to tell him we want data! */
    buf[0] = 0x00;
    
    if (!twi_send_data(SLAVE_ADDR, buf, 1)) {
        goto fend;
    }

    _delay_ms(2);
    // _delay_us(100);

    // ------ now get 6 bytes of data
    
    if (!(twi_receive_data(SLAVE_ADDR, buf, 6))) {
        goto fend;
    }

    // for (i = 0; i < 6; i++) {
    for (i = 0; i < 6; i++) {
        rawData[i] = (buf[i] ^ 0x17) + 0x17; // decrypt data
    }

    // split out buttons
#define BTN_rT      GET_BIT(~rawData[4], 1)
#define BTN_start   GET_BIT(~rawData[4], 2)
#define BTN_home    GET_BIT(~rawData[4], 3)
#define BTN_select  GET_BIT(~rawData[4], 4)
#define BTN_lT      GET_BIT(~rawData[4], 5)
#define BTN_down    GET_BIT(~rawData[4], 6)
#define BTN_right   GET_BIT(~rawData[4], 7)
#define BTN_up      GET_BIT(~rawData[5], 0)
#define BTN_left    GET_BIT(~rawData[5], 1)
#define BTN_rZ      GET_BIT(~rawData[5], 2)
#define BTN_x       GET_BIT(~rawData[5], 3)
#define BTN_a       GET_BIT(~rawData[5], 4)
#define BTN_y       GET_BIT(~rawData[5], 5)
#define BTN_b       GET_BIT(~rawData[5], 6)
#define BTN_lZ      GET_BIT(~rawData[5], 7)

    // button mappings (button 0..15)
#define BUTTON_X              0
#define BUTTON_A              1
#define BUTTON_B              2
#define BUTTON_Y              3
#define BUTTON_START          4
#define BUTTON_SELECT         5
#define BUTTON_HOME           6
#define BUTTON_RIGHT_TRIGGER  7
#define BUTTON_LEFT_TRIGGER   8
#define BUTTON_RIGHT_Z        9
#define BUTTON_LEFT_Z        10
#define BUTTON_UP            11
#define BUTTON_DOWN          12
#define BUTTON_LEFT          13
#define BUTTON_RIGHT         14
#define NO_BUTTON            15

#define SET_BUTTON(BUTTON, SOURCE) SET_BIT_VALUE(reportBuffer[reportBufferPos].buttons[BUTTON/8],BUTTON%8,SOURCE)

    reportBufferPos = (reportBufferPos + 1) % REPORT_BUFFER_COUNT;

    SET_BUTTON(BUTTON_X, BTN_x);
    SET_BUTTON(BUTTON_A, BTN_a);
    SET_BUTTON(BUTTON_B, BTN_b);
    SET_BUTTON(BUTTON_Y, BTN_y);
    SET_BUTTON(BUTTON_START, BTN_start);
    SET_BUTTON(BUTTON_SELECT, BTN_select);
    SET_BUTTON(BUTTON_HOME, BTN_home);
    SET_BUTTON(BUTTON_RIGHT_TRIGGER, BTN_rT);
    SET_BUTTON(BUTTON_LEFT_TRIGGER, BTN_lT);
    SET_BUTTON(BUTTON_RIGHT_Z, BTN_rZ);
    SET_BUTTON(BUTTON_LEFT_Z, BTN_lZ);
    SET_BUTTON(BUTTON_UP, BTN_up);
    SET_BUTTON(BUTTON_DOWN, BTN_down);
    SET_BUTTON(BUTTON_LEFT, BTN_left);
    SET_BUTTON(BUTTON_RIGHT, BTN_right);
    SET_BUTTON(NO_BUTTON, 0);


    return 1;

fend:
    // TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    _delay_us(20);

    /*all_fine:*/
    
    twi_stop();
    return 0;
}


/* This function sets up stuff */
void myInit(void) {

    _delay_ms(300);
    // SET_BIT(PORTC,0);
    myI2CInit();
    _delay_ms(120);
    myWiiInit();
    _delay_ms(1);
    fillReportWithWii();
}


/**
 * This enum maps megadrive buttons to the io pin of PORTA
 * of the ATMEGA16.
 *  e.g. 3 means PA3.
 */
typedef enum {
    MEGADRIVE_A = 3,
    MEGADRIVE_B = 2,
    MEGADRIVE_C = 0,
    MEGADRIVE_START = 1,
    MEGADRIVE_LEFT = 5,
    MEGADRIVE_RIGHT = 4,
    MEGADRIVE_UP = 7,
    MEGADRIVE_DOWN = 6
} megadrive_button;


/**
 * This enum amiga mouse signals to the io pin of PORTA
 * of the ATMEGA16.
 *  e.g. 3 means PA3.
 */
typedef enum {
	MOUSE_V_PULSE = 7,
	MOUSE_VQ_PULSE = 5,
	MOUSE_H_PULSE = 6,
	MOUSE_HQ_PULSE = 4,
	MOUSE_LEFT_BUTTON = 2,
	MOUSE_RIGHT_BUTTON = 0,
} mouse_signals;


/**
 * Sets the megadrive port with given button number (0-7)
 * to pressed state if pressed is true, to non pressed
 * state otherwise.
 * \param button The megadrive button to set
 * \param pressed If true, button will be set to pressed, to unpressed otherwise.
 */
void setMegadriveButton(megadrive_button button, bool pressed)
{
    if (pressed)
    {
        // make pin an output and set to low
        CLR_BIT(PORTA, button);
        SET_BIT(DDRA, button);
    } else {
        // make pin an input (high impedance) and set to high (pull up)
        CLR_BIT(DDRA, button);
        SET_BIT(PORTA, button);
    }
}

#define buttonPressed(button, buffer) ((buffer.buttons[button/8] & (1 << (button%8))) != 0)

/**
 * This function takes the global variable reportBuffer
 * and sets the megadrive port accordingly.
 */
void setMegadrive(bool jumpAndRunMode, bool delayMode)
{
    report_t report;
    if(delayMode)
    {
        report = reportBuffer[(reportBufferPos + 1) % REPORT_BUFFER_COUNT];
    } else {
        report = reportBuffer[reportBufferPos];
    }

    if (jumpAndRunMode)
    {
        setMegadriveButton(MEGADRIVE_A, false);
        setMegadriveButton(MEGADRIVE_B, buttonPressed(BUTTON_A, report) || buttonPressed(BUTTON_B, report));
        setMegadriveButton(MEGADRIVE_C, false);
        setMegadriveButton(MEGADRIVE_START, false);
        setMegadriveButton(MEGADRIVE_UP, buttonPressed(BUTTON_X, report) || buttonPressed(BUTTON_Y, report) || buttonPressed(BUTTON_UP, report));
    }
    else
    {
        setMegadriveButton(MEGADRIVE_A, buttonPressed(BUTTON_Y, report));
        setMegadriveButton(MEGADRIVE_B, buttonPressed(BUTTON_X, report) || buttonPressed(BUTTON_B, report));
        setMegadriveButton(MEGADRIVE_C, buttonPressed(BUTTON_A, report));
        setMegadriveButton(MEGADRIVE_START, buttonPressed(BUTTON_START, report));
        setMegadriveButton(MEGADRIVE_UP, buttonPressed(BUTTON_UP, report));
    }

    setMegadriveButton(MEGADRIVE_LEFT, buttonPressed(BUTTON_LEFT, report));
    setMegadriveButton(MEGADRIVE_RIGHT, buttonPressed(BUTTON_RIGHT, report));
    setMegadriveButton(MEGADRIVE_DOWN, buttonPressed(BUTTON_DOWN, report));
}


/**
 *  This function sets the outputs used by the megadrive plug to
 *  high impedance and pull down.
 */
void setupMegadrive()
{
    // all inputs == high impedance
    DDRA = 0x00;

    // all low == pulldown
    PORTA = 0x00;
}

static int mouseStateHorizontal = 0;
static int mouseStateVertical = 0;

/* ------------------------------------------------------------------------- */

int main(void)
{
    // bool jumpAndRunMode = false;
    // bool delayMode = false;

start:
    cli();
    wdt_disable();
    /* Even if you don't use the watchdog, turn it off here. On newer devices,
     * the status of the watchdog (on/off, period) is PRESERVED OVER RESET!
     */

    // initialize reportBuffer with 0
    for (int i = 0; i < REPORT_BUFFER_COUNT; ++i)
    {
        reportBuffer[i].buttons[0] = 0;
        reportBuffer[i].buttons[1] = 0;
    }

    // make PORTD_0 (LED) an output pin
    SET_BIT(DDRD, 0);

    sei();
    myInit();
    setupMegadrive();

    for (int i = 0; i < 8; ++i)
    {
    	setMegadriveButton(i, 0);
    }

    for(;;){                /* main event loop */
        wdt_reset();
        fillReportWithWii();
        
        // setMegadrive(jumpAndRunMode, delayMode);
        // ++counter;

//        if (reportBufferPos % REPORT_BUFFER_COUNT == 0) SET_BIT(PORTD, 0);
//        else CLR_BIT(PORTD, 0);

        report_t report = reportBuffer[reportBufferPos];

//        if (buttonPressed(BUTTON_HOME, report) && buttonPressed(BUTTON_START, report))
//        {
//            // enable jump and run mode
//            jumpAndRunMode = true;
//            // SET_BIT(PORTD, 0);
//        }
//
//        if (buttonPressed(BUTTON_HOME, report) && buttonPressed(BUTTON_SELECT, report))
//        {
//            // disable jump and run mode
//            jumpAndRunMode = false;
//            // CLR_BIT(PORTD, 0);
//        }
//
//        if (buttonPressed(BUTTON_HOME, report) && buttonPressed(BUTTON_DOWN, report))
//        {
//            // enable delay mode
//            delayMode = true;
//            SET_BIT(PORTD, 0);
//        }
//
//        if (buttonPressed(BUTTON_HOME, report) && buttonPressed(BUTTON_UP, report))
//        {
//            // disable delay mode
//            delayMode = false;
//            CLR_BIT(PORTD, 0);
//        }

        if (buttonPressed(BUTTON_LEFT, report)) --mouseStateHorizontal;
        if (buttonPressed(BUTTON_RIGHT, report)) ++mouseStateHorizontal;
        if (buttonPressed(BUTTON_DOWN, report)) ++mouseStateVertical;
        if (buttonPressed(BUTTON_UP, report)) --mouseStateVertical;


        switch(mouseStateHorizontal % 4)
        {
        case 0:
        	setMegadriveButton(MOUSE_H_PULSE, 0);
        	setMegadriveButton(MOUSE_HQ_PULSE, 0);
        	break;
        case 1:
			setMegadriveButton(MOUSE_H_PULSE, 1);
			setMegadriveButton(MOUSE_HQ_PULSE, 0);
			break;
        case 2:
			setMegadriveButton(MOUSE_H_PULSE, 1);
			setMegadriveButton(MOUSE_HQ_PULSE, 1);
			break;
		case 3:
			setMegadriveButton(MOUSE_H_PULSE, 0);
			setMegadriveButton(MOUSE_HQ_PULSE, 1);
			break;
        }

        switch(mouseStateVertical % 4)
		{
		case 0:
			setMegadriveButton(MOUSE_V_PULSE, 0);
			setMegadriveButton(MOUSE_VQ_PULSE, 0);
			break;
		case 1:
			setMegadriveButton(MOUSE_V_PULSE, 1);
			setMegadriveButton(MOUSE_VQ_PULSE, 0);
			break;
		case 2:
			setMegadriveButton(MOUSE_V_PULSE, 1);
			setMegadriveButton(MOUSE_VQ_PULSE, 1);
			break;
		case 3:
			setMegadriveButton(MOUSE_V_PULSE, 0);
			setMegadriveButton(MOUSE_VQ_PULSE, 1);
			break;
		}


        /* If the gamepad starts feeding us 0xff, we have to restart to recover */
        if ((rawData[0] == 0xff) && (rawData[1] == 0xff) && (rawData[2] == 0xff) && (rawData[3] == 0xff) && (rawData[4] == 0xff) && (rawData[5] == 0xff)) {
            goto start;
        }
    }
    return 0;
}
