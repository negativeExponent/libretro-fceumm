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

/* NES 2.0 Mapper 418 denotes the 820106-C/821007C circuit boards for the LH42 bootleg cartridge versions of Highway Star. */

#include "mapinc.h"
#include "n118.h"

static void M418FixCHR(void) {
	setchr2(0x0000, (n118.reg[0] & 0x3F) >> 1);
	setchr2(0x0800, (n118.reg[1] & 0x3F) >> 1);
	setchr1(0x1000, n118.reg[2] & 0x3F);
	setchr1(0x1400, n118.reg[3] & 0x3F);
	setchr1(0x1800, n118.reg[4] & 0x3F);
	setchr1(0x1C00, n118.reg[5] & 0x3F);
    setmirror((n118.reg[5] & 0x01) ^ 0x01);
}

void Mapper418_Init(CartInfo *info) {
	N118_Init(info, 0, 0);
	N118_FixCHR = M418FixCHR;
}
