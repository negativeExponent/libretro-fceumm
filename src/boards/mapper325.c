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

/* NES 2.0 325
 * UNIF UNL-MALISB
 */

#include "mapinc.h"
#include "mmc3.h"

static void M325PW(uint32 A, uint8 V) {
	setprg8(A, (V & 0xF3) | ((V & 0x08) >> 1) | ((V & 0x04) << 1));
}

static void M325CW(uint32 A, uint8 V) {
	setchr1(A, (V & 0xDD) | ((V & 0x20) >> 4) | ((V & 0x02) << 4));
}

static DECLFW(M325Write) {
    A = (A & 0xFFFE) | ((A >> 3) & 1);
    MMC3_Write(A, V);
}

static void M325Power(void) {
	GenMMC3Power();
	SetWriteHandler(0x8000, 0xFFFF, M325Write);
}

void Mapper325_Init(CartInfo *info) {
	GenMMC3_Init(info, 0, 0);
	MMC3_pwrap = M325PW;
	MMC3_cwrap = M325CW;
	info->Power = M325Power;
}
