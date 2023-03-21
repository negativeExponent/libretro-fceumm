/* FCEUmm - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2019 Libretro Team
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

/* NES 2.0 Mapper 322
 * BMC-K-3033
 * 35-in-1 (K-3033)
 * http://wiki.nesdev.com/w/index.php/NES_2.0_Mapper_322
 */

#include "mapinc.h"
#include "mmc3.h"

static void BMCK3033CW(uint32 A, uint8 V) {
	if (mmc3.expregs[0] & 0x20) {
		uint32 outer = ((mmc3.expregs[0] >> 4) & 4) | ((mmc3.expregs[0] >> 3) & 3);
		if (mmc3.expregs[0] & 0x80) {
			setchr1(A, (outer << 8) | (V & 0xFF));
		} else {
			setchr1(A, (outer << 7) | (V & 0x7F));
		}
	} else {
		setchr1(A, (V & 0x7F));
	}
}

static void BMCK3033PW(uint32 A, uint8 V) {
	uint32 outer = ((mmc3.expregs[0] >> 4) & 4) | ((mmc3.expregs[0] >> 3) & 3);
	if (mmc3.expregs[0] & 0x20) {
		if (mmc3.expregs[0] & 0x80) {
			setprg8(A, (outer << 5) | (V & 0x1F));
		} else {
			setprg8(A, (outer << 4) | (V & 0x0F));
		}
	} else {
		if (mmc3.expregs[0] & 0x03) {
			setprg32(0x8000, (outer << 3) | ((mmc3.expregs[0] >> 1) & 3));
		} else {
			setprg16(0x8000, (outer << 3) | (mmc3.expregs[0] & 7));
			setprg16(0xC000, (outer << 3) | (mmc3.expregs[0] & 7));
		}
	}
}

static DECLFW(BMCK3033Write) {
	if (MMC3CanWriteToWRAM()) {
		mmc3.expregs[0] = A & 0xFF;
		FixMMC3PRG(mmc3.cmd);
		FixMMC3CHR(mmc3.cmd);
	}
}

static void BMCK3033Power(void) {
	mmc3.expregs[0] = 0;
	GenMMC3Power();
	SetWriteHandler(0x6000, 0x7FFF, BMCK3033Write);
}

static void BMCK3033Reset(void) {
	mmc3.expregs[0] = 0;
	MMC3RegReset();
}

void BMCK3033_Init(CartInfo *info) {
	GenMMC3_Init(info, 256, 256, 0, 0);
	mmc3.pwrap = BMCK3033PW;
	mmc3.cwrap = BMCK3033CW;
	info->Power = BMCK3033Power;
	info->Reset = BMCK3033Reset;
	AddExState(mmc3.expregs, 1, 0, "EXPR");
}
