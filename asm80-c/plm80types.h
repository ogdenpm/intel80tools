#pragma once
#include <stdbool.h>
#include <memory.h>
typedef unsigned char byte;
typedef unsigned short word;
typedef byte *pointer;
typedef word *apointer;

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
	byte type;
	word len;
	byte segid;
	word offset;
	byte dta[121];
	byte crc;
} content_t;

typedef struct {
	byte type;
	word len;
	byte crc;
} eof_t;

typedef struct {
	byte type;
	word len;
	byte dta[124];
	byte crc;
} extnames_t;

typedef struct {
	byte type;
	word len;
	byte hilo;
	word dta[30];
	byte crc;
} extref_t;

typedef struct {
	byte type;
	word len;
	byte segid;
	byte hilo;
	word dta[29];
	byte crc;
} interseg_t;

typedef struct {
	byte type;
	word len;
	byte modtyp;
	byte segid;
	word offset;
	byte crc;
} modend_t;

typedef struct {
	byte type;
	word len;
	byte dta[26];
} modhdr_t;

typedef struct {
	byte type;
	word len;
	byte segid;
	byte dta[124];
	byte crc;
} publics_t;

typedef struct {
	byte type;
	word len;
	byte hilo;
	word dta[62];
} reloc_t;

typedef struct {
	byte name[15];
	word blk, byt;
	byte b19;
} file_t;

typedef struct {
	byte condSP;
	byte ifDepth;
	byte mtype;
	byte b3;
	word w4;
	pointer bufP;
	word blk;
	word w10;
	pointer w12;
	word w14;
} macro_t;

typedef struct {
	word tok[2];
	union {
		struct {
			byte base;
			byte delta;
		};
		word line;
		word value;
	};
	byte type;
	byte flags;
} tokensym_t;

typedef struct {
	word tok[2];
	byte base;
	byte delta;
	byte type;
	byte flags;
} keyword_t;


typedef union {
	byte	all[20];
	struct {
		bool debug;
		bool macroDebug;
		bool xref;
		bool symbols;
		bool paging;
		bool tty;
		bool mod85;
		bool print;
		bool object;
		bool macroFile;
		byte pageWidth;
		byte pageLength;
		byte inlcude;
		bool title;
		byte save;
		byte restore;
		byte eject;
		/* SAVE/RESTORE act on next 3 opts */
		bool list;
		bool gen;
		bool cond;
	};
} controls_t;

typedef union {
	tokensym_t *stack[9];
	tokensym_t *curP;
} tokensymStk_t;

typedef union {
	macro_t stack[10];
	macro_t top;
} macroStk_t;
#pragma pack(pop)