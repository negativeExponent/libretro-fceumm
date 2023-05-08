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

static uint8 regs[4];
static uint8 *WRAM = NULL;
static uint32 WRAMSIZE;

static SFORMAT StateRegs[] = {
	{ regs, 4, "EXPR" },
	{ 0 }
};

static uint32 getPRGBank(void) {
	return ((regs[1] << 5) | ((regs[0] << 1) & 0x1E) | ((regs[0] >> 5) & 1));
}

static void Sync(void) {
	uint32 prg = getPRGBank();
	if (regs[0] & 0x40) {
		if (regs[0] & 0x10) {
			setprg16(0x8000, prg);
			setprg16(0xC000, prg);
		} else {
			setprg32(0x8000, prg >> 1);
		}
	} else {
		setprg16(0x8000, (prg & ~7) | (regs[2] & 7));
		setprg16(0xC000, (prg & ~7) | 7);
	}
	setprg8r(0x10, 0x6000, 0);
	setchr8(0);
	setmirror(((regs[0] >> 7) & 1) ^ 1);
}

static DECLFR(M466ReadLatch) {
	/* Return open bus when selecting unpopulated PRG chip */
	if ((getPRGBank() & 0x20) && (PRGsize[0] < (1024 * 1024))) {
		return X.DB;
	}
	return CartBR(A);
}

static DECLFW(M466Write5000) {
	regs[(A >> 11) & 1] = A & 0xFF;
	Sync();
}

static DECLFW(M466WriteLatch) {
	regs[2] = V;
	Sync();
}

static void M466Reset(void) {
	regs[0] = regs[1] = 0;
	Sync();
}


static void M466Close(void) {
	if (WRAM) {
		FCEU_gfree(WRAM);
	}
	WRAM = NULL;
}

static void M466Power(void) {
	regs[0] = regs[1] = 0;
	Sync();
	SetReadHandler(0x6000, 0xFFFF, CartBR);
	SetWriteHandler(0x6000, 0x7FFF, CartBW);
	FCEU_CheatAddRAM(WRAMSIZE >> 10, 0x6000, WRAM);
	SetReadHandler(0x8000, 0xFFFF, M466ReadLatch);
	SetWriteHandler(0x5000, 0x5FFF, M466Write5000);
	SetWriteHandler(0x8000, 0xFFFF, M466WriteLatch);
}

static void StateRestore(int version) {
	Sync();
}

void Mapper466_Init(CartInfo *info) {
	info->Power = M466Power;
	info->Close = M466Close;
	info->Reset = M466Reset;
	GameStateRestore = StateRestore;

	WRAMSIZE = 8192;
	WRAM = (uint8 *)FCEU_gmalloc(WRAMSIZE);
	SetupCartPRGMapping(0x10, WRAM, WRAMSIZE, 1);
	AddExState(WRAM, WRAMSIZE, 0, "WRAM");
	AddExState(StateRegs, ~0, 0, 0);
}
