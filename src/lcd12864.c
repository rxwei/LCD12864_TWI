/*
 * lcd12864.c
 *
 *  Created on: Apr 8, 2012
 *      Author: xinranmsn
 */

#include "lcd12864.h"
#include "lcd12864_helper.h"
#include <avr/io.h>
//#define LCD12864_USING_TICK_DELAY
#ifndef LCD12864_USING_TICK_DELAY
#include <util/delay.h>
#endif

/* Private typedefs */

/* Private variables */

/* Private functions */
static void writecmd(struct lcd12864 *lcd, char cmd);
static void writedat(struct lcd12864 *lcd, char dat);

#ifdef LCD12864_USING_TICK_DELAY
static void delay_ms(struct lcd12864 *lcd, unsigned int ms);
#else
#define delay_ms(__lcd__, __ms__) _delay_ms(__ms__)
#endif

/* Commands */
#define CLEAR_SCREEN 0x01
#define AC_INIT		 0x02
#define CURSE_ADD	 0x06
#define FUN_MODE	 0x30
#define DISPLAY_ON	 0x0c
#define DISPLAY_OFF	 0x08
#define CURSE_DIR	 0x14
#define SET_CG_AC	 0x40
#define SET_DD_AC	 0x80

struct iopin iopin(char port, char ddr, char pin, char bitvec)
{
	struct iopin retiopin = { port, ddr, pin, bitvec };
	return retiopin;
}

struct ioport ioport(char port, char ddr, char pin, char flip)
{
	struct ioport retioport = { port, ddr, pin, flip };
	return retioport;
}

void lcd12864_deinit(struct lcd12864 *lcd)
{
	if (!lcd->stat.initialized)
		return;

	switch (lcd->mode) {
		case lcd12864_serial: {
			struct lcd12864_serpinstruct *pinstruct = &lcd->pinstruct.serpinstruct;
			pinstruct->cs.ddr &= ~pinstruct->cs.bitvec;
			pinstruct->sclk.ddr &= ~pinstruct->sclk.bitvec;
			pinstruct->sid.ddr &= ~pinstruct->sid.bitvec;
			break;
		}
		case lcd12864_parallel4: {
			struct lcd12864_par4pinstruct *pinstruct = &lcd->pinstruct.par4pinstruct;
			pinstruct->db.ddr &= ~(pinstruct->db.flip ? (0xf0 >> pinstruct->offset) : (0x0f << pinstruct->offset));
			pinstruct->rs.ddr &= ~pinstruct->rs.bitvec;
			pinstruct->rw.ddr &= ~pinstruct->rw.bitvec;
			pinstruct->e.ddr &= ~pinstruct->e.bitvec;
			break;
		}
		case lcd12864_parallel8: {
			struct lcd12864_par8pinstruct *pinstruct = &lcd->pinstruct.par8pinstruct;
			pinstruct->db.ddr = 0x00;
			pinstruct->rs.ddr &= ~pinstruct->rs.bitvec;
			pinstruct->rw.ddr &= ~pinstruct->rw.bitvec;
			pinstruct->e.ddr &= ~pinstruct->e.bitvec;
			break;
		}
	}
	lcd->stat.initialized = 0;
}

void lcd12864_init(struct lcd12864 *lcd)
{
	if (lcd->stat.initialized)
		return;

	switch (lcd->mode) {
		case lcd12864_serial: {
			struct lcd12864_serpinstruct *pinstruct = &lcd->pinstruct.serpinstruct;
			pinstruct->cs.ddr |= pinstruct->cs.bitvec;
			pinstruct->sclk.ddr |= pinstruct->sclk.bitvec;
			pinstruct->sid.ddr |= pinstruct->sid.bitvec;
			_clr_cs(lcd);
			_clr_sclk(lcd);
			_clr_sid(lcd);
			break;
		}
		case lcd12864_parallel4: {
			struct lcd12864_par4pinstruct *pinstruct = &lcd->pinstruct.par4pinstruct;
			pinstruct->db.ddr |= pinstruct->db.flip ? (0xf0 >> pinstruct->offset) : (0x0f << pinstruct->offset);
			pinstruct->rs.ddr |= pinstruct->rs.bitvec;
			pinstruct->rw.ddr |= pinstruct->rw.bitvec;
			pinstruct->e.ddr |= pinstruct->e.bitvec;
			_clr_rs(lcd);
			_clr_rw(lcd);
			_clr_e(lcd);
			_write_db(lcd, 0x00);
			break;
		}
		case lcd12864_parallel8: {
			struct lcd12864_par8pinstruct *pinstruct = &lcd->pinstruct.par8pinstruct;
			pinstruct->db.ddr = 0xff;
			pinstruct->rs.ddr |= pinstruct->rs.bitvec;
			pinstruct->rw.ddr |= pinstruct->rw.bitvec;
			pinstruct->e.ddr |= pinstruct->e.bitvec;
			_clr_rs(lcd);
			_clr_rw(lcd);
			_clr_e(lcd);
			_write_db(lcd, 0x00);
			break;
		}
	}

	writecmd(lcd, FUN_MODE);
	delay_ms(lcd, 5);
	writecmd(lcd, FUN_MODE);
	delay_ms(lcd, 5);
	writecmd(lcd, DISPLAY_ON);
	delay_ms(lcd, 5);
	writecmd(lcd, CLEAR_SCREEN);
	delay_ms(lcd, 5);

	lcd->stat.initialized = 1;
}

void lcd12864_tick(struct lcd12864 *lcd)
{
	++lcd->tick;
}

void lcd12864_setpos(struct lcd12864 *lcd, char x, char y)
{

}

void lcd12864_print(struct lcd12864 *lcd, char *str)
{
	while (*str != '\0' && *str) {
		writedat(lcd, *str);
		str++;
		delay_ms(lcd, 10);
	}
}

#ifdef LCD12864_USING_TICK_DELAY
static void delay_ms(struct lcd12864 *lcd, unsigned int ms)
{
	lcd->tick = 0;
	while (lcd->tick != ms);
}
#endif

static void writecmd(struct lcd12864 *lcd, char cmd)
{
	switch (lcd->mode) {
		case lcd12864_serial: {
			_set_cs(lcd);
			unsigned char i;
			// 1 1 1 1 1
			for (i = 0; i < 5; i++) {
				_set_sid(lcd);
				_set_sclk(lcd);
				_clr_sclk(lcd);
			}
			// RS
			_clr_sid(lcd);
			_set_sclk(lcd);
			_clr_sclk(lcd);
			// RW
			_clr_sid(lcd);
			_set_sclk(lcd);
			_clr_sclk(lcd);
			// 0
			_clr_sid(lcd);
			_set_sclk(lcd);
			_clr_sclk(lcd);
			// High part (7 6 5 4)
			for (i = 7; i >= 4; i--) {
				if ((cmd >> i) & 0x01)
					_set_sid(lcd);
				else
					_clr_sid(lcd);
				_set_sclk(lcd);
				_clr_sclk(lcd);
			}
			// 0 0 0 0
			for (i = 0; i < 4; i++) {
				_clr_sid(lcd);
				_set_sclk(lcd);
				_clr_sclk(lcd);
			}
			// Low part (3 2 1 0)
			for (i = 4; i >= 0; i--) {
				if ((cmd >> i) & 0x01)
					_set_sid(lcd);
				else
					_clr_sid(lcd);
				_set_sclk(lcd);
				_clr_sclk(lcd);
			}
			// 0 0 0 0
			for (i = 0; i < 4; i++) {
				_clr_sid(lcd);
				_set_sclk(lcd);
				_clr_sclk(lcd);
			}
			_clr_cs(lcd);
			break;
		}
		case lcd12864_parallel4: {
			_clr_rs(lcd);
			_clr_rw(lcd);
			_set_e(lcd);
			_write_db(lcd, cmd & 0x0f);
			delay_ms(lcd, 10);
			_write_db(lcd, cmd >> 4);
			delay_ms(lcd, 10);
			_clr_e(lcd);
			break;
		}
		case lcd12864_parallel8: {
			_clr_rs(lcd);
			_clr_rw(lcd);
			_set_e(lcd);
			_write_db(lcd, cmd);
			delay_ms(lcd, 10);
			_clr_e(lcd);
			break;
		}
	}
}

static void writedat(struct lcd12864 *lcd, char dat)
{
	switch (lcd->mode) {
		case lcd12864_serial: {
			_set_cs(lcd);
			unsigned char i;
			// 1 1 1 1 1
			for (i = 0; i < 5; i++) {
				_set_sid(lcd);
				_set_sclk(lcd);
				_clr_sclk(lcd);
			}
			// RS
			_set_sid(lcd);
			_set_sclk(lcd);
			_clr_sclk(lcd);
			// RW
			_clr_sid(lcd);
			_set_sclk(lcd);
			_clr_sclk(lcd);
			// 0
			_clr_sid(lcd);
			_set_sclk(lcd);
			_clr_sclk(lcd);
			// High part (7 6 5 4)
			for (i = 7; i >= 4; i--) {
				if ((dat >> i) & 0x01)
					_set_sid(lcd);
				else
					_clr_sid(lcd);
				_set_sclk(lcd);
				_clr_sclk(lcd);
			}
			// 0 0 0 0
			for (i = 0; i < 4; i++) {
				_clr_sid(lcd);
				_set_sclk(lcd);
				_clr_sclk(lcd);
			}
			// Low part (3 2 1 0)
			for (i = 4; i >= 0; i--) {
				if ((dat >> i) & 0x01)
					_set_sid(lcd);
				else
					_clr_sid(lcd);
				_set_sclk(lcd);
				_clr_sclk(lcd);
			}
			// 0 0 0 0
			for (i = 0; i < 4; i++) {
				_clr_sid(lcd);
				_set_sclk(lcd);
				_clr_sclk(lcd);
			}
			_clr_cs(lcd);
			break;
		}
		case lcd12864_parallel4: {
			_set_rs(lcd);
			_clr_rw(lcd);
			_set_e(lcd);
			_write_db(lcd, dat & 0x0f);
			delay_ms(lcd, 10);
			_write_db(lcd, dat >> 4);
			delay_ms(lcd, 10);
			_clr_e(lcd);
			break;
		}
		case lcd12864_parallel8: {
			_set_rs(lcd);
			_clr_rw(lcd);
			_set_e(lcd);
			_write_db(lcd, dat);
			delay_ms(lcd, 10);
			_clr_e(lcd);
			break;
		}
	}
}
