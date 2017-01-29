/*
 * main.c
 *
 *  Created on: Apr 8, 2012
 *      Author: xinranmsn
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd12864.h"
#include "nokia5110lcd.h"

#define delay_ms(__ms__) _delay_ms(__ms__)

unsigned int timtick;

struct lcd12864 lcd1;

static inline void inittimer()
{
	TCCR0 |= (1 << CS02);
	TCNT0 = -(uint8_t)-250; // 1 millisecond scheduled interrupt
	TIMSK = (1 << TOIE0);
}

void initdev()
{
	asm("cli");
	inittimer();
	asm("sei");
}

#if 0
static void delay_ms(unsigned int ms)
{
	timtick = 0;
	while (timtick == ms);
}
#endif

int main(void)
{
	initdev();

	lcd1.mode                        = lcd12864_serial;
	lcd1.pinstruct.serpinstruct.cs   = iopin(PORTB, DDRB, PINB, _BV(PB2));
	lcd1.pinstruct.serpinstruct.sid  = iopin(PORTB, DDRB, PINB, _BV(PB1));
	lcd1.pinstruct.serpinstruct.sclk = iopin(PORTB, DDRB, PINB, _BV(PB0));
	lcd1.pinstruct.serpinstruct.rst  = iopin(PORTB, DDRB, PINB, _BV(PB3));

	lcd12864_init(&lcd1);
	lcd12864_setpos(&lcd1, 0, 0);
	lcd12864_print(&lcd1, "hello");

	nokia5110lcd_init();
	nokia5110lcd_printstratpos(0, 0, "Hello");

	while (1) {

	}
}

ISR(TIMER0_OVF_vect)
{
	TCNT0 = (uint8_t)-250;
	++timtick;
	lcd12864_tick(&lcd1);
}
