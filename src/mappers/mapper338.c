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
 *
 */

/* BMC-SA005-A */
/* NES 2.0 mapper 338 is used for a 16-in-1 and a 200/300/600/1000-in-1 multicart.
 * http://wiki.nesdev.com/w/index.php/NES_2.0_Mapper_338 */

#include "mapinc.h"
#include "latch.h"

static void Sync(void) {
	setprg16(0x8000, latch.addr);
	setprg16(0xC000, latch.addr);
	setchr8(latch.addr);
	setmirror((latch.addr >> 3) & 0x01);
}

void Mapper338_Init(CartInfo *info) {
	Latch_Init(info, Sync, NULL, 0, 0);
	info->Reset = Latch_RegReset;
}
