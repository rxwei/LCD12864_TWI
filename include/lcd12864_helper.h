/*
 * lcd12864_helper.h
 *
 *  Created on: Apr 9, 2012
 *      Author: xinranmsn
 */

#ifndef LCD12864_HELPER_H_
#define LCD12864_HELPER_H_

char flipbyte_lookup[16] = {
	0x0, 0x8, 0x4, 0xC,
	0x2, 0xA, 0x6, 0xE,
	0x1, 0x9, 0x5, 0xD,
	0x3, 0xB, 0x7, 0xF
};

char flipbyte(char n)
{
   return (flipbyte_lookup[n & 0x0F] << 4) | flipbyte_lookup[n >> 4];
}

inline void _set_cs(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.cs.port |= lcd->pinstruct.serpinstruct.cs.bitvec;
}

inline void _clr_cs(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.cs.port &= ~lcd->pinstruct.serpinstruct.cs.bitvec;
}

inline void _set_sclk(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.sclk.port |= lcd->pinstruct.serpinstruct.sclk.bitvec;
}

inline void _clr_sclk(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.sclk.port &= ~lcd->pinstruct.serpinstruct.sclk.bitvec;
}

inline void _set_sid(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.sid.port |= lcd->pinstruct.serpinstruct.sid.bitvec;
}

inline void _clr_sid(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.sid.port &= ~lcd->pinstruct.serpinstruct.sid.bitvec;
}

inline void _write_db(struct lcd12864 *lcd, char dat)
{
	if (lcd->mode == lcd12864_parallel4) {
		lcd->pinstruct.par4pinstruct.db.ddr =
			lcd->pinstruct.par4pinstruct.db.flip
				? (flipbyte(dat) >> lcd->pinstruct.par4pinstruct.offset)
			: (dat << lcd->pinstruct.par4pinstruct.offset);
	}
	else if (lcd->mode == lcd12864_parallel8) {
		lcd->pinstruct.par8pinstruct.db.port =
			lcd->pinstruct.par8pinstruct.db.flip
				? flipbyte(dat)
				: dat;
	}
}

inline void _set_rs(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.cs.port |= lcd->pinstruct.serpinstruct.cs.bitvec;
}

inline void _clr_rs(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.cs.port |= lcd->pinstruct.serpinstruct.cs.bitvec;
}

inline void _set_rw(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.cs.port |= lcd->pinstruct.serpinstruct.cs.bitvec;
}

inline void _clr_rw(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.cs.port |= lcd->pinstruct.serpinstruct.cs.bitvec;
}

inline void _set_e(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.cs.port |= lcd->pinstruct.serpinstruct.cs.bitvec;
}

inline void _clr_e(struct lcd12864 *lcd)
{
	lcd->pinstruct.serpinstruct.cs.port |= lcd->pinstruct.serpinstruct.cs.bitvec;
}

#endif /* LCD12864_HELPER_H_ */
