/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2005 CaH4e3
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

static uint8 *DummyCHR = NULL;
static uint8 datareg;

static uint8 submapper = 0;

static SFORMAT StateRegs[] =
{
	{ &datareg, 1, "DREG" },
	{ 0 }
};

/*   on    off
1  0x0F, 0xF0 - Bird Week
2  0x33, 0x00 - B-Wings
3  0x11, 0x00 - Mighty Bomb Jack
4  0x22, 0x20 - Sansuu 1 Nen, Sansuu 2 Nen
5  0xFF, 0x00 - Sansuu 3 Nen
6  0x21, 0x13 - Spy vs Spy
7  0x20, 0x21 - Seicross
*/

static void Sync(void) {
	if ((submapper != 4 && (datareg & 3) != 0 && datareg != 0x13) || /* 1, 2, 3, 4, 5, 6 */
		(submapper == 4 && (datareg & 1) == 0)) { /* 7 */
		setchr8(0);
	} else {
		setchr8r(0x10, 0);
	}
}

static DECLFW(MWrite) {
	datareg = V;
	Sync();
}

static void M185Power(void) {
	datareg = 0;
	Sync();
	setprg16(0x8000, 0);
	setprg16(0xC000, ~0);
	SetWriteHandler(0x8000, 0xFFFF, MWrite);
	SetReadHandler(0x8000, 0xFFFF, CartBR);
}

static void M185Close(void) {
	if (DummyCHR)
		FCEU_gfree(DummyCHR);
	DummyCHR = NULL;
}

static void StateRestore(int version) {
	Sync();
}

void Mapper185_Init(CartInfo *info) {
	int x;
	info->Power = M185Power;
	info->Close = M185Close;
	submapper = info->submapper;
	GameStateRestore = StateRestore;
	DummyCHR = (uint8 *)FCEU_gmalloc(8192);
	for (x = 0; x < 8192; x++)
		DummyCHR[x] = 0xff;
	SetupCartCHRMapping(0x10, DummyCHR, 8192, 0);
	AddExState(StateRegs, ~0, 0, 0);
}
