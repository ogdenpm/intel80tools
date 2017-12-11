#include <stdbool.h>
#include <string.h>
#include "error.h"
typedef unsigned char byte;
typedef unsigned short word;
typedef byte *pointer;
typedef word *wpointer;

#define High(n)	((n) >> 8)
#define Low(n)	((n) & 0xff)
#define Shr(v, n)	((word)(v) >> (n))
#define Shl(v, n)	((word)(v) << (n))
#define RorB(v, n)	((byte)(((v) * 0x101) >> (n)))
#define RolB(v, n)	((byte)(Ror(v, (8 - n)))
#define Move(s, d, c)	memcpy(d, s, c)

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

typedef struct _segfrag {
	struct _segfrag *link;
	word bot;
	word top;
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

typedef struct _symbol {
	struct _symbol *hashLink;
	byte flags;
	byte linkedSeg;
	union {
		word offsetOrSym;
		word len;
		word symId;
	};
	struct _symbol *nxtSymbol;
	byte name[1];
} symbol_t;

typedef struct _module {
	struct _module *link;
	word blk;
	word byt;
	symbol_t *symlist;
	word scode;
	word sdata;
	byte name[1];
} module_t;


typedef struct _library {
	struct _library *link;
	module_t *modList;
	byte publicsMode;
	byte hasModules;
	byte name[1];
} library_t;

typedef struct _reloc {
	struct _reloc *link;
	word offset;
} reloc_t;

typedef struct _fixup {
	struct _fixup *link;
	word typeAndSeg;
	reloc_t *relocList;
} fixup_t;

typedef struct _extfixup {
	struct _extfixup *link;
	word offset;
	word symId;
} extfixup_t;

// record fragments
typedef struct {
	word symId;
	word offset;
} extref_t;

typedef struct {
	byte segId;
	byte fixType;
} interseg_t;

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


typedef struct {
	byte combine;
	word lenOrLinkedSeg;
} comseginfo_t;

typedef struct {
	word blk;
	word byt;
} loc_t;
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
#define ERR219  219	/* Phase Error */
#define ERR220  220 /* No EOF */
#define ERR221  221 /* Segment too large */
#define ERR224  224 /* BAD RECORD SEQUENCE */
#define ERR225  225 /* INVALID NAME */
#define ERR226  226 /* NAME TOO LONG */
#define ERR227  227 /* LEFT PARENTHESIS EXPECTED */
#define ERR228  228 /* RIGHT PARENTHESIS EXPECTED */
#define ERR229  229 /* UNRECOGNIZED CONTROL */
#define ERR233  233 /* 'TO' EXPECTED */
#define ERR234  234 /* Duplicate file name */
#define ERR235  235 /* Not a library */
#define ERR236  236 /* Too many common segments */
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
#define ANONE	255
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

/* segment types */
#define SEG_ABS         0
#define SEG_CODE        1
#define SEG_DATA        2
#define SEG_STACK       3
#define SEG_MEMORY      4
#define SEG_RESERVE     5		/* reserved for future Intel use */
#define SEG_NAMCOM      6
#define SEG_BLANK       255

/* module type */
#define MT_NOTMAIN		0
#define MT_MAIN			1

/* flags */
#define F_ALNMASK       0xf
#define F_SCOPEMASK     0xf0
#define F_EXTERN        0x40
#define F_PUBLIC        0x80
// link.plm
word actRead;
byte alignType[];
pointer membot;
pointer bufP;
symbol_t *comdefInfoP;
byte CRLF[];
module_t *curModule;
library_t *curObjFile;
bool mapWanted;
byte DUMMYREC[];
pointer ebufP;
word modEndOffset;
byte modEndSegId;
pointer eoutP;
pointer erecP;
byte filePath[];
symbol_t *hashTab[];
symbol_t *headSegOrderLink;
symbol_t *headUnresolved;
word inBlk;
pointer inbP;
word inByt;
byte inCRC;
byte inFileName[];
pointer inP;
record_t *inRecordP;
byte linkTmpFile[];
word maxExternCnt;
byte outTranId;
byte outTranVn;
byte modEndModTyp;
byte outModuleName[];
word npbuf;
library_t *objFileHead;
pointer outP;
address pad$4565;
byte printFileName[];
word printFileNo;
word inFile;
byte recErrMsg[];
word recLen;
word recNum;
pointer sbufP;
word segLen[];
byte segmap[];
pointer soutP;
word statusIO;
symbol_t *symbolP;
word tmpfilefd;
word tofilefd;
byte toFileName[];
pointer topHeap;
word unresolved;
byte VERSION[];


// link3a.plm
byte controls[];

// linkov.plm
byte modName[];
byte msgrefin[];
record_t *outRecordP;
byte OVERLAYVERSION[];


pointer MEMORY;

void AddExtMap(symbol_t *symP);
void AddFileToInputList();
void BadRecordSeq();
void BinAsc(word number, byte base, byte pad, pointer bufp, byte ndigits);
void ChainUnresolved();
void CheckFile();
void ChkLP();
void ChkRead(word cnt);
void ChkRP();
void Close(word conn, wpointer statusP);
void CloseObjFile();
void ConOutStr(pointer pstr, word count);
void CreateFragment(byte seg, word bot, word top);
void CrStrUpper(pointer PCH);
pointer Deblank(pointer pch);
void Delete(pointer pathP, wpointer statusP);
pointer Delimit(pointer pch);
void EmitANCESTOR();
void EmitCOMDEF();
void EmitEnding();
void EmitEXTNAMES();
void EmitMODHDR();
void EmitPUBLICS();
void EndRecord();
void ErrNotDiscFile();
void Error(word ErrorNum);
void Exit();
void ExpectChar(byte ch, byte errCode);
void ExpectComma();
void ExpectType(byte type);
bool ExtendRec(word cnt);
void FatalCmdLineErr(word errCode);
void FatalErr(byte errCode);
void FileError(word errCode, pointer file, bool errExit);
void FlushTo();
pointer GetHigh(word count);
void GetInputListItem();
pointer GetLow(word count);
void GetModuleName(pointer pstr);
void GetRecord();
symbol_t *GetSymbolP(word symId);
byte HashF(pointer pstr);
void IllegalRelo();
void IllFmt();
void InitExternsMap();
void InitRecord(byte type);
//void Load(address pathP, address LoadOffset, address switch, address entryP, address statusP);
bool Lookup(pointer pstr, symbol_t **pitemRef, byte mask);
void MakeFullName(spath_t *pinfo, pointer pstr);
pointer MemCk();
//void MemMov(address cnt, address srcp, address dstp);
void Open(wpointer connP, pointer pathP, word access, word echo, wpointer statusP);
void OpenObjFile();
void P1CommonSegments();
void P1LibScan();
void P1LibUserModules();
void P1ModEnd();
void P1ModHdr();
void P1Records(byte newModule);
void P1StdSegments();
void PageInExtMap();
void PageOutExtMap();
void ParseCmdLine();
void ParseControl();
void Pass1COMDEF();
void Pass1CONTENT();
void Pass1EXTNAMES();
void Pass1PUBNAMES();
void Pass2ANCESTOR();
void Pass2COMDEF();
void Pass2CONTENT();
void Pass2EXTNAMES();
void Pass2LINENO();
void Pass2LOCALS();
void Pass2MODHDR();
void Phase1();
void Phase2();
void Position(word blk, word byt);
void PrimeRecord();
void Pstrcpy(pointer psrc, pointer pdst);
void Read(word conn, pointer buffP, word count, wpointer actualP, wpointer statusP);
void ReadBlkByt();
void ReadCmdLine();
byte ReadName();
void ReportError(word errCode);
void Rescan(word conn, wpointer statusP);
pointer ScanBlank(pointer pch);
void Seek(word conn, word mode, wpointer blockP, wpointer byteP, wpointer statusP);
void SeekExtMap();
byte SelectInSeg(byte seg);
byte SelectOutSeg(byte seg);
void SkipNonArgChars(pointer arg1w);
void SkipRecord();
void Spath(pointer pathP, spath_t *infoP, wpointer statusP);
void Start();
bool Strequ(pointer pstr1, pointer pstr2, byte len);
void WAEFnAndMod(pointer buffP, word count);
void Write(word conn, pointer buffP, word count, wpointer statusP);
void WriteAndEcho(pointer buffP, word count);
void WriteBaseSizeAlign(word baddr, word bsize, byte align);
void WriteBytes(pointer bufP, word count);
void WriteCRLF();
void WriteStats();
