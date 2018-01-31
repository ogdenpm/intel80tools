// $Id: plm.hpp,v 1.1 2003/10/04 21:08:48 Mark Ogden Exp $
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#pragma pack(push, 1)
typedef	unsigned char byte;
typedef unsigned short word;

#ifdef _DEBUG
void copyFile(char *src, char *dst);
#endif

const char ISISEOF = (char) 0x81;

struct cmdLine_t;
struct info_t;
struct symbol_t;
class symbol_pt;

class address {
    static byte *memory;
    word loc;
protected:
    byte *getaddr() { return memory + loc; }
    void setloc(int n) { loc = n; }
    void setloc(address n) { loc = n.getloc(); }
    void setaddr(void *p) { loc = (byte *)p - memory; }
    word getloc() { return loc; }
public:
    address(int n=0) : loc(n) {}
    address(void *p) : loc((byte *)p - memory) {}
    address& operator =(int n) { loc = n; return *this; }
    address& operator =(address n) { loc = n.getloc(); return *this;}
    address& operator =(void *p) { loc = (byte *)p - memory; return *this;}
    operator word() { return loc; }
    operator byte *() { return memory + loc; }
    operator char *() { return (char *)(memory + loc); }
    operator void *() { return (void *)(memory + loc); }
    operator word *() { return (word *)(memory + loc); }
    word * operator &() { return &loc; }
    address operator +(word n) { return loc + n; }
    address operator +(int n) { return loc + n; }
    address& operator +=(word n) { loc += n; return *this; }
    address& operator -=(word n) { loc -= n; return *this; }
    word operator -(address n) { return loc - n.getloc(); }
    address operator -(int n) { return loc - n; }
    address operator -(word n) { return loc - n; }
    address operator --(int n) { loc--; return loc + 1; }
    operator symbol_pt *() { return (symbol_pt *) (memory + loc); }
    bool operator==(address n) { return loc == n.getloc(); }
    bool operator<(address n) { return loc < n.getloc(); }
    bool operator<=(address n) { return loc <= n.getloc(); }
    bool operator<(int n) { return loc < n; }
    
};


class cmdLine_pt : public address {
public:
    cmdLine_t *operator ->() { return (cmdLine_t *)getaddr(); }
    cmdLine_pt operator =(cmdLine_pt n) { setloc(n.getloc()); return *this; }
    bool operator !=(int n) { return getloc() != n; }
    cmdLine_pt operator =(int n) { setloc(n); return *this; }
    bool operator==(int n) { return getloc() == n; }
};

class info_pt : public address {
public:
    info_pt(int n=0) : address(n) {}
    info_pt(address n) { setloc(n); }
    info_pt operator =(int n) { setloc(n); return *this; }
    info_pt operator =(address n) { setloc(word(n)); return *this; }
    info_t *operator ->() { return (info_t *)getaddr(); }
    bool operator !=(address n) { return getloc() != word(n); }
    bool operator !=(int n) { return getloc() != n; }
    bool operator==(int n) { return getloc() == n; }
    bool operator==(word n) { return getloc() == n; }
    bool operator>=(address n) { return getloc() >= word(n); }
    bool operator>=(word n) { return getloc() >= n; }


};

class symbol_pt : public address {
public:
    symbol_pt(int n=0) : address(n) {}
    symbol_pt& operator =(int n) { setloc(n); return *this; }
    bool operator !=(symbol_pt n) { return getloc() != n.getloc(); }
    bool operator !=(int n) { return getloc() != n; }
    symbol_t *operator ->() { return (symbol_t *)getaddr(); }
    bool operator ==(symbol_pt n) { return getloc() == n.getloc(); }
    bool operator ==(int n) { return getloc() == n; }
    operator symbol_pt *() { return (symbol_pt *) this; }
};



struct cmdLine_t {
	cmdLine_pt link;
	byte	len;
	char	text[1];
};



struct info_t {
	byte	len;				// 0
	byte	type;				// 1
	word	symbolOffset;		// 2
	word	infoScope;			// 4
	union {						// 6
		word	nextInfoOffset;
		word	addr;
	};
    union {						// 8
	    byte	flags[3];
        word litaddr;       // can't use address here because its in a union
		struct {
			byte builtinId;
			byte builtinParamCnt;
			byte builtinDataType;
		};
		byte cflag;				// used for simple controls
    };
	byte	externId;			// 11
	word	dimension;			// 12
	word	basedOffset;
	union {						// 14
		word parentOffset;
		word size;
	};
	byte	dataType;			// 18
	byte	intrNo;				// 19
	byte	paramCnt;			// 20
	byte	procID;				// 21
};


struct symbol_t {
    symbol_pt link;   //	symbol_t *link;
    info_pt infoChain; // info_pt infoChain;
	byte	name[1];
};



struct loc_t {
	word	block;
	word	byte;
};


struct control_t {
	byte tokenId;
	byte primary;
	byte controlId;
	byte controlVal;
	byte PrimaryId;
};

struct macroStk_t {
	char *inChr_p;
	info_pt info_p;
};

enum ISIS { OPEN=0, CLOSE, DELETE, READ, WRITE, SEEK, LOAD, RENAME,
	    CONSOL, EXIT, ATTRIB, RESCAN, ERROR, WHOCON, SPATH};

enum { F_PUBLIC=0, F_EXTERNAL, F_BASED, F_INITIAL, F_REENTRANT, F_DATA,
	F_INTERRUPT, F_AT, F_ARRAY, F_STARDIM, F_PARAMETER, F_MEMBER,
	F_LABEL, F_AUTOMATIC, F_PACKED, F_ABSOLUTE, F_MEMORY, F_DECLARED, F_DEFINED, F_MODGOTO};

enum ERRORS {
ERR1 = 1,// INVALID PL/M-80 CHARACTER
ERR2,	// UNPRINTABLE ASCII CHARACTER
ERR3,	// IDENTIFIER, STRING, OR NUMBER TOO LONG, TRUNCATED
ERR4,	// ILLEGAL NUMERIC CONSTANT TYPE
ERR5,	// INVALID CHARACTER IN NUMERIC CONSTANT
ERR6,	// ILLEGAL MACRO REFERENCE, RECURSIVE EXPANSION
ERR7,	// LIMIT EXCEEDED: MACROS NESTED TOO DEEPLY
ERR8,	// INVALID CONTROL FORMAT
ERR9,	// INVALID CONTROL
ERR10,	// ILLEGAL USE OF PRIMARY CONTROL AFTER NON-CONTROL LINE
ERR11,	// MISSING CONTROL PARAMETER
ERR12,	// INVALID CONTROL PARAMETER
ERR13,	// LIMIT EXCEEDED: INCLUDE NESTING
ERR14,	// INVALID CONTROL FORMAT, INCLUDE NOT LAST CONTROL
ERR15,	// MISSING INCLUDE CONTROL PARAMETER
ERR16,	// ILLEGAL PRINT CONTROL
ERR17,	// INVALID PATH-NAME
ERR18,	// INVALID MULTIPLE LABELS AS MODULE NAMES
ERR19,	// INVALID LABEL IN MODULE WITHOUT MAIN PROGRAM
ERR20,	// MISMATCHED IDENTIFIER AT END OF BLOCK
ERR21,	// MISSING PROCEDURE NAME
ERR22,	// INVALID MULTIPLE LABELS AS PROCEDURE NAMES
ERR23,	// INVALID LABELLED END IN EXTERNAL PROCEDURE
ERR24,	// INVALID STATEMENT IN EXTERNAL PROCEDURE
ERR25,	// UNDECLARED PARAMETER
ERR26,	// INVALID DECLARATION, STATEMENT OUT OF PLACE
ERR27,	// LIMIT EXCEEDED: NUMBER OF DO BLOCKS
ERR28,	// MISSING 'THEN'
ERR29,	// ILLEGAL STATEMENT
ERR30,	// LIMIT EXCEEDED: NUMBER OF LABELS ON STATEMENT
ERR31,	// LIMIT EXCEEDED: PROGRAM TOO COMPLEX
ERR32,	// INVALID SYNTAX, TEXT IGNORED UNTIL ';'
ERR33,	// DUPLICATE LABEL DECLARATION
ERR34,	// DUPLICATE PROCEDURE DECLARATION
ERR35,	// LIMIT EXCEEDED: NUMBER OF PROCEDURES
ERR36,	// MISSING PARAMETER
ERR37,	// MISSING ') ' AT END OF PARAMETER LIST
ERR38,	// DUPLICATE PARAMETER NAME
ERR39,	// INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL
ERR40,	// DUPLICATE ATTRIBUTE
ERR41,	// CONFLICTING ATTRIBUTE
ERR42,	// INVALID INTERRUPT VALUE
ERR43,	// MISSING INTERRUPT VALUE
ERR44,	// ILLEGAL ATTRIBUTE, 'INTERRUPT' WITH PARAMETERS
ERR45,	// ILLEGAL ATTRIBUTE, 'INTERRUPT' WITH TYPED PROCEDURE
ERR46,	// ILLEGAL USE OF LABEL
ERR47,	// MISSING ') ' AT END OF FACTORED DECLARATION
ERR48,	// ILLEGAL DECLARATION STATEMENT SYNTAX
ERR49,	// LIMIT EXCEEDED: NUMBER OF ITEMS IN FACTORED DECLARE
ERR50,	// INVALID ATTRIBUTES FOR BASE
ERR51,	// INVALID BASE, SUBSCRIPTING ILLEGAL
ERR52,	// INVALID BASE, MEMBER OF BASED STRUCTURE OR ARRAY OF STRUCTURES
ERR53,	// INVALID STRUCTURE MEMBER IN BASE
ERR54,	// UNDECLARED BASE
ERR55,	// UNDECLARED STRUCTURE MEMBER IN BASE
ERR56,	// INVALID MACRO TEXT, NOT A STRING CONSTANT
ERR57,	// INVALID DIMENSION, ZERO ILLEGAL
ERR58,	// INVALID STAR DIMENSION IN FACTORED DECLARATION
ERR59,	// ILLEGAL DIMENSION ATTRIBUTE
ERR60,	// MISSING ') ' AT END OF DIMENSION
ERR61,	// MISSING TYPE
ERR62,	// INVALID STAR DIMENSION WITH 'STRUCTURE' OR 'EXTERNAL'
ERR63,	// INVALID DIMENSION WITH THIS ATTRIBUTE
ERR64,	// MISSING STRUCTURE MEMBERS
ERR65,	// MISSING ') ' AT END OF STRUCTURE MEMBER LIST
ERR66,	// INVALID STRUCTURE MEMBER, NOT AN IDENTIFIER
ERR67,	// DUPLICATE STRUCTURE MEMBER NAME
ERR68,	// LIMIT EXCEEDED: NUMBER OF STRUCTURE MEMBERS
ERR69,	// INVALID STAR DIMENSION WITH STRUCTURE MEMBER
ERR70,	// INVALID MEMBER TYPE, 'STRUCTURE' ILLEGAL
ERR71,	// INVALID MEMBER TYPE, 'LABEL' ILLEGAL
ERR72,	// MISSING TYPE FOR STRUCTURE MEMBER
ERR73,	// INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL
ERR74,	// INVALID STAR DIMENSION, NOT WITH 'DATA' OR 'INITIAL'
ERR75,	// MISSING ARGUMENT OF 'AT' , 'DATA' , OR 'INITIAL'
ERR76,	// CONFLICTING ATTRIBUTE WITH PARAMETER
ERR77,	// INVALID PARAMETER DECLARATION, BASE ILLEGAL
ERR78,	// DUPLICATE DECLARATION
ERR79,	// ILLEGAL PARAMETER TYPE, NOT BYTE OR ADDRESS
ERR80,	// INVALID DECLARATION, LABEL MAY NOT BE BASED
ERR81,	// CONFLICTING ATTRIBUTE WITH 'BASE'
ERR82,	// INVALID SYNTAX, MISMATCHED '('
ERR83,	// LIMIT EXCEEDED: DYNAMIC STORAGE
ERR84,	// LIMIT EXCEEDED: BLOCK NESTING
ERR85,	// LONG STRING ASSUMED CLOSED AT NEXT SEMICOLON OR QUOTE
ERR86,	// LIMIT EXCEEDED: SOURCE LINE LENGTH
ERR87,	// MISSING 'END' , END-OF-FILE ENCOUNTERED
ERR88,	// INVALID PROCEDURE NESTING, ILLEGAL IN REENTRANT PROCEDURE
ERR89,	// MISSING 'DO' FOR MODULE
ERR90,	// MISSING NAME FOR MODULE
ERR91,	// ILLEGAL PAGELENGTH CONTROL VALUE
ERR92,	// ILLEGAL PAGEWIDTH CONTROL VALUE
ERR93,	// MISSING 'DO' FOR 'END' , 'END' IGNORED
ERR94,	// ILLEGAL CONSTANT, VALUE > 65535
ERR95,	// ILLEGAL RESPECIFICATION OF PRIMARY CONTROL IGNORED
ERR96,	// COMPILER ERROR: SCOPE STACK UNDERFLOW
ERR97,	// COMPILER ERROR: PARSE STACK UNDERFLOW
ERR98,	// INCLUDE FILE IS NOT A DISKETTE FILE
ERR99,	// ?? unused
ERR100,	// INVALID STRING CONSTANT IN EXPRESSION
ERR101,	// INVALID ITEM FOLLOWS DOT OPERATOR
ERR102,	// MISSING PRIMARY OPERAND
ERR103,	// MISSING ') ' AT END OF SUBEXPRESSION
ERR104,	// ILLEGAL PROCEDURE INVOCATION WITH DOT OPERATOR
ERR105,	// UNDECLARED IDENTIFIER
ERR106,	// INVALID INPUT/OUTPUT PORT NUMBER
ERR107,	// ILLEGAL INPUT/OUTPUT PORT NUMBER, NOT NUMERIC CONSTANT
ERR108,	// MISSING ') ' AFTER INPUT/OUTPUT PORT NUMBER
ERR109,	// MISSING INPUT/OUTPUT PORT NUMBER
ERR110,	// INVALID LEFT OPERAND OF QUALIFICATION, NOT A STRUCTURE
ERR111,	// INVALID RIGHT OPERAND OF QUALIFICATION, NOT IDENTIFIER
ERR112,	// UNDECLARED STRUCTURE MEMBER
ERR113,	// MISSING ') ' AT END OF ARGUMENT LIST
ERR114,	// INVALID SUBSCRIPT, MULTIPLE SUBSCRIPTS ILLEGAL
ERR115,	// MISSING ') ' AT END OF SUBSCRIPT
ERR116,	// MISSING '=' IN ASSIGNMENT STATEMENT
ERR117,	// MISSING PROCEDURE NAME IN CALL STATEMENT
ERR118,	// INVALID INDIRECT CALL, IDENTIFIER NOT AN ADDRESS SCALAR
ERR119,	// LIMIT EXCEEDED: PROGRAM TOO COMPLEX
ERR120,	// LIMIT EXCEEDED: EXPRESSION TOO COMPLEX
ERR121,	// LIMIT EXCEEDED: EXPRESSION TOO COMPLEX
ERR122,	// LIMIT EXCEEDED: PROGRAM TOO COMPLEX
ERR123,	// INVALID DOT OPERAND, BUILT-IN PROCEDURE ILLEGAL
ERR124,	// MISSING ARGUMENTS FOR BUILT-IN PROCEDURE
ERR125,	// ILLEGAL ARGUMENT FOR BUILT-IN PROCEDURE
ERR126,	// MISSING ') ' AFTER BUILT-IN PROCEDURE ARGUMENT LIST
ERR127,	// INVALID SUBSCRIPT ON NON-ARRAY
ERR128,	// INVALID LEFT-HAND OPERAND OF ASSIGNMENT
ERR129,	// ILLEGAL 'CALL' WITH TYPED PROCEDURE
ERR130,	// ILLEGAL REFERENCE TO OUTPUT FUNCTION
ERR131,	// ILLEGAL REFERENCE TO UNTYPED PROCEDURE
ERR132,	// ILLEGAL USE OF LABEL
ERR133,	// ILLEGAL REFERENCE TO UNSUBSCRIPTED ARRAY
ERR134,	// ILLEGAL REFERENCE TO UNSUBSCRIPTED MEMBER ARRAY
ERR135,	// ILLEGAL REFERENCE TO AN UNQUALIFIED STRUCTURE
ERR136,	// INVALID RETURN FOR UNTYPED PROCEDURE, VALUE ILLEGAL
ERR137,	// MISSING VALUE IN RETURN FOR TYPED PROCEDURE
ERR138,	// MISSING INDEX VARIABLE
ERR139,	// INVALID INDEX VARIABLE TYPE, NOT BYTE OR ADDRESS
ERR140,	// MISSING '=' FOLLOWING INDEX VARIABLE
ERR141,	// MISSING 'TO' CLAUSE
ERR142,	// MISSING IDENTIFIER FOLLOWING GOTO
ERR143,	// INVALID REFERENCE FOLLOWING GOTO, NOT A LABEL
ERR144,	// INVALID GOTO LABEL, NOT AT LOCAL OR MODULE LEVEL
ERR145,	// MISSING 'TO' FOLLOWING 'GO'
ERR146,	// MISSING ') ' AFTER 'AT' RESTRICTED EXPRESSION
ERR147,	// MISSING IDENTIFIER FOLLOWING DOT OPERATOR
ERR148,	// INVALID QUALIFICATION IN RESTRICTED REFERENCE
ERR149,	// INVALID SUBSCRIPTING IN RESTRICTED REFERENCE
ERR150,	// MISSING ') ' AT END OF RESTRICTED SUBSCRIPT
ERR151,	// INVALID OPERAND IN RESTRICTED EXPRESSION
ERR152,	// MISSING ') ' AFTER CONSTANT LIST
ERR153,	// INVALID NUMBER OF ARGUMENTS IN CALL, TOO MANY
ERR154,	// INVALID NUMBER OF ARGUMENTS IN CALL, TOO FEW
ERR155,	// INVALID RETURN IN MAIN PROGRAM
ERR156,	// MISSING RETURN STATEMENT IN TYPED PROCEDURE
ERR157,	// INVALID ARGUMENT, ARRAY REQUIRED FOR LENGTH OR LAST
ERR158,	// INVALID DOT OPERAND, LABEL ILLEGAL
ERR159,	// COMPILER ERROR: PARSE STACK UNDERFLOW
ERR160,	// COMPILER ERROR: OPERAND STACK UNDERFLOW
ERR161,	// COMPILER ERROR: ILLEGAL OPERAND STACK EXCHANGE
ERR162,	// COMPILER ERROR: OPERATOR STACK UNDERFLOW
ERR163,	// COMPILER ERROR: GENERATION FAILURE
ERR164,	// COMPILER ERROR: SCOPE STACK OVERFLOW
ERR165,	// COMPILER ERROR: SCOPE STACK UNDERFLOW
ERR166,	// COMPILER ERROR: CONTROL STACK OVERFLOW
ERR167,	// COMPILER ERROR: CONTROL STACK UNDERFLOW
ERR168,	// COMPILER ERROR: BRANCH MISSING IN 'IF' STATEMENT
ERR169,	// ILLEGAL FORWARD CALL
ERR170,	// ILLEGAL RECURSIVE CALL
ERR171,	// INVALID USE OF DELIMITER OR RESERVED WORD IN EXPRESSION
ERR172,	// INVALID LABEL: UNDEFINED
ERR173,	// INVALID LEFT SIDE OF ASSIGNMENT: VARIABLE DECLARED WITH DATA ATTRIBUTE
ERR174,	// INVALID NULL PROCEDURE
ERR175,	// unused
ERR176,	// INVALID INTVECTOR INTERVAL VALUE
ERR177,	// INVALID INTVECTOR LOCATION VALUE
ERR178,	// INVALID 'AT' RESTRICTED REFERENCE, EXTERNAL ATTRIBUTE CONFLICTS WITH PUBLIC ATTRIBUTE
ERR179,	// OUTER 'IF' MAY NOT HAVE AN 'ELSE' PART
ERR180,	// MISSING OR INVALID CONDITIONAL COMPILATION PARAMETER
ERR181,	// MISSING OR INVALID CONDITIONAL COMPILATION CONSTANT
ERR182,	// MISPLACED ELSE OR ELSEIF OPTION
ERR183,	// MISPLACED ENDIF OPTION
ERR184,	// CONDITIONAL COMPILATION PARAMETER NAME TOO LONG
ERR185,	// MISSING OPERATOR IN CONDITIONAL COMPILATION EXPRESSION
ERR186,	// INVALID CONDITIONAL COMPILATION CONSTANT, TOO LARGE
ERR187,	// LIMIT EXCEEDED: NUMBER OF SAVE LEVELS > 5
ERR188,	// MISPLACED RESTORE OPTION
ERR189,	// NULL STRING NOT ALLOWED
ERR200,	// LIMIT EXCEEDED: STATEMENT SIZE
ERR201,	// INVALID DO CASE BLOCK, AT LEAST ONE CASE REQUIRED
ERR202,	// LIMIT EXCEEDED: NUMBER OF ACTIVE CASES
ERR203,	// LIMIT EXCEEDED: NESTING OF TYPED PROCEDURE CALLS
ERR204,	// LIMIT EXCEEDED: NUMBER OF ACTIVE PROCEDURES AND DO CASE GROUPS
ERR205,	// ILLEGAL NESTING OF BLOCKS, ENDS NOT BALANCED
ERR206,	// LIMIT EXCEEDED: CODE SEGMENT SIZE
ERR207,	// LIMIT EXCEEDED: SEGMENT SIZE
ERR208,	// LIMIT EXCEEDED: STRUCTURE SIZE
ERR209,	// ILLEGAL INITIALIZATION OF MORE SPACE THAN DECLARED
ERR210,	// ILLEGAL INITIALIZATION OF A BYTE TO A VALUE > 255
ERR211,	// INVALID IDENTIFIER IN 'AT' RESTRICTED REFERENCE
ERR212,	// INVALID RESTRICTED REFERENCE IN 'AT' , BASE ILLEGAL
ERR213,	// UNDEFINED RESTRICTED REFERENCE IN 'AT'
ERR214,	// COMPILER ERROR: OPERAND CANNOT BE TRANSFORMED
ERR215,	// COMPILER ERROR: EOF READ IN FINAL ASSEMBLY
ERR216,	// COMPILER ERROR: BAD LABEL ADDRESS
ERR217,	// ILLEGAL INITIALIZATION OF AN EXTERNAL VARIABLE
ERR218,	// ILLEGAL SUCCESSIVE USES OF RELATIONAL OPERATORS
ERR219	// LIMIT EXCEEDED: NUMBER OF EXTERNALS > 255
};

enum TOKENS {
T_VARIABLE=0, T_NUMBER, T_STRING, T_PLUSSIGN,
T_MINUSSIGN, T_STAR, T_SLASH, T_MOD,		// 4
T_PLUS, T_MINUS, T_AND, T_OR,			// 8
T_XOR, T_NOT, T_LT=15,				// 12
T_LE, T_EQ, T_NE, T_GE,				// 16
T_GT, T_COLON_EQUALS, T_COLON, T_SEMICOLON,	// 20
T_PERIOD, T_LPAREN, T_RPAREN, T_COMMA,		// 24
T_CALL,		// start2 - 28
T_DECLARE,	// start3
T_DISABLE,	// start4
T_DO,		// start5
T_ENABLE,	// start6 - 32
T_END,		// start7
T_GO,		// start8
T_GOTO,		// start9
T_HALT,		// start10 - 36
T_IF,		// start11
T_PROCEDURE,	// start12
T_RETURN,	// start13
T_ADDRESS, T_AT, T_BASED, T_BYTE,		// 40
T_DATA, T_EXTERNAL, T_INITIAL, T_INTERRUPT,	// 44
T_LABEL, T_LITERALLY, T_PUBLIC, T_REENTRANT,	// 48
T_STRUCTURE, T_BY, T_CASE, T_ELSE,		// 52
T_EOF, T_THEN, T_TO, T_WHILE			// 56
};

enum LEXITEMS {
L_LINEINFO, L_SYNTAXERROR, L_TOKENERROR, L_LIST, // 0
L_NOLIST, L_CODE, L_NOCODE, L_EJECT,
L_INCLUDE, L_STMTCNT, L_LABELDEF, L_LOCALLABEL,
L_JMP, L_JMPFALSE, L_PROCEDURE, L_SCOPE,
L_END, L_DO, L_DOLOOP, L_WHILE,			// 10
L_CASE, L_CASELABEL, L_IF, L_STATEMENT,
L_CALL, L_RETURN, L_GO, L_GOTO,
L_SEMICOLON, L_ENABLE, L_DISABLE, L_HALT,
L_EOF, L_AT, L_INITIAL, L_DATA,			// 20
L_VARIABLE, L_NUMBER, L_STRING, L_PLUSSIGN,
L_MINUSSIGN, L_PLUS, L_MINUS, L_STAR,
L_SLASH, L_MOD, L_COLONEQUALS, L_AND,
L_OR, L_XOR, L_NOT, L_LT,			// 30
L_LE, L_EQ, L_NE, L_GE,
L_GT, L_COMMA, L_LPAREN, L_RPAREN,
L_PERIOD, L_TO, L_BY, L_INVALID,
L_MODULE, L_XREFINFO, L_XREF, L_EXTERNAL,	// 40
L_ERROR
};


enum /*T2CODES*/ {
    T2_LT, T2_LE, T2_NE, T2_EQ, T2_GE, T2_GT, T2_ROL, T2_ROR,
    T2_SCL, T2_SCR, T2_SHL, T2_SHR, T2_JMPFALSE, T2_13, T2_14, T2_15,
    T2_16, T2_17, T2_DOUBLE, T2_ADDB, T2_MINUSSIGN, T2_STAR, T2_SLASH, T2_MOD,
    T2_AND, T2_OR, T2_XOR, T2_BASED, T2_BYTEINDEX, T2_WORDINDEX, T2_MEMBER, T2_UNARYMINUS,
    T2_NOT, T2_LOW, T2_HIGH, T2_ADDRESSOF, T2_PLUS, T2_MINUS, T2_38, T2_39,
    T2_40, T2_41, T2_42, T2_43, T2_44, T2_45, T2_46, T2_47,
    T2_48, T2_49, T2_50, T2_51, T2_52, T2_53, T2_54, T2_55,
    T2_56, T2_TIME, T2_STKBARG, T2_STKWARG, T2_DEC, T2_STORE, T2_OUTPUT, T2_63,
    T2_STKARG, T2_65, T2_66, T2_67, T2_68, T2_MOVE, T2_70, T2_RETURNBYTE,
    T2_RETURNWORD, T2_RETURN, T2_74, T2_75, T2_76, T2_77, T2_78, T2_79,
    T2_80, T2_81, T2_82, T2_83, T2_84, T2_85, T2_86, T2_87,
    T2_88, T2_89, T2_90, T2_91, T2_92, T2_93, T2_94, T2_95,
    T2_96, T2_97, T2_98, T2_99, T2_100, T2_101, T2_102, T2_103,
    T2_104, T2_105, T2_106, T2_107, T2_108, T2_109, T2_110, T2_111,
    T2_112, T2_113, T2_114, T2_115, T2_116, T2_117, T2_118, T2_119,
    T2_120, T2_121, T2_122, T2_123, T2_124, T2_125, T2_126, T2_127,
    T2_128, T2_129, T2_ADDW, T2_BEGMOVE, T2_CALL, T2_CALLVAR, T2_134, T2_PROCEDURE,
    T2_LOCALLABEL, T2_CASELABEL, T2_LABELDEF,T2_INPUT , T2_GO_TO, T2_JMP, T2_JNC, T2_JNZ,
    T2_SIGN, T2_ZERO, T2_PARITY, T2_CARRY, T2_DISABLE, T2_ENABLE, T2_HALT, T2_STMTCNT,
    T2_LINEINFO, T2_MODULE, T2_SYNTAXERROR, T2_TOKENERROR, T2_EOF, T2_LIST, T2_NOLIST, T2_CODE,
    T2_NOCODE, T2_EJECT, T2_INCLUDE, T2_ERROR, T2_164, T2_165, T2_166, T2_167,
    T2_168, T2_169, T2_170, T2_171, T2_VARIABLE, T2_NUMBER, T2_BIGNUMBER, T2_175,
    T2_176, T2_177, T2_178, T2_179, T2_180, T2_STACKPTR, T2_SEMICOLON, T2_OPTBACKREF,
    T2_CASE, T2_ENDCASE, T2_ENDPROC, T2_LENGTH, T2_LAST, T2_SIZE, T2_BEGCALL, T2_191,
    T2_192, T2_193, T2_194, T2_195, T2_196, T2_197, T2_198, T2_199,
    T2_200, T2_201, T2_202, T2_203, T2_204, T2_205, T2_206, T2_207,
    T2_208, T2_209, T2_210, T2_211, T2_212, T2_213, T2_214, T2_215,
    T2_216, T2_217, T2_218, T2_219, T2_220, T2_221, T2_222, T2_223,
    T2_224, T2_225, T2_226, T2_227, T2_228, T2_229, T2_230, T2_231,
    T2_232, T2_233, T2_234, T2_235, T2_236, T2_237, T2_238, T2_239,
    T2_240, T2_241, T2_242, T2_243, T2_244, T2_245, T2_246, T2_247,
    T2_248, T2_249, T2_250, T2_251, T2_252, T2_253, T2_254
}; 



enum /*Code Frags*/ {
    CF_0, CF_1, CF_2, CF_3, CF_POP, CF_XTHL, CF_6, CF_7,
    CF_SCL, CF_SCR, CF_SHL, CF_SHR, CF_JMPFALSE, CF_13, CF_XCHG, CF_15,
    CF_MOVRPM, CF_17, CF_MOVLRM, CF_MOVMRPR, CF_MOVMLR, CF_DW, CF_SPHL, CF_PUSH,
    CF_INX, CF_DCX, CF_DCXH, CF_RET, CF_BYTEINDEX, CF_WORDINDEX, CF_MEMBER, CF_UNARYMINUS,
    CF_NOT, CF_LOW, CF_HIGH, CF_ADDRESSOF, CF_PLUS, CF_MINUS, CF_38, CF_39,
    CF_40, CF_41, CF_42, CF_43, CF_44, CF_45, CF_46, CF_47,
    CF_48, CF_49, CF_50, CF_51, CF_52, CF_53, CF_54, CF_55,
    CF_56, CF_57, CF_58, CF_SHLD, CF_STA, CF_61, CF_MOVMRP, CF_CALLADDR,
    CF_STKARG, CF_65, CF_66, CF_67, CF_68, CF_MOVE, CF_70, CF_RETURNBYTE,
    CF_RETURNWORD, CF_RETURN, CF_74, CF_75, CF_76, CF_77, CF_78, CF_79,
    CF_80, CF_81, CF_82, CF_83, CF_84, CF_85, CF_86, CF_87,
    CF_88, CF_89, CF_90, CF_91, CF_92, CF_93, CF_94, CF_95,
    CF_96, CF_DELAY, CF_98, CF_99, CF_100, CF_101, CF_102, CF_MOVE_HL,
    CF_104, CF_105, CF_106, CF_107, CF_108, CF_109, CF_MOVLRHR, CF_111,
    CF_112, CF_MOVHRLR, CF_MOVHRM, CF_MOVMHR, CF_INXSP, CF_DCXSP, CF_JMPTRUE, CF_119,
    CF_120, CF_121, CF_122, CF_123, CF_124, CF_125, CF_126, CF_127,
    CF_128, CF_129, CF_ADDW, CF_BEGMOVE, CF_CALL, CF_CALLVAR, CF_134, CF_PROCEDURE,
    CF_136, CF_CASELABEL, CF_LABELDEF,CF_INPUT , CF_GO_TO, CF_JMP, CF_JNC, CF_JNZ,
    CF_144, CF_ZERO, CF_PARITY, CF_CARRY, CF_DISABLE, CF_EI, CF_HALT, CF_STMTCNT,
    CF_152, CF_MODULE, CF_SYNTAXERROR, CF_TOKENERROR, CF_EOF, CF_LIST, CF_NOLIST, CF_CODE,
    CF_160, CF_EJECT, CF_INCLUDE, CF_ERROR, CF_164, CF_165, CF_166, CF_167,
    CF_168, CF_169, CF_170, CF_171, CF_VARIABLE, CF_NUMBER, CF_174, CF_175,
    CF_176, CF_177, CF_178, CF_179, CF_180, CF_STACKPTR, CF_SEMICOLON, CF_OPTBACKREF,
    CF_CASE, CF_ENDCASE, CF_ENDPROC, CF_LENGTH, CF_LAST, CF_SIZE, CF_BEGCALL, CF_191,
    CF_192, CF_193, CF_194, CF_195, CF_196, CF_197, CF_198, CF_199,
    CF_200, CF_201, CF_202, CF_203, CF_204, CF_205, CF_206, CF_207,
    CF_208, CF_209, CF_210, CF_211, CF_212, CF_213, CF_214, CF_215,
    CF_216, CF_217, CF_218, CF_219, CF_220, CF_221, CF_222, CF_223,
    CF_224, CF_225, CF_226, CF_227, CF_228, CF_229, CF_230, CF_231,
    CF_232, CF_233, CF_234, CF_235, CF_236, CF_237, CF_238, CF_239,
    CF_240, CF_241, CF_242, CF_243, CF_244, CF_245, CF_246, CF_247,
    CF_248, CF_249, CF_250, CF_251, CF_252, CF_253, CF_254
}; 


enum ICODES {
I_STRING=0, I_VARIABLE, I_NUMBER, I_PLUSSIGN, I_MINUSSIGN, I_PLUS, I_MINUS,
I_STAR, I_SLASH, I_MOD, I_AND, I_OR, I_XOR, I_NOT, I_LT, I_LE,
I_EQ, I_NE, I_GE, I_GT, I_ADDRESSOF,
I_UNARYMINUS, I_STACKPTR, I_INPUT, I_OUTPUT, I_CALL, I_CALLVAR,
I_BYTEINDEX, I_WORDINDEX, I_COLONEQUALS, I_MEMBER, I_BASED,
I_CARRY, I_DEC, I_DOUBLE, I_HIGH, I_LAST, I_LENGTH, I_LOW,
I_MOVE, I_PARITY, I_ROL, I_ROR, I_SCL, I_SCR, I_SHL, I_SHR, I_SIGN,
I_SIZE, I_TIME, I_ZERO
};

// 0 -> Literally
// 1 -> Label
// 2 -> Byte
// 3 -> Address
// 4 -> Structure
// 5 -> Procedure
// 6 -> BuiltIn
// 7 -> Macro (being expanded)
// 9 -> temp

enum /*TYPES*/ { LIT_T = 0, LABEL_T, BYTE_T, ADDRESS_T, STRUCT_T, PROC_T, BUILTIN_T,
MACRO_T, UNK_T, TEMP_T };


enum /* AT file types */ { AT_AHDR = 0, AT_DHDR, AT_2, AT_STRING, AT_DATA, AT_END, AT_EOF};

typedef struct {
	byte type;
	word info_p;
	word stmtNum;
	word varInfoOffset;
	word varArrayIndex;
	word varNestedArrayIndex;
	word val;
} atFData_t;	




void Close(FILE *conn, word * status_p);
void exit();
// void FILL(word, address , byte);
#define FILL(cnt, loc, val) memset(loc, val, cnt)

void Load(char *path_p, word load_offset, word sw, word * entry_p, word * status_p);
//void movemem(word, void *, void *);

#define movemem(cnt, src, dest) memmove(dest, src, cnt)

void Open(FILE ** conn_p, char *path_p, word access, word echo, word * status_p);
void Read(FILE *conn, void *buf_p, word count, word * actual_p, word * status_p);
//void reboot();
void Rescan(word conn, word *status_p);
void Write(FILE *conn, void *buff_p, word count, word * status_p);
void Delete(char *path, word *status_p);
//byte strncmp(char *, char *, byte);

struct srcFiletbl_t{
    char filename[16];
    word block;
    word byte;
};

struct file_t {
    FILE *aftn;
    char shortName[6];
    char fullName[16];
    byte *bufptr;
    word bufSize;
    word actual;
    word curoff;
};

extern int gargc;
extern char **gargv;


extern address topMem;
extern address botMem;
extern address botInfo;
extern address topInfo;
extern address topSymbol;
extern symbol_pt botSymbol;
extern symbol_pt curSymbol_p;
extern info_pt curInfo_p;
extern word offNextCmdChM1;
extern word LEFTMARGIN;
extern word localLabelCnt;
extern word srcFileIdx;
extern symbol_pt *hashChains_p;
extern word blockDepth;
extern address localLabels_p;
extern address word_381E;
extern address helpers_p;
extern address word_3822;
extern word linesRead;
extern word programErrCnt;
extern word procCnt;
extern word word_382A;
extern word uninitDataSize;
extern word initDataSize;
extern word objBlk;
extern word objByte;
 
extern srcFiletbl_t srcFileTable[6];
 
extern file_t srcFile, lstFile, objFile, conFile, tx1File;
extern file_t tx2File, atFile, nmsFile, xrfFile, ixiFile;
 
extern word procChains[35];
extern word procInfo[255];
extern word blk1Used;
extern word blk2Used;
extern address word_3C34;
extern word blkSize1;
extern word blkSize2;
extern byte srcStemLen;
extern byte byte_3C3B;
extern byte IXREFSet;
extern byte PRINTSet;
extern byte OBJECTSet;
extern byte debugFlag;
extern byte unexpectedEOF;
extern byte haveModule;
extern byte fatalErrorCode;
extern byte pad3C43;
extern address word_3C44;
extern byte CONTROLS[];
#define PRINT	CONTROLS[0]
#define XREF	CONTROLS[1]
#define SYMBOLS	CONTROLS[2]
#define DEBUG	CONTROLS[3]
#define PAGING	CONTROLS[4]
#define OBJECT	CONTROLS[5]
#define OPTIMIZE	CONTROLS[6]
#define IXREF	CONTROLS[7]


extern byte pad_3C4E[2];
extern byte srcStemName[10];
extern byte debugSwitches[26];
extern cmdLine_pt cmdLine_p;
extern cmdLine_pt startCmdLine_p;
//extern char aF0Plm80_ov1[/*15*/];
//extern char aF0Plm80_ov2[/*15*/];
//extern char aF0Plm80_ov3[/*15*/];
//extern char aF0Plm80_ov4[/*15*/];
//extern char aF0Plm80_ov5[/*15*/];
extern char ixiFileName[15];
extern char lstFileName[15];
extern char objFileName[15];
extern word pageNo;
extern char byte_3CF2;
extern char *lstBufPtr;     // 3CF3
extern word lstChCnt;
extern word lstBufSize;

extern byte lstFileOpen;	// 3CF9
extern byte linesLeft;
extern byte byte_3CFB;
extern byte byte_3CFC;
extern byte byte_3CFD;
extern byte col;
extern byte skipCnt;
extern byte tabWidth;
extern byte TITLELEN;
extern byte PAGELEN;
extern word PAGEWIDTH;
extern byte listingMargin;
extern char DATE[9];
extern char aPlm80Compiler[/*20*/];
extern char TITLE[60];
extern word ISISVECTOR;
extern word REBOOTVECTOR;

extern byte intVecNum;
extern word intVecLoc;
extern byte hasErrors;
//extern char aOverlay6[/*10*/];
//extern char a_ov6[/*5*/];
extern char version[/*5*/];
extern char invokeName[/*10*/];
//extern char a_ov0[/*5*/];
extern byte pad_3DAC[];
extern char aC197619771982I[/*31*/];

int overlay0();
int overlay1();
int overlay2();
int overlay3();
int overlay4();
int overlay5();
int overlay6();

void fatalError_main(byte errcode);
void fatalError_ov0(byte errcode);
void fatalError_ov1(byte errcode);
void fatalError_ov6(byte errcode);

typedef struct {
	byte type;
	word len;
	byte data[1];
} rec;

void recAddWord(rec *arg1w, byte arg2b, word arg3w);
void recAddByte(rec *arg1w, byte arg2b, byte arg3b);
void writeRec(rec *arg1w, byte arg2b);


// useful inline functions
inline word info2Off(word val) { return val - botInfo; } 
inline address off2Info(word val) { return botInfo + val; }


void printExitMsg();
void putcLst(byte ch);
void putnstrLst(char *arg1w, word arg2b);
void newLineLst();
void newLineLeaderLst();
void xputcLst(byte arg1b);
void xputnstrLst(char *arg1w, byte arg2w);
void xputstr2cLst(char *arg1w, byte arg2b);
void xnumLst(word arg1w, byte arg2b, byte arg3b);
void tabLst(byte arg1b);
void newPageNextChLst();
void put2cLst(word arg1w);
void newPageLst();
void setSkipLst(byte arg1b);
void lstModuleInfo();
struct _linfo {
    byte itemType; // 0
    word lineCnt;
    word stmtCnt;
    word blkCnt;
};
extern struct _linfo linfo;
#pragma pack(pop)
