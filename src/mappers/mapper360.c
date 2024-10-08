/* FCEUmm - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2020
 *  Copyright (C) 2023-2024 negativeExponent
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/* NES 2.0 Mapper 360 - Bit Corp's 31-in-1 multicart (3150) */

#include "mapinc.h"

static uint8 reg;

static SFORMAT StateRegs[] = {
	{ &reg, 1, "DPSW" },
	{ 0 }
};

static void Sync(void) {
	if ((~reg & 0x20) && (iNESCart.submapper == 1)) {
		setprg8(0x8000, 0x40);
		setprg8(0xA000, 0x40);
		setprg8(0xC000, 0x40);
		setprg8(0xE000, 0x40);
	} else {
		/* dip 0 and 1 is the same game SMB) */
		if (reg < 2) {
			setprg32(0x8000, reg >> 1);
		} else {
			setprg16(0x8000, reg);
			setprg16(0xC000, reg);
		}
	}
	setchr8(reg);
	setmirror(((reg & 0x10) >> 4) ^ 1);

}

static DECLFW(M360Write4) {
	reg = V;
	Sync();
}

static void M360Power(void) {
	reg = 0;
	Sync();
	if (iNESCart.submapper == 1) {
		SetWriteHandler(0x4100, 0x4FFF, M360Write4);	
	}
	SetReadHandler(0x8000, 0xFFFF, CartBR);
	SetWriteHandler(0x8000, 0XFFFF, CartBW);
}

static void M360Reset(void) {
	if (iNESCart.submapper == 0) {
		reg = (reg + 1) & 31;
	} else {
		reg = 0;
	}
	Sync();
	FCEU_printf("reg = %d\n", reg);
}

static void StateRestore(int version) {
	Sync();
}

void Mapper360_Init(CartInfo *info) {
	info->Reset = M360Reset;
	info->Power = M360Power;
	GameStateRestore = StateRestore;
	AddExState(StateRegs, ~0, 0, NULL);
}
