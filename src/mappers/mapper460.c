/* FCEUmm - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2023
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

#include "mapinc.h"
#include "mmc3.h"

static uint8 *CHRRAM = NULL;
static uint8 reg;
static uint8 dipsw;

static void M460PW(uint32 A, uint8 V) {
	uint32 mask = 0x0F;
	uint32 base = reg << 4;

	if ((reg & 0x20) && ((reg != 0x20) || (~dipsw & 0x01))) {
		/* Menu selection by selectively connecting CPU D7 to reg or not */
		if (reg & 0x10) {
			setprg8(0x8000, (base & ~mask) | ((mmc3.reg[6] & ~2) & mask));
			setprg8(0xA000, (base & ~mask) | ((mmc3.reg[7] & ~2) & mask));
			setprg8(0xC000, (base & ~mask) | ((mmc3.reg[6] |  2) & mask));
			setprg8(0xE000, (base & ~mask) | ((mmc3.reg[7] |  2) & mask));
		} else {
			setprg8(0x8000, (base & ~mask) | (mmc3.reg[6] & mask));
			setprg8(0xA000, (base & ~mask) | (mmc3.reg[7] & mask));
			setprg8(0xC000, (base & ~mask) | (mmc3.reg[6] & mask));
			setprg8(0xE000, (base & ~mask) | (mmc3.reg[7] & mask));
		}
	} else {
		setprg8(A, (base & ~mask) | (V & mask));
	}
}

static void M460CW(uint32 A, uint8 V) {
	if (reg & 0x04) {
		setchr2(0x0000, mmc3.reg[0] & 0xFE);
		setchr2(0x0800, mmc3.reg[0] | 0x01);
		setchr2(0x1000, mmc3.reg[2]);
		setchr2(0x1800, mmc3.reg[5]);
	} else {
		setchr8r(0x10, 0);
	}
}

static DECLFR(M460Read) {
	/* Menu selection by selectively connecting reg's D7 to PRG /CE or not */
	if ((reg & 0x80) && (dipsw & 0x01)) {
		return X.DB;
	}
	return CartBR(A);
}

static DECLFW(M460WriteLow) {
	if (MMC3_WRAMWritable(A)) {
		reg = A & 0xFF;
		MMC3_FixPRG();
		MMC3_FixCHR();
	}
}

static void M460Reset(void) {
	reg = 0;
	dipsw++;
	MMC3_Reset();
}

static void M460Power(void) {
	reg = 0;
	dipsw = 0;
	MMC3_Power();
	SetReadHandler(0x8000, 0xFFFF, M460Read);
	SetWriteHandler(0x6000, 0x7FFF, M460WriteLow);
}

static void M460close(void) {
	MMC3_Close();
	if (CHRRAM) {
		FCEU_gfree(CHRRAM);
	}
	CHRRAM = NULL;
}

void Mapper460_Init(CartInfo *info) {
	MMC3_Init(info, 0, 0);
	MMC3_cwrap = M460CW;
	MMC3_pwrap = M460PW;
	info->Power = M460Power;
	info->Reset = M460Reset;
	info->Close = M460close;
	AddExState(mmc3.expregs, 2, 0, "EXPR");

	CHRRAM = (uint8 *)FCEU_gmalloc(8192);
	SetupCartCHRMapping(0x10, CHRRAM, 8192, 1);
	AddExState(CHRRAM, 8192, 0, "CRAM");
}
