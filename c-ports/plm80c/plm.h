#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <setjmp.h>
#include "error.h"

typedef unsigned char byte;
typedef unsigned short word;
typedef byte *pointer;
typedef word *wpointer;
typedef word offset_t;

#define High(n)	((n) >> 8)
#define Low(n)	((n) & 0xff)
#define Ror(v, n)	(((v) >> n) | ((v) << (8 - n)))
#define Rol(v, n)	(((v) << n) | (((v) >> (8 - n)))
#define Move(s, d, c)	memcpy(d, s, c)
#define Length(str) (sizeof(str) - 1)


/* isis command codes */
enum {IOPEN = 0, ICLOSE = 1, IDELETE = 2, IREAD = 3, IWRITE = 4,
    ISEEK = 5, ILOAD = 6, IRENAME = 7, ICONSOL = 8, IEXIT = 9,
    IATTRIB = 10, IRESCAN = 11, IERROR = 12, IWHOCON = 13, ISPATH = 14};

#define FILE_NAME_LEN   15

#define TAB 9
#define CR  '\r'
#define LF  '\n'
#define QUOTE   '\''
#define ISISEOF 0x81

/* flags */
enum {F_PUBLIC = 0, F_EXTERNAL = 1, F_BASED = 2, F_INITIAL = 3, F_REENTRANT = 4,
    F_DATA = 5, F_INTERRUPT = 6, F_AT = 7, F_ARRAY = 8, F_STARDIM = 9,
    F_PARAMETER = 10, F_MEMBER = 11, F_LABEL = 12, F_AUTOMATIC = 13, F_PACKED = 14,
    F_ABSOLUTE = 15, F_MEMORY = 16, F_DECLARED = 17, F_DEFINED = 18, F_MODGOTO = 19};

/* token info types */
enum {LIT_T = 0, LABEL_T = 1, BYTE_T = 2, ADDRESS_T = 3, STRUCT_T = 4,
    PROC_T = 5, BUILTIN_T = 6, MACRO_T = 7, UNK_T = 8, CONDVAR_T = 9};

/* Lex tokens */
enum {L_LINEINFO = 0, L_SYNTAXERROR = 1, L_TOKENERROR = 2, L_LIST = 3, L_NOLIST = 4,
    L_CODE = 5, L_NOCODE = 6, L_EJECT = 7, L_INCLUDE = 8, L_STMTCNT = 9,
    L_LABELDEF = 10, L_LOCALLABEL = 11, L_JMP = 12, L_JMPFALSE = 13, L_PROCEDURE = 14,
    L_SCOPE = 15, L_END = 16, L_DO = 17, L_DOLOOP = 18, L_WHILE = 19,
    L_CASE = 20, L_CASELABEL = 21, L_IF = 22, L_STATEMENT = 23, L_CALL = 24,
    L_RETURN = 25, L_GO = 26, L_GOTO = 27, L_SEMICOLON = 28, L_ENABLE = 29,
    L_DISABLE = 30, L_HALT = 31, L_EOF = 32, L_AT = 33, L_INITIAL = 34,
    L_DATA = 35, L_IDENTIFIER = 36, L_NUMBER = 37, L_STRING = 38, L_PLUSSIGN = 39,
    L_MINUSSIGN = 40, L_PLUS = 41, L_MINUS = 42, L_STAR = 43, L_SLASH = 44,
    L_MOD = 45, L_COLONEQUALS = 46, L_AND = 47, L_OR = 48, L_XOR = 49,
    L_NOT = 50, L_LT = 51  , L_LE = 52, L_EQ = 53, L_NE = 54,
    L_GE = 55, L_GT = 56, L_COMMA = 57, L_LPAREN = 58, L_RPAREN = 59,
    L_PERIOD = 60, L_TO = 61, L_BY = 62, L_INVALID = 63, L_MODULE = 64,
    L_XREFUSE = 65, L_XREFDEF = 66, L_EXTERNAL = 67};

/* character classes */
enum {CC_BINDIGIT = 0, CC_OCTDIGIT = 1, CC_DECDIGIT = 2, CC_HEXCHAR = 3, CC_ALPHA = 4,
    CC_PLUS = 5, CC_MINUS = 6, CC_STAR = 7, CC_SLASH = 8, CC_LPAREN = 9,
    CC_RPAREN = 10, CC_COMMA = 11, CC_COLON = 12, CC_SEMICOLON = 13, CC_QUOTE = 14,
    CC_PERIOD = 15, CC_EQUALS = 16, CC_LESS = 17, CC_GREATER = 18, CC_WSPACE = 19,
    CC_DOLLAR = 20, CC_INVALID = 21, CC_NONPRINT = 22, CC_NEWLINE = 23};

/* intermediate tokens */
enum {T_IDENTIFIER = 0, T_NUMBER = 1, T_STRING = 2, T_PLUSSIGN = 3, T_MINUSSIGN = 4,
    T_STAR = 5, T_SLASH = 6, T_MOD = 7, T_PLUS = 8, T_MINUS = 9,
    T_AND = 10, T_OR = 11, T_XOR = 12, T_NOT = 13,
    T_LT = 15, T_LE = 16, T_EQ = 17, T_NE = 18, T_GE = 19,
    T_GT = 20, T_COLON_EQUALS = 21, T_COLON = 22, T_SEMICOLON = 23, T_PERIOD = 24,
    T_LPAREN = 25, T_RPAREN = 26, T_COMMA = 27, T_CALL = 28, T_DECLARE = 29,
    T_DISABLE = 30, T_DO = 31, T_ENABLE = 32, T_END = 33, T_GO = 34,
    T_GOTO = 35, T_HALT = 36, T_IF = 37, T_PROCEDURE = 38, T_RETURN = 39,
    T_ADDRESS = 40, T_AT = 41, T_BASED = 42, T_BYTE = 43, T_DATA = 44,
    T_EXTERNAL = 45, T_INITIAL = 46, T_INTERRUPT = 47, T_LABEL = 48, T_LITERALLY = 49,
    T_PUBLIC = 50, T_REENTRANT = 51, T_STRUCTURE = 52, T_BY = 53, T_CASE = 54,
    T_ELSE = 55, T_EOF = 56, T_THEN = 57, T_TO = 58, T_WHILE = 59};

/* start statement codes */
enum { S_IDENTIFIER = 0, S_SEMICOLON, S_CALL, S_DECLARE, S_DISABLE, S_DO,
       S_ENABLE, S_END, S_GO, S_GOTO, S_HALT, S_IF, S_PROCEDURE, S_RETURN};


/* T2 codes */
enum {T2_LT = 0, T2_LE = 1, T2_NE = 2, T2_EQ = 3, T2_GE = 4,
    T2_GT = 5, T2_ROL = 6, T2_ROR = 7, T2_SCL = 8, T2_SCR = 9,
    T2_SHL = 10, T2_SHR = 11, T2_JMPFALSE = 12,
    T2_DOUBLE = 18, T2_PLUSSIGN = 19,
    T2_MINUSSIGN = 20, T2_STAR = 21, T2_SLASH = 22, T2_MOD = 23, T2_AND = 24,
    T2_OR = 25, T2_XOR = 26, T2_BASED = 27, T2_BYTEINDEX = 28, T2_WORDINDEX = 29,
    T2_MEMBER = 30, T2_UNARYMINUS = 31, T2_NOT = 32, T2_LOW = 33, T2_HIGH = 34,
    T2_ADDRESSOF = 35, T2_PLUS = 36, T2_MINUS = 37,
    T2_44 = 44,
    T2_51 = 51,
    T2_56 = 56, T2_TIME = 57, T2_STKBARG = 58, T2_STKWARG = 59,
    T2_DEC = 60, T2_COLONEQUALS = 61, T2_OUTPUT = 62, T2_63 = 63, T2_STKARG = 64,
    T2_65 = 65, T2_MOVE = 69,
    T2_RETURNBYTE = 71, T2_RETURNWORD = 72, T2_RETURN = 73,
    T2_ADDW = 130, T2_BEGMOVE = 131, T2_CALL = 132, T2_CALLVAR = 133,
    T2_PROCEDURE = 135, T2_LOCALLABEL = 136, T2_CASELABEL = 137, T2_LABELDEF = 138, T2_INPUT = 139,
    T2_GOTO = 140, T2_JMP = 141, T2_JNC = 142, T2_JNZ = 143, T2_SIGN = 144,
    T2_ZERO = 145, T2_PARITY = 146, T2_CARRY = 147, T2_DISABLE = 148, T2_ENABLE = 149,
    T2_HALT = 150, T2_STMTCNT = 151, T2_LINEINFO = 152, T2_MODULE = 153, T2_SYNTAXERROR = 154,
    T2_TOKENERROR = 155, T2_EOF = 156, T2_LIST = 157, T2_NOLIST = 158, T2_CODE = 159,
    T2_NOCODE = 160, T2_EJECT = 161, T2_INCLUDE = 162, T2_ERROR = 163,
    T2_IDENTIFIER = 172, T2_NUMBER = 173, T2_BIGNUMBER = 174,
    T2_STACKPTR = 181, T2_SEMICOLON = 182, T2_OPTBACKREF = 183, T2_CASE = 184,
    T2_ENDCASE = 185, T2_ENDPROC = 186, T2_LENGTH = 187, T2_LAST = 188, T2_SIZE = 189,
    T2_BEGCALL = 190,
    T2_254 = 254};

/* ICodes */
enum {I_STRING = 0, I_IDENTIFIER = 1, I_NUMBER = 2, I_PLUSSIGN = 3, I_MINUSSIGN = 4,
    I_PLUS = 5, I_MINUS = 6, I_STAR = 7, I_SLASH = 8, I_MOD = 9,
    I_AND = 10, I_OR = 11, I_XOR = 12, I_NOT = 13, I_LT = 14,
    I_LE = 15, I_EQ = 16, I_NE = 17, I_GE = 18, I_GT = 19,
    I_ADDRESSOF = 20, I_UNARYMINUS = 21, I_STACKPTR = 22, I_INPUT = 23, I_OUTPUT = 24,
    I_CALL = 25, I_CALLVAR = 26, I_BYTEINDEX = 27, I_WORDINDEX = 28, I_COLONEQUALS = 29,
    I_MEMBER = 30, I_BASED = 31, I_CARRY = 32, I_DEC = 33, I_DOUBLE = 34,
    I_HIGH = 35, I_LAST = 36, I_LENGTH = 37, I_LOW = 38, I_MOVE = 39,
    I_PARITY = 40, I_ROL = 41, I_ROR = 42, I_SCL = 43, I_SCR = 44,
    I_SHL = 45, I_SHR = 46, I_SIGN = 47, I_SIZE = 48, I_TIME = 49,
    I_ZERO = 50};

/* AT Icodes */
enum {ATI_AHDR = 0, ATI_DHDR = 1, ATI_2 = 2, ATI_STRING = 3, ATI_DATA = 4,
    ATI_END = 5, ATI_EOF = 6};

/* CF codes */
enum {CF_3 = 3, CF_POP = 4,
    CF_XTHL = 5, CF_6 = 6, CF_7 = 7,
    CF_XCHG = 14,
    CF_MOVRPM = 16, CF_MOVLRM = 18, CF_MOVMRPR = 19,
    CF_MOVMLR = 20, CF_DW = 21, CF_SPHL = 22, CF_PUSH = 23, CF_INX = 24,
    CF_DCX = 25, CF_DCXH = 26, CF_RET = 27,
    CF_SHLD = 59,
    CF_STA = 60, CF_MOVMRP = 62,
    CF_67 = 67, CF_68 = 68,
    CF_DELAY = 97,
    CF_MOVE_HL = 103,
    CF_MOVLRHR = 110, CF_MOVHRLR = 113, CF_MOVHRM = 114,
    CF_MOVMHR = 115, CF_INXSP = 116, CF_DCXSP = 117, CF_JMPTRUE = 118,
    CF_134 = 134,
    CF_EI = 149,
    CF_171 = 171, CF_174 = 174};

enum { DO_PROC = 0, DO_LOOP = 1, DO_WHILE = 2, DO_CASE = 3 };

/* Error codes */
#define ERR1    1 /* INVALID PL/M-80 CHARACTER */
#define ERR2    2 /* UNPRINTABLE ASCII CHARACTER */
#define ERR3    3 /* IDENTIFIER, STRING, OR NUMBER TOO LONG, TRUNCATED */
#define ERR4    4 /* ILLEGAL NUMERIC CONSTANT TYPE */
#define ERR5    5 /* INVALID CHARACTER IN NUMERIC CONSTANT */
#define ERR6    6 /* ILLEGAL MACRO REFERENCE, RECURSIVE EXPANSION */
#define ERR7    7 /* LIMIT EXCEEDED: MACROS NESTED TOO DEEPLY */
#define ERR8    8 /* INVALID CONTROL FORMAT */
#define ERR9    9 /* INVALID CONTROL */
#define ERR10   10 /* ILLEGAL USE OF PRIMARY CONTROL AFTER NON-CONTROL LINE */
#define ERR11   11 /* MISSING CONTROL PARAMETER */
#define ERR12   12 /* INVALID CONTROL PARAMETER */
#define ERR13   13 /* LIMIT EXCEEDED: INCLUDE NESTING */
#define ERR14   14 /* INVALID CONTROL FORMAT, INCLUDE NOT LAST CONTROL */
#define ERR15   15 /* MISSING INCLUDE CONTROL PARAMETER */
#define ERR16   16 /* ILLEGAL PRINT CONTROL */
#define ERR17   17 /* INVALID PATH-NAME */
#define ERR18   18 /* INVALID MULTIPLE LABELS AS MODULE NAMES */
#define ERR19   19 /* INVALID LABEL IN MODULE WITHOUT MAIN PROGRAM */
#define ERR20   20 /* MISMATCHED IDENTIFIER AT END OF BLOCK */
#define ERR21   21 /* MISSING PROCEDURE NAME */
#define ERR22   22 /* INVALID MULTIPLE LABELS AS PROCEDURE NAMES */
#define ERR23   23 /* INVALID LABELLED END IN EXTERNAL PROCEDURE */
#define ERR24   24 /* INVALID STATEMENT IN EXTERNAL PROCEDURE */
#define ERR25   25 /* UNDECLARED PARAMETER */
#define ERR26   26 /* INVALID DECLARATION, STATEMENT OUT OF PLACE */
#define ERR27   27 /* LIMIT EXCEEDED: NUMBER OF DO BLOCKS */
#define ERR28   28 /* MISSING 'THEN' */
#define ERR29   29 /* ILLEGAL STATEMENT */
#define ERR30   30 /* LIMIT EXCEEDED: NUMBER OF LABELS ON STATEMENT */
#define ERR31   31 /* LIMIT EXCEEDED: PROGRAM TOO COMPLEX */
#define ERR32   32 /* INVALID SYNTAX, TEXT IGNORED UNTIL ';' */
#define ERR33   33 /* DUPLICATE LABEL DECLARATION */
#define ERR34   34 /* DUPLICATE PROCEDURE DECLARATION */
#define ERR35   35 /* LIMIT EXCEEDED: NUMBER OF PROCEDURES */
#define ERR36   36 /* MISSING PARAMETER */
#define ERR37   37 /* MISSING ') ' AT END OF PARAMETER LIST */
#define ERR38   38 /* DUPLICATE PARAMETER NAME */
#define ERR39   39 /* INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL */
#define ERR40   40 /* DUPLICATE ATTRIBUTE */
#define ERR41   41 /* CONFLICTING ATTRIBUTE */
#define ERR42   42 /* INVALID INTERRUPT VALUE */
#define ERR43   43 /* MISSING INTERRUPT VALUE */
#define ERR44   44 /* ILLEGAL ATTRIBUTE, 'INTERRUPT' WITH PARAMETERS */
#define ERR45   45 /* ILLEGAL ATTRIBUTE, 'INTERRUPT' WITH TYPED PROCEDURE */
#define ERR46   46 /* ILLEGAL USE OF LABEL */
#define ERR47   47 /* MISSING ') ' AT END OF FACTORED DECLARATION */
#define ERR48   48 /* ILLEGAL DECLARATION STATEMENT SYNTAX */
#define ERR49   49 /* LIMIT EXCEEDED: NUMBER OF ITEMS IN FACTORED DECLARE */
#define ERR50   50 /* INVALID ATTRIBUTES FOR BASE */
#define ERR51   51 /* INVALID BASE, SUBSCRIPTING ILLEGAL */
#define ERR52   52 /* INVALID BASE, MEMBER OF BASED STRUCTURE OR ARRAY OF STRUCTURES */
#define ERR53   53 /* INVALID STRUCTURE MEMBER IN BASE */
#define ERR54   54 /* UNDECLARED BASE */
#define ERR55   55 /* UNDECLARED STRUCTURE MEMBER IN BASE */
#define ERR56   56 /* INVALID MACRO TEXT, NOT A STRING CONSTANT */
#define ERR57   57 /* INVALID DIMENSION, ZERO ILLEGAL */
#define ERR58   58 /* INVALID STAR DIMENSION IN FACTORED DECLARATION */
#define ERR59   59 /* ILLEGAL DIMENSION ATTRIBUTE */
#define ERR60   60 /* MISSING ') ' AT END OF DIMENSION */
#define ERR61   61 /* MISSING TYPE */
#define ERR62   62 /* INVALID STAR DIMENSION WITH 'STRUCTURE' OR 'EXTERNAL' */
#define ERR63   63 /* INVALID DIMENSION WITH THIS ATTRIBUTE */
#define ERR64   64 /* MISSING STRUCTURE MEMBERS */
#define ERR65   65 /* MISSING ') ' AT END OF STRUCTURE MEMBER LIST */
#define ERR66   66 /* INVALID STRUCTURE MEMBER, NOT AN IDENTIFIER */
#define ERR67   67 /* DUPLICATE STRUCTURE MEMBER NAME */
#define ERR68   68 /* LIMIT EXCEEDED: NUMBER OF STRUCTURE MEMBERS */
#define ERR69   69 /* INVALID STAR DIMENSION WITH STRUCTURE MEMBER */
#define ERR70   70 /* INVALID MEMBER TYPE, 'STRUCTURE' ILLEGAL */
#define ERR71   71 /* INVALID MEMBER TYPE, 'LABEL' ILLEGAL */
#define ERR72   72 /* MISSING TYPE FOR STRUCTURE MEMBER */
#define ERR73   73 /* INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL */
#define ERR74   74 /* INVALID STAR DIMENSION, NOT WITH 'DATA' OR 'INITIAL' */
#define ERR75   75 /* MISSING ARGUMENT OF 'AT' , 'DATA' , OR 'INITIAL' */
#define ERR76   76 /* CONFLICTING ATTRIBUTE WITH PARAMETER */
#define ERR77   77 /* INVALID PARAMETER DECLARATION, BASE ILLEGAL */
#define ERR78   78 /* DUPLICATE DECLARATION */
#define ERR79   79 /* ILLEGAL PARAMETER TYPE, NOT BYTE OR ADDRESS */
#define ERR80   80 /* INVALID DECLARATION, LABEL MAY NOT BE BASED */
#define ERR81   81 /* CONFLICTING ATTRIBUTE WITH 'BASE' */
#define ERR82   82 /* INVALID SYNTAX, MISMATCHED '(' */
#define ERR83   83 /* LIMIT EXCEEDED: DYNAMIC STORAGE */
#define ERR84   84 /* LIMIT EXCEEDED: BLOCK NESTING */
#define ERR85   85 /* LONG STRING ASSUMED CLOSED AT NEXT SEMICOLON OR QUOTE */
#define ERR86   86 /* LIMIT EXCEEDED: SOURCE LINE LENGTH */
#define ERR87   87 /* MISSING 'END' , END-OF-FILE ENCOUNTERED */
#define ERR88   88 /* INVALID PROCEDURE NESTING, ILLEGAL IN REENTRANT PROCEDURE */
#define ERR89   89 /* MISSING 'DO' FOR MODULE */
#define ERR90   90 /* MISSING NAME FOR MODULE */
#define ERR91   91 /* ILLEGAL PAGELENGTH CONTROL VALUE */
#define ERR92   92 /* ILLEGAL PAGEWIDTH CONTROL VALUE */
#define ERR93   93 /* MISSING 'DO' FOR 'END' , 'END' IGNORED */
#define ERR94   94 /* ILLEGAL CONSTANT, VALUE > 65535 */
#define ERR95   95 /* ILLEGAL RESPECIFICATION OF PRIMARY CONTROL IGNORED */
#define ERR96   96 /* COMPILER ERROR: SCOPE STACK UNDERFLOW */
#define ERR97   97 /* COMPILER ERROR: PARSE STACK UNDERFLOW */
#define ERR98   98 /* INCLUDE FILE IS NOT A DISKETTE FILE */
#define ERR99   99 /* ?? unused */
#define ERR100  100 /* INVALID STRING CONSTANT IN Expression */
#define ERR101  101 /* INVALID ITEM FOLLOWS DOT OPERATOR */
#define ERR102  102 /* MISSING PRIMARY OPERAND */
#define ERR103  103 /* MISSING ') ' AT END OF SUBEXPRESSION */
#define ERR104  104 /* ILLEGAL PROCEDURE INVOCATION WITH DOT OPERATOR */
#define ERR105  105 /* UNDECLARED IDENTIFIER */
#define ERR106  106 /* INVALID INPUT/OUTPUT PORT NUMBER */
#define ERR107  107 /* ILLEGAL INPUT/OUTPUT PORT NUMBER, NOT NUMERIC CONSTANT */
#define ERR108  108 /* MISSING ') ' AFTER INPUT/OUTPUT PORT NUMBER */
#define ERR109  109 /* MISSING INPUT/OUTPUT PORT NUMBER */
#define ERR110  110 /* INVALID LEFT OPERAND OF QUALIFICATION, NOT A STRUCTURE */
#define ERR111  111 /* INVALID RIGHT OPERAND OF QUALIFICATION, NOT IDENTIFIER */
#define ERR112  112 /* UNDECLARED STRUCTURE MEMBER */
#define ERR113  113 /* MISSING ') ' AT END OF ARGUMENT LIST */
#define ERR114  114 /* INVALID SUBSCRIPT, MULTIPLE SUBSCRIPTS ILLEGAL */
#define ERR115  115 /* MISSING ') ' AT END OF SUBSCRIPT */
#define ERR116  116 /* MISSING '=' IN ASSIGNMENT STATEMENT */
#define ERR117  117 /* MISSING PROCEDURE NAME IN CALL STATEMENT */
#define ERR118  118 /* INVALID INDIRECT CALL, IDENTIFIER NOT AN ADDRESS SCALAR */
#define ERR119  119 /* LIMIT EXCEEDED: PROGRAM TOO COMPLEX */
#define ERR120  120 /* LIMIT EXCEEDED: Expression TOO COMPLEX */
#define ERR121  121 /* LIMIT EXCEEDED: Expression TOO COMPLEX */
#define ERR122  122 /* LIMIT EXCEEDED: PROGRAM TOO COMPLEX */
#define ERR123  123 /* INVALID DOT OPERAND, BUILT-IN PROCEDURE ILLEGAL */
#define ERR124  124 /* MISSING ARGUMENTS FOR BUILT-IN PROCEDURE */
#define ERR125  125 /* ILLEGAL ARGUMENT FOR BUILT-IN PROCEDURE */
#define ERR126  126 /* MISSING ') ' AFTER BUILT-IN PROCEDURE ARGUMENT LIST */
#define ERR127  127 /* INVALID SUBSCRIPT ON NON-ARRAY */
#define ERR128  128 /* INVALID LEFT-HAND OPERAND OF ASSIGNMENT */
#define ERR129  129 /* ILLEGAL 'CALL' WITH TYPED PROCEDURE */
#define ERR130  130 /* ILLEGAL REFERENCE TO OUTPUT FUNCTION */
#define ERR131  131 /* ILLEGAL REFERENCE TO UNTYPED PROCEDURE */
#define ERR132  132 /* ILLEGAL USE OF LABEL */
#define ERR133  133 /* ILLEGAL REFERENCE TO UNSUBSCRIPTED ARRAY */
#define ERR134  134 /* ILLEGAL REFERENCE TO UNSUBSCRIPTED MEMBER ARRAY */
#define ERR135  135 /* ILLEGAL REFERENCE TO AN UNQUALIFIED STRUCTURE */
#define ERR136  136 /* INVALID RETURN FOR UNTYPED PROCEDURE, VALUE ILLEGAL */
#define ERR137  137 /* MISSING VALUE IN RETURN FOR TYPED PROCEDURE */
#define ERR138  138 /* MISSING INDEX VARIABLE */
#define ERR139  139 /* INVALID INDEX VARIABLE TYPE, NOT BYTE OR ADDRESS */
#define ERR140  140 /* MISSING '=' FOLLOWING INDEX VARIABLE */
#define ERR141  141 /* MISSING 'TO' CLAUSE */
#define ERR142  142 /* MISSING IDENTIFIER FOLLOWING GOTO */
#define ERR143  143 /* INVALID REFERENCE FOLLOWING GOTO, NOT A LABEL */
#define ERR144  144 /* INVALID GOTO LABEL, NOT AT LOCAL OR MODULE LEVEL */
#define ERR145  145 /* MISSING 'TO' FOLLOWING 'GO' */
#define ERR146  146 /* MISSING ') ' AFTER 'AT' RESTRICTED Expression */
#define ERR147  147 /* MISSING IDENTIFIER FOLLOWING DOT OPERATOR */
#define ERR148  148 /* INVALID QUALIFICATION IN RESTRICTED REFERENCE */
#define ERR149  149 /* INVALID SUBSCRIPTING IN RESTRICTED REFERENCE */
#define ERR150  150 /* MISSING ') ' AT END OF RESTRICTED SUBSCRIPT */
#define ERR151  151 /* INVALID OPERAND IN RESTRICTED Expression */
#define ERR152  152 /* MISSING ') ' AFTER CONSTANT LIST */
#define ERR153  153 /* INVALID NUMBER OF ARGUMENTS IN CALL, TOO MANY */
#define ERR154  154 /* INVALID NUMBER OF ARGUMENTS IN CALL, TOO FEW */
#define ERR155  155 /* INVALID RETURN IN MAIN PROGRAM */
#define ERR156  156 /* MISSING RETURN STATEMENT IN TYPED PROCEDURE */
#define ERR157  157 /* INVALID ARGUMENT, ARRAY REQUIRED FOR LENGTH OR LAST */
#define ERR158  158 /* INVALID DOT OPERAND, LABEL ILLEGAL */
#define ERR159  159 /* COMPILER ERROR: PARSE STACK UNDERFLOW */
#define ERR160  160 /* COMPILER ERROR: OPERAND STACK UNDERFLOW */
#define ERR161  161 /* COMPILER ERROR: ILLEGAL OPERAND STACK EXCHANGE */
#define ERR162  162 /* COMPILER ERROR: OPERATOR STACK UNDERFLOW */
#define ERR163  163 /* COMPILER ERROR: GENERATION FAILURE */
#define ERR164  164 /* COMPILER ERROR: SCOPE STACK OVERFLOW */
#define ERR165  165 /* COMPILER ERROR: SCOPE STACK UNDERFLOW */
#define ERR166  166 /* COMPILER ERROR: CONTROL STACK OVERFLOW */
#define ERR167  167 /* COMPILER ERROR: CONTROL STACK UNDERFLOW */
#define ERR168  168 /* COMPILER ERROR: BRANCH MISSING IN 'IF' STATEMENT */
#define ERR169  169 /* ILLEGAL FORWARD CALL */
#define ERR170  170 /* ILLEGAL RECURSIVE CALL */
#define ERR171  171 /* INVALID USE OF DELIMITER OR RESERVED WORD IN Expression */
#define ERR172  172 /* INVALID LABEL: UNDEFINED */
#define ERR173  173 /* INVALID LEFT SIDE OF ASSIGNMENT: VARIABLE DECLARED WITH DATA ATTRIBUTE */
#define ERR174  174 /* INVALID NULL PROCEDURE */
#define ERR175  175 /* unused */
#define ERR176  176 /* INVALID INTVECTOR INTERVAL VALUE */
#define ERR177  177 /* INVALID INTVECTOR LOCATION VALUE */
#define ERR178  178 /* INVALID 'AT' RESTRICTED REFERENCE, EXTERNAL ATTRIBUTE CONFLICTS WITH PUBLIC ATTRIBUTE */
#define ERR179  179 /* OUTER 'IF' MAY NOT HAVE AN 'ELSE' PART */
#define ERR180  180 /* MISSING OR INVALID CONDITIONAL COMPILATION PARAMETER */
#define ERR181  181 /* MISSING OR INVALID CONDITIONAL COMPILATION CONSTANT */
#define ERR182  182 /* MISPLACED ELSE OR ELSEIF OPTION */
#define ERR183  183 /* MISPLACED ENDIF OPTION */
#define ERR184  184 /* CONDITIONAL COMPILATION PARAMETER NAME TOO LONG */
#define ERR185  185 /* MISSING OPERATOR IN CONDITIONAL COMPILATION Expression */
#define ERR186  186 /* INVALID CONDITIONAL COMPILATION CONSTANT, TOO LARGE */
#define ERR187  187 /* LIMIT EXCEEDED: NUMBER OF SAVE LEVELS > 5 */
#define ERR188  188 /* MISPLACED RESTORE OPTION */
#define ERR189  189 /* NULL STRING NOT ALLOWED */
#define ERR200  200 /* LIMIT EXCEEDED: STATEMENT SIZE */
#define ERR201  201 /* INVALID DO CASE BLOCK, AT LEAST ONE CASE REQUIRED */
#define ERR202  202 /* LIMIT EXCEEDED: NUMBER OF ACTIVE CASES */
#define ERR203  203 /* LIMIT EXCEEDED: NESTING OF TYPED PROCEDURE CALLS */
#define ERR204  204 /* LIMIT EXCEEDED: NUMBER OF ACTIVE PROCEDURES AND DO CASE GROUPS */
#define ERR205  205 /* ILLEGAL NESTING OF BLOCKS, ENDS NOT BALANCED */
#define ERR206  206 /* LIMIT EXCEEDED: CODE SEGMENT SIZE */
#define ERR207  207 /* LIMIT EXCEEDED: SEGMENT SIZE */
#define ERR208  208 /* LIMIT EXCEEDED: STRUCTURE SIZE */
#define ERR209  209 /* ILLEGAL INITIALIZATION OF MORE SPACE THAN DECLARED */
#define ERR210  210 /* ILLEGAL INITIALIZATION OF A BYTE TO A VALUE > 255 */
#define ERR211  211 /* INVALID IDENTIFIER IN 'AT' RESTRICTED REFERENCE */
#define ERR212  212 /* INVALID RESTRICTED REFERENCE IN 'AT' , BASE ILLEGAL */
#define ERR213  213 /* UNDEFINED RESTRICTED REFERENCE IN 'AT' */
#define ERR214  214 /* COMPILER ERROR: OPERAND CANNOT BE TRANSFORMED */
#define ERR215  215 /* COMPILER ERROR: EOF READ IN FINAL ASSEMBLY */
#define ERR216  216 /* COMPILER ERROR: BAD LABEL ADDRESS */
#define ERR217  217 /* ILLEGAL INITIALIZATION OF AN EXTERNAL VARIABLE */
#define ERR218  218 /* ILLEGAL SUCCESSIVE USES OF RELATIONAL OPERATORS */
#define ERR219  219 /* LIMIT EXCEEDED: NUMBER OF EXTERNALS > 255 */
#define IOERR_254 254 /* ATTEMPT TO READ PAS EOF */

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



/* standard structures */
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
    word *wp;
} address;

typedef struct {
    byte deviceId;
    byte name[6];
    byte ext[3];
    byte deviceType;
    byte driveType;
} spath_t;

typedef struct {
    word aftn;
    byte sNam[6];
    byte fNam[16];
    pointer bufP;
    word bsize;
    word actual;
    word curoff;
} file_t;

typedef struct {
    byte len;
    byte type;
    word sym;
    word scope;
    word ilink;
    byte flag[3];
    byte extId;
    word dim;
    word baseoff;
    word parent;
    byte dtype;
    byte intno;
    byte pcnt;
    byte procId;
} info_t;



typedef struct {
    offset_t link;
    offset_t infoP;
    byte name[1];
} sym_t;

typedef struct {
    byte len;
    byte type;
    word sym;
    word scope;
    word ilink;
    offset_t litAddr;
} lit_t;

typedef struct {
    word blk;
    word byt;
} loc_t;

typedef struct {
    offset_t link;
    byte pstr[1];
} cmd_t;

typedef struct {
    pointer inChrP;
    pointer infoP;
} mac_t;

typedef struct {
    byte type;
    word lineCnt;
    word stmtCnt;
    word blkCnt;
} linfo_t;

typedef struct {
    byte type;
    word dataw[129];
} tx1item_t;

typedef struct {
    byte len;
    byte str[1];
} pstr_t;

typedef struct {	// generic record header
    byte type;
    word len;
    byte val[1];
} rec_t;

typedef struct {
    byte type;
    word len;
    byte subType;
    byte seg;
    word addr;
    byte crc;
} rec4_t;

typedef struct {
    byte type;
    word len;
    byte seg;
    word addr;
    byte val[1];
} rec6_t;

typedef struct { offset_t infoOffset; word arrayIndex, nestedArrayIndex, val; } var_t;

typedef struct { word num; offset_t info; word stmt; } err_t;

#pragma pack(pop)
// helper functions for converting offsets to pointers
pointer off2Ptr(offset_t off);
offset_t ptr2Off(pointer addr);

// useful macros to cast off2Ptr
#define ByteP(off)   off2Ptr(off)
#define WordP(off)   ((wpointer)off2Ptr(off))
#define InfoP(off)   ((info_t *)off2Ptr(off))
#define LitP(off)    ((lit_t *)off2Ptr(off))
#define SymbolP(off) ((sym_t *)off2Ptr(off))
#define CmdP(off)    ((cmd_t *)off2Ptr(off))
#define PstrP(off)   ((pstr_t *)off2Ptr(off))


// array sizes
#define EXPRSTACKSIZE   100



extern offset_t MEMORY;
extern void initMem();       // used to create the info/symbol memory

/* plm main */

/* File(main.plm) */
/* main.plm,plm0a.plm,plm1a.plm.plm6b.plm */
void FatalError(byte err);
void FatalError_main(byte err);
void FatalError_ov0(byte err);
void FatalError_ov1(byte err);
void FatalError_ov46(byte err);

// the longjmp buffer
extern jmp_buf exception;


word Start();
word Start0();
word Start1();
word Start2();
word Start3();
word Start4();
word Start5();
word Start6();

/* plmA.plm */
extern pointer cmdTextP;
void SignOnAndGetSourceName();

/* plmb.plm */
void InitKeywordsAndBuiltins();

/* plmc.plm */
extern byte verNo[];

/* plmd.plm */
void SetMarginAndTabW(byte startCol, byte width);
void SetPageNo(word v);
void SetTitle(pointer str,byte len);

/* plmd.plm,lstsp4.plm,lstsp5.plm,lstsp6.plm */
void SetMarkerInfo(byte markerCol, byte marker, byte textCol);

/* plmd.plm,plm0h.plm */
void SetDate(pointer str,byte len);
void SetPageLen(word len);
void SetPageWidth(word width);


/* plm0A.plm */
extern byte cClass[];
extern word curBlkCnt;
extern byte curScope[2];        
#define DOBLKCNT    0       // indexes
#define PROCID      1

extern offset_t curMacroInfoP;
extern wpointer curScopeP;
extern word curStmtCnt;
extern word doBlkCnt;
extern word ifDepth;
extern byte inbuf[];
extern pointer inChrP;		// has to be pointer as it accesses data outside info/symbol space
extern bool isNonCtrlLine;
extern offset_t stmtStartSymbol;
extern byte stmtStartToken;
extern byte nextCh;
extern byte startLexCode;
extern byte lineBuf[];
extern bool lineInfoToWrite;
extern word macroDepth;
extern pointer macroPtrs[];
extern offset_t markedSymbolP;
extern bool skippingCOND;
extern byte state;
extern word stateIdx;
extern word stateStack[];
extern word stmtLabelCnt;
extern offset_t stmtLabels[];
extern byte stmtStartCode;
extern byte tok2oprMap[];
extern byte tokenStr[];
extern byte tokenType;
extern word tokenVal;
extern bool yyAgain;
void CreateTxi1File();
void PopBlock();
void PushBlock(word idAndLevel);
void RewindTx1();
void SyntaxError(byte err);
void TokenError(byte err, offset_t symP);
void TokenErrorAt(byte err);
void WrBuf(pointer buf,word len);
void WrByte(byte v);
void WriteLineInfo();
void WriteTx1(pointer buf, word len);
void WrInfoOffset(offset_t addr);
void WrLexToken();
void WrWord(word v);
void WrXrefDef();
void WrXrefUse();

/* plm0A.plm,main1.plm */
extern word curProcInfoP;
extern linfo_t linfo;

/* plm0A.plm,pdata4.plm */
extern word offCurCh;
extern word offLastCh;

/* plm0A.plm,plm3a.plm,pdata4.plm */
extern byte tx1Buf[];

/* plm0b.plm, plm0c.asm*/
void ParseControlLine(pointer pch);
void GNxtCh();
extern bool trunc;


/* plm0e.plm */
void ParseDeclareElementList();
void ParseExpresion(byte endTok);
void ProcProcStmt();
void SetYyAgain();
void Yylex();
bool YylexMatch(byte token);
bool YylexNotMatch(byte token);

/* plm0f.plm */
extern word curState;
extern bool endSeen;
void ParseProgram();

/* plm0g.plm */
offset_t CreateLit(pointer pstr);

/* File(plm0h.plm) */

/* plm overlay 1 */
/* main1.plm */
extern bool tx2LinfoPending;
extern byte b91C0;
extern word curStmtNum;
extern word markedStSP;
extern bool regetTx1Item;
extern word t2CntForStmt;
extern byte tx1Aux1;
extern byte tx1Aux2;
extern tx1item_t tx1Item;
extern byte tx1RdBuf[];
extern byte tx2Buf[];
extern var_t var;
extern byte xrfBuf[];

/* main1.plm,plm3a.plm */
extern byte atBuf[];

/* plm1a.plm */
extern byte tx1ToTx2Map[];
extern byte lexHandlerIdxTable[];
extern byte icodeToTx2Map[];
extern byte b4172[];
extern byte builtinsMap[];
extern byte ex1Stack[];
extern byte ex2Stack[];
extern word ex3Stack[];
extern word exSP;
extern word operatorSP;
extern word operatorStack[];
extern word parseSP;
extern word parseStack[];
extern byte st1Stack[];
extern byte st2Stack[];
extern word st3Stack[];
extern word stSP;
void MapLToT2();
void OptWrXrf();
void RdTx1Item();
void SetRegetTx1Item();
word Sub_42EF(word arg1w);
void WrTx2Error(byte arg1b);
void WrTx2ExtError(byte arg1b);
void WrTx2File(pointer buf,byte cnt);
word WrTx2Item(byte arg1b);
word WrTx2Item1Arg(byte arg1b,word arg2w);
word WrTx2Item2Arg(byte arg1b,word arg2w,word arg3w);
word WrTx2Item3Arg(byte arg1b,word arg2w,word arg3w,word arg4w);

/* plm1b.pl3 */
void ExpectRParen(byte arg1b);
void GetTx1Item();
bool MatchTx2AuxFlag(byte arg1b);
bool MatchTx1Item(byte arg1b);
bool NotMatchTx1Item(byte arg1b);
void ResyncRParen();
void RecoverRPOrEndExpr();
void ChkIdentifier();
void ChkStructureMember();
void GetVariable();
void WrAtFile(pointer buf, word cnt);
void WrAtFileByte(byte arg1b);
void WrAtFileWord(word arg1w);

/* plm1c.plm */
void AcceptOpAndArgs();
void ExprMakeNode(byte arg1b,byte arg2b);
void ExprPop();
void ExprPush2(byte arg1b,word arg2w);
byte GetCallArgCnt();
void RestrictedExpression();
void MkIndexNode();
void MoveExpr2Stmt();
word InitialValueList(offset_t arg1w);
void ParsePortNum(byte arg1b);
void PopOperatorStack();
void PopParseStack();
void PushOperator(byte arg1b);
void PushParseByte(byte arg1b);
void PushParseWord(word arg1w);
void ResetStacks();
void FixupBased(offset_t arg1w);
void Sub_4D2C();
void ChkTypedProcedure();
void Sub_4DCF(byte arg1b);
void Sub_50D5();
byte Sub_512E(word arg1w);
void ConstantList();

/* plm1d.plm */
void ExpressionStateMachine();
void ExprParse0();
void ExprParse1();
void ExprParse10();
void ExprParse11();
void ExprParse12();
void ExprParse13();
void ExprParse14();
void ExprParse15();
void ExprParse16();
void ExprParse17();
void ExprParse18();
void ExprParse19();
void ExprParse2();
void ExprParse20();
void ExprParse3();
void ExprParse4();
void ExprParse5();
void ExprParse6();
void ExprParse7();
void ExprParse8();
void ExprParse9();

/* plm1e.plm */
word StmtParseMachine(word arg1w);
byte Sub_5945();
byte Sub_59D4();
void Expression();
void ParseLexItems();

/* plm1f.plm */
void Sub_6EE0();

/* plm overlay 2 */
/* main2.plm */
extern byte bC045[];
extern byte bC04E[];
extern byte bC0A8[];
extern byte bC0B1;
extern byte bC0B2;
extern byte bC0B3[];
extern byte bC0B5[];
extern byte bC0B7[];
extern byte bC0B9[];
extern byte bC0BB[];
extern byte bC0BD[];
extern byte bC0BF[];
extern byte bC0C1[];
extern byte bC0C3[];
extern byte bC140[];
extern byte bC1BD;
extern byte bC1BF;
extern byte bC1D2;
extern byte bC1D9;
extern byte bC1DB;
extern byte bC1E6;
extern byte bC209[];
extern offset_t blkCurInfo[];
extern byte blkOverCnt;
extern byte blkSP;
extern bool boC057[];
extern bool boC060[];
extern bool boC069[];
extern bool boC072[];
extern bool boC07B[];
extern bool boC1CC;
extern bool boC1CD;
extern bool boC1D8;
extern bool boC20F;
extern byte buf_C1E7[];
extern byte cfrag1;
extern byte curExtProcId;
extern byte curOp;
extern bool eofSeen;
extern byte extProcId[];
extern byte padC1D3;
extern word pc;
extern byte procCallDepth;
extern byte procChainId;
extern byte procChainNext[];
extern byte tx2Aux1b[];
extern byte tx2Aux2b[];
extern word tx2Auxw[];
extern word tx2op1[];
extern word tx2op2[];
extern word tx2op3[];
extern byte tx2opc[];
extern byte tx2qEnd;
extern byte tx2qp;
extern word wAF54[];
extern word wB488[];
extern word wB4B0[];
extern word wB4D8[];
extern word wB528[];
extern word wB53C[];
extern word wC084[];
extern word wC096[];
extern word wC1C3;
extern word wC1C5;
extern word wC1C7;
extern word wC1CF;
extern word wC1D6;
extern word wC1DC[];

/* plm2a.plm */
extern byte b3FCD[];
extern byte b4029[];
extern byte b4128[];
extern byte b413B[];
extern byte b418C[];
extern byte b425D[];
extern byte b4273[];
extern byte b42F9[];
extern byte b43F8[];
extern byte b44F7[];
extern byte b46EB[];
extern byte b499B[];
extern byte b4A21[];
extern byte b4C15[];
extern byte b4C2D[];
extern byte b4C45[];
extern byte b4CB4[];
extern byte b4D23[];
extern byte b4FA3[];
extern byte b5012[];
extern byte b5048[];
extern byte b50AD[];
extern byte b5112[];
extern byte b5124[];
extern byte b51E3[];
extern byte b5202[];
extern byte b5221[];
extern byte b5286[];
extern byte b528D[];
extern byte b52B5[];
extern byte b52DD[];
extern byte unused[];
extern word w48DF[];
extern word w493D[];
extern word w502A[];
void EmitTopItem();
void EncodeFragData(byte arg1b);
bool EnterBlk();
bool ExitBlk();
void FillTx2Q();
void GetTx2Item();
void PutTx1Byte(byte arg1b);
void PutTx1Word(word arg1w);
byte Sub_5679(byte arg1b);
void Sub_56A0(byte arg1b,byte arg2b);
byte Sub_5748(byte arg1b);
word Sub_575E(offset_t arg1w);
void Sub_5795(word arg1w);
void Sub_58F5(byte arg1b);
void Sub_597E();
void Sub_5B96(byte arg1b,byte arg2b);
void Sub_5C1D(byte arg1b);
void Sub_5C97(byte arg1b);
void Sub_5D27(byte arg1b);
void Sub_5D6B(byte arg1b);
void Sub_5E66(byte arg1b);
void Sub_5EE8();
void Sub_5F4B(word arg1w, word arg2w, byte arg3b, byte arg4b);
void Sub_5FBF(byte arg1b, wpointer arg2wP, wpointer arg3wP);
void Sub_611A();
void Sub_61A9(byte arg1b);
void Sub_61E0(byte arg1b);
void Sub_636A(byte arg1b);
void Sub_63AC(byte arg1b);
void Sub_6416(byte arg1b);
void Sub_652B();
void Sub_67A9();
void Tx2SyntaxError(byte arg1b);
void WrFragData();

/* plm2b.pl3 */
void Sub_689E();

/* plm2c.plm */
void Sub_6BD6();

/* plm2d.plm */
void Sub_717B();
void Sub_7550();

/* plm2e.plm */
void Sub_7A85();
void Sub_7DA9();
void Sub_84ED();

/* plm2f.plm */
void Sub_87CB();
void Sub_9457();

/* plm2g.pl3 */
void FindParamInfo(byte arg1b);
void Inxh();
void MovDem();
void OpB(byte arg1b);
void OpD(byte arg1b);
void Sub_9514();
void Sub_9560();
void Sub_9624(word arg1w);
void Sub_9646(word arg1w);
void Sub_9706();
void Sub_975F();
void Sub_978E();
void Sub_981C();
void Sub_994D();

/* plm2h.plm */
void Sub_9BB0();
void Sub_9D06();
void Sub_9DD7();
void Sub_9EF8();
void Sub_9F14();
void Sub_9F2F();
void Sub_9F9F();
void Sub_A072(byte arg1b);
void Sub_A0C4();
void Sub_A10A();
void Sub_A153();

/* plm overlay 3 */
/* File(main3.plm) no externals */

/* plm3a.plm */
extern byte b42A8[];
extern byte b42D6[];
extern byte b4813[];
extern byte b7199;
extern byte nmsBuf[];
extern byte rec12[];
extern byte rec16_1[];
extern byte rec16_2[];
extern byte rec16_3[];
extern byte rec16_4[];
extern byte rec18[];
extern byte rec2[];
extern byte rec6[];
extern byte rec6_4[];
extern word w7197;
void Sub_4889();
void Sub_48BA(pointer arg1w, byte arg2b, byte arg3b, pointer arg4bP);
void Sub_4908(pointer arg1wP, word arg2w, byte arg3b);
word Sub_4938();
word Sub_4984();
void Sub_49BC(word arg1w, word arg2w, word arg3w);
void Sub_49F9();

/* plm3a.plm,pdata4.plm */
extern byte objBuf[];
extern byte rec20[];
extern byte rec22[];
extern byte rec24_1[];
extern byte rec24_2[];
extern byte rec24_3[];
extern byte rec8[];
extern byte rec4[];
//
/* plm3b.plm,wrec.plm */
void RecAddByte(pointer recP, byte arg2b, byte arg3b);
void RecAddWord(pointer arg1w, byte arg2b, word arg3w);
void WriteRec(pointer recP,byte arg2b);

/* File(lstsp4.plm) */
/* lstsp4.plm,lstsp6.plm */
void SetStartAndTabW(byte arg1b,byte arg2b);

/* lstsp4.plm,lstsp5.plm,lstsp6.plm */
void FlushLstBuf();
void NewLineLst();
void NewPageNextChLst();
void SetSkipLst(byte arg1b);
void TabLst(byte arg1b);
void XnumLst(word num,byte width,byte radix);
void Xputstr2cLst(pointer str,byte endch);
void XwrnstrLst(pointer str,byte cnt);

/* plm overlay 4 */
/* File(main4.plm) */
/* main4.plm,main6.plm */

/* File(page.plm) */
/* page.plm,page1.plm */
void NewPgl();

/* page.plm,page2.plm */
void NlLead();

/* pdata4.plm */
extern byte b9692;
extern byte b969C;
extern byte b969D;
extern byte b96B0[];
//extern byte b96B1[];
extern byte b96D6;
extern word baseAddr;
extern bool bo812B;
extern bool linePrefixChecked;
extern bool linePrefixEmitted;
extern byte cfCode;
extern byte commentStr[];
extern byte curExtId;
extern word blkCnt;
extern byte dstRec;
//extern byte endHelperId; now local var
extern byte helperId;
//extern byte helperModId; now local var
extern byte helperStr[];
extern byte line[];
extern byte locLabStr[];
extern err_t errData;
extern byte lstLine[];
extern byte opByteCnt;
extern byte opBytes[];
//rec4_t rec4;
//rec6_t rec6_4;
//rec8_t rec8;
extern word stmtNo;
extern pointer sValAry[];
extern word w812F;
extern pointer w969E;
extern word w96D7;
extern word wValAry[];

/* pdata4.plm,main5.plm,pdata6.plm */
extern byte lstBuf[];

/* pdata4.plm,pdata6.plm */
extern bool codeOn;
extern word stmtCnt;
extern bool listing;
extern bool listOff;
extern byte lstLineLen;
extern byte srcbuf[];

/* plm4a.plm */
extern byte b42A8[];
extern byte b42D6[];
extern byte b4029[];
extern byte b4128[];
extern byte b413B[];
extern byte b418C[];
extern byte b425D[];
extern byte b4273[];
extern byte b4602[];
extern byte b473D[];
extern byte b475E[];
extern byte b4774[];
extern byte b478A[];
extern byte b47A0[];
extern byte b4A03[];
extern byte b4A78[];
extern byte opcodes[];
extern byte regIdx[];
extern byte regNo[];
extern byte stackOrigin[];
extern byte stkRegIdx[];
extern byte stkRegNo[];
extern word w47C1[];
extern word w4919[];
extern word w506F[];
void Sub_54BA();

/* plm4b.plm */
void AddWrdDisp(pointer strP, word arg2w);
void EmitLabel();
void EmitStatementNo();
void FlushRecs();
void EmitLinePrefix();
void FatalError(byte arg1b);
void ListCodeBytes();

/* plm4b.plm,plm6b.plm */
void EmitError();
void FindErrStr();

void GetSourceLine();

/* plm4c.plm */
void Sub_5FE7(word arg1w,byte arg2b);
void Sub_668B();

/* File(wrec.plm) */

/* File(lstsp5.plm) */

/* main5.plm */
extern byte b3F0B;
extern byte b66D8;
extern offset_t dictionaryP;
extern word dictSize;
extern offset_t dictTopP;
extern byte maxSymLen;
extern word w66D2;
extern word w66D4;
extern offset_t xrefItemP;
//dictionary      AS..dictionaryP
//xrefItem        AS..xrefItemP


/* File(page1.plm) */

/* File(page2.plm) */

/* plm5a.plm */

/* File(lstsp6.plm) */

/* File(main6.plm) */

/* File(page.plm) */

/* pdata6.plm */
extern bool b7AD9;
extern byte b7ADA;
extern bool b7AE4;
extern word offCurCh;
extern word offLastCh;
extern word lineCnt;
extern word blkCnt;
extern word stmtNo;

/* plm6a.plm */
void Sub_42E7();
void MiscControl();     // merged with 
/* plm6b.plm */
void EmitLinePrefix();


/* files in common dir */
/* friendly names for the controls */
#define PRINT    controls[0]
#define XREF     controls[1]
#define SYMBOLS  controls[2]
#define DEBUG    controls[3]
#define PAGING   controls[4]
#define OBJECT   controls[5]
#define OPTIMIZE controls[6]
#define IXREF    controls[7]

//cursym          "SYM_ST"..curSymbolP
//info            "INFO_ST"..curInfoP
//litinfo         "LIT_ST"..curInfoP
//cmd             "CMD_ST"..cmdLineP
//inChr           B..inChrP
//*curScopeP        A..curScopeP
#ifdef _DEBUG
extern int symMode;    // used to control debug of symbol space
extern int infoMode;

void showInfo(offset_t off, FILE *fp);
void dumpAllInfo();
void dumpBuf(file_t *fp);
void copyFile(pointer src, pointer dst);
char *tx2Name(byte op);
void DumpLexStream();
#endif

/* adninf.plm */
void AdvNxtInfo();

/* Alloc.plm */
void Alloc(word size1, word size2);

/* allocs.plm */
offset_t AllocSymbol(word spc);

/* Backup.plm */
void Backup(loc_t *locP, word cnt);

/* Chain.plm */
void Chain(pointer fNam);

/* ciflag.plm */
void ClrInfoFlag(byte flag);

/* Close.plm */
void Close(word conn, wpointer statusP);

/* CloseF.plm */
void CloseF(file_t *fileP);

/* clrflg.plm */
void ClrFlag(pointer base,byte flag);

/* cpyflg.plm */
void CpyFlags(pointer base);

/* cpytil.plm */
void CpyTill(pointer srcP, pointer dstP, word cnt, byte endch);

/* CreatF.plm */
void CreatF(file_t *fp, pointer buf, word bsize, byte mode);

/* creati.plm */
offset_t AllocInfo(word infosize);
void CreateInfo(word scope, byte type);

/* data.plm */
extern file_t atFile;
extern byte b3CF2;
extern byte wrapMarkerCol;
extern byte wrapMarker;
extern byte wrapTextCol;
extern byte skipCnt;
extern word blk1Used;
extern word blk2Used;
extern word blkSize1;
extern word blkSize2;
extern word blockDepth;
extern offset_t botInfo;
extern offset_t botMem;
extern offset_t botSymbol;
extern offset_t cmdLineP;
extern byte col;
extern file_t conFile;
extern byte controls[];
extern word csegSize;
extern word curInfoP;	// individually cast
extern offset_t curSymbolP;
extern byte DATE[];
extern word dsegSize;
extern byte fatalErrorCode;
extern bool hasErrors;
extern offset_t hashChainsP;
extern bool haveModuleLevelUnit;
extern offset_t helpersP;
extern word intVecLoc;
extern byte intVecNum;
extern address ISIS;
extern file_t ixiFile;
extern byte ixiFileName[];
extern bool IXREFSet;
extern pointer lBufP;
extern word lBufSz;
extern word lChCnt;
extern word LEFTMARGIN;
extern bool lfOpen;
extern word linesRead;
extern byte linLft;
extern word localLabelCnt;
extern offset_t localLabelsP;
extern file_t lstFil;
extern byte lstFileName[];
extern byte margin;
extern file_t nmsFile;
extern word objBlk;
extern word objByte;
extern bool OBJECTSet;
extern file_t objFile;
extern byte objFileName[];
extern word offFirstChM1;
extern byte PAGELEN;
extern word pageNo;
extern byte plm80Compiler[];

extern bool PRINTSet;
extern word procChains[];
extern word procCnt;
extern word procInfo[];
extern word programErrCnt;
extern byte PWIDTH;
extern word REBOOTVECTOR;
extern file_t srcFil;
extern word srcFileIdx;
extern word srcFileTable[];	/* 6 * (8 words fNam, blkNum, bytNum) */
extern byte srcStemLen;
extern byte srcStemName[];
extern bool standAlone;
extern offset_t startCmdLineP;
extern byte TITLE[];
extern byte TITLELEN;
extern offset_t topInfo;
extern offset_t topMem;
extern offset_t topSymbol;
extern byte tWidth;
extern file_t tx1File;
extern file_t tx2File;
extern bool afterEOF;
extern byte version[];
extern offset_t w381E;
extern offset_t w3822;
extern word cmdLineCaptured;
extern offset_t ov1Boundary;
extern offset_t ov0Boundary;
extern file_t xrfFile;

/* Delete.plm */
void Delete(pointer pathP, wpointer statusP);

/* DeletF.plm */
void DeletF(file_t *fileP);

/* endcom.plm */
void EndCompile();

/* error.plm */
void Error(word ErrorNum);

/* exit.plm */
__declspec(noreturn) void Exit();

/* Fatal.plm */
void Fatal(pointer str,byte len);

/* FatlIO.plm */
void FatlIO(file_t *fileP, word errNum);

/* Fflush.plm */
void Fflush(file_t *fp);

/* fi.plm */
void FindInfo();

/* fill.asm */
// void Fill(word cnt, pointer dst, byte val); // use memset
//void Fillx(address cnt,address dst,address val);

/* findmi.plm */
void FindMemberInfo();

/* fiscop.plm */
void FindScopedInfo(word scp);

/* fread.asm */
void Fread(file_t *file, pointer buf, word cnt);

/* fwrite.asm */
void Fwrite(file_t *file, pointer buf, word cnt);

/* gibin.plm */
byte GetBuiltinId();

/* gibseo.plm */
offset_t GetBaseOffset();

/* gibsev.plm */
word GetBaseVal();

/* gicond.plm */
byte GetCondFlag();

/* gidim.plm */
word GetDimension();

/* gidim2.plm */
word GetDimension2();

/* gidtyp.plm */
byte GetDataType();

/* gieid.plm */
byte GetExternId();

/* giintn.plm */
byte GetIntrNo();

/* gilen.plm */
byte GetLen();

/* gilit.plm */
offset_t GetLitAddr();

/* gilnko.plm */
offset_t GetLinkOffset();

/* gilnkv.plm */
word GetLinkVal();

/* giparo.plm */
word GetParentOffset();

/* giparv.plm */
word GetParentVal();

/* gipcnt.plm */
byte GetParamCnt();

/* gipid.plm */
byte GetProcId();

/* giscop.plm */
word GetScope();

/* gisym.plm */
offset_t GetSymbol();

/* gitype.plm */
byte GetType();

/* InitF.plm */
void InitF(file_t *fileP, pointer sNam, pointer fNam);

/* itoa.plm */
byte Num2Asc(word num,byte width,byte radix,pointer bufP);

/* Load.plm */
void Load(pointer pathP, word LoadOffset, word switch_, wpointer entryP, wpointer statusP);

/* Lookup.plm */
void Lookup(pointer pstr);

/* lstinf.plm */
void LstModuleInfo();

/* lstlin.plm */
void LstLineNo();

/* memchk.asm */
offset_t MemCk();

/* movmem.asm */
void movmem(address cnt,address src,address dst);

/* Open.plm */
void Open(wpointer connP, pointer pathP, word access, word echo, wpointer statusP);

/* OpenF.plm */
void OpenF(file_t *fileP,byte access);

/* prints.plm */
void PrintStr(pointer str,byte len);

/* PutLst.asm */
void PutLst(byte ch);

/* Read.plm */
void Read(word conn, pointer buffP, word count, wpointer actualP, wpointer statusP);

/* Readf.plm */
void ReadF(file_t *fileP, pointer bufP, word len, wpointer actualP);

/* Rename.plm */
void Rename(pointer oldP,pointer newP,wpointer statusP);

/* Rescan.plm */
void Rescan(word conn, wpointer statusP);

/* Rewind.plm */
void Rewind(file_t *fileP);

/* Seek.plm */
void Seek(word conn, word mode, wpointer blockP, wpointer byteP, wpointer statusP);

/* SeekF.plm */
void SeekF(file_t *fileP, loc_t *locP);

/* SekEnd.plm */
void SekEnd(file_t *fileP);

/* setflg.plm */
void SetFlag(pointer base,byte flag);

/* sibin.plm */
void SetBuiltinId(byte id);

/* sibseo.plm */
void SetBaseOffset(offset_t baseP);

/* sibsev.plm */
void SetBaseVal(word val);

/* sicond.plm */
void SetCondFlag(byte flag);

/* sidim.plm */
void SetDimension(word dim);

/* sidtyp.plm */
void SetDataType(byte dtype);

/* sieid.plm */
void SetExternId(byte id);

/* siflag.plm */
void SetInfoFlag(byte flag);

/* siintn.plm */
void SetIntrNo(byte intNo);

/* silen.plm */
void SetLen(byte len);

/* silit.plm */
void SetLitAddr(offset_t litaddr);

/* silnko.plm */
void SetLinkOffset(offset_t link);

/* silnkv.plm */
void SetLinkVal(word val);

/* siparo.plm */
void SetParentOffset(word parent);

/* sipcnt.plm */
void SetParamCnt(byte cnt);

/* sipid.plm */
void SetProcId(byte id);

/* siscop.plm */
void SetScope(word scope);

/* sisym.plm */
void SetSymbol(offset_t symbol);

/* sitype.plm */
void SetType(byte type);

/* strcmp.plm */
byte Strncmp(pointer s1P, pointer s2P,byte cnt);

/* TellF.plm */
void TellF(file_t * fileP, loc_t *locP);

/* tiflag.plm */
byte TestInfoFlag(byte flag);

/* tstflg.plm */
bool TestFlag(pointer base,byte flag);

/* wr2lst.plm */
void Wr2cLst(word arg1w);

/* wrclst.asm */
void WrcLst(byte ch);

/* Write.plm */
void Write(word conn, pointer buffP, word count, wpointer statusP);

/* WriteF.plm */
void WriteF(file_t *fp, pointer buf, word count);

/* wrslst.plm */
void WrnStrLst(pointer strP, word cnt);

/* zerflg.plm */
void ClrFlags(pointer base);
