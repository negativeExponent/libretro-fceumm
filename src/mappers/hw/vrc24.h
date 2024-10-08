/* FCEUmm - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2023-2024 negativeExponent
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _VRC24_H
#define _VRC24_H

enum {
	VRC2a = 1,	/* Mapper 22 */
	VRC2b,		/* Mapper 23 */
	VRC2c,		/* Mapper 25 */
	VRC4a,		/* Mapper 21 */
	VRC4b,		/* Mapper 25 */
	VRC4c,		/* Mapper 21 */
	VRC4d,		/* Mapper 25 */
	VRC4e,		/* Mapper 23 */
	VRC4f,		/* Mapper 23 */
	VRC4_544,
	VRC4_559
};

typedef enum __VRC24TYPE {
	VRC2 = 0,
	VRC4 = 1
} VRC24TYPE;

typedef struct __VRC24 {
    uint8 prg[2];
    uint16 chr[8];
    uint8 cmd;
    uint8 mirr;
	uint8 latch; /* VRC2 $6000-$6FFF */

	VRC24TYPE type; /* type */
	uint16 A0;
	uint16 A1;
} VRC24;

extern VRC24 vrc24;

DECLFW(VRC24_Write);

void VRC24_IRQCPUHook(int a);
void VRC24_Reset(void);
void VRC24_Power(void);
void VRC24_Close(void);

void VRC24_Init(CartInfo *info, VRC24TYPE vrc4, uint32 A0, uint32 A1, int wram, int irqRepeated);

extern void (*VRC24_FixPRG)(void);
extern void (*VRC24_FixCHR)(void);
extern void (*VRC24_FixMIR)(void);

extern void (*VRC24_pwrap)(uint16 A, uint16 V);
extern void (*VRC24_cwrap)(uint16 A, uint16 V);
extern void (*VRC24_miscWrite)(uint16 A, uint8 V);

#endif /* _VRC24_H */
