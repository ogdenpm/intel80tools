#include "plm.h"

/* plm4a.plm */
static byte digits[] = " 123456789";
static byte errStrings[] = {            // merged plm4b & plm6b messages
    "\x1" "INVALID PL/M-80 CHARACTER\0"
    "\x2" "UNPRINTABLE ASCII CHARACTER\0"
    "\x3" "IDENTIFIER, STRING, OR NUMBER TOO LONG, TRUNCATED\0"
    "\x4" "ILLEGAL NUMERIC CONSTANT TYPE\0"
    "\x5" "INVALID CHARACTER IN NUMERIC CONSTANT\0"
    "\x6" "ILLEGAL MACRO REFERENCE, RECURSIVE EXPANSION\0"
    "\x7" "LIMIT EXCEEDED: MACROS NESTED TOO DEEPLY\0"
    "\x8" "INVALID CONTROL FORMAT\0"
    "\x9" "INVALID CONTROL\0"
    "\xA" "ILLEGAL USE OF PRIMARY CONTROL AFTER NON-CONTROL LINE\0"
    "\xB" "MISSING CONTROL PARAMETER\0"
    "\xC" "INVALID CONTROL PARAMETER\0"
    "\xD" "LIMIT EXCEEDED: INCLUDE NESTING\0"
    "\xE" "INVALID CONTROL FORMAT, INCLUDE NOT LAST CONTROL\0"
    "\xF" "MISSING INCLUDE CONTROL PARAMETER\0"
    "\x10" "ILLEGAL PRINT CONTROL\0"
    "\x11" "INVALID PATH-NAME\0"
    "\x12" "INVALID MULTIPLE LABELS AS MODULE NAMES\0"
    "\x13" "INVALID LABEL IN MODULE WITHOUT MAIN PROGRAM\0"
    "\x14" "MISMATCHED IDENTIFIER AT END OF BLOCK\0"
    "\x15" "MISSING PROCEDURE NAME\0"
    "\x16" "INVALID MULTIPLE LABELS AS PROCEDURE NAMES\0"
    "\x17" "INVALID LABELLED END IN EXTERNAL PROCEDURE\0"
    "\x18" "INVALID STATEMENT IN EXTERNAL PROCEDURE\0"
    "\x19" "UNDECLARED PARAMETER\0"
    "\x1A" "INVALID DECLARATION, STATEMENT OUT OF PLACE\0"
    "\x1B" "LIMIT EXCEEDED: NUMBER OF DO BLOCKS\0"
    "\x1C" "MISSING 'THEN'\0"
    "\x1D" "ILLEGAL STATEMENT\0"
    "\x1E" "LIMIT EXCEEDED: NUMBER OF LABELS ON STATEMENT\0"
    "\x1F" "LIMIT EXCEEDED: PROGRAM TOO COMPLEX\0"
    "\x20" "INVALID SYNTAX, TEXT IGNORED UNTIL ';'\0"
    "\x21" "DUPLICATE LABEL DECLARATION\0"
    "\x22" "DUPLICATE PROCEDURE DECLARATION\0"
    "\x23" "LIMIT EXCEEDED: NUMBER OF PROCEDURES\0"
    "\x24" "MISSING PARAMETER\0"
    "\x25" "MISSING ')' AT END OF PARAMETER LIST\0"
    "\x26" "DUPLICATE PARAMETER NAME\0"
    "\x27" "INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL\0"
    "\x28" "DUPLICATE ATTRIBUTE\0"
    "\x29" "CONFLICTING ATTRIBUTE\0"
    "\x2A" "INVALID INTERRUPT VALUE\0"
    "\x2B" "MISSING INTERRUPT VALUE\0"
    "\x2C" "ILLEGAL ATTRIBUTE, 'INTERRUPT' WITH PARAMETERS\0"
    "\x2D" "ILLEGAL ATTRIBUTE, 'INTERRUPT' WITH TYPED PROCEDURE\0"
    "\x2E" "ILLEGAL USE OF LABEL\0"
    "\x2F" "MISSING ')' AT END OF FACTORED DECLARATION\0"
    "\x30" "ILLEGAL DECLARATION STATEMENT SYNTAX\0"
    "\x31" "LIMIT EXCEEDED: NUMBER OF ITEMS IN FACTORED DECLARE\0"
    "\x32" "INVALID ATTRIBUTES FOR BASE\0"
    "\x33" "INVALID BASE, SUBSCRIPTING ILLEGAL\0"
    "\x34" "INVALID BASE, MEMBER OF BASED STRUCTURE OR ARRAY OF STRUCTURES\0"
    "\x35" "INVALID STRUCTURE MEMBER IN BASE\0"
    "\x36" "UNDECLARED BASE\0"
    "\x37" "UNDECLARED STRUCTURE MEMBER IN BASE\0"
    "\x38" "INVALID MACRO TEXT, NOT A STRING CONSTANT\0"
    "\x39" "INVALID DIMENSION, ZERO ILLEGAL\0"
    "\x3A" "INVALID STAR DIMENSION IN FACTORED DECLARATION\0"
    "\x3B" "ILLEGAL DIMENSION ATTRIBUTE\0"
    "\x3C" "MISSING ')' AT END OF DIMENSION\0"
    "\x3D" "MISSING TYPE\0"
    "\x3E" "INVALID STAR DIMENSION WITH 'STRUCTURE' OR 'EXTERNAL'\0"
    "\x3F" "INVALID DIMENSION WITH THIS ATTRIBUTE\0"
    "\x40" "MISSING STRUCTURE MEMBERS\0"
    "\x41" "MISSING ')' AT END OF STRUCTURE MEMBER LIST\0"
    "\x42" "INVALID STRUCTURE MEMBER, NOT AN IDENTIFIER\0"
    "\x43" "DUPLICATE STRUCTURE MEMBER NAME\0"
    "\x44" "LIMIT EXCEEDED: NUMBER OF STRUCTURE MEMBERS\0"
    "\x45" "INVALID STAR DIMENSION WITH STRUCTURE MEMBER\0"
    "\x46" "INVALID MEMBER TYPE, 'STRUCTURE' ILLEGAL\0"
    "\x47" "INVALID MEMBER TYPE, 'LABEL' ILLEGAL\0"
    "\x48" "MISSING TYPE FOR STRUCTURE MEMBER\0"
    "\x49" "INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL\0"
    "\x4A" "INVALID STAR DIMENSION, NOT WITH 'DATA' OR 'INITIAL'\0"
    "\x4B" "MISSING ARGUMENT OF 'AT', 'DATA', OR 'INITIAL'\0"
    "\x4C" "CONFLICTING ATTRIBUTE WITH PARAMETER\0"
    "\x4D" "INVALID PARAMETER DECLARATION, BASE ILLEGAL\0"
    "\x4E" "DUPLICATE DECLARATION\0"
    "\x4F" "ILLEGAL PARAMETER TYPE, NOT BYTE OR ADDRESS\0"
    "\x50" "INVALID DECLARATION, LABEL MAY NOT BE BASED\0"
    "\x51" "CONFLICTING ATTRIBUTE WITH 'BASE'\0"
    "\x52" "INVALID SYNTAX, MISMATCHED '('\0"
    "\x53" "LIMIT EXCEEDED: DYNAMIC STORAGE\0"
    "\x54" "LIMIT EXCEEDED: BLOCK NESTING\0"
    "\x55" "LONG STRING ASSUMED CLOSED AT NEXT SEMICOLON OR QUOTE\0"
    "\x56" "LIMIT EXCEEDED: SOURCE LINE LENGTH\0"
    "\x57" "MISSING 'END', END-OF-FILE ENCOUNTERED\0"
    "\x58" "INVALID PROCEDURE NESTING, ILLEGAL IN REENTRANT PROCEDURE\0"
    "\x59" "MISSING 'DO' FOR MODULE\0"
    "\x5A" "MISSING NAME FOR MODULE\0"
    "\x5B" "ILLEGAL PAGELENGTH CONTROL VALUE\0"
    "\x5C" "ILLEGAL PAGEWIDTH CONTROL VALUE\0"
    "\x5D" "MISSING 'DO' FOR 'END', 'END' IGNORED\0"
    "\x5E" "ILLEGAL CONSTANT, VALUE > 65535\0"
    "\x5F" "ILLEGAL RESPECIFICATION OF PRIMARY CONTROL IGNORED\0"
    "\x60" "COMPILER ERROR: SCOPE STACK UNDERFLOW\0"
    "\x61" "COMPILER ERROR: PARSE STACK UNDERFLOW\0"
    "\x62" "INCLUDE FILE IS NOT A DISKETTE FILE\0"
    /* 0X63 */
    "\x64" "INVALID STRING CONSTANT IN Expression\0"
    "\x65" "INVALID ITEM FOLLOWS DOT OPERATOR\0"
    "\x66" "MISSING PRIMARY OPERAND\0"
    "\x67" "MISSING ')' AT END OF SUBEXPRESSION\0"
    "\x68" "ILLEGAL PROCEDURE INVOCATION WITH DOT OPERATOR\0"
    "\x69" "UNDECLARED IDENTIFIER\0"
    "\x6A" "INVALID INPUT/OUTPUT PORT NUMBER\0"
    "\x6B" "ILLEGAL INPUT/OUTPUT PORT NUMBER, NOT NUMERIC CONSTANT\0"
    "\x6C" "MISSING ')' AFTER INPUT/OUTPUT PORT NUMBER\0"
    "\x6D" "MISSING INPUT/OUTPUT PORT NUMBER\0"
    "\x6E" "INVALID LEFT OPERAND OF QUALIFICATION, NOT A STRUCTURE\0"
    "\x6F" "INVALID RIGHT OPERAND OF QUALIFICATION, NOT IDENTIFIER\0"
    "\x70" "UNDECLARED STRUCTURE MEMBER\0"
    "\x71" "MISSING ')' AT END OF ARGUMENT LIST\0"
    "\x72" "INVALID SUBSCRIPT, MULTIPLE SUBSCRIPTS ILLEGAL\0"
    "\x73" "MISSING ')' AT END OF SUBSCRIPT\0"
    "\x74" "MISSING '=' IN ASSIGNMENT STATEMENT\0"
    "\x75" "MISSING PROCEDURE NAME IN CALL STATEMENT\0"
    "\x76" "INVALID INDIRECT CALL, IDENTIFIER NOT AN ADDRESS SCALAR\0"
    "\x77" "LIMIT EXCEEDED: PROGRAM TOO COMPLEX\0"
    "\x78" "LIMIT EXCEEDED: Expression TOO COMPLEX\0"
    "\x79" "LIMIT EXCEEDED: Expression TOO COMPLEX\0"
    "\x7A" "LIMIT EXCEEDED: PROGRAM TOO COMPLEX\0"
    "\x7B" "INVALID DOT OPERAND, BUILT-IN PROCEDURE ILLEGAL\0"
    "\x7C" "MISSING ARGUMENTS FOR BUILT-IN PROCEDURE\0"
    "\x7D" "ILLEGAL ARGUMENT FOR BUILT-IN PROCEDURE\0"
    "\x7E" "MISSING ')' AFTER BUILT-IN PROCEDURE ARGUMENT LIST\0"
    "\x7F" "INVALID SUBSCRIPT ON NON-ARRAY\0"
    "\x80" "INVALID LEFT-HAND OPERAND OF ASSIGNMENT\0"
    "\x81" "ILLEGAL 'CALL' WITH TYPED PROCEDURE\0"
    "\x82" "ILLEGAL REFERENCE TO OUTPUT FUNCTION\0"
    "\x83" "ILLEGAL REFERENCE TO UNTYPED PROCEDURE\0"
    "\x84" "ILLEGAL USE OF LABEL\0"
    "\x85" "ILLEGAL REFERENCE TO UNSUBSCRIPTED ARRAY\0"
    "\x86" "ILLEGAL REFERENCE TO UNSUBSCRIPTED MEMBER ARRAY\0"
    "\x87" "ILLEGAL REFERENCE TO AN UNQUALIFIED STRUCTURE\0"
    "\x88" "INVALID RETURN FOR UNTYPED PROCEDURE, VALUE ILLEGAL\0"
    "\x89" "MISSING VALUE IN RETURN FOR TYPED PROCEDURE\0"
    "\x8A" "MISSING INDEX VARIABLE\0"
    "\x8B" "INVALID INDEX VARIABLE TYPE, NOT BYTE OR ADDRESS\0"
    "\x8C" "MISSING '=' FOLLOWING INDEX VARIABLE\0"
    "\x8D" "MISSING 'TO' CLAUSE\0"
    "\x8E" "MISSING IDENTIFIER FOLLOWING GOTO\0"
    "\x8F" "INVALID REFERENCE FOLLOWING GOTO, NOT A LABEL\0"
    "\x90" "INVALID GOTO LABEL, NOT AT LOCAL OR MODULE LEVEL\0"
    "\x91" "MISSING 'TO' FOLLOWING 'GO'\0"
    "\x92" "MISSING ')' AFTER 'AT' RESTRICTED Expression\0"
    "\x93" "MISSING IDENTIFIER FOLLOWING DOT OPERATOR\0"
    "\x94" "INVALID QUALIFICATION IN RESTRICTED REFERENCE\0"
    "\x95" "INVALID SUBSCRIPTING IN RESTRICTED REFERENCE\0"
    "\x96" "MISSING ')' AT END OF RESTRICTED SUBSCRIPT\0"
    "\x97" "INVALID OPERAND IN RESTRICTED Expression\0"
    "\x98" "MISSING ')' AFTER CONSTANT LIST\0"
    "\x99" "INVALID NUMBER OF ARGUMENTS IN CALL, TOO MANY\0"
    "\x9A" "INVALID NUMBER OF ARGUMENTS IN CALL, TOO FEW\0"
    "\x9B" "INVALID RETURN IN MAIN PROGRAM\0"
    "\x9C" "MISSING RETURN STATEMENT IN TYPED PROCEDURE\0"
    "\x9D" "INVALID ARGUMENT, ARRAY REQUIRED FOR LENGTH OR LAST\0"
    "\x9E" "INVALID DOT OPERAND, LABEL ILLEGAL\0"
    "\x9F" "COMPILER ERROR: PARSE STACK UNDERFLOW\0"
    "\xA0" "COMPILER ERROR: OPERAND STACK UNDERFLOW\0"
    "\xA1" "COMPILER ERROR: ILLEGAL OPERAND STACK EXCHANGE\0"
    "\xA2" "COMPILER ERROR: OPERATOR STACK UNDERFLOW\0"
    "\xA3" "COMPILER ERROR: GENERATION FAILURE\0"
    "\xA4" "COMPILER ERROR: SCOPE STACK OVERFLOW\0"
    "\xA5" "COMPILER ERROR: SCOPE STACK UNDERFLOW\0"
    "\xA6" "COMPILER ERROR: CONTROL STACK OVERFLOW\0"
    "\xA7" "COMPILER ERROR: CONTROL STACK UNDERFLOW\0"
    "\xA8" "COMPILER ERROR: BRANCH MISSING IN 'IF' STATEMENT\0"
    "\xA9" "ILLEGAL FORWARD CALL\0"
    "\xAA" "ILLEGAL RECURSIVE CALL\0"
    "\xAB" "INVALID USE OF DELIMITER OR RESERVED WORD IN Expression\0"
    "\xAC" "INVALID LABEL: UNDEFINED\0"
    "\xAD" "INVALID LEFT SIDE OF ASSIGNMENT: VARIABLE DECLARED WITH DATA ATTRIBUTE\0"
    "\xAE" "INVALID NULL PROCEDURE\0"
    /* xAF */
    "\xB0" "INVALID INTVECTOR INTERVAL VALUE\0"
    "\xB1" "INVALID INTVECTOR LOCATION VALUE\0"
    "\xB2" "INVALID 'AT' RESTRICTED REFERENCE, EXTERNAL ATTRIBUTE CONFLICTS WITH PUBLIC ATTRIBUTE\0"
    "\xB3" "OUTER 'IF' MAY NOT HAVE AN 'ELSE' PART\0"
    "\xB4" "MISSING OR INVALID CONDITIONAL COMPILATION PARAMETER\0"
    "\xB5" "MISSING OR INVALID CONDITIONAL COMPILATION CONSTANT\0"
    "\xB6" "MISPLACED ELSE OR ELSEIF OPTION\0"
    "\xB7" "MISPLACED ENDIF OPTION\0"
    "\xB8" "CONDITIONAL COMPILATION PARAMETER NAME TOO LONG\0"
    "\xB9" "MISSING OPERATOR IN CONDITIONAL COMPILATION Expression\0"
    "\xBA" "INVALID CONDITIONAL COMPILATION CONSTANT, TOO LARGE\0"
    "\xBB" "LIMIT EXCEEDED: NUMBER OF SAVE LEVELS > 5\0"
    "\xBC" "MISPLACED RESTORE OPTION\0"
    "\xBD" "NULL STRING NOT ALLOWED\0"
    /* 0XBE, 0XBF, 0XC0, 0XC1, 0XC2, 0XC3, 0XC4, 0XC5, 0XC7 */
    "\xC8" "LIMIT EXCEEDED: STATEMENT SIZE\0"
    "\xC9" "INVALID DO CASE BLOCK, AT LEAST ONE CASE REQUIRED\0"
    "\xCA" "LIMIT EXCEEDED: NUMBER OF ACTIVE CASES\0"
    "\xCB" "LIMIT EXCEEDED: NESTING OF TYPED PROCEDURE CALLS\0"
    "\xCC" "LIMIT EXCEEDED: NUMBER OF ACTIVE PROCEDURES AND DO CASE GROUPS\0"
    "\xCD" "ILLEGAL NESTING OF BLOCKS, ENDS NOT BALANCED\0"
    "\xCE" "LIMIT EXCEEDED: CODE SEGMENT SIZE\0"
    "\xCF" "LIMIT EXCEEDED: SEGMENT SIZE\0"
    "\xD0" "LIMIT EXCEEDED: STRUCTURE SIZE\0"
    "\xD1" "ILLEGAL INITIALIZATION OF MORE SPACE THAN DECLARED\0"
    "\xD2" "ILLEGAL INITIALIZATION OF A BYTE TO A VALUE > 255\0"
    "\xD3" "INVALID IDENTIFIER IN 'AT' RESTRICTED REFERENCE\0"
    "\xD4" "INVALID RESTRICTED REFERENCE IN 'AT', BASE ILLEGAL\0"
    "\xD5" "UNDEFINED RESTRICTED REFERENCE IN 'AT'\0"
    "\xD6" "COMPILER ERROR: OPERAND CANNOT BE TRANSFORMED\0"
    "\xD7" "COMPILER ERROR: EOF READ IN FINAL ASSEMBLY\0"
    "\xD8" "COMPILER ERROR: BAD LABEL ADDRESS\0"
    "\xD9" "ILLEGAL INITIALIZATION OF AN EXTERNAL VARIABLE\0"
    "\xDA" "ILLEGAL SUCCESSIVE USES OF RELATIONAL OPERATORS\0"
    "\xDB" "LIMIT EXCEEDED: NUMBER OF EXTERNALS > 255\0" };


void FlushRecs()
{
    WriteRec(rec8, 1);
    WriteRec(rec6_4, 3);
    WriteRec(rec22, 1);
    WriteRec(rec24_1, 2);
    WriteRec(rec24_2, 2);
    WriteRec(rec24_3, 2);
    WriteRec(rec20, 1);
    ((rec6_t *)rec6_4)->addr = baseAddr;
}



void AddWrdDisp(pointer str, word arg2w)
{
    if (arg2w != 0) { 
        str[0]++;
        if (arg2w > 0x8000) {
            str[str[0]] = '-';
            arg2w = -arg2w;
        } else
            str[str[0]] = '+';
        str[0] += Num2Asc(arg2w, 0, -16, &str[str[0] + 1]);
    }
}


void EmitLinePrefix()
{
    if (! linePrefixChecked && listing) {
        SetStartAndTabW(15, 4);
        if (stmtCnt != 0)
            LstLineNo();
        TabLst(-7);
        if (blkCnt < 10) {
            PutLst(' ');
            PutLst(digits[blkCnt]);
        }
        else
            XnumLst(blkCnt, 2, 10);
        if (srcFileIdx != 0) { 
            TabLst(-11);
            XwrnstrLst("=", 1);
            if (srcFileIdx != 10) 
                XnumLst(srcFileIdx / 10 - 1, 1, 10);
        }
        if (lstLineLen > 0) {
            TabLst(-15);
            XwrnstrLst(lstLine, lstLineLen);
        }

        NewLineLst();
        linePrefixEmitted = true;
    }
    linePrefixChecked = true;
    listing = ! listOff && PRINT;
}



void EmitStatementNo()
{
    EmitLinePrefix();
    TabLst(-50);
    XwrnstrLst("; STATEMENT # ", 14);
    XnumLst(stmtNo, 0, 0xA);
    NewLineLst();
}


void EmitLabel()
{
    if (codeOn) { 
        EmitLinePrefix();
        TabLst(-26);
        XwrnstrLst(&locLabStr[1], locLabStr[0]);
        XwrnstrLst(":", 1);
        NewLineLst();
    }
}

static  word errOff, errLen;

void FindErrStr()
{
    word p, q;

    q = 0;
    while (errStrings[q] != 0) {
        p = q;
        while (errStrings[q] != 0)
            q = q + 1;
        if (errStrings[p] == errData.num) {
            errOff = p + 1;
            errLen = q - errOff;
            return;
        }
        q = q + 1;
    }
    errLen = 0;
}

void EmitError()
{

    programErrCnt++;
    if (PRINT) {
        linePrefixChecked = linePrefixEmitted;
        listing = true;
        EmitLinePrefix();
        XwrnstrLst("*** ERROR #", 11);
        XnumLst(errData.num, 0, 10);
        XwrnstrLst(", ", 2);
        if (errData.stmt != 0)
        {
            XwrnstrLst("STATEMENT #", 11);
            XnumLst(errData.stmt, 0, 10);
            XwrnstrLst(", ", 2);
        }
        if (errData.info != 0)
        { 
            XwrnstrLst("NEAR '", 6);
            curInfoP = errData.info + botInfo;
            curSymbolP = GetSymbol();
            if (curSymbolP != 0)
                XwrnstrLst(&SymbolP(curSymbolP)->name[1], SymbolP(curSymbolP)->name[0]);
            else
                Xputstr2cLst("<LONG CONSTANT>", 0);
            XwrnstrLst("', ", 3);
        }
        FindErrStr();
        TabLst(2);
        if (errLen == 0) 
            XwrnstrLst("UNKNOWN ERROR", 13);
        else
            XwrnstrLst(&errStrings[errOff], (byte)errLen);
        NewLineLst();
    }
}

void FatalError_ov46(byte arg1b)
{
    errData.num = fatalErrorCode = arg1b;
    errData.info = 0;
    errData.stmt = stmtNo;
    EmitError();
    longjmp(exception, -1);
}

void ListCodeBytes()
{
    byte i;
    if (codeOn) { 
        if (opByteCnt > 0) {
            TabLst(-12);
            XnumLst(baseAddr, -4, 0x10);
            i = 0;
            TabLst(-18);
            while (i < opByteCnt) {
                XnumLst(opBytes[i], -2, 0x10);
                i = i + 1;
            }
        }
        TabLst(-26);
        SetStartAndTabW(26, 8);
        XwrnstrLst(&line[1], line[0]);
        NewLineLst();
    }
}



static byte crCnt;


static byte GetSourceCh()
{
    if (offCurCh == offLastCh) {
        while (1) {
            ReadF(&srcFil, srcbuf, 1280, &offLastCh);       // uses larger i/o buffer 
            offCurCh = 0;
            if (offLastCh > 0)
                break;     /* there are more chars */
            if (lstLineLen != 0)       /* we already have a part line */
                return '\n';

            if (srcFileIdx == 0) {     /* top level file */
                if (crCnt != 0)    /* allow file to finish with cr */
                    return '\n';
                else
                    FatalError(ERR215);
            }
            CloseF(&srcFil);        /* unnest include file */
            srcFileIdx = srcFileIdx - 10;
            InitF(&srcFil, "SOURCE", (pointer)&srcFileTable[srcFileIdx]);
            OpenF(&srcFil, 1);
            SeekF(&srcFil, (loc_t *)&srcFileTable[srcFileIdx + 8]);
        }
        offLastCh = offLastCh - 1;      /* correct for 0 index */
    } else
        offCurCh = offCurCh + 1;        /* advance to next char */
    return srcbuf[offCurCh] & 0x7f;       /* remove High() bits */
}



void GetSourceLine()
{
    lstLineLen = 0;
    crCnt = 0;

    while (1) {
        lstLine[lstLineLen] = GetSourceCh();
        if (lstLine[lstLineLen] == '\n') {
            linePrefixChecked = false;
            linePrefixEmitted = false;
            return;
        } else if (lstLine[lstLineLen] == '\r')
            crCnt++;
        else if (lstLine[lstLineLen] != '\r' && lstLineLen < 128)
            lstLineLen++;
    }
}

