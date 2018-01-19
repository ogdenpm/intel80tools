#include "plm.h"

void GetTx1Item()
{
    if (regetTx1Item) {
        regetTx1Item = false;
        return;
    }
    while (1) {
        RdTx1Item();
        if (tx1Item.type == L_TOKENERROR) {
            if ((curSymbolP = tx1Item.dataw[1]) != 0) {
                if (High(SymbolP(curSymbolP)->infoP) == 0xff)
                    SymbolP(curSymbolP)->infoP = 0;
                if ((curInfoP = SymbolP(curSymbolP)->infoP) == 0)
                    CreateInfo(0, UNK_T);
                tx1Item.dataw[1] = curInfoP - botInfo;
            }
            MapLToT2();
        } else if ((tx1Item.type == L_XREFINFO && XREF)
              || (tx1Item.type == L_XREF && (IXREF || XREF || SYMBOLS))) {
            tx1Item.dataw[1] = curStmtNum;
            Fwrite(&xrfFile, (pointer)&tx1Item, 5);
        } else if (tx1Item.type == L_LINEINFO) {
            if (b88B3)
                WrTx2File((pointer)&linfo, 7);
            memcpy(&linfo, &tx1Item, 7);
            linfo.type = T2_LINEINFO;
            b88B3 = true;
        } else if ((tx1Aux2 & 0x20) != 0)
            MapLToT2();
        else if (tx1Item.type == L_STMTCNT && tx1Item.dataw[0] == 0)
            MapLToT2();
        else if (tx1Item.type != L_XREF && tx1Item.type != L_XREFINFO)
            break;
    }
	if (tx1Item.type == L_VARIABLE)
        curSymbolP = tx1Item.dataw[0];
    if ((tx1Aux2 & 0x10) != 0)
        tx1Item.dataw[0] = tx1Item.dataw[0] + botInfo;
}

bool MatchTx2Item(byte arg1b)
{
    GetTx1Item();
    if (tx1Item.type == arg1b)
        return true;
    else {
        SetRegetTx1Item();
        return false;
    }
}

bool NotMatchTx2Item(byte arg1b)
{
    return ! MatchTx2Item(arg1b);
}

bool MatchTx2AuxFlag(byte arg1b)
{
    GetTx1Item();
    if ((tx1Aux2 & arg1b) != 0)
        return true;
    else {
        SetRegetTx1Item();
        return false;
    }
}


void Sub_4599()
{
    while (1) {
        GetTx1Item();
        if ((tx1Aux2 & 0x80) == 0)
            break;
        else if (tx1Item.type == L_RPAREN)
            break;
    }
    SetRegetTx1Item();
}

void ResyncRparen()
{
    Sub_4599();
    if (MatchTx2Item(L_RPAREN))
        ;
}

void ExpectRparen(byte arg1b)
{
    if (NotMatchTx2Item(L_RPAREN)) {
        WrTx2ExtError(arg1b);
        ResyncRparen();
    }
}

void Sub_45E0()
{
    FindInfo();
    if (curInfoP == 0 || GetType() == LIT_T)
        CreateInfo(256, BYTE_T);
    OptWrXrf();
    if (GetType() != BUILTIN_T)
        if (! TestInfoFlag(F_LABEL))
            if (! TestInfoFlag(F_DECLARED)) {
                WrTx2ExtError(105);	/* UNDECLARED IDENTIFIER */
                SetInfoFlag(F_DECLARED);
            }
}

void Sub_4631()
{
    word tmp;

    tmp = curInfoP;
    FindMemberInfo();
    if (curInfoP == 0) {
        CreateInfo(0, BYTE_T);
        SetParentOffset(tmp);
        SetInfoFlag(F_MEMBER);
    }
    if (! TestInfoFlag(F_LABEL))
        if (! TestInfoFlag(F_DECLARED)) {
            WrTx2ExtError(112);	/* UNDECLARED structure MEMBER */
            SetInfoFlag(F_DECLARED);
        }
    OptWrXrf();
}

void Sub_467D()
{
    Sub_45E0();
    if (MatchTx2Item(L_PERIOD))
        if (GetType() != STRUCT_T)
            WrTx2ExtError(110);	/* INVALID LEFT OPERAND OF QUALIFICATION, not A structure */
        else if (NotMatchTx2Item(L_VARIABLE))
            WrTx2ExtError(111);	/* INVALID RIGHT OPERAND OF QUALIFICATION, not IDENTIFIER */
        else
            Sub_4631();
}

void WrAtFile(pointer buf, word cnt)
{
    Fwrite(&atFile, buf, cnt);
}

void WrAtFileByte(byte arg1b)
{
    WrAtFile(&arg1b, 1);
}

void WrAtFileWord(word arg1w)
{
    WrAtFile((pointer)&arg1w, 2);
}
