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

#include "mapinc.h"

static uint8 reg;

static SFORMAT StateRegs[] =
{
	{ &reg, 1, "REG" },
	{ 0 }
};

static void Sync() {
	setprg32(0x8000, reg & 3);
	setchr8(reg >> 2);
}

static DECLFW(M38Write) {
	reg = V;
	Sync();
}

static void M38Power(void) {
	reg = 0;
	Sync();
	SetReadHandler(0x8000, 0xFFFF, CartBR);
	SetWriteHandler(0x7000, 0x7FFF, M38Write);
}

static void StateRestore(int version) {
	Sync();
}

void Mapper38_Init(CartInfo *info) {
	info->Power = M38Power;
	GameStateRestore = StateRestore;
	AddExState(&StateRegs, ~0, 0, 0);
}
