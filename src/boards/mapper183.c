/* FCEU mm - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2005 CaH4e3
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
 * iNES mapper 183
 * Gimmick Bootleg (VRC4 mapper)
 */

#include "mapinc.h"
#include "vrc24.h"

static uint8 prg[4];

static SFORMAT StateRegs[] =
{
	{ prg, 4, "PRG" },
	{ 0 }
};

static void M183PW(void) {
	setprg8(0x6000, prg[3]);
	setprg8(0x8000, prg[0]);
	setprg8(0xA000, prg[1]);
	setprg8(0xC000, prg[2]);
	setprg8(0xE000, ~0);
}

static DECLFW(M183Write) {
	switch (A & 0xF80C) {
	case 0x6800:
		prg[3] = A & 0x3F;
		FixVRC24PRG();
		break;
	case 0x8800:
		prg[0] = V;
		FixVRC24PRG();
		break;
	case 0x9800:
		VRC24Write(0x9000 | (A & 0xC0), V);
		break;
	case 0xA800:
		prg[1] = V;
		FixVRC24PRG();
		break;
	case 0xA000:
		prg[2] = V;
		FixVRC24PRG();
		break;
	}
}

static void M183Power(void) {
	GenVRC24Power();
	SetReadHandler(0x6000, 0xFFFF, CartBR);
	SetWriteHandler(0x6000, 0xAFFF, M183Write);
}

void Mapper183_Init(CartInfo *info) {
	GenVRC24_Init(info, VRC4e, 0);
	info->Power = M183Power;
	vrc24.pwrap = M183PW;
	AddExState(&StateRegs, ~0, 0, 0);
}
