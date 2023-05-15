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

/* NES 2.0 Mapper 445
 * DG574B MMC3-compatible multicart circuit board.
 */

#include "mapinc.h"
#include "mmc3.h"

static void M445PW(uint32 A, uint8 V) {
    uint8 mask = (mmc3.expregs[2] & 0x01) ? 0x0F : 0x1F;
    setprg8(A, (mmc3.expregs[0] & ~mask) | (V & mask));
}

static void M445CW(uint32 A, uint8 V) {
    uint8 mask = (mmc3.expregs[2] & 0x08) ? 0x7F : 0xFF;
	setchr1(A, ((mmc3.expregs[1] << 3) & ~mask) | (V & mask));
}

static DECLFW(M445Write) {
    if (!(mmc3.expregs[3] & 0x20)) {
        mmc3.expregs[A & 0x03] = V;
        FixMMC3PRG(mmc3.cmd);
        FixMMC3CHR(mmc3.cmd);
    }
}

static void M445Reset(void) {
	mmc3.expregs[0] = 0x00;
	mmc3.expregs[1] = 0x00;
	mmc3.expregs[2] = 0x00;
    mmc3.expregs[3] = 0x00;
	MMC3RegReset();
}

static void M445Power(void) {
    mmc3.expregs[0] = 0x00;
	mmc3.expregs[1] = 0x00;
	mmc3.expregs[2] = 0x00;
    mmc3.expregs[3] = 0x00;
	GenMMC3Power();
	SetWriteHandler(0x5000, 0x5FFF, M445Write);
}

void Mapper445_Init(CartInfo *info) {
	GenMMC3_Init(info, 512, 512, info->PRGRamSize + info->PRGCRC32, info->battery);
	mmc3.pwrap = M445PW;
	mmc3.cwrap = M445CW;
	info->Power = M445Power;
	info->Reset = M445Reset;
	AddExState(mmc3.expregs, 4, 0, "EXPR");
}
