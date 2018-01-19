#include "plm.h"

static byte b7A81[4] = { 0x3C, 0x46, 0x50, 0x5A };

static byte bC28B[4], bC28F;

static void Sub_7D32()
{
	EncodeFragData(CF_XCHG);
	pc = pc + 1;
	Sub_5B96(3, 4);
	Sub_5B96(2, 3);
	Sub_5B96(4, 2);
}

static void Sub_7D54()
{
	wC1DC[0] = 0xa;
	wC1DC[1] = wC1C3;
	EncodeFragData(CF_XTHL);
	pc = pc + 1;
	Sub_5C97(4);
	Sub_5C1D(3);
	Sub_5B96(4, 3);
}


static void Sub_7D7E(byte arg1b)
{
	if (arg1b <= 3)
		if (bC28B[arg1b] < 0xc8)
			bC28B[arg1b] = bC28B[arg1b] + bC28F;
}

void Sub_7A85()
{
	byte i, j, k, m, n;

	Sub_597E();
	for (i = 0; i <= 3; i++) {
		if (boC072[i] || boC069[i])
			bC28B[i] = 0xc8;
		else if (boC060[i])
			bC28B[i] = b7A81[i] + 0xf;
		else
			bC28B[i] = b7A81[i];
	}

	if (Shr(bC0C3[wC1C3], 4) != 0xb && bC140[wC1C3] != 0)
		bC28B[0] = 0xC8;
	for (m = 0; m <= 1; m++) {
		if (bC0B7[m] != 0) {
			if (bC0B5[m] == 9 && bC140[wC1C3] == bC0B7[m]) {
				k = m;
				bC28F = 0xce;
				boC1D8 = 0;
			}
			else
				bC28F = 0x32;
			Sub_7D7E(b52B5[bC0BF[m]]);
			Sub_7D7E(Shr(b4C2D[bC0BD[m]], 5));
		}
	}

	n = 0xc8;
	for (i = 0; i <= 3; i++) {
		if (bC28B[i] <= n)
			n = bC28B[j = i];
	}

	if (n == 0xC8) {
		if (boC069[3]) {
			Sub_7D32();
			if (bC0B5[0] == 3) {
				bC0B5[0] = 2;
				Sub_61A9(0);
			}
			else {
				bC0B5[1] = 2;
				Sub_61A9(1);
			}
		}
		Sub_7D54();
		j = 3;
	}
	else
		Sub_6416(j);

	if (bC045[j] == 0xb) {
		bC045[j] = 0;
		if (j != 0 && bC04E[j] != 0) {
			wC1DC[0] = j;
			EncodeFragData(CF_MOVLRHR);
			pc = pc + 1;
		}
	}
	if (!boC1D8) {
		if (bC0B5[1 - k] == 9) {
			if (bC0B7[1 - k] == bC0B7[k]) {
				bC0B5[1 - k] = j;
				bC0B3[1 - k] = bC045[j];
				Sub_61A9(1 - k);
			}
			else
				boC1D8 = 0xff;
		}
		bC0B5[k] = j;
		bC0B3[k] = bC045[j];
		Sub_61A9(k);
	}
}


static byte bC294, bC295, bC296, bC297, bC298;

static bool Sub_7FD9(byte arg1b)
{
	if (arg1b <= 3)
		if (bC0B5[bC296] == arg1b)
			return true;
	return false;
}

static void Sub_7F19()
{
	for (bC295 = 0; bC295 <= 1; bC295++) {
		if (bC0B7[bC295] != 0 && bC0BB[bC295] != 0) {
			if (bC0BB[bC296 = 1 - bC295] != 0) {
				if (Sub_7FD9(Shr(b4C2D[bC0BD[bC295]], 5)) || Sub_7FD9(b52B5[bC0BF[bC295]]))
					bC0BB[bC296] = 10 + bC0BB[bC296];
			}
		}
	}

	if (bC0BB[0] > bC0BB[1])
		bC295 = 0;
	else
		bC295 = 1;

	bC296 = 1 - bC295;
	bC298 = bC0BD[bC295];
}



static void Sub_7FFC()
{
	if (bC298 == 0x13) {
		if (boC072[3] || boC069[3])
			bC298 = 0x15;
	}
	else if (bC298 == 0x14) {
		bC294 = 5 - bC297;
		if (bC0B5[0] == bC0B5[1])
			if (bC0B3[bC295] == 0)
				bC298 = 0xE;
			else
				bC298 = 0x11;
	}
	else if (bC298 == 8) {
		if (tx2op1[bC0B7[bC295]] != 0) {
			bC298 = 6;
			if (bC294 == 0)
				bC294 = 1;
		}
	}
}



static void Sub_8086()
{
	if (9 <= bC298 && bC298 <= 13)
		Sub_63AC(bC297);

	if (b4C2D[bC298]) {
		if (bC294 != bC297)
			Sub_5D6B(bC294);
		else if (9 <= bC298 && bC298 <= 13) {
			bC0B5[bC295] = 9;
			Sub_597E();
			Sub_5D6B(bC294);
			bC0B5[bC295] = bC297;
			Sub_597E();
		}
	}
	else if (bC298 == 0x15) {
		if (boC069[3])
			bC0B5[bC296] = 9;
	}
	else if (bC298 == 0x14) {
		if (bC0B5[bC296] == bC294)
			bC0B5[bC296] = bC297;
	}
}

static void Sub_8148(byte arg1b, byte arg2b)
{
	byte i;
	if (arg2b == 0)
		return;
	if (arg2b == 1 || arg2b == 2) {
		if (arg1b == 0)
			wC1DC[bC1DB] = bC297;
		else
			wC1DC[bC1DB] = bC294;
		bC1DB = bC1DB + 1;
	}
	else if (arg1b == 2) {
		wC1DC[bC1DB] = 8;
		bC1DB = bC1DB + 2;
	}
	else if (arg1b == 3) {
		wC1DC[bC1DB] = 0xA;
		wC1DC[bC1DB + 1] = wC1C3;
		bC1DB = bC1DB + 3;
	}
	else {
		i = bC1DB;
		Sub_636A(bC295);
	}
}

static void Sub_8207()
{
	switch (Shr(b4C15[bC298], 4)) {
	case 0:
		Sub_5C97(bC294);
		wC1C3 = wC1C3 - 1;
		break;
	case 1:
		Sub_5C97(4);
		Sub_5C1D(3);
		Sub_5B96(4, 3);
		break;
	case 2:
		boC057[bC294] = true;
		bC0A8[bC294] = 0;
		bC04E[bC294] = bC0B7[bC295];
		wC096[bC294] = 0x100;
		if (wC1DC[0] == 0xA) {
			wC084[bC294] = -Shl(wC1DC[1], 1);
			if (bC0C3[tx2op3[bC0B7[bC295]]] == 0xb0)
				if (bC298 == 5) {
					wC084[bC294] = wC084[bC294] - 1;
					wC1DC[2] = wC1DC[2] + 1;
				}
		}
		else
			wC084[bC294] = wC1DC[3] - Shl(wC1C3, 1);
		break;
	case 3:
		boC057[bC294] = true;
		bC0A8[bC294] = 0;
		bC04E[bC294] = bC0B7[bC295];
		Sub_5FBF(bC04E[bC294], &wC084[bC294], &wC096[bC294]);
		break;
	case 4:
		boC057[bC294] = 0;
		bC04E[bC294] = bC0B7[bC295];
		if (bC0B3[bC295] == 4 || bC0B3[bC295] == 5) {
			bC0A8[bC294] = bC0C3[tx2op3[bC0B7[bC295]]] & 0xf;
			if (bC0A8[bC294] > 7)
				bC0A8[bC294] = bC0A8[bC294] | 0xf0;
		}
		else
			bC0A8[bC294] = 0;
		break;
	case 5:
		Sub_5B96(bC297, bC294);
		break;
	case 6:
		Sub_5B96(3, 4);
		Sub_5B96(2, 3);
		Sub_5B96(4, 2);
		break;
	case 7: break;
	}
}

static void Sub_841E()
{
	switch (b4C15[bC298] & 0xf) {
	case 0:  break;
	case 1: bC045[bC294] = 1; break;
	case 2: bC045[bC294] = 0; break;
	case 3: bC045[bC294] = 6; break;
	case 4:
		if (bC0B5[bC295] != 8)
			bC045[bC294] = Sub_5748(bC0B3[bC295]);
		else if (bC0B3[bC295] == 0)
			bC045[bC294] = 6;
		else
			bC045[bC294] = bC0B3[bC295];
		break;
	case 5:
		bC045[bC294] = bC0B3[bC295] - 2;
		bC0A8[3] = bC0A8[3] + 1;
		break;
	}
}



void Sub_7DA9()
{
	byte i;

	Sub_7F19();
	if (bC298 == 0x17)
		Sub_58F5(ERR214);
	else if (bC298 == 0x16) {
		bC0C1[bC295] = bC0BF[bC295];
		bC0B3[bC295] = b528D[bC0C1[bC295]];
		bC0B5[bC295] = b52B5[bC0C1[bC295]];
	}
	else if (bC298 == 0x12)
		boC1D8 = true;
	else {
		bC294 = Shr(b4C2D[bC298], 5);
		if (bC294 > 3)
			bC294 = b52B5[bC0BF[bC295]];
		bC297 = bC0B5[bC295];
		Sub_597E();
		Sub_7FFC();
		i = b5012[bC298];
		Sub_8086();
		bC1DB = 0;
		Sub_8148(Shr(b4C2D[bC298], 3) & 3, Shr(b4029[i], 4) & 7);
		Sub_8148(Shr(b4C2D[bC298], 1) & 3, Shr(b4029[i], 1) & 7);
		Sub_8207();
		Sub_841E();
		bC0B3[bC295] = bC045[bC294];
		bC0B5[bC295] = bC294;
		Sub_61A9(0);
		Sub_61A9(1);
		EncodeFragData(i);
		pc = pc + (b43F8[i] & 0x1f);

	}
}



static byte Sub_8683(byte arg1b)
{
	if (bC0B5[0] == arg1b)
		return 1;
	else
		return 0;
}


static void Sub_8698(byte arg1b, byte arg2b)
{
	byte i;
	word p;
	switch (arg1b) {
	case 0: return;
	case 1: i = 0; break;
	case 2: i = 1; break;
	case 3:
		wC1DC[bC1DB] = 0xA;
		wC1DC[bC1DB + 1] = wC1C3;
		bC1DB = bC1DB + 3;
		return;
	case 4: i = Sub_8683(3); break;
	case 5: i = Sub_8683(0); break;
	case 6:
		if (arg2b == 7) {
			wC1DC[bC1DB] = 0x10;
			wC1DC[bC1DB + 1] = tx2op3[tx2qp] - botInfo;
			bC1DB = bC1DB + 2;
		}
		else
			Sub_61E0((byte)tx2op3[tx2qp]);
		return;
	}
	if (arg2b <= 3)
		Sub_636A(i);
	else {
		wC1DC[bC1DB] = arg2b + 9;
		if (arg2b == 6)
			wC1DC[bC1DB + 1] = tx2op2[1];
		else
			Sub_5FBF(bC0B7[i], &wC1DC[bC1DB + 1], &p);
		bC1DB = bC1DB + 2;
	}
}

void Sub_84ED()
{
	byte i;

	if (cfrag1 > CF_3) {
		bC1DB = 0;
		Sub_8698(Shr(b42F9[cfrag1], 4), Shr(b4029[cfrag1], 4) & 7);
		if (cfrag1 == CF_67 || cfrag1 == CF_68)
			wC1DC[bC1DB - 1] = wC1DC[bC1DB - 1] + 2;
		Sub_8698(b42F9[cfrag1] & 0xf, Shr(b4029[cfrag1], 1) & 7);
		EncodeFragData(cfrag1);
		pc = pc + (b43F8[cfrag1] & 0x1f);
		if (cfrag1 == CF_DELAY) {
			WordP(helpersP)[105] = 1;
			if (wC1C5 < Shl(wC1C3 + 1, 1))
				wC1C5 = Shl(wC1C3 + 1, 1);
		}
		else if (cfrag1 > CF_171) {
			i = b413B[cfrag1 - CF_174];
			i = b4128[i] + 11 * b425D[b4273[curOp]];
			i = b3FCD[Shr(b418C[i], 2)] + (b418C[i] & 3);
			WordP(helpersP)[i] = 1;
			if (curOp == T2_SLASH || curOp == T2_MOD || curOp == T2_44) {
				if (wC1C5 < Shl(wC1C3 + 2, 1))
					wC1C5 = Shl(wC1C3 + 2, 1);
			} else if (wC1C5 < Shl(wC1C3 + 1, 1))
				wC1C5 = Shl(wC1C3 + 1, 1);
		}
	}
}


