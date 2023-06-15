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

static void M420CW(uint32 A, uint8 V) {
	uint8 mask = (mmc3.expregs[1] & 0x80) ? 0x7F : 0xFF;
	setchr1(A, ((mmc3.expregs[1] << 1) & 0x100) | ((mmc3.expregs[1] << 5) & 0x80) | (V & mask));
}

static void M420PW(uint32 A, uint8 V) {
	if (mmc3.expregs[0] & 0x80) {
		setprg32(0x8000, ((mmc3.expregs[2] >> 2) & 8) | ((mmc3.expregs[0] >> 1) & 7));
	} else {
		uint8 mask = (mmc3.expregs[0] & 0x20) ? 0x0F : ((mmc3.expregs[3] & 0x20) ? 0x1F : 0x3F);
		setprg8(A, ((mmc3.expregs[3] << 3) & 0x20) | (V & mask));
	}
}

static DECLFW(M420Write) {
	/* writes possible regardless of MMC3 wram state */
	mmc3.expregs[A & 3] = V;
	FixMMC3PRG(mmc3.cmd);        
	FixMMC3CHR(mmc3.cmd);
}

static void M420Reset(void) {
	mmc3.expregs[0] = mmc3.expregs[1] = mmc3.expregs[2] = mmc3.expregs[3] = 0;
	MMC3RegReset();
}

static void M420Power(void) {
	mmc3.expregs[0] = mmc3.expregs[1] = mmc3.expregs[2] = mmc3.expregs[3] = 0;
	GenMMC3Power();
	SetWriteHandler(0x6000, 0x7FFF, M420Write);
}

void Mapper420_Init(CartInfo *info) {
	GenMMC3_Init(info, 0, 0);
	mmc3.cwrap = M420CW;
	mmc3.pwrap = M420PW;
	info->Reset = M420Reset;
	info->Power = M420Power;
	AddExState(mmc3.expregs, 4, 0, "EXPR");
}
