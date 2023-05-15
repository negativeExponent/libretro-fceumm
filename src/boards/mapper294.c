/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2022 NewRisingSun
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

static uint8 reg;

static void Sync(void) {
	setprg16(0x8000, (reg << 3) | (latch.data & 0x07));
	setprg16(0xC000, (reg << 3) | 0x07);
	setchr8(0);
	setmirror(((reg >> 4) & 0x01) ^ 0x01);
}

static DECLFW(M294Write) {
	if (A & 0x100) {
		reg = V;
		Sync();
	}
}

static void M294Reset(void) {
	reg = 0;
	LatchHardReset();
}

static void M294Power(void) {
	LatchPower();
	SetWriteHandler(0x4020, 0x4FFF, M294Write);
}

void Mapper294_Init(CartInfo *info) {
	Latch_Init(info, Sync, NULL, 0, 0);
	info->Power = M294Power;
	info->Reset = M294Reset;
	AddExState(&reg, 1, 0, "REGS");
}
