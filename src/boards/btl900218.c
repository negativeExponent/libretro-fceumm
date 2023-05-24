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

/* -------------------- BTL-900218 -------------------- */
/* http://wiki.nesdev.com/w/index.php/UNIF/900218
 * NES 2.0 Mapper 524 describes the PCB used for the pirate port Lord of King or Axe of Fight.
 * UNIF board name is BTL-900218.
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

static DECLFW(BTL900218Write) {
	switch (A & 0xF00C) {
	case 0xF008: IRQa = 1; break;
	case 0xF00C:
		IRQa = 0;
		IRQCount = 0;
		X6502_IRQEnd(FCEU_IQEXT);
		break;
	}
}

void FP_FASTAPASS(1) BTL900218IRQHook(int a) {
	if (!IRQa)
		return;

	IRQCount += a;
	if (IRQCount & 1024)
		X6502_IRQBegin(FCEU_IQEXT);
}

static void BTL900218Power(void) {
	IRQa = IRQCount = IRQLatch = 0;
	GenVRC24Power();
	SetWriteHandler(0xF000, 0xFFFF, BTL900218Write);
}

void BTL900218_Init(CartInfo *info) {
	GenVRC24_Init(info, VRC2b, 1);
	info->Power = BTL900218Power;
	MapIRQHook = BTL900218IRQHook;
	AddExState(IRQStateRegs, ~0, 0, NULL);
}
