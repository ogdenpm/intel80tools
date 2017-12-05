#include <stdbool.h>
#include "error.h"

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned short address;
typedef byte *pointer;
typedef word *wpointer;

extern pointer MEMORY;

#pragma pack(push, 1)
typedef struct _module {
	struct _module *link;
	word blk;
	word byt;
	byte found;
	byte name[1];
} module_t;

typedef struct _arg {
	struct _arg *link;
	module_t *modules;
	pointer name;
} arg_t;

typedef struct _file {
	struct _file *link;
	pointer pathP;
	byte isopen;
	byte conn;
} file_t;

typedef struct {
	word modCnt;
	word idxBlk;
	word idxByte;
} libhdr_t;

typedef struct {
	byte crc;
	byte type;
	word len;
	word bytesLeft;
	word curBlk;
	word curByte;
	word posBlk;
	word posByte;
} librec_t;

typedef struct _line {
	struct _line *next;
	word len;
	byte text[1];
} line_t;

typedef struct _symbol {
	struct _symbol *link;
	pointer curCmd;
	word moduleCnt;
	byte symbol[1];
} symbol_t;

typedef struct {
	byte deviceId;
	byte name[6];
	byte ext[3];
	byte deviceType;
	byte driveType;
} spath_t;


#pragma pack(pop)
#define	CR	0xd
#define	ENDFILE	0xE
#define	ERR203	203
#define	ERR204	204
#define	ERR208	208
#define	ERR224	224
#define	ERR210	210
#define	ERR217	217
#define	ERR218	218
#define	ERR225	225
#define	ERR226	226
#define	ERR228	228
#define	ERR230	230
#define	ERR232	232
#define	ERR233	233
#define	ERR235	235
#define	LF	0xa
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

extern word actual;
extern pointer argFilename;
extern librec_t curRec;
extern file_t *fileHead;
extern file_t *fileP;
extern symbol_t *hashTable[];
extern pointer heapTop;
extern word inConn;
extern bool inModuleList;
extern pointer lookAheadP;
extern pointer memTop;
extern pointer tokenP;
extern byte tokLen;
extern word status;

void AcceptRecord(byte type);
pointer AllocUp(word cnt);
void Close(word conn, wpointer statusP);
void CloseFile(word conn, wpointer statusP);
void Delete(pointer pathP, wpointer statusP);
void DeleteFile(pointer pathP, wpointer statusP);
void Error(word errorNum);
void Exit();
void Fatal(byte err);
void FileStatusChk(word errCode, pointer pathP, bool isFatal);
void GetCmd();
void GetName(pointer buf);
void GetRecordBytes(word count, pointer bufP);
void GetToken();
__declspec(noreturn) void LibError(byte err);
void LibSeek(byte modeB, wpointer blkByteP);
void Log(pointer buf, word cnt);
void LogCRLF(pointer buf, word cnt); 
bool LookupSymbol(pointer modNameP, symbol_t **hashP);
bool MatchLookAhead(pointer chaP, byte len);
bool MatchToken(pointer chaP, byte len); 
pointer MemCk();
void Open(wpointer connP, pointer pathP, word access, word echo, wpointer statusP);
void OpenFile(wpointer connP, pointer pathP, word access, word echo, wpointer statusP);
void OpenLib();
pointer PastFileName(pointer chP);
void PrepRecord();
void Read(word conn, pointer buffP, word count, wpointer actualP, wpointer statusP);
void ReadChkCrc();
void ReadFile(word conn, pointer buffP, word count, wpointer actualP, wpointer statusP);
void Rename(pointer oldP, pointer newP, wpointer statusP);
void RenameFile(pointer oldP, pointer newP, wpointer statusP);
void RestoreCurPos();
void SaveCurPos();
void Seek(word conn, word mode, wpointer blockP, wpointer byteP, wpointer statusP);
void SeekFile(word conn, word mode, wpointer blockP, wpointer byteP, wpointer statusP);
void SkipCurRec();
void SkipModule();
pointer SkipSpc(pointer chP);
void Start();
void SwapCurSavedRec();
void Write(word conn, pointer buffP, word count, wpointer statusP);
void WriteFile(word conn, pointer buffP, word count, wpointer statusP);
void WriteErrStr(word err);
