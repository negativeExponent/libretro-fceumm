/* FCEUmm - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2007 CaH4e3
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

/* -------------------- UNL-TH2131-1 -------------------- */
/* https://wiki.nesdev.com/w/index.php/NES_2.0_Mapper_308
 * NES 2.0 Mapper 308 is used for a bootleg version of the Sunsoft game Batman
 * similar to Mapper 23 Submapper 3) with custom IRQ functionality.
 * UNIF board name is UNL-TH2131-1.
 */

#include "mapinc.h"
#include "vrc24.h"

static uint16 IRQCount;
static uint8 IRQLatch, IRQa;

static SFORMAT IRQStateRegs[] = {
	{ &IRQCount, 2, "IRQC" },
	{ &IRQLatch, 1, "IRQL" },
	{ &IRQa, 1, "IRQA" },

	{ 0 }
};

static DECLFW(TH2131Write) {
	switch (A & 0xF003) {
	case 0xF000: X6502_IRQEnd(FCEU_IQEXT); IRQa = 0; IRQCount = 0; break;
	case 0xF001: IRQa = 1; break;
	case 0xF003: IRQLatch = (V & 0xF0) >> 4; break;
	}
}

void FP_FASTAPASS(1) TH2131IRQHook(int a) {
	int count;

	if (!IRQa)
		return;

	for (count = 0; count < a; count++) {
		IRQCount++;
		if ((IRQCount & 0x0FFF) == 2048)
			IRQLatch--;
		if (!IRQLatch && (IRQCount & 0x0FFF) < 2048)
			X6502_IRQBegin(FCEU_IQEXT);
	}
}

static void TH2131Power(void) {
	IRQa = IRQCount = IRQLatch = 0;
	GenVRC24Power();
	SetWriteHandler(0xF000, 0xFFFF, TH2131Write);
}

void UNLTH21311_Init(CartInfo *info) {
	GenVRC24_Init(info, VRC2b, 1);
	info->Power = TH2131Power;
	MapIRQHook = TH2131IRQHook;
	AddExState(IRQStateRegs, ~0, 0, NULL);
}
