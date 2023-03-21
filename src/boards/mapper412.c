/* FCEUmm - NES/Famicom Emulator
 *
 * Copyright notice for this file:
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

/* NES 2.0 Mapper 412 is a variant of mapper 45 where the
 * ASIC's PRG A21/CHR A20 output (set by bit 6 of the third write to $6000)
 * selects between regularly-banked CHR-ROM (=0) and 8 KiB of unbanked CHR-RAM (=1).
 * It is used solely for the Super 8-in-1 - 98格鬥天王＋熱血 (JY-302) multicart.
 */

#include "mapinc.h"
#include "mmc3.h"

static uint8 *CHRRAM;

static void M412CW(uint32 A, uint8 V) {
    if (mmc3.expregs[2] & 2) {
        setchr8(mmc3.expregs[0] >> 2);
    } else {
        setchr1(A, (mmc3.expregs[1] & 0x80) | (V & 0x7F));
    }
}

static void M412PW(uint32 A, uint8 V) {
    if (mmc3.expregs[2] & 2) {
        uint8 bank = mmc3.expregs[2] >> 3;
        if (mmc3.expregs[2] & 4) {
            setprg32(0x8000, bank >> 1);
        } else {
            setprg16(0x8000, bank);
            setprg16(0xC000, bank);
        }
    } else {
		uint32 mask = 0x3F & ~((mmc3.expregs[1] << 3) & 0x38);
		uint32 base = (mmc3.expregs[1] >> 2) & 0x3E;
		setprg8(A, base | (V & mask));
	}
}

static DECLFW(M412Write) {
	if (MMC3CanWriteToWRAM()) {
		mmc3.expregs[A & 3] = V;
		FixMMC3PRG(mmc3.cmd);        
		FixMMC3CHR(mmc3.cmd);
	}
}

static void M412Reset(void) {
	mmc3.expregs[0] = mmc3.expregs[1] = mmc3.expregs[2] = mmc3.expregs[3] = 0;
	MMC3RegReset();
}

static void M412Power(void) {
    mmc3.expregs[0] = mmc3.expregs[1] = mmc3.expregs[2] = mmc3.expregs[3] = 0;
	GenMMC3Power();
	SetWriteHandler(0x6000, 0x7FFF, M412Write);
}

void Mapper412_Init(CartInfo *info) {
	GenMMC3_Init(info, 512, 256, 0, 0);
	cwrap = M412CW;
	pwrap = M412PW;
	info->Reset = M412Reset;
	info->Power = M412Power;
	AddExState(mmc3.expregs, 4, 0, "EXPR");
}