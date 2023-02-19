/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2011 CaH4e3
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
 * Just another pirate cart with pirate mapper, instead of original MMC1
 * Kaiser Highway Star
 *
 */

#include "mapinc.h"
#include "latch.h"

static uint8 prg;

static SFORMAT StateRegs[] =
{
	{ &prg, 1, "REGS" },
	{ 0 }
};

static void Sync(void) {
	setprg16(0x8000, prg);
	setprg16(0xc000, ~0);
	setmirror((latch.data & 1) ^ 1);
	setchr8(0);
}

static DECLFW(UNLKS7013BLoWrite) {
	prg = V;
	Sync();
}

static void UNLKS7013BPower(void) {
	prg = 0;
	LatchPower();
	SetWriteHandler(0x6000, 0x7FFF, UNLKS7013BLoWrite);
}

void UNLKS7013B_Init(CartInfo *info) {
	Latch_Init(info, Sync, NULL, 0, 0);
	info->Power = UNLKS7013BPower;
	AddExState(&StateRegs, ~0, 0, 0);
}
