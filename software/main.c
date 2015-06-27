#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

int main()
{
	wdt_disable();

	// Aktiviere PIN0 von PORT B.
    DDRD = 0b11111111;
 
    // Hauptschleife des Programms
    while (1) 
    {
		PORTD = 0b01000000;
		_delay_ms(250);
		PORTD = 0b00100000;
		_delay_ms(250);
    }

	return 0;
}

