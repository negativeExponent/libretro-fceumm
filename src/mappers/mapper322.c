/* FCEUmm - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2019 Libretro Team
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

/* NES 2.0 Mapper 322
 * BMC-K-3033
 * 35-in-1 (K-3033)
 * http://wiki.nesdev.com/w/index.php/NES_2.0_Mapper_322
 */

#include "mapinc.h"
#include "mmc3.h"

static uint8 reg;

static void M322CW(uint32 A, uint8 V) {
	if (reg & 0x20) {
		uint32 base = ((reg >> 4) & 0x04) | ((reg >> 3) & 0x03);

		if (reg & 0x80) {
			setchr1(A, (base << 8) | (V & 0xFF));
		} else {
			setchr1(A, (base << 7) | (V & 0x7F));
		}
	} else {
		setchr1(A, (V & 0x7F));
	}
}

static void M322PW(uint32 A, uint8 V) {
	uint32 base = ((reg >> 4) & 0x04) | ((reg >> 3) & 0x03);

	if (reg & 0x20) {
		if (reg & 0x80) {
			setprg8(A, (base << 5) | (V & 0x1F));
		} else {
			setprg8(A, (base << 4) | (V & 0x0F));
		}
	} else {
		if (reg & 0x03) {
			setprg32(0x8000, (base << 3) | ((reg >> 1) & 3));
		} else {
			setprg16(0x8000, (base << 3) | (reg & 7));
			setprg16(0xC000, (base << 3) | (reg & 7));
		}
	}
}

static DECLFW(M322Write) {
	if (MMC3_WRAMWritable(A)) {
		reg = A & 0xFF;
		MMC3_FixPRG();
		MMC3_FixCHR();
	}
}

static void M322Power(void) {
	reg = 0;
	MMC3_Power();
	SetWriteHandler(0x6000, 0x7FFF, M322Write);
}

static void M322Reset(void) {
	reg = 0;
	MMC3_Reset();
}

void Mapper322_Init(CartInfo *info) {
	MMC3_Init(info, 0, 0);
	MMC3_pwrap = M322PW;
	MMC3_cwrap = M322CW;
	info->Power = M322Power;
	info->Reset = M322Reset;
	AddExState(&reg, 1, 0, "EXPR");
}
