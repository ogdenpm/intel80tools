// vim:ts=4:expandtab:shiftwidth=4:
/* character classes */
enum {CC_BAD = 0, CC_WS, CC_SEMI, CC_COLON, CC_CR, CC_PUN, CC_DOLLAR,
    CC_QUOTE, CC_DIG, CC_LET, CC_MAC, CC_ESC};

#define TT_ID   0
#define TT_NUM  2
#define TT_STR  4

#define CR  '\r'
#define ESC 0x1B
#define FF  '\f'
#define LF  '\n'
#define TAB '\t'
#define MACROEOB    0xFE
#define MACROPARAM	0x80

#define	MAXSYMSIZE	6

/* op types */
enum {T_VALUE = 0, T_CR, T_LPAREN, T_RPAREN, T_STAR, T_PLUS, T_COMMA,
    T_MINUS, T_UPLUS, T_SLASH, T_UMINUS};

enum {
O_ID = 1, O_TARGET, O_LABEL, O_SET, O_EQU, O_REF, TT_EQU, TT_SET,
O_NAME = 9, O_STRING, O_DATA, O_NUMBER, O_PARAM
};
enum {K_SPECIAL = 5, K_REGNAME = 7, K_SP,
    K_EQ = 11, K_LT, K_LE, K_GT, K_GE, K_NE,
    K_NOT, K_AND, K_OR, K_XOR,
    K_MOD, K_SHL, K_SHR, K_HIGH, K_LOW,
    K_DB, K_DW, K_DS, K_EQU, K_SET,
    K_ORG, K_END, K_IF, K_ELSE, K_ENDIF,
    K_LXI, K_REG16, K_LDSTAX, K_ARITH, K_IMM8,
    K_MVI, K_INRDCR, K_MOV, K_IMM16, K_SINGLE,
    K_RST, K_ASEG, K_CSEG, K_DSEG, K_PUBLIC,
    K_EXTRN, K_NAME, K_STKLN, K_MACRO, O_MACROPARAM,
    K_ENDM, K_EXITM, T_MACRONAME, K_IRP, K_IRPC,
    O_ITERPARAM, K_REPT, K_LOCAL, O_NULVAL, K_NUL,
    O_SETEQUNAME = 100
};

/* yyTypes */
enum {
    Y_CR = 1, Y_LPAREN, Y_RPAREN, Y_STAR, Y_PLUS, Y_COMMA, Y_MINUS, Y_UPLUS, Y_SLASH, Y_UMINUS,
    Y_EQ = 11, Y_LT, Y_LE, Y_GT, Y_GE, Y_NE,
    Y_NOT, Y_AND, Y_OR, Y_XOR,
    Y_MOD, Y_SHL, Y_SHR, Y_HIGH, Y_LOW,
    Y_DB, Y_DW, Y_DS, Y_EQU, Y_SET,
    Y_ORG, Y_END, Y_IF, Y_ELSE, Y_ENDIF,
    Y_LXI, Y_REG16, Y_LDSTAX, Y_ARITH, Y_IMM8,
    Y_MVI, Y_INRDCR, Y_MOV, Y_IMM16, Y_SINGLE,
    Y_RST, Y_ASEG, Y_CSEG, Y_DSEG, Y_PUBLIC,
    Y_EXTRN, Y_NAME, Y_STKLN, Y_MACRO, Y_MACROPARAM,
    Y_ENDM, Y_EXITM, Y_MACRONAME, Y_IRP, Y_IRPC,
    Y_ITERPARAM, Y_REPT, Y_LOCAL, Y_NULVAL, Y_NUL,
};

/* relocatable record types */
#define OMF_MODHDR  2
#define OMF_MODEND  4
#define OMF_CONTENT 6
#define OMF_EOF     0xe
#define OMF_LOCALS  0x12
#define OMF_PUBLICS 0x16
#define OMF_EXTNAMES    0x18
#define OMF_EXTREF  0x20
#define OMF_RELOC   0x22
#define OMF_INTERSEG    0x24

/* seek modes */
#define SEEKTELL    0
#define SEEKBACK    1
#define SEEKABS 2
#define SEEKFWD 3
#define SEEKEND 4

/* segments */
#define SEG_ABS 0
#define SEG_CODE    1
#define SEG_DATA    2
#define SEG_STACK   3
#define SEG_MEMORY  4


/* stack table Ids */
#define TID_KEYWORD 0
#define TID_SYMBOL  1
#define TID_MACRO   2


#define UF_SEGMASK  7
#define UF_RLOW  8
#define UF_RHIGH 0x10
#define UF_RBOTH 0x18
#define UF_PUBLIC   0x20
#define UF_EXTRN    0x40

/* file open modes */
#define READ_MODE   1
#define WRITE_MODE  2
#define UPDATE_MODE 3

#define XREF_DEF    0
#define XREF_REF    1
#define XREF_FIN    2

#define M_MACRO		0
#define M_IRP		1
#define M_IRPC		2
#define M_REPT		3
#define M_INVOKE	4
#define M_BODY		5


/* runtime errors */
#define	RTE_STACK	0
#define	RTE_TABLE	1
#define RTE_CMDLINE	2
#define RTE_EOF		3
#define	RTE_FILE	4

/* label usage */
#define L_SETEQU      1 
#define L_TARGET      2 
#define L_REF         0