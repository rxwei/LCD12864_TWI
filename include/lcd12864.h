/*
 * lcd12864.h
 *
 *  Created on: Apr 8, 2012
 *      Author: xinranmsn
 *
 *
 *  Serial driver for LCD12864 (ST7920 controller) on Atmel AVR 8-bit microcontrollers.
 *  Tested on ATmega16, ATmega8.
 */

#ifndef LCD12864_H_
#define LCD12864_H_

/*
 * @brief Pin structure
 * @member PORT, DDR, PIN and BV
 * @len 4 bytes
 */
struct iopin {
	char port, ddr, pin, bitvec __attribute__ ((__aligned__(1)));
};

/*
 * @brief Pin structure constructor
 * @param PORT, DDR, PIN and BV
 * @retval iopin structure
 */
struct iopin iopin(char port, char ddr, char pin, char bitvec);

/*
 * @brief Port structure
 * @member PORT, DDR, PIN and BV
 * @len 3 bytes
 */
struct ioport {
	char port, ddr, pin         __attribute__ ((__aligned__(1)));
	char flip                   __attribute__ ((__aligned__(1)));
};

/*
 * @brief Port structure constructor
 * @param PORT, DDR, PIN and flip
 * @retval ioport structure
 */
struct ioport ioport(char port, char ddr, char pin, char flip);

/*
 * @brief Structure for LCD pin initialization (serial mode)
 * @member IO SFRs and bits of SCL, SDA, CS pins
 * @len 9 bytes
 */
struct lcd12864_serpinstruct {
	struct iopin sclk, sid, cs, rst  __attribute__ ((__aligned__(4)));
};

/*
 * @brief Structure for LCD pin initialization (half-byte parallel mode)
 * @member IO SFRs and bits of SCL, SDA, CS pins
 * @len 17 bytes
 */
struct lcd12864_par4pinstruct {
	struct iopin  rs, rw, e, rst     __attribute__ ((__aligned__(4)));
	struct ioport db                 __attribute__ ((__aligned__(4)));
	char          offset             __attribute__ ((__aligned__(1)));
};

/*
 * @brief Structure for LCD pin initialization (byte parallel mode)
 * @member IO SFRs and bits of SCL, SDA, CS pins
 * @len 16 bytes
 */
struct lcd12864_par8pinstruct {
	struct iopin  rs, rw, e, rst     __attribute__ ((__aligned__(4)));
	struct ioport db                 __attribute__ ((__aligned__(4)));
};

/*
 * @brief LCD mode
 * @member Serial, half-byte parallel, byte parallel
 */
enum lcd12864_mode {
	lcd12864_serial = 0,
	lcd12864_parallel4 = 1,
	lcd12864_parallel8 = 2
};

/*
 * @brief LCD status bit field
 * @member
 * @length 1 byte
 */
struct lcd12864_stat {
	char initialized :1;
	char enabled     :1;
	// ...
};

/*
 * @brief LCD class
 * @member
 * @len
 */
struct lcd12864 {
	enum lcd12864_mode mode;
	struct lcd12864_stat stat;
	unsigned int tick;
	union {
		struct lcd12864_serpinstruct serpinstruct;
		struct lcd12864_par4pinstruct par4pinstruct;
		struct lcd12864_par8pinstruct par8pinstruct;
	} pinstruct;
};

/*
 * @brief LCD object attribute setter
 * @param
 * @retval None
 */


/*
 * @brief Command states
 * @member DISABLE and ENABLE
 */
enum lcd12864_cmd {
	DISABLE = 0,
	ENABLE = 1
};

/*
 * @brief De-initializes pins and the LCD.
 * @param LCD instance
 * @retval None
 */
void lcd12864_deinit(struct lcd12864 *lcd);

/*
 * @brief Initializes pins and the LCD.
 * @param LCD instance
 * @retval None
 */
void lcd12864_init(struct lcd12864 *lcd);

/*
 * @brief Tick for LCD delays. Should be called in CPU timer interrupt.
 * @param LCD instance
 * @retval None
 */
void lcd12864_tick(struct lcd12864 *lcd);

/*
 * @brief Commands driver to enable or disable the LCD.
 * @param LCD instance, and the command of either ENABLE or DISABLE
 * @retval None
 */
void lcd12864_cmd(struct lcd12864 *lcd, enum lcd12864_cmd cmd);

/*
 * @brief Sets position.
 * @param X and Y position
 * @retval None
 */
void lcd12864_setpos(struct lcd12864 *lcd, char x, char y);

/*
 * @brief Prints a string.
 * @param String address
 * @retval None
 */
void lcd12864_print(struct lcd12864 *lcd, char *str);

/*
 * @brief Prints a string at a position. Combination of setpos and write.
 * @param X, Y position and string address
 * @retval None
 */
#define lcd12864_printatpos(__lcd__, __posx__, __posy__, __str__) do { \
		lcd12864_setpos(__lcd__, __posx__, __posy__); \
		lcd12864_print(__lcd__, __str__); \
	} while (0);

#endif /* LCD12864_H_ */
