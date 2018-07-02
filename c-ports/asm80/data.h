// linker defined
extern byte *MEMORY;
// defined in asm2m.c
extern byte opFlags[];
// defined in asm5m.c
extern byte labelUse;
// defined in startm.c
extern word seekMZero;
extern byte b3782[2];

extern byte spaces24[];
#define spaces15	(spaces24 + 9) 
#define spaces6		(spaces24 + 18)
#define spaces5		(spaces24 + 19)
#define spaces4		(spaces24 + 20)
#define spaces2		(spaces24 + 22)
extern byte ascCRLF[];
extern byte signonMsg[];
#define asmHeader	(signonMsg + 2)
#define aModulePage	(signonMsg + 43)
extern byte bZERO;
extern byte bTRUE;
extern pointer aErrStrs[];
extern byte aErrStrsLen[];

// defined in globlm.c

#define	IN_BUF_SIZE	512
#define	OUT_BUF_SIZE	512

extern byte macroLine[129];
extern pointer macroP;
extern bool inQuotes;
extern bool excludeCommentInExpansion;
extern bool inAngleBrackets;
extern byte expandingMacro;
extern bool macroDivert;
extern bool inMacroBody;
extern byte mSpoolMode;
extern bool b9060;
extern bool nestMacro;
extern byte savedMtype;
extern byte macroDepth;
extern byte macroSpoolNestDepth;
extern byte paramCnt;
extern byte startNestCnt;
extern byte argNestCnt;
extern pointer pMacro;
extern pointer macroInPtr;
extern macroStk_t macro;
extern word curMacroBlk;
extern word nxtMacroBlk;
extern word maxMacroBlk;
extern word macroBlkCnt;
extern byte macroBuf[129];
extern pointer savedMacroBufP;
extern pointer pNextArg;
extern word localIdCnt;
extern pointer startMacroLine;
extern pointer startMacroToken;
extern byte irpcChr[3];

extern byte localVarName[];
extern pointer contentBytePtr;
extern byte fixupSeg;
extern word fixOffset;
extern byte curFixupHiLoSegId;
extern byte curFixupType;
extern byte fixIdxs[4];
#define	fix22Idx	fixIdxs[0]
#define	fix24Idx	fixIdxs[1]
#define	fix20Idx	fixIdxs[2]
#define	fix6Idx		fixIdxs[3]
extern byte extNamIdx;
extern bool initFixupReq[4];
extern bool firstContent;
extern eof_t rEof;
extern extnames_t rExtnames;
extern byte moduleNameLen;
extern content_t rContent;
extern publics_t rPublics;
#define rReloc	(*(reloc_t *)&rPublics)	//rReloc RELOC_T at[&rPublics];
extern interseg_t rInterseg;
extern extref_t rExtref;
extern modend_t rModend;
extern word wZERO;
extern bool inComment;
extern bool noOpsYet;
extern byte nameLen;
extern byte startSeg;
extern byte activeSeg;
extern bool inPublic;
extern bool inExtrn;
extern bool segDeclared[2];
extern byte alignTypes[4];
extern word externId;
extern word itemOffset;
extern bool badExtrn;
extern byte startDefined;	/* 0 or 1 */
extern word startOffset;
extern byte tokenIdx;
extern byte lineBuf[128];
extern pointer tokStart[9];
#define tokPtr	tokStart[0]
extern tokensymStk_t tokenSym;
extern byte tokenType[9];
extern byte tokenSize[9];
extern byte tokenAttr[9];
extern word tokenSymId[9];
extern pointer endLineBuf;
extern byte ifDepth;
extern bool skipIf[9];
extern bool inElse[9];
extern byte macroCondSP;
extern byte macroCondStk[17];
extern byte opSP;
extern byte opStack[17];
extern word_t accum[2];
#define accum1	accum[0].w
#define accum2	accum[1].w
#define accum1Lb accum[0].lb
#define accum1Hb accum[0].hb
#define accum2Lb accum[1].lb
#define accum2Hb accum[1].hb
extern byte acc1RelocFlags;
extern byte acc2RelocFlags;
extern bool hasVarRef;
extern byte acc1ValType;
extern byte acc2ValType;
extern word acc1RelocVal;
extern word acc2RelocVal;
extern byte curChar;
extern byte reget;
extern byte lookAhead;
extern tokensym_t *symTab[3];
extern tokensym_t *endSymTab[3];
extern pointer symHighMark;
extern pointer baseMacroTbl;
extern byte gotLabel;
extern byte name[6];
extern byte savName[6];
extern bool lhsUserSymbol;
extern bool rhsUserSymbol;
extern bool xRefPending;
extern byte passCnt;
extern bool createdUsrSym;
extern bool usrLookupIsID;
extern bool needsAbsValue;
extern word objfd;
extern word xreffd;
extern word infd;
extern word outfd;
extern word macrofd;
extern word statusIO;
extern word openStatus;  /* status of last open for Read */
extern byte asmErrCode;
extern bool spooledControl;
extern bool primaryValid;
extern byte tokI;
extern bool errorOnLine;
extern bool atStartLine;
extern byte curCol;
extern pointer endItem;
extern pointer startItem;
extern word pageLineCnt;
extern address effectiveAddr;
extern word pageCnt;
extern bool showAddr;
extern bool lineNumberEmitted;
extern bool b68AE;
extern byte tokStr[7];
extern word sizeInBuf;
extern byte inBuf[IN_BUF_SIZE];
extern byte outbuf[OUT_BUF_SIZE+1];
extern pointer outP;
extern pointer endOutBuf;
extern byte objFile[15];
extern byte lstFile[15];
extern byte asxrefTmp[];
extern byte asxref[];
extern byte asmacRef[];
extern word srcLineCnt;
extern byte asciiLineNo[4];
extern byte spIdx;
extern byte lastErrorLine[4];
extern controls_t controls;
extern bool ctlListChanged;
extern byte titleLen;
extern bool controlSeen[12];
extern byte saveStack[8][3];
extern byte saveIdx;
extern byte titleStr[64];
extern byte tokBufLen;
extern byte tokType;
extern byte controlId;
extern byte tokBuf[64];
extern byte tokBufIdx;
extern word tokNumVal;
extern bool isControlLine;
extern bool scanCmdLine;
extern bool inDB;
extern bool inDW;
extern bool inExpression;
extern bool has16bitOperand;
extern byte phase;
extern byte curOpFlags;
extern byte yyType;
extern byte newOp;
extern byte topOp;
extern bool b6B2C;
extern byte nextTokType;
extern bool finished;
extern bool inNestedParen;
extern bool expectingOperands;
extern bool expectingOpcode;
extern bool condAsmSeen;    /* true when IF, ELSE, ENDIF seen [also macro to check] */
extern bool b6B33;
extern bool isInstr;
extern bool expectOp;
extern bool b6B36;
extern word segLocation[5];
extern word maxSegSize[3];
extern byte cmdLineBuf[129];
extern address actRead;
extern word errCnt;
extern pointer w6BCE;
extern word azero;
extern pointer cmdchP;
extern pointer controlsP;
extern bool skipRuntimeError;
extern bool nestedMacroSeen;
extern byte ii;
extern byte jj;
extern byte kk;
extern pointer curFileNameP;

extern address aVar;

extern word controlFileType;     /* 1->INCLUDE 2->PRINT, 3->OBJECT or MACROFILE */

// defined in keym.h
extern keyword_t extKeywords[151];

// defined in rdsrc.c

extern bool pendingInclude;
extern bool includeOnCmdLine;
extern byte fileIdx;
extern pointer endInBufP;
extern bool missingEnd;
extern word srcfd;
extern word rootfd;
extern pointer inChP;
extern pointer startLineP;
extern byte lineChCnt;
extern file_t files[6];
