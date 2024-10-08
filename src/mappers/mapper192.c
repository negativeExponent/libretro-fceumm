/* FCEUmm - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2023-2024 negativeExponent
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

static void M192CW(uint16 A, uint16 V) {
	if ((V & ~0x03) == 0x08) {
		setchr1r(0x10, A, V & 0x03);
	} else {
		setchr1(A, V);
	}
}

static void M192Close(void) {
	MMC3_Close();
}

void Mapper192_Init(CartInfo *info) {
	MMC3_Init(info, MMC3B, 8, info->battery);
	info->Close = M192Close;
	MMC3_cwrap = M192CW;

	CHRRAMSIZE = 4096;
	CHRRAM = (uint8 *)FCEU_gmalloc(CHRRAMSIZE);
	SetupCartCHRMapping(0x10, CHRRAM, CHRRAMSIZE, 1);
	AddExState(CHRRAM, CHRRAMSIZE, 0, "CHRR");
}
