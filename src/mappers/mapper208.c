/* FCE Ultra - NES/Famicom Emulator
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
 */
 
 /* 2022-2-14
  * - add support for iNESCart.submapper 1, Mortal Kombat (JJ-01) (Ch) [!]
  * - add mirroring
  */

#include "mapinc.h"
#include "mmc3.h"

static uint8 reg;
static uint8 protIndex;
static uint8 protReg[4];

static const uint8 lut[256] = {
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x49, 0x19, 0x09, 0x59, 0x49, 0x19, 0x09,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x51, 0x41, 0x11, 0x01, 0x51, 0x41, 0x11, 0x01,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x49, 0x19, 0x09, 0x59, 0x49, 0x19, 0x09,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x51, 0x41, 0x11, 0x01, 0x51, 0x41, 0x11, 0x01,
	0x00, 0x10, 0x40, 0x50, 0x00, 0x10, 0x40, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x08, 0x18, 0x48, 0x58, 0x08, 0x18, 0x48, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x40, 0x50, 0x00, 0x10, 0x40, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x08, 0x18, 0x48, 0x58, 0x08, 0x18, 0x48, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x58, 0x48, 0x18, 0x08, 0x58, 0x48, 0x18, 0x08,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x50, 0x40, 0x10, 0x00, 0x50, 0x40, 0x10, 0x00,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x58, 0x48, 0x18, 0x08, 0x58, 0x48, 0x18, 0x08,
	0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x50, 0x40, 0x10, 0x00, 0x50, 0x40, 0x10, 0x00,
	0x01, 0x11, 0x41, 0x51, 0x01, 0x11, 0x41, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x09, 0x19, 0x49, 0x59, 0x09, 0x19, 0x49, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x11, 0x41, 0x51, 0x01, 0x11, 0x41, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x09, 0x19, 0x49, 0x59, 0x09, 0x19, 0x49, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static void M208PRG(void) {
	setprg32(0x8000, (reg & 0x01) | ((reg >> 3) & 0x02));
}

static void M208MIR(void) {
	setmirror(((reg >> 5) & 0x01) ^ 0x01);
}

static void M208PRG_Sub1(void) {
	setprg32(0x8000, mmc3.reg[6] >> 2);
}

static void M208MIR_Sub1(void) {
	setmirror((mmc3.mirr & 0x01) ^ 0x01);
}

static DECLFW(M208Write) {
	reg = V;
	MMC3_FixPRG();
	MMC3_FixMIR();
}

static DECLFW(M208ProtWrite) {
	if (A & 0x800) {
		protReg[(A & 0x03)] = V ^ lut[protIndex];
	} else {
		protIndex = V;
	}
}

static DECLFR(M208ProtRead) {
	return (protReg[(A & 0x3)]);
}

static DECLFW(M208WriteCMD) {
	switch (A & 0xE001) {
	case 0x8001:
		switch (mmc3.cmd & 0x07) {
		case 6:
		case 7:
			mmc3.reg[mmc3.cmd & 0x07] = V;
			MMC3_FixPRG();
			break;
		default:
			MMC3_CMDWrite(A, V);
			break;
		}
	default:
		MMC3_CMDWrite(A, V);
		break;
	}
}

static void M208Power(void) {
	reg = 0x11;
	MMC3_Power();
	SetWriteHandler(0x4800, 0x4fff, M208Write);
	SetWriteHandler(0x6800, 0x6fff, M208Write);
	SetWriteHandler(0x5000, 0x5fff, M208ProtWrite);
	SetReadHandler(0x5800, 0x5fff, M208ProtRead);
	SetReadHandler(0x8000, 0xffff, CartBR);

	if (iNESCart.submapper == 1) {
		SetWriteHandler(0x8000, 0x9FFF, M208WriteCMD);
		MMC3_FixPRG = M208PRG_Sub1;
		MMC3_FixMIR = M208MIR_Sub1;
		MMC3_Reset();
	}
}

void Mapper208_Init(CartInfo *info) {
	MMC3_Init(info, 0, 0);
	MMC3_FixPRG = M208PRG;
	MMC3_FixMIR = M208MIR;
	info->Power = M208Power;
	AddExState(&reg, 1, 0, "EXPR");
	AddExState(&protIndex, 1, 0, "PRID");
	AddExState(&protReg, 4, 0, "PRRG");
}
