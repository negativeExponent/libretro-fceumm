/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2012 CaH4e3
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

static void SA9602BPW(uint32 A, uint8 V) {
	setprg8(A, (mmc3.expregs[1] & 0xC0) | (V & 0x3F));
	if (mmc3.cmd & 0x40)
		setprg8(0x8000, 62);
	else
		setprg8(0xc000, 62);
	setprg8(0xe000, 63);
}

static DECLFW(SA9602BWrite) {
	switch (A & 0xe001) {
	case 0x8000: mmc3.expregs[0] = V; break;
	case 0x8001:
		if ((mmc3.expregs[0] & 7) < 6) {
			mmc3.expregs[1] = V;
			FixMMC3PRG(mmc3.cmd);
		}
		break;
	}
	MMC3_CMDWrite(A, V);
}

static void SA9602BPower(void) {
	mmc3.expregs[0] = mmc3.expregs[1] = 0;
	GenMMC3Power();
	SetReadHandler(0x8000, 0xFFFF, CartBR);
	SetWriteHandler(0x8000, 0xBFFF, SA9602BWrite);
}

void SA9602B_Init(CartInfo *info) {
	GenMMC3_Init(info, 512, 0, 0, 0);
	mmc3.pwrap = SA9602BPW;
	mmc3.opts |= 2;
	info->SaveGame[0] = UNIFchrrama;
	info->SaveGameLen[0] = 32 * 1024;
	info->Power = SA9602BPower;
	AddExState(mmc3.expregs, 2, 0, "EXPR");
}
