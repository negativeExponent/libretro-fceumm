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
 * SL1632 2-in-1 protected board, similar to SL12
 * Samurai Spirits Rex (Full)
 *
 */

#include "mapinc.h"
#include "mmc3.h"

static uint8 chrcmd[8], prg0, prg1, bbrk, mirr, swap;
static SFORMAT StateRegs[] =
{
	{ chrcmd, 8, "CHRC" },
	{ &prg0, 1, "PRG0" },
	{ &prg1, 1, "PRG1" },
	{ &bbrk, 1, "BRK" },
	{ &mirr, 1, "MIRR" },
	{ &swap, 1, "SWAP" },
	{ 0 }
};

static void Sync(void) {
	int i;
	setprg8(0x8000, prg0);
	setprg8(0xA000, prg1);
	setprg8(0xC000, ~1);
	setprg8(0xE000, ~0);
	for (i = 0; i < 8; i++)
		setchr1(i << 10, chrcmd[i]);
	setmirror(mirr ^ 1);
}

static void M014CW(uint32 A, uint8 V) {
	int cbase = (mmc3.cmd & 0x80) << 5;
	int page0 = (bbrk & 0x08) << 5;
	int page1 = (bbrk & 0x20) << 3;
	int page2 = (bbrk & 0x80) << 1;
	setchr1(cbase ^ 0x0000, page0 | (mmc3.regs[0] & (~1)));
	setchr1(cbase ^ 0x0400, page0 | mmc3.regs[0] | 1);
	setchr1(cbase ^ 0x0800, page0 | (mmc3.regs[1] & (~1)));
	setchr1(cbase ^ 0x0C00, page0 | mmc3.regs[1] | 1);
	setchr1(cbase ^ 0x1000, page1 | mmc3.regs[2]);
	setchr1(cbase ^ 0x1400, page1 | mmc3.regs[3]);
	setchr1(cbase ^ 0x1800, page2 | mmc3.regs[4]);
	setchr1(cbase ^ 0x1c00, page2 | mmc3.regs[5]);
}

static DECLFW(M014CMDWrite) {
	if (A == 0xA131) {
		bbrk = V;
	}
	if (bbrk & 2) {
		MMC3_FixPRG();
		MMC3_FixCHR();
		if (A < 0xC000)
			MMC3_CMDWrite(A, V);
		else
			MMC3_IRQWrite(A, V);
	} else {
		if ((A >= 0xB000) && (A <= 0xE003)) {
			int ind = ((((A & 2) | (A >> 10)) >> 1) + 2) & 7;
			int sar = ((A & 1) << 2);
			chrcmd[ind] = (chrcmd[ind] & (0xF0 >> sar)) | ((V & 0x0F) << sar);
		} else
			switch (A & 0xF003) {
			case 0x8000: prg0 = V; break;
			case 0xA000: prg1 = V; break;
			case 0x9000: mirr = V & 1; break;
			}
		Sync();
	}
}

static void StateRestore(int version) {
	if (bbrk & 2) {
		MMC3_FixPRG();
		MMC3_FixCHR();
	} else
		Sync();
}

static void M014Power(void) {
	GenMMC3Power();
	SetWriteHandler(0x4100, 0xFFFF, M014CMDWrite);
}

void Mapper014_Init(CartInfo *info) {
	GenMMC3_Init(info, 0, 0);
	MMC3_cwrap = M014CW;
	info->Power = M014Power;
	GameStateRestore = StateRestore;
	AddExState(&StateRegs, ~0, 0, 0);
}
