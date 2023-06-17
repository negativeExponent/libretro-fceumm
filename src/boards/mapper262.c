/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2006 CaH4e3
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

/* NES 2.0 Mapper 262 - UNL-SHERO */

#include "mapinc.h"
#include "mmc3.h"

static uint8 *CHRRAM;
static uint8 tekker;

static void M262CW(uint32 A, uint8 V) {
	if (mmc3.expregs[0] & 0x40)
		setchr8r(0x10, 0);
	else {
		if (A < 0x800)
			setchr1(A, V | ((mmc3.expregs[0] & 8) << 5));
		else if (A < 0x1000)
			setchr1(A, V | ((mmc3.expregs[0] & 4) << 6));
		else if (A < 0x1800)
			setchr1(A, V | ((mmc3.expregs[0] & 1) << 8));
		else
			setchr1(A, V | ((mmc3.expregs[0] & 2) << 7));
	}
}

static DECLFW(M262Write) {
	mmc3.expregs[0] = V;
	MMC3_FixCHR();
}

static DECLFR(M262Read) {
	return(tekker);
}

static void M262Reset(void) {
	MMC3RegReset();
	tekker ^= 0xFF;
}

static void M262Power(void) {
	tekker = 0x00;
	GenMMC3Power();
	SetWriteHandler(0x4100, 0x4100, M262Write);
	SetReadHandler(0x4100, 0x4100, M262Read);
}

static void M262Close(void) {
	GenMMC3Close();
	if (CHRRAM)
		FCEU_gfree(CHRRAM);
	CHRRAM = NULL;
}

void Mapper262_Init(CartInfo *info) {
	GenMMC3_Init(info, 0, 0);
	MMC3_cwrap = M262CW;
	info->Power = M262Power;
	info->Reset = M262Reset;
	info->Close = M262Close;
	CHRRAM = (uint8*)FCEU_gmalloc(8192);
	SetupCartCHRMapping(0x10, CHRRAM, 8192, 1);
	AddExState(mmc3.expregs, 4, 0, "EXPR");
	AddExState(&tekker, 1, 0, "DIPSW");
}
