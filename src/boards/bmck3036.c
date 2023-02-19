/* FCEUmm - NES/Famicom Emulator
 *
 * Copyright (C) 2019 Libretro Team
 * Copyright (C) 2023
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
 *
 */

#include "mapinc.h"
#include "latch.h"

static void Sync(void) {
	if (latch.addr & 0x20) { /* NROM-128 */
		SetupCartCHRMapping(0, CHRptr[0], 0x2000, 0);
		if (latch.addr & 1) {
			setprg16(0x8000, ((latch.addr >> 2) & 0x20) | (latch.addr & 0x1F));
			setprg16(0xC000, ((latch.addr >> 2) & 0x20) | (latch.addr & 0x1F));
		} else {
			setprg32(0x8000, ((latch.addr >> 3) & 0x10) | ((latch.addr >> 1) & 0xF));
		}
	} else { /* UNROM */
		SetupCartCHRMapping(0, CHRptr[0], 0x2000, 1);
		setprg16(0x8000, ((latch.addr >> 2) & 0x20) | latch.addr | (latch.data & 7));
		setprg16(0xC000, ((latch.addr >> 2) & 0x20) | latch.addr | 7);
	}
	setchr8(0);
	setmirror(((latch.addr & 0x40) || ((latch.addr & 0x20) && (latch.addr & 0x04))) ? MI_H : MI_V);
}

void BMCK3036_Init(CartInfo *info) {
	Latch_Init(info, Sync, NULL, 0, 0);
	info->Reset = LatchHardReset;
}
