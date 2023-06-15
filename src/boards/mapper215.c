/* FCE Ultra - NES/Famicom Emulator
 *
 * Copyright notice for this file:
 *  Copyright (C) 2011 CaH4e3
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
 * Submapper 0, UNIF board name UNL-8237:
 * Earthworm Jim 2
 * Mortal Kombat 3 (SuperGame, not Extra 60, not to be confused by similarly-named games from other developers)
 * Mortal Kombat 3 Extra 60 (both existing ROM images are just extracts of the 2-in-1 multicart containing this game)
 * Pocahontas Part 2
 * 2-in-1: Aladdin, EarthWorm Jim 2 (Super 808)
 * 2-in-1: The Lion King, Bomber Boy (GD-103)
 * 2-in-1: Super Golden Card: EarthWorm Jim 2, Boogerman (king002)
 * 2-in-1: Mortal Kombat 3 Extra 60, The Super Shinobi (king005)
 * 3-in-1: Boogerman, Adventure Island 3, Double Dragon 3 (Super 308)
 * 5-in-1: Golden Card: Aladdin, EarthWorm Jim 2, Garo Densetsu Special, Silkworm, Contra Force (SPC005)
 * 6-in-1: Golden Card: EarthWorm Jim 2, Mortal Kombat 3, Double Dragon 3, Contra 3, The Jungle Book, Turtles Tournament Fighters (SPC009)
 *
 * Submapper 1, UNIF board name UNL-8237A:
 * 9-in-1 High Standard Card: The Lion King, EarthWorm Jim 2, Aladdin, Boogerman, Somari, Turtles Tournament Fighters, Mortal Kombat 3, Captain Tsubasa 2, Taito Basketball (king001)
 */

#include "mapinc.h"
#include "mmc3.h"

static uint8 submapper;

static uint8 regperm[8][8] =
{
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0, 2, 6, 1, 7, 3, 4, 5 },
	{ 0, 5, 4, 1, 7, 2, 6, 3 },		/* unused */
	{ 0, 6, 3, 7, 5, 2, 4, 1 },
	{ 0, 2, 5, 3, 6, 1, 7, 4 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },		/* empty */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },		/* empty */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },		/* empty */
};

static uint8 adrperm[8][8] =
{
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 3, 2, 0, 4, 1, 5, 6, 7 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },		/* unused */
	{ 5, 0, 1, 2, 3, 7, 6, 4 },
	{ 3, 1, 0, 5, 2, 4, 6, 7 },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },		/* empty */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },		/* empty */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },		/* empty */
};

static uint8 protarray[8][8] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* 0 Super Hang-On               */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00 }, /* 1 Monkey King                 */
	{ 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00 }, /* 2 Super Hang-On/Monkey King   */
	{ 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x05, 0x00 }, /* 3 Super Hang-On/Monkey King   */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* 4                             */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* 5                             */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, /* 6                             */
	{ 0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x0F, 0x00 }  /* 7 (default) Blood of Jurassic */
};

static void M215CW(uint32 A, uint8 V) {
	uint16 outer_bank;

	if (submapper == 1)
		outer_bank = ((mmc3.expregs[1] & 0xE) << 7);
	else
		outer_bank = ((mmc3.expregs[1] & 0xC) << 6);

	if (mmc3.expregs[0] & 0x40)
		setchr1(A, outer_bank | (V & 0x7F) | ((mmc3.expregs[1] & 0x20) << 2));
	else
		setchr1(A, outer_bank | V);
}

static void M215PW(uint32 A, uint8 V) {
	uint8 outer_bank = ((mmc3.expregs[1] & 3) << 5);

	if (submapper == 1)
		outer_bank |= ((mmc3.expregs[1] & 8) << 4);

	if (mmc3.expregs[0] & 0x40) {
		uint8 sbank = (mmc3.expregs[1] & 0x10);
		if (mmc3.expregs[0] & 0x80) { /* NROM */
			uint8 bank = (outer_bank >> 1) | (mmc3.expregs[0] & 0x7) | (sbank >> 1);
			if (mmc3.expregs[0] & 0x20) /* NROM-256 */
				setprg32(0x8000, bank >> 1);
			else { /* NROM-128 */
				setprg16(0x8000, bank);
				setprg16(0xC000, bank);
			}
		} else
			setprg8(A, outer_bank | (V & 0x0F) | sbank);
	} else {
		if (mmc3.expregs[0] & 0x80) { /* NROM */
			uint8 bank = (outer_bank >> 1) | (mmc3.expregs[0] & 0xF);
			if (mmc3.expregs[0] & 0x20) /* NROM-256 */
				setprg32(0x8000, bank >> 1);
			else { /* NROM-128 */
				setprg16(0x8000, bank);
				setprg16(0xC000, bank);
			}
		} else
			setprg8(A, outer_bank | (V & 0x1F));
	}
}

static DECLFR(M215ProtRead) {
	return (protarray[mmc3.expregs[3]][A & 7] & 0x0F) | 0x50;
}

static DECLFW(M215Write) {
	uint8 dat = V;
	uint8 adr = adrperm[mmc3.expregs[2]][((A >> 12) & 6) | (A & 1)];
	uint16 addr = (adr & 1) | ((adr & 6) << 12) | 0x8000;
	if (adr < 4) {
		if (!adr)
			dat = (dat & 0xC0) | (regperm[mmc3.expregs[2]][dat & 7]);
		MMC3_CMDWrite(addr, dat);
	} else
		MMC3_IRQWrite(addr, dat);
}

static DECLFW(M215ExWrite) {
	switch (A & 0xF007) {
		case 0x5000:
			mmc3.expregs[0] = V;
			FixMMC3PRG(mmc3.cmd);
			FixMMC3CHR(mmc3.cmd);
			break;
		case 0x5001:
			mmc3.expregs[1] = V;
			FixMMC3PRG(mmc3.cmd);
			FixMMC3CHR(mmc3.cmd);
			break;
		case 0x5002:
			mmc3.expregs[3] = V;
			break;
		case 0x5007:
			mmc3.expregs[2] = V;
			break;
	}
}

static void M215Power(void) {
	mmc3.expregs[0] = mmc3.expregs[2] = 0;
	mmc3.expregs[1] = 0x0F;
	mmc3.expregs[3] = 7;
	GenMMC3Power();
	SetWriteHandler(0x8000, 0xFFFF, M215Write);
	SetReadHandler(0x5000, 0x5FFF, M215ProtRead);
	SetWriteHandler(0x5000, 0x5FFF, M215ExWrite);
}

static void M215Reset(void) {
	mmc3.expregs[0] = mmc3.expregs[2] = 0;
	mmc3.expregs[1] = 0x0F;
	mmc3.expregs[3] = 7;
	MMC3RegReset();
}

void Mapper215_Init(CartInfo *info) {
	GenMMC3_Init(info, 0, 0);
	mmc3.cwrap = M215CW;
	mmc3.pwrap = M215PW;
	info->Power = M215Power;
	info->Reset = M215Reset;
	AddExState(mmc3.expregs, 4, 0, "EXPR");
	if (info->iNES2)
		submapper = info->submapper;
}

void UNL8237A_Init(CartInfo *info) {
	Mapper215_Init(info);
	submapper = 1;
}
