/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2005 CaH4e3
 *  Copyright (C) 2022
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
 
 /* 2022-2-14
  * - add support for submapper 1, Mortal Kombat (JJ-01) (Ch) [!]
  * - add mirroring
  */

#include "mapinc.h"
#include "mmc3.h"

static uint8 submapper;

static const uint8 lut[256] = {
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x49, 0x19, 0x09, 0x59, 0x49, 0x19, 0x09,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x51, 0x41, 0x11, 0x01, 0x51, 0x41, 0x11, 0x01,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x49, 0x19, 0x09, 0x59, 0x49, 0x19, 0x09,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x51, 0x41, 0x11, 0x01, 0x51, 0x41, 0x11, 0x01,
	0x00, 0x10, 0x40, 0x50, 0x00, 0x10, 0x40, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x08, 0x18, 0x48, 0x58, 0x08, 0x18, 0x48, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x40, 0x50, 0x00, 0x10, 0x40, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x08, 0x18, 0x48, 0x58, 0x08, 0x18, 0x48, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x58, 0x48, 0x18, 0x08, 0x58, 0x48, 0x18, 0x08,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x50, 0x40, 0x10, 0x00, 0x50, 0x40, 0x10, 0x00,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x58, 0x48, 0x18, 0x08, 0x58, 0x48, 0x18, 0x08,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x50, 0x40, 0x10, 0x00, 0x50, 0x40, 0x10, 0x00,
	0x01, 0x11, 0x41, 0x51, 0x01, 0x11, 0x41, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x09, 0x19, 0x49, 0x59, 0x09, 0x19, 0x49, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x11, 0x41, 0x51, 0x01, 0x11, 0x41, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x09, 0x19, 0x49, 0x59, 0x09, 0x19, 0x49, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static void M208PW(uint32 A, uint8 V) {
	if (submapper == 1)
		setprg32(0x8000, mmc3.regs[6] >> 2);
	else
		setprg32(0x8000, (mmc3.expregs[5] & 0x1) | ((mmc3.expregs[5] >> 3) & 0x2));
}

static void M208MW(uint8 V) {
	if (submapper == 1)
		setmirror((V & 1) ^ 1);
	else
		setmirror(((mmc3.expregs[5] >> 5) & 1) ^ 1);
}

static DECLFW(M208Write) {
	mmc3.expregs[5] = V;
	MMC3_FixPRG();
}

static DECLFW(M208ProtWrite) {
	if (A <= 0x57FF)
		mmc3.expregs[4] = V;
	else
		mmc3.expregs[(A & 0x03)] = V ^ lut[mmc3.expregs[4]];
}

static DECLFR(M208ProtRead) {
	return (mmc3.expregs[(A & 0x3)]);
}

static void M208Power(void) {
	mmc3.expregs[5] = 0x11;
	GenMMC3Power();
	SetWriteHandler(0x4800, 0x4fff, M208Write);
	SetWriteHandler(0x6800, 0x6fff, M208Write);
	SetWriteHandler(0x5000, 0x5fff, M208ProtWrite);
	SetReadHandler(0x5800, 0x5fff, M208ProtRead);
	SetReadHandler(0x8000, 0xffff, CartBR);
}

void Mapper208_Init(CartInfo *info) {
	GenMMC3_Init(info, 0, 0);
	MMC3_pwrap = M208PW;
	MMC3_mwrap = M208MW;
	info->Power = M208Power;
	AddExState(mmc3.expregs, 6, 0, "EXPR");
	submapper = info->submapper;
}
