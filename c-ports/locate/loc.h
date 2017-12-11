/*
 * vim:ts=4:shiftwidth=4:expandtab:
 */
#include <stdbool.h>
#include <string.h>
#include "error.h"
typedef unsigned char byte;
typedef unsigned short word;
typedef byte *pointer;
typedef word *wpointer;

// ISIS uses MemCk to return the top of memory
// this is used to calculate a size for MEMORY seg
// as MemCk returns the top of memory for other uses
// defined a MEMCK value to use in calculation
#define MEMCK	0xff00

// accessor macros for HIGH and LOW
// word cast is to avoid 64bit warnings
#define High(n) ((word)((n) >> 8))
#define Low(n)  ((word)((n) & 0xff))

#pragma pack(push, 1)
typedef union {
	word w;
	struct {
		byte lb, hb;
	};
} word_t;

typedef union {
	word w;
	struct {
		byte lb, hb;
	};
	byte b[2];
	byte *bp;
	word *ap;
} address;

typedef struct {
    word saddr;
    word eaddr;
} dataFrag_t;

typedef struct {
    byte state;
    byte mpage;
} page1_t;

typedef struct {
    byte pageIdx;
    byte fileIdx;
} page2_t;

typedef struct {
    byte rectyp;
    word reclen;
    byte record[1];
} record_t;

typedef struct {
    byte flags;
    byte seg;
    word start;
    word len;
} segFrag_t;

typedef union {
    byte all[9];
    struct {
        byte start;
        byte stackSize;
        byte restart0;
        byte map;
        byte publics;
        byte symbols;
        byte lines;
        byte purge;
        byte name;
    };
} seen_t;

typedef struct {
	byte deviceId;
	byte name[6];
	byte ext[3];
	byte deviceType;
	byte driveType;
} spath_t;



// record fragments
typedef struct {
	word namIdx;
	word offset;
} extfix_t;

typedef struct {
	byte segId;
	word offset;
	byte dat[1];
} moddat_t;

typedef struct {
	byte modType;
	byte segId;
	word offset;
} modend_t;

typedef struct {
	word offset;
	word linNum;
} line_t;

typedef struct {
	word offset;
	byte name[1];
} def_t;

typedef struct {
	byte segId;
	byte name[1];
} comnam_t;

typedef struct {
	byte segId;
	word len;
	byte combine;
} segdef_t;
#pragma pack(pop)



/* Seek modes */
#define SEEKTELL 0
#define SEEKBCK 1
#define SEEKABS 2
#define SEEKFWD 3
#define SEEKEND 4

#define ERR2    2 /* ILLEGAL AFTN ARGUMENT */
#define ERR3    3 /* TOO MANY OPEN FILES */
#define ERR4    4 /* INCORRECTLY SPECIFIED FILE */
#define ERR5    5 /* UNRECOGNIZED DEVICE NAME */
#define ERR9    9 /* DISK DIRECTORY FULL */
#define ERR12   12 /* FILE IS ALREADY OPEN */
#define ERR13   13 /* NO SUCH FILE */
#define ERR14   14 /* WRITE PROTECTED */
#define ERR17   17 /* NOT A DISK FILE */
#define ERR19   19 /* ATTEMPTED SEEK ON NON-DISK FILE */
#define ERR20   20 /* ATTEMPTED BACK SEEK TOO FAR */
#define ERR21   21 /* CANT RESCAN */
#define ERR22   22 /* ILLEGAL ACCESS MODE TO OPEN */
#define ERR23   23 /* MISSING FILENAME */
#define ERR27   27 /* ILLEGAL SEEK COMMAND */
#define ERR28   28 /* MISSING EXTENSION */
#define ERR31   31 /* CANT SEEK ON WRITE ONLY FILE */
#define ERR32   32 /* CANT DELETE OPEN FILE */
#define ERR35   35 /* SEEK PAST EOF */
#define ERR203  203 /* INVALID SYNTAX */
#define ERR204  204 /* PREMATURE EOF */
#define ERR208  208 /* CHECKSUM ERROR */
#define ERR210  210 /* INSUFFICIENT MEMORY */
#define ERR211  211 /* RECORD TOO LONG */
#define ERR212  212 /* ILLEGAL RELO RECORD */
#define ERR213  213 /* FIXUP BOUNDS ERROR */
#define ERR218  218 /* ILLEGAL RECORD FORMAT */
#define ERR224  224 /* BAD RECORD SEQUENCE */
#define ERR225  225 /* INVALID NAME */
#define ERR226  226 /* NAME TOO LONG */
#define ERR227  227 /* LEFT PARENTHESIS EXPECTED */
#define ERR228  228 /* RIGHT PARENTHESIS EXPECTED */
#define ERR229  229 /* UNRECOGNIZED CONTROL */
#define ERR233  233 /* 'TO' EXPECTED */
#define ERR237  237 /* COMMON NOT FOUND */
#define ERR238  238 /* ILLEGAL STACK CONTENT RECORD */
#define ERR239  239 /* NO MODULE HEADER RECORD */
#define ERR240  240 /* PROGRAM EXCEEDS 64K */

/* Record types */
#define R_MODHDR 2
#define R_MODEND 4
#define R_MODDAT 6
#define R_LINNUM 8
#define R_MODEOF 0xE
#define R_ANCEST 0x10
#define R_LOCDEF 0x12
#define R_PUBDEF 0x16
#define R_EXTNAM 0x18
#define R_FIXEXT 0x20
#define R_FIXLOC 0x22
#define R_FIXSEG 0x24
#define R_LIBLOC 0x26
#define R_LIBNAM 0x28
#define R_LIBDIC 0x2A
#define R_LIBHDR 0x2C
#define R_COMDEF 0x2E

/* Segments */
#define SABS    0
#define SCODE   1
#define SDATA   2
#define SSTACK  3
#define SMEMORY 4
#define SRESERVED   5
#define SNAMED  6   /* through 254 */
#define SBLANK  255

/* Alignments  & Flags*/
#define AMASK   0xf
#define AABS    0
#define AUNKNOWN    0
#define AINPAGE 1
#define APAGE   2
#define ABYTE   3
#define FHASADDR    0x80
#define FWRAP0  0x40
#define FSEGSEEN    0x10

/* Fixup type */
#define FLOW    1
#define FHIGH   2
#define FBOTH   3


#define	LIBDICT	0x2A
#define	LIBHDR	0x2C
#define	LIBLOC	0x26
#define	NAMES	0x28
#define	OBJEND	4
#define	OBJHDR	2
#define	PUBLICS	0x16
#define	readOnly	1
#define	writeOnly	2

/* file open modes */
#define READ_MODE   1
#define WRITE_MODE  2
#define UPDATE_MODE 3

/* seek modes */
#define SEEKTELL    0
#define SEEKBACK    1
#define SEEKABS 2
#define SEEKFWD 3
#define SEEKEND 4
#define TRUE    0xff
#define FALSE   0

/* controls seen tags */
//#define seen.start   seen(0)
//#define seen.stackSize   seen(1)
//#define seen.restart0    seen(2)
//#define seen.map     seen(3)
//#define seen.publics seen(4)
//#define seen.symbols seen(5)
//#define seen.lines   seen(6)
//#define seen.purge   seen(7)
//#define seen.name  seen(8)

extern pointer MEMORY;

extern word actRead;
extern byte aRecordType[];
extern pointer baseMemImage;
extern pointer botHeap;
extern byte columns;
extern byte crlf[];
extern dataFrag_t *curDataFragP;
extern segFrag_t *curSegFragP;
extern pointer eiBufP;
extern pointer eoutP;
extern pointer epbufP;
extern pointer erecP;
extern byte havePagingFile;
extern pointer iBufP;
extern word inBlk;
extern pointer inbP;
extern word inByt;
extern byte inCRC;
extern byte inFileName[];
extern dataFrag_t inFragment;
extern pointer inP;
extern record_t *inRecordP;
extern byte isMain;
extern byte modhdrX1;
extern byte modhdrX2;
extern byte moduleName[];
extern dataFrag_t *nextDataFragP;
extern segFrag_t *nextSegFragP;
extern word npbuf;
extern byte nxtPageNo;
extern byte outFileName[];
extern pointer outP;
extern word outputfd;
extern bool outRealFile;
extern pointer outRecordP;
extern byte pageCacheSize;
extern byte pageIndexTmpFil;
extern page1_t *pageTab1P;
extern page2_t *pageTab2P;
extern pointer pbufP;
extern word printfd;
extern byte printFileName[];
extern word readfd;
extern word recLen;
extern word recNum;
extern byte roundRobinIndex;
extern seen_t seen;
extern word segBases[256];
extern byte segFlags[256];
extern byte segOrder[255];
extern word segSizes[256];
extern pointer sibufP;
extern pointer spbufP;
extern word startAddr;
extern word statusIO;
extern word tmpfd;
extern byte tmpFileName[];
extern pointer topDataFrags;
extern pointer topHeap;
extern bool usePrintBuf;
extern byte version[];
extern byte alin[];
extern byte aMod[];
extern byte aPub[];
extern byte aReadFromFile[];
extern byte aReferenceToUns[];
extern byte aSym[];
extern byte aSymbol[];
extern byte aSymbolTableOfM[];
extern byte aUnsatisfiedExt[];
extern byte aValueType[];
extern byte aWrittenToFile[];
extern byte curcol;
extern address curColumn;
extern pointer curListField;
extern byte loHiBoth;
extern record_t *lsoutP;
extern byte nameLen;
extern byte outSegType;
extern byte spc32[];
extern word unsatisfiedCnt;
extern word workingSegBase;
extern byte x5[];
extern byte a0LengthSegment[];
extern byte aAddresses[];
extern byte alignNames[];
extern byte aMemoryMapOfMod[];
extern byte aMemOverlap[];
extern byte aModuleIsNotAMa[];
extern byte aModuleStartAdd[];
extern byte aRestartControl[];
extern byte aStartControlIg[];
extern byte aStartStopLengt[];
extern byte segNames[];
extern byte aCommandTailErr[];
extern byte aInvokedBy[];
extern byte cin[];
extern pointer cmdP;
extern byte controls[];
extern byte cout[];
extern byte mdebug[];
extern byte mstar2[];
extern byte mto[];
extern byte mtoand[];
extern pointer scmdP;
extern byte signonMsg[];
extern spath_t spathInfo;
extern byte tmpFileInfo[];
extern byte aInpageSegment2[];
extern byte nxtSegOrder;
extern byte pad7935[];
extern byte segId;

void AddDataFrag(word saddr, word eaddr);
pointer AddrInCache(word addr);
void AddSegFrag(byte flags, byte seg, word start, word len);
void Alloc(word cnt);
pointer AllocNewPage(byte page);
void AnotherPage(byte page);
void BadRecordSeq();
void BinAsc(word number, byte base, byte pad, pointer bufp, byte ndigits);
void ChkRead(word cnt);
void Close(word conn, wpointer statusP);
void CmdErr(word err);
void ConAndPrint(pointer buf, word cnt);
void ConStrOut(pointer buf, word cnt);
void Delete(pointer pathP, wpointer statusP);
void EmitModDat(dataFrag_t *curDataFragP);
void EndRecord();
void ErrChkReport(word errCode, pointer file, bool errExit);
void Errmsg(word errCode);
void ErrNotADisk();
void Error(word ErrorNum);
void Exit();
void ExpectChar(byte ch, byte err);
void ExpectLP();
void ExpectRP();
void ExpectSlash();
void FatalErr(byte errCode);
void FixupBoundsChk(word addr);
void FlushOut();
void FlushPrintBuf();
void ForceSOL();
pointer GetCommonName(byte segid);
byte GetCommonSegId();
void GetFile();
void GetPstrName(pointer pstr);
void GetRecord();
void IllegalRecord();
void IllegalReloc();
void InitRecOut(byte rectyp);
void InitSegOrder();
void InsSegIdOrder(byte seg);
void LoadModdat(byte segId);
void LocateFile();
void MakeFullName(spath_t *pinfo, pointer pstr);
pointer MemCk();
void ObjSeek(word blk, word byt);
void Open(wpointer connP, pointer pathP, word access, word echo, wpointer statusP);
void PageOut(byte page, pointer bufp);
word ParseLPNumRP();
word ParseNumber(pointer *ppstr);
pointer PastAFN(pointer pch);
pointer PastFileName(pointer pch);
void PrintColumn(pointer field, pointer pstr);
void PrintCrLf();
void PrintListingHeader(pointer buf, word len);
void PrintMemoryMap();
void PrintString(pointer bufp, word cnt);
void ProcAncest();
void ProcArgsInit();
void ProcComdef();
void ProcDefs(byte list, pointer template);
void ProcessControls();
void ProcExtnam();
void ProcHdrAndComDef();
void ProcLinNum();
void ProcModdat();
void ProcModend();
void ProcModhdr();
void PStrcpy(pointer psrc, pointer pdst);
void Read(word conn, pointer buffP, word count, wpointer actualP, wpointer statusP);
void ReadCmdLine();
void Rescan(word conn, wpointer statusP);
void Seek(word conn, word mode, wpointer blockP, wpointer byteP, wpointer statusP);
void SeekOutFile(word mode, wpointer pblk, wpointer pbyt);
void SeekPagingFile(byte para);
byte SetWorkingSeg(byte seg);
void SkipNonArgChars(pointer pch);
pointer SkipSpc(pointer pch);
void Spath(pointer pathP, spath_t *infoP, wpointer statusP);
void Start();
bool Strequ(pointer pstr1, pointer pstr2, byte len);
void StrUpr(pointer pch);
byte ToUpper(byte ch);
void Write(word conn, pointer buffP, word count, wpointer statusP);
void WriteBytes(pointer bufP, word cnt);
