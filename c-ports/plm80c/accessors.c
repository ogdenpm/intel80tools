#include "plm.h"

byte GetBuiltinId()
{

	return InfoP(curInfoP)->flag[0];
}

void SetBuiltinId(byte id)
{

	InfoP(curInfoP)->flag[0] = id;
}


offset_t GetBaseOffset()
{
	if (InfoP(curInfoP)->baseoff == 0 )
		return 0;
	else
		return InfoP(curInfoP)->baseoff + botInfo;
}

void SetBaseOffset(offset_t baseP)
{

	if (baseP == 0 )
		InfoP(curInfoP)->baseoff = 0;
	else
		InfoP(curInfoP)->baseoff = baseP - botInfo;
}


word GetBaseVal()
{
	return InfoP(curInfoP)->baseoff;
}

void SetBaseVal(word val)
{
	InfoP(curInfoP)->baseoff = val;
}

byte GetCondFlag()
{
	return InfoP(curInfoP)->flag[0];
}

void SetCondFlag(byte flag)
{
	InfoP(curInfoP)->flag[0] = flag;
}

word GetDimension()
{
	return InfoP(curInfoP)->dim;
}

word GetDimension2()
{
	return InfoP(curInfoP)->dim;
}

void SetDimension(word dim)
{
	InfoP(curInfoP)->dim = dim;
}

byte GetDataType()
{
	if (InfoP(curInfoP)->type == PROC_T )
		return InfoP(curInfoP)->dtype;
	else
		return InfoP(curInfoP)->flag[2];
}

void SetDataType(byte dtype)
{
	if (InfoP(curInfoP)->type == PROC_T )
		InfoP(curInfoP)->dtype = dtype;
	else
		InfoP(curInfoP)->flag[2] = dtype;
}

byte GetExternId()
{
	return InfoP(curInfoP)->extId;
}

void SetExternId(byte id)
{
	InfoP(curInfoP)->extId = id;;
}

void SetInfoFlag(byte flag)
{
	SetFlag(InfoP(curInfoP)->flag, flag);
}

byte GetIntrNo()
{
	return InfoP(curInfoP)->intno;
}

void SetIntrNo(byte intNo)
{
	InfoP(curInfoP)->intno = intNo;
}

byte GetLen()
{
	return InfoP(curInfoP)->len;
}

void SetLen(byte len)
{
	InfoP(curInfoP)->len = len;
}

offset_t GetLitAddr()
{
	return LitP(curInfoP)->litAddr;
}

void SetLitAddr(offset_t litAddr)
{
	LitP(curInfoP)->litAddr = litAddr;
}

offset_t GetLinkOffset()
{
	if (InfoP(curInfoP)->ilink == 0 )
		return 0;
	else
		return botInfo + InfoP(curInfoP)->ilink;
}

void SetLinkOffset(offset_t link)
{
	if (link == 0 )
		InfoP(curInfoP)->ilink = 0;
	else
		InfoP(curInfoP)->ilink = link - botInfo;
}

word GetLinkVal()
{
	return InfoP(curInfoP)->ilink;
}

void SetLinkVal(word val)
{
	InfoP(curInfoP)->ilink = val;
}

offset_t GetParentOffset()
{
	if (InfoP(curInfoP)->type == STRUCT_T)
		return InfoP(curInfoP)->parent;
	else if (InfoP(curInfoP)->parent == 0)
		return 0;
	else
		return InfoP(curInfoP)->parent + botInfo;
}

void SetParentOffset(offset_t parent)
{
	if (InfoP(curInfoP)->type == STRUCT_T ) {
		InfoP(curInfoP)->parent = parent;
		return;
	}
	if (parent == 0 )
		InfoP(curInfoP)->parent = 0;
	else
		InfoP(curInfoP)->parent = parent - botInfo;
}

word GetParentVal()
{
	return InfoP(curInfoP)->parent;
}

byte GetParamCnt()
{
	if (InfoP(curInfoP)->type == PROC_T )
		return InfoP(curInfoP)->pcnt;
	else
		return InfoP(curInfoP)->flag[1];
}

void SetParamCnt(byte cnt)
{
	if (InfoP(curInfoP)->type == PROC_T )
		InfoP(curInfoP)->pcnt = cnt;
	else
		InfoP(curInfoP)->flag[1] = cnt;
}

byte GetProcId()
{
	return InfoP(curInfoP)->procId;
}

void SetProcId(byte id)
{
	InfoP(curInfoP)->procId = id;
}

word GetScope()
{
	return InfoP(curInfoP)->scope;
}

void SetScope(word scope)
{
	InfoP(curInfoP)->scope = scope;
}

offset_t GetSymbol()
{
	if (InfoP(curInfoP)->sym == 0)
		return 0;
	else
		return topSymbol - InfoP(curInfoP)->sym;
}

void SetSymbol(offset_t symbol)
{
	if (symbol == 0 )
		InfoP(curInfoP)->sym = 0;
	else
		InfoP(curInfoP)->sym = topSymbol - symbol;
}

byte GetType()
{
	return InfoP(curInfoP)->type;
}

void SetType(byte type)
{
	InfoP(curInfoP)->type = type;
}

// flag access functions

static byte tblOffsets[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
1, 1, 1, 1, 1, 1, 1, 1, 2, 2 };
static byte tblBitFlags[] = { 0x80, 0x40, 0x20, 0x10, 0x10, 8, 8, 4, 2, 1,
0x80, 0x40, 0x20, 0x10, 8, 4, 2, 1, 0x80, 0x40 };


void ClrFlag(pointer base, byte flag)
{
	byte off, mask;

	off = tblOffsets[flag];
	mask = ~tblBitFlags[flag];
	base[off] &= mask;
}

void ClrFlags(pointer base)
{
	byte i;

	for (i = 0; i <= 2; i++) {
		base[i] = 0;
	}
}

void ClrInfoFlag(byte flag)
{
	ClrFlag(InfoP(curInfoP)->flag, flag);
}

void CpyFlags(pointer base)
{
	byte i;

	for (i = 0; i <= 2; i++) {
		InfoP(curInfoP)->flag[i] = base[i];
	}
}

void SetFlag(pointer base, byte flag)
{
	byte off, bit;

	off = tblOffsets[flag];
	bit = tblBitFlags[flag];
	base[off] |= bit;
}


bool TestFlag(pointer base, byte flag)
{
	byte off, bit;

	off = tblOffsets[flag];
	bit = tblBitFlags[flag];
	if ((base[off] & bit) != 0 )
		return true;
	else
		return false;
} /* TestFlag() */

byte TestInfoFlag(byte flag)
{
	return TestFlag(InfoP(curInfoP)->flag, flag);
}


