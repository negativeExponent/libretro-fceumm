/* FCE Ultra - NES/Famicom Emulator
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
#include "vrc2and4.h"

static void M025CW(uint32 A, uint32 V) {
	setchr1(A, V & 0xFFF);
}

void Mapper025_Init(CartInfo *info) {
	/* Mapper 25 - VRC2c, VRC4b, VRC4d */
	switch (info->submapper) {
	case 1:  GenVRC24_Init(info, VRC4, 0x02, 0x01, 1, 1); break;
	case 2:  GenVRC24_Init(info, VRC4, 0x08, 0x04, 1, 1); break;
	case 3:  GenVRC24_Init(info, VRC2, 0x02, 0x01, 1, 1); break;
	default: GenVRC24_Init(info, VRC4, 0x0A, 0x05, 1, 1); break;
	}
	vrc24.cwrap = M025CW;
}
