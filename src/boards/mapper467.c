/* FCEUmm - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2022 NewRisingSun
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

static void Mapper467_PRGWrap(uint32 A, uint8 V) {
	if (mmc3.expregs[0] & 0x20) {
		int prgAND = (mmc3.expregs[0] & 0x40) ? 0x0F : 0x03;
		int prgOR = ((mmc3.expregs[0] << 1) & 0x3C) | 0x40;
		setprg8(A, (V & prgAND) | (prgOR & ~prgAND));
	} else if (~A & 0x2000)
		setprg16(A, mmc3.expregs[0] & 0x1F);
}

static void Mapper467_CHRWrap(uint32 A, uint8 V) {
	if (~A & 0x0800) {
		A = (A & ~0x400) | ((A << 1) & 0x800);
		if (mmc3.expregs[0] & 0x40)
			setchr2(A, V | 0x100);
		else
			setchr2(A, (V & ~3) | ((A >> 11) & 3));
	}
}

static DECLFW(Mapper467_WriteExtra) {
	mmc3.expregs[0] = V;
	FixMMC3PRG(mmc3.cmd);
	FixMMC3CHR(mmc3.cmd);
	setmirror(mmc3.expregs[0] & 0x80 ? MI_H : MI_V);
}

static DECLFW(Mapper467_WriteMMC3) {
	if (~A & 1)
		V &= 0x3F;
	MMC3_CMDWrite(A, V);
}

static void Mapper467_Reset(void) {
	mmc3.expregs[0] = 0;
	MMC3RegReset();
}

static void Mapper467_Power(void) {
	mmc3.expregs[0] = 0;
	GenMMC3Power();
	SetWriteHandler(0x8000, 0x8FFF, Mapper467_WriteMMC3);
	SetWriteHandler(0x9000, 0x9FFF, Mapper467_WriteExtra);
}

void Mapper467_Init(CartInfo *info) {
	GenMMC3_Init(info, 0, 0);
	mmc3.cwrap = Mapper467_CHRWrap;
	mmc3.pwrap = Mapper467_PRGWrap;
	info->Power = Mapper467_Power;
	info->Reset = Mapper467_Reset;
	AddExState(mmc3.expregs, 1, 0, "EXPR");
}
