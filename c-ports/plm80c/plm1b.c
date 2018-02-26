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
            if ((curSymbolP = tx1Item.dataw[1]) != 0) {         // check we have symbol info
                if (High(SymbolP(curSymbolP)->infoP) == 0xff)   // is it a keyword?
                    SymbolP(curSymbolP)->infoP = 0;             // reset info link
                if ((curInfoP = SymbolP(curSymbolP)->infoP) == 0)
                    CreateInfo(0, UNK_T);                       // allocate an UNK_T info block 
                tx1Item.dataw[1] = curInfoP - botInfo;          // replace sym ref with info offset
            }
            MapLToT2();
        } else if ((tx1Item.type == L_XREFUSE && XREF)
              || (tx1Item.type == L_XREFDEF && (IXREF || XREF || SYMBOLS))) {
            tx1Item.dataw[1] = curStmtNum;
            Fwrite(&xrfFile, (pointer)&tx1Item, 5);             // write the L_XREFUSE/L_XREFDEF infoP and stmtNum to xrfFile
        } else if (tx1Item.type == L_LINEINFO) {
            if (tx2LinfoPending)                                // write any pending lInfo
                WrTx2File((pointer)&linfo, 7);
            memcpy(&linfo, &tx1Item, 7);                        // copy new info
            linfo.type = T2_LINEINFO;                           // update T2 code
            tx2LinfoPending = true;                             // flag as pending
        } else if ((tx1Aux2 & 0x20) != 0)                       // pass through
            MapLToT2();
        else if (tx1Item.type == L_STMTCNT && tx1Item.dataw[0] == 0)    // pass through null stmt cnts
            MapLToT2();
        else if (tx1Item.type != L_XREFDEF && tx1Item.type != L_XREFUSE)    // break if not handled (or ignored)
            break;
    }
    if (tx1Item.type == L_IDENTIFIER)                   // set symbolP up
        curSymbolP = tx1Item.dataw[0];
    if ((tx1Aux2 & 0x10) != 0)                          // procedure, at, data, initial or external - adjust  offset
        tx1Item.dataw[0] += botInfo;
}

bool MatchTx1Item(byte arg1b)       // check for requested Lex item. If present return true else return false and don't consume item
{
    GetTx1Item();
    if (tx1Item.type == arg1b)
        return true;
    else {
        SetRegetTx1Item();
        return false;
    }
}

bool NotMatchTx1Item(byte arg1b)    /// check for requested Lex item. If present consume item and return false else return true and don't consume item
{
    return ! MatchTx1Item(arg1b);
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


void RecoverRPOrEndExpr()
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

void ResyncRParen()
{
    RecoverRPOrEndExpr();
    if (MatchTx1Item(L_RPAREN))     // consume the RP if seen
        ;
}

void ExpectRParen(byte arg1b)
{
    if (NotMatchTx1Item(L_RPAREN)) {
        WrTx2ExtError(arg1b);
        ResyncRParen();
    }
}

void ChkIdentifier()
{
    FindInfo();
    if (curInfoP == 0 || GetType() == LIT_T)        // doesn't exist or appeared in recursive LIT!!
        CreateInfo(0x100, BYTE_T);                  // assume a var was declared
    OptWrXrf();
    if (GetType() != BUILTIN_T)                     // check if declaraed
        if (! TestInfoFlag(F_LABEL))                // labels ok as they may be forward ref
            if (! TestInfoFlag(F_DECLARED)) {
                WrTx2ExtError(105);	/* UNDECLARED IDENTIFIER */
                SetInfoFlag(F_DECLARED);            // flag as now declared
            }
}

void ChkStructureMember()
{
    word tmp;

    tmp = curInfoP;     // save parent info
    FindMemberInfo();   // get the member info
    if (curInfoP == 0) {    // oops not there
        CreateInfo(0, BYTE_T);  // create a member to allow compiler to continue
        SetParentOffset(tmp);
        SetInfoFlag(F_MEMBER);
    }
    if (! TestInfoFlag(F_LABEL))    // not a genuine member
        if (! TestInfoFlag(F_DECLARED)) {   // warn once if not declared
            WrTx2ExtError(112);	/* UNDECLARED STRUCTURE MEMBER */
            SetInfoFlag(F_DECLARED);
        }
    OptWrXrf();
}

void GetVariable()
{
    ChkIdentifier();
    if (MatchTx1Item(L_PERIOD))
        if (GetType() != STRUCT_T)
            WrTx2ExtError(110);	/* INVALID LEFT OPERAND OF QUALIFICATION, NOT A STRUCTURE */
        else if (NotMatchTx1Item(L_IDENTIFIER))
            WrTx2ExtError(111);	/* INVALID RIGHT OPERAND OF QUALIFICATION, NOT IDENTIFIER */
        else
            ChkStructureMember();
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
