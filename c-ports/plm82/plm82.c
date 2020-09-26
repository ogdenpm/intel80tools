// end of change
/*************************************************************************/


/*         8 0 8 0   p l / m   c o m p i l e r ,   p a s s - 2          */
/*                                 plm82                                */
/*                              version 2.0/4.0 hybrid                  */
/*                             january/November, 1975                   */

/*                          copyright (c) 1975                          */
/*                          intel corporation                           */
/*                          3065 bowers avenue                          */
/*                          santa clara, california 95051               */

/*  modifyed by jeff ogden (um), december 1977.                         */

/*************************************************************************/

/*         p a s s - 2     e r r o r    m e s s a g e s*/

/*  error                     message*/
/*  number*/
/*  ------  --- -------------------------------------------------------*/

/*   101     reference to storage locations outside the virtual memory*/
/*           of pass-2.  RE-compile pass-2 with larger 'memory' array.*/

/*   102         "*/

/*   103     virtual memory overflow.  program is too large to compile*/
/*           with present size of 'memory.'  either shorten program OR*/
/*           recompile pass-2 with a larger virtual memory.*/

/*   104     (same as 103).*/

/*   105     $toggle used improperly IN pass-2.  attempt to complement*/
/*           a toggle which has a value other than 0 OR 1.*/

/*   106     register allocation table underflow.  may be due to a pre-*/

/*   107     register allocation error.  no registers available.  may*/
/*           be caused by a previous error, OR pass-2 compiler error.*/

/*   108     pass-2 symbol table overflow.  reduce number of*/
/*           symbols, OR RE-compile pass-2 with larger symbol table.*/

/*   109     symbol table overflow (see error 108).*/

/*   110     memory allocation error.  too much storage specified IN*/
/*           the source program (16k max).  reduce source program*/
/*           memory requirements.*/

/*   111     inline data format error.  may be due to improper*/
/*           record size IN symbol table file passed to pass-2.*/

/*   112     (same as error 107).*/

/*   113     register allocation stack overflow.  either simplify the*/
/*           program OR increase the size of the allocation stacks.*/

/*   114     pass-2 compiler error IN 'litadd' -- may be due to a*/
/*           previous error.*/

/*   115     (same as 114).*/

/*   116     (same as 114).*/

/*   117     line width set too narrow for code dump (use $width=n)*/

/*   118     (same as 107).*/

/*   119     (same as 110).*/

/*   120     (same as 110, but may be a pass-2 compiler error).*/

/*   121     (same as 108).*/

/*   122     program requires too much program AND variable storage.*/
/*           (program AND variables exceed 16k).*/

/*   123     initialized storage overlaps previously initialized storage.*/

/*   124     initialization table format error.  (see error 111).*/

/*   125     inline data error.  may have been caused by previous error.*/
//           This can also be caused by insufficient symbol space when
//           an inline constant is being defined (See card 44130)

/*   126     built-IN function improperly called.*/

/*   127     invalid intermediate language format. (see error 111).*/

/*   128     (same as error 113).*/

/*   129     invalid use of built-IN function IN an assignment.*/

/*   130     pass-2 compiler error.  invalid variable precision (NOT */
/*           single byte OR double byte).  may be due to previous error.*/

/*   131     LABEL resolution error IN pass-2 (may be compiler error).*/

/*   132     (same as 108).*/

/*   133     (same as 113).*/

/*   134     invalid program transfer (only computed jumps are allowed */
/*           with a 'go to').*/

/*   135     (same as 134).*/

/*   136     error IN built-IN function call. */

/*   137     (NOT used)*/

/*   138     (same as 107). */

/*   139     error IN changing variable to address reference. may*/
/*           be a pass-2 compiler error, OR may be caused by pre- */
/*           vous error.*/

/*   140     (same as 107).*/

/*   141     invalid origin.  code has already been generated IN the*/
/*           specified locations.*/

/*   142     a symbol table dump has been specified (using the $memory */
/*           toggle IN pass-1), but no file has been specified to RE-*/
/*           ceive the bnpf tape (use the $bnpf=n control).*/

/*   143     invalid format for the simulator symbol table dump (see*/
/*           error 111). */

/*   144     stack NOT empty at END of compilation.  possibly caused */
/*           by previous compilation error.*/

/*   145     procedures nested too deeply (hl optimization) */
/*           simplify nesting, OR RE-compile with larger pstack */

/*   146     procedure optimization stack underflow.  may be a */
/*           return IN outer block. */

/*   147     pass-2 compiler error IN loadv. register */
/*           stack order is invalid.  may be due to previous error. */

/*   148     pass-2 compiler error.  attempt to unstack too*/
/*           many values.  may be due to previous error.*/

/*   149     pass-2 compiler error. attempt to convert invalid */
/*           value to address type.  may be due to previous error.*/

/*   150     (same as 147) */

/*   151     pass-2 compiler error. unbalanced  execution stack*/
/*           at block END.  may be due to a previous error.*/

/*    152    invalid stack order IN apply.  may be due to previous*/
/*           error.*/
#ifdef V4
//    153    Illegal Control Record: First nonblank characterr was
//           other than a dollar sign

//    154    Bad code origin from pass-1 (e.g. Pass-1 error 48)
#endif

/*              i m p l e m e n t a t i o n    n o t e s */
/*              - - - - - - - - - - - - - -    - - - - -*/
/*    the pl/m compiler is intended to be written IN ansi standard*/
/*    fortran - iv, AND thus it should be possible to compile AND*/
/*    execute this program on any machine which supports this fortran*/
/*    standard.  both pass-1 AND pass-2, however, assume the host */
/*    machine word size is at least 31 bits, excluding the SIGN bit*/
/*    (i.e., 32 bits if the SIGN is included).*/

/*    the implementor may find it necessary to change the source program*/
/*    IN order to account for system dependencies.  these changes are*/
/*    as follows*/

/*    1)   the fortran logical unit numbers for various devices*/
/*         may have to be changed IN the 'gnc' AND 'writel' subrou-*/
/*         tines (see the file definitions below).*/

/*     2)   the host machine may NOT have the pl/m 52 character set*/
/*           0123456789abcdefghijklmnopqrstuvwxyz$=./()+-'*,<>:;*/
/*         (the last 15 special characters are*/
/*         dollar,  equal,  period,  slash, left paren,*/
/*         right paren, plus,   minus,  quote, asterisk,*/
/*         comma, less-than, greater-than, colon, semi-colon)*/
/*         IN this case, it is necessary to change the 'otran' vector IN*/
/*         block data to a character set which the host machine supports*/

/*     3)  although the distribution version of pass-2 assumes a*/
/*        minimum of 31 bits per word on the host machine, better*/
/*        storage utilization is obtained by altering the 'wdsize'*/
/*        parameter IN block data (second to last line of this program).*/
/*        the wdsize is currently set to 31 bits (for the s/360), AND*/
/*        thus will execute on all machines with a larger word size.  the*/
/*        value of wdsize may be set to the number of usable bits IN*/
/*        a fortran integer, excluding the SIGN bit (e.g., on a*/
/*        cdc 6x00, set wdsize to 44, AND on a univac 1108, set wdsize*/
/*        to 35).  IN general, larger values of wdsize allow larger 8080*/
/*        programs to be compiled without changing the size of the*/
/*        'mem' vector.*/

/*     4)  the host fortran system may have a limitation on the number*/
/*         of contiguous comment records (e.g. s/360 level g). if so,*/
/*         intersperse the declaration statements integer i1000, integer*/
/*         i1001, etc., as necessary to break up the length of comments.*/
/*         the symbols i1xxx are reserved for this purpose.*/

/*    there are a number of compiler parameters which may have to*/
/*    be changed for your installation.  these parameters are defined*/
/*    below (see 'scanner commands'), AND the corresponding default*/
/*    values are set following their definition.  for example, the*/
/*                  $rightmargin = i*/
/*    parameter determines the right margin of the input source line.*/
/*    the parameter is set externally by a single line starting with*/
/*    '$r' IN columns one AND two (the remaining characters up to*/
/*    the '=' are ignored).  the internal compiler representation*/
/*    of the character 'r' is 29 (see character codes below), AND thus*/
/*    the value of the $rightmargin parameter corresponds to element 29*/
/*    of the 'contrl' vector.*/

/*    1)  if operating IN  an interactive mode, it is often*/
/*        desirable to minimize output from pass-2.  thus, the following*/
/*        parameters are usually set as defaults*/
/*               $terminal   =  1*/
/*               $input      =  1*/
/*               $output     =  1*/
/*               $generate   =  0*/
/*               $finish     =  0*/

/*        all other parameters are then selected from the console*/

/*    2)  if operating IN batch mode, a number of default toggles are*/
/*        often set which provide useful information when debugging*/
/*        the final program*/
/*               $terminal   =  0*/
/*               $input      =  2*/
/*               $output     =  2*/
/*               $generate   =  1 (line number vs. code locations)*/
/*               $finish     =  1 (decode program into mnemonics at END)*/

/*    3)  if operating with an intellec 8/80, it may be useful to set*/
/*        the code generation header at 16, past the monitor's variables.*/
/*               $header     = 16*/

/*    recall, of course, that the programmer can always override these*/
/*    default toggles -- they are only a convenience to the programmer.*/

/*    5)  the characteristics of the intermediate language files*/
/*        produced by pass-1 are monitored by the $j, $r, $u, AND*/
/*        $z parameters.  these parameters correspond to the source*/
/*        AND width of the intermediate code file ($j AND $r), AND*/
/*        source AND width of the intermediate symbol table ($u*/
/*        AND $r).  some fortran systems _delete the leading character*/
/*        of the files produced by other fortran programs.  the $z*/
/*        parameter may be used to read extra blanks at the beginning of*/
/*        the intermediate files if this becomes a problem on the host*/
/*        system.*/

/*         under normal circumstances, these parameters will NOT*/
/*        have to be changed.  IN any case, experiment with various*/
/*        values of the $ parameters by setting them externally be-*/
/*        fore actually changing the defaults.*/

/*    the implementor may also wish to increase OR decrease the size*/
/*    of pass-1 OR pass-2 tables.  the tables IN pass-2 that may be*/
/*    changed IN size are 'mem' AND 'symbol' which correspond to*/
/*    the areas which hold the compiled program AND program symbol*/
/*    attributes, respectively.  it is impossible to provide an*/
/*    exact formula which relates the number of symbols held by*/
/*    the symbol table since the various types of symbols require*/
/*    differing amounts of storage IN the table.*/

/*    1)  IN the case of the mem vector, the length is determined*/
/*        by the wdsize parameter AND the largest program which you*/
/*        wish to compile.  the number of 8080 (8-bit) words which are*/
/*        packed into each mem element is*/

/*                      p = wdsize/8*/

/*        AND thus the largest program which can be compiled is*/

/*                      t = p * n*/

/*        where n is the declared size of the mem vector.  to change*/
/*        the size of mem, alter all occurrences of*/

/*                         mem(2500)*/

/*        IN each subroutine to mem(n), where n represents the new*/
/*        integer constant size.  IN addition, the 'data' statement*/
/*        IN block data (last program segment) must be changed for the*/
/*        macro parameters based upon the constant value n to*/

/*          data wdsize /31/, two8 /256/, maxmem /n/*/

/*    2)  if the implementor wishes to increase OR decrease the size*/
/*        of the symbol table, then all occurrences of*/

/*                          symbol(3000)*/

/*        must be changed to symbol(m), where m is the desired integer*/
/*        constant size.  the 'data' statements for symbol table para-*/
/*        meters must also be altered as shown below.*/

/*             data symax /m/, sytop /0/, syinfo /m/*/

/*    good  luck (again) ...*/

/*     f  i  l  e     d  e  f  i  n  i  t  i  o  n  s*/
/*            input                        output*/

/*     file   fortran  mts       default   fortran  mts      default*/
/*     num    i/o unit i/o unit  fdname    i/o unit i/o unit fdname*/

/*      1       1      guser     *msource*   11     sercom   *msink**/
/*      2       2      scards    *source*    12     sprint   *sink**/
/*      3       3      3                     13     13*/
/*      4       4      4         -plm16##    14     14*/
/*      5       5      5                     15     15*/
/*      6       6      6                     16     16*/
/*      7       7      7         -plm17##    17     spunch   -load*/

/*   all input records are 80 characters OR less.  all*/
/*   output records are 120 characters OR less.*/
/*   the fortran unit numbers can be changed IN the*/
/*   subroutines gnc AND writel (these are the only oc-*/
/*   currences of references to these units).*/

/*    0 1 2 3 4 5 6 7 8 9*/
/*    0 0 0 0 0 0 0 0 1 1*/
/*    2 3 4 5 6 7 8 9 0 1*/

/*    $ = . / ( ) + - ' * , < > : ;*/
/*    3 3 4 4 4 4 4 4 4 4 4 4 5 5 5*/
/*    8 9 0 1 2 3 4 5 6 7 8 9 0 1 2*/

/*    a b c d e f g h i j k l m n o p q r s t u v w x y z*/
/*    1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3*/
/*    2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7*/

/*  seqno               SUB/func name*/
/*  16280000      subroutine inital*/
/*  16560000      integer function get(ip)*/
/*  16740000      subroutine put(ip,x)*/
/*  16960000      integer function alloc(i)*/
/*  17150000      function icon(i)*/
/*  17340000      integer function gnc(q)*/
/*  18690000      function imin(i,j)*/
/*  18760000      subroutine _form(cc,chars,start,finish,length)*/
/*  19040000      subroutine writel(nspace) */
/*  19580000      subroutine conout(cc,k,n,base)*/
/*  19900000      subroutine pad(cc,chr,i)*/
/*  20010000      subroutine error(i,level)*/
/*  20310000      integer function shr(i,j)*/
/*  20350000      integer function shl(i,j)*/
/*  20390000      integer function right(i,j)*/
/*  20430000      subroutine _delete(n)*/
/*  20680000      subroutine apply(op,op2,com,cyflag)*/
/*  23380000      subroutine genreg(np,ia,ib)*/
/*  24400000      subroutine loadsy*/
/*  26100000      subroutine loadv(is,typv)*/
/*  28330000      subroutine setadr(val)*/
/*  28790000      subroutine ustack*/
/*  28900000      integer function chain(sy,loc)*/
/*  29070000      subroutine gensto(keep)*/
/*  30880000      subroutine litadd(s)*/
/*  32120000      subroutine dump(l,u,fa,fe)*/
/*  33080000      integer function decode(cc,i,w)*/
/*  34540000      subroutine emit(opr,opa,opb)*/
/*  36950000      subroutine puncod(lb,ub,mode)*/
/*  38010000      subroutine cvcond(s)*/
/*  38730000      subroutine saver*/
/*  40000000      subroutine reloc*/
/*  41970000      subroutine loadin*/
/*  42770000      subroutine emitbf(l)*/
/*  43510000      subroutine inldat*/
/*  44780000      subroutine unary(ival)*/
/*  45950000      subroutine exch*/
/*  46690000      subroutine stack(n)*/
/*  46790000      subroutine readcd*/
/*  52230000      subroutine operat(val)*/
/*  66220000      subroutine sydump*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
void showVersion(FILE *fp, bool full);

#define MAXSYM  6000
#define MAXMEM  16000
FILE *files[20];

/* global variables*/
#define ZPAD    0
/* stack size = overridden bytes*/
int smssg[29 + 1] =
{ ZPAD, 30, 31, 12, 14, 22, 1, 30, 20, 37, 16, 1, 39, 1, 26, 33, 16,
29, 29, 20, 15, 15, 16, 25, 1, 13, 36, 31, 16, 30
};


/* base */
int debase = 16;

/* cntrl */
int contrl[64 + 1];


/* sthed */
int sthead[12 + 1] =
{ ZPAD, 27, 29, 30, 31, 29, 12, 30, 25, 23, 20, 31, 33 };




/* types */
/* prstrasnlitv*/
enum { VARB = 1, INTR = 2, PROC = 3, LABEL = 4, LITER = 6 };

/* bifloc */
// int inloc = 16;
const int outloc = 17;
const int firsti = 7;
int casjmp = 0;

/* rgmapp */
int regmap[9 + 1] = { ZPAD, 7, 0, 1, 2, 3, 4, 5, 6, 6 };

enum {
    SPACE =
    1, CHZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
    CHA, CHB, CHC, CHD, CHE, CHF, CHG, CHH, CHI, CHJ, CHK, CHL, CHM,
    CHN, CHO, CHP, CHQ, CHR, CHS, CHT, CHU, CHV, CHW, CHX, CHY, CHZ,
    DOLLAR, EQUALS, DOT, SLASH, LPAREN, RPAREN, PLUS, MINUS, QUOTE,
    STAR, COMMA, LESS, GREATER, COLON, SEMICOLON
};

/* ops */
enum ops {
    LD = 1, IN, DC, AD, AC, SU, SB, ND, XR, OR, CP, ROT, JMP, JMC, CAL,
    CLC, RTN, RTC, RST, INP, OUT, HALT, STA, LDA, XCHG, SPHL,
    PCHL, CMA, STC, CMC, DAA, SHLD, LHLD, EI, DI, LXI, PUSH, POP,
    DAD, STAX, LDAX, INCX, DCX
};
enum reg {
    RA = 1, RB = 2, RC = 3, RD = 4, RE = 5, RH = 6, RL =
    7, RSP = 9, ME = 8
};
enum flags {
    LFT = 9, RGT = 10, TRU = 12, FAL = 11, CY = 13, ACC =
    14, CARRY = 15, ZERO = 16, SIGN = 17, PARITY = 18
};

/* titles */    // "PLM2 VERS "
// int title[10 + 1] = { ZPAD, 27, 23, 24, 4, 1, 33, 16, 29, 30, 1 };


/* ... plm2 vers ...*/
int vers = 40;


/* pass-noprogram*/
/* error*/
/* ()nearat*/
/* parse stack*/
/* symbol  addr wds chrs   length pr ty*/
//int mssg[77 + 1] =
//{ ZPAD, 27, 12, 30, 30, 45, 25, 26, 27, 29, 26, 18, 29, 12, 24, 1, 16,
//29, 29, 26, 29, 42, 43, 25, 16, 12, 29, 12, 31, 27, 12, 29, 30, 16, 1, 30, 31, 12,
//14, 22, 51, 1, 30, 36, 24, 13, 26, 23, 1, 1, 12, 15, 15, 29, 1, 34, 15, 30, 1, 14,
//19, 29, 30, 1, 1, 1, 23, 16, 25, 18, 31, 19, 1, 27, 29, 1, 31, 36
//};


/* symbl */
int symbol[MAXSYM + 1];
int symax = MAXSYM;
int maxmem = MAXMEM;
int sytop = 0;
int syinfo = MAXSYM;
int lmem;


/* xfropt is used IN branch optimiztion*/
int xfrloc = -1;
int xfrsym = 0;
int tstloc = -1;
int conloc = -1;
int defsym = 0;
int defrh = -1;
int defrl = -1;

/* inter */

int intpro[8 + 1];


/* built-IN function code (multiplication AND division)*/
/* built-IN function vector --*/
/* multiply AND divide OR mod*/
/* +  first two give base locations of BIF code segments*/
/* +  next comes number of bytes, number of relocations, AND*/
/* +  a vector of absolute locations where stuffs occur*/

/* the code segments are absolute, packed three per entry*/

/* multiply*/

/* 121 147 120 154 242 012 000 096 105 235 068 077 033 000 000 235*/
/* 120 177 200 235 120 031 071 121 031 079 210 030 000 025 235 041*/
/* 195 016 000*/

/* divide*/

/* 122 047 087 123 047 095 019 033 000 000 062 017 229 025 210 018*/
/* 000 227 225 245 121 023 079 120 023 071 125 023 111 124 023 103*/
/* 241 061 194 012 000 183 124 031 087 125 031 095 201*/
int biftab[41 + 1] =
{ ZPAD, -3, -20, 35, 3, 5, 27, 33, 7902073, 848538, 6905856, 5063915,
33, 11630827, 7924680, 7948063, 13782815, 1638430, 12790251, 16, 45, 2, 15, 35,
5713786, 6238075, 8467, 1129984, 13769189, 14876690, 7992801, 7884567, 8210199,
8154903, 15820567, 836157, 8173312, 8214303, 13197087, 0, 0, 0
};

int bifpar = 0;

/* code */
int codloc = 0;
bool alter;

/* STA    011    000    LDA    011    000    XCHG   SPHL   PCHL*/
/* CMA    STC    CMC    DAA    SHLD   011    000    LHLD   011*/
/* 000    EI     DI     LXI b  011    000    PUSH b POP b  DAD b*/
/* STAX b LDAX b INX b  DCX b  NOP    NOP    NOP    NOP    NOP*/
/* 050 011 000 058 011 000 235 249 233 047 055 063 039 034 011 000*/
/* 042 011 000 251 243 001 011 000 197 193 009 002 010 003 011 000*/



/* files */
char ibuff[80 + 1];
char obuff[120 + 1];
int ibp = 81;
int obp = 0;
int itran[256 + 1];
const char otran[] =
"  0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ$=./()+-'*,<>:;            ";

/* inst */
int ctran[256 + 1] =
{ ZPAD, 835, 36, 40, 42, 1057, 2081, 1280, 35, 995, 39, 41, 43, 1089,
2113, 2304, 67, 995, 100, 104, 106, 1121, 2145, 3328, 99, 995, 103, 105, 107, 1153,
2177, 4352, 131, 995, 164, 707, 170, 1185, 2209, 5376, 675, 995, 167, 739, 171,
1217, 2241, 6400, 579, 995, 292, 387, 298, 1249, 2273, 7424, 611, 995, 295, 419,
299, 1025, 2049, 256, 643, 1056, 1088, 1120, 1152, 1184, 1216, 1248, 1024, 2080,
2112, 2144, 2176, 2208, 2240, 2272, 2048, 3104, 3136, 3168, 3200, 3232, 3264, 3296,
3072, 4128, 4160, 4192, 4224, 4256, 4288, 4320, 4096, 5152, 5184, 5216, 5248, 5280,
5312, 5344, 5120, 6176, 6208, 6240, 6272, 6304, 6336, 6368, 6144, 7200, 7232, 7264,
7296, 7328, 7360, 355, 7168, 32, 64, 96, 128, 160, 192, 224, 0, 3105, 3137, 3169,
3201, 3233, 3265, 3297, 3073, 4129, 4161, 4193, 4225, 4257, 4289, 4321, 4097, 5153,
5185, 5217, 5249, 5281, 5313, 5345, 5121, 6177, 6209, 6241, 6273, 6305, 6337, 6369,
6145, 7201, 7233, 7265, 7297, 7329, 7361, 7393, 7169, 8225, 8257, 8289, 8321, 8353,
8385, 8417, 8193, 9249, 9281, 9313, 9345, 9377, 9409, 9441, 9217, 10273, 10305,
10337, 10369, 10401, 10433, 10465, 10241, 3106, 38, 1058, 163, 2082, 37, 3329, 259,
3234, 227, 1186, 995, 2210, 195, 4353, 1283, 3074, 102, 1026, 323, 2050, 101, 5377,
2307, 3202, 995, 1154, 291, 2178, 995, 6401, 3331, 3170, 166, 1122, 483, 2146, 165,
7425, 4355, 3298, 547, 1250, 451, 2274, 995, 8449, 5379, 3138, 6, 1090, 803, 2114,
5, 9473, 6403, 3266, 515, 1218, 771, 2242, 995, 10497, 7427
};

int insym[284 + 1] =
{ ZPAD, 15, 38, 60, 66, 108, 116, 234, 240, 247, 253, 259, 266, 273,
279, 10, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 38, 12, 13, 14, 15, 16, 19, 23, 24,
20, 30, 27, 8, 48, 50, 52, 53, 55, 56, 57, 58, 60, 25, 14, 25, 37, 27, 27, 26, 14, 37,
24, 27, 16, 1, 63, 66, 24, 26, 33, 10, 78, 81, 84, 87, 90, 93, 96, 99, 102, 105, 108,
20, 25, 29, 15, 14, 29, 12, 15, 15, 12, 15, 14, 30, 32, 13, 30, 13, 14, 12, 25, 12, 35,
29, 12, 26, 29, 12, 14, 24, 27, 3, 113, 114, 115, 116, 21, 14, 29, 31, 149, 152, 155,
158, 161, 164, 168, 171, 174, 176, 179, 182, 185, 188, 192, 196, 200, 204, 207, 210,
213, 216, 220, 224, 226, 228, 231, 231, 231, 231, 231, 234, 29, 23, 14, 29, 29, 14,
29, 12, 23, 29, 12, 29, 21, 24, 27, 14, 12, 23, 23, 29, 16, 31, 29, 30, 31, 20, 25,
26, 32, 31, 19, 23, 31, 30, 31, 12, 23, 15, 12, 35, 14, 19, 18, 35, 31, 19, 23, 30,
27, 19, 23, 27, 14, 19, 23, 14, 24, 12, 30, 31, 14, 14, 24, 14, 15, 12, 12, 30, 19, 23,
15, 23, 19, 23, 15, 16, 20, 15, 20, 25, 26, 27, 45, 45, 45, 1, 237, 240, 23, 35, 20,
1, 243, 247, 27, 32, 30, 19, 1, 250, 253, 27, 26, 27, 1, 256, 259, 15, 12, 15, 1, 262,
266, 30, 31, 12, 35, 1, 269, 273, 23, 15, 12, 35, 1, 276, 279, 20, 25, 35, 1, 282,
285, 15, 14, 35
};


/* number of bytes following first 13 instructions IN category 3*/
int ibytes[23 + 1] =
{ ZPAD, 0, 0, 0, 0, 2, 2, 0, 0, 1, 1, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2 };

/* compilation terminated*/
int terr[22 + 1] =
{ ZPAD, 14, 26, 24, 27, 20, 23, 12, 31, 20, 26, 25, 1, 31, 16, 29, 24,
20, 25, 12, 31, 16, 15 };
bool errflg = false;

/* peep is used IN peephole optimization (see emit)*/
/* lapol is a one element polish look-ahead*/
/* lastld is codloc of last register to memory store*/
/* lastrg is the effected register*/
/* lastin is the codloc of the last increment*/
/* (used IN do-loop index increment)*/
/* lastex is location of last XCHG operator*/
/* lastir is the codloc of the last register increment*/
/* (used IN apply AND gensto to gen inr memory)*/
int lapol = -1;
int lastld = 0;
int lastrg = 0;
int lastin = 0;
int lastex = 0;
int lastir = 0;
int lastli = 0; // location of last load reg immediate
int lastri = 0;

/* pstack is the procedure stack used IN hl optimization*/
int prstk[15 + 1];
int maxdep[16 + 1];
int curdep[16 + 1];
int prsmax = 15;
int prsp = 0;
int lxis = 0;

/* ilcod */
enum { OPR = 0, ADR, VLU, DEF, LIT, LIN };
enum {
    NOP = 0, ADD, ADC, SUB, SBC, MUL, DIV, MDF, NEG, AND, IOR, XOR, NOT,
    EQL, LSS, GTR, NEQ, LEQ, GEQ, INX, TRA, TRC, PRO, RET, STO, STD,
    XCH, DEL, DAT, LOD, BIF, INC, CSE, END, ENB, ENP, HAL, RTL, RTR,
    SFL, SFR, HIV, LOV, CVA, ORG, DRT, ENA, DIS, AX1, AX2, AX3
};

/* memory */
int memtop;
int membot;
unsigned char mem[MAXMEM];
int offset = 0;
int preamb;

/* regall */
int regs[7 + 1];
int regv[7 + 1] = { ZPAD, -1, -1, -1, -1, -1, -1, -1 };
int lock[7 + 1];

int prec[16 + 1];
int st[16 + 1];
int rasn[16 + 1];
int litv[16 + 1];
int sp = 0;
int maxsp = 16;

/* intbas is the largest intrinsic symbol number*/
int intbas = 23;

int accum[32 + 1];


/* function declarations */
int main(const int argc, char **argv);
void inital();
int get(int ip);
void put(int ip, const int x);
int alloc(const int i);
int icon(const int i);
int gnc(const int q);
int imin(const int i, const int j);
void _form(const int cc, const int chars[ /*length+1 */],
    const int start, const int finish, const int length);
void form(char *fmt, ...);	// ascii version using printf formats
void putch(const int chr);
void writel(int nspac);
void error(const int i, const int level);
int shr(const int i, const int j);
int shl(const int i, const int j);
int right(const int i, const int j);
void _delete(int n);
void apply(const int op, const int op2, const int com, const int cyflag);
void genreg(const int np, int *ia, int *ib);
void loadsy();
void loadv(const int is, const int typv);
void setadr(const int val);
void ustack();
int chain(const int sy, const int loc);
void gensto(const int keep);
void litadd(const int s);
void dump(const int l, const int u, const int fa, const int fe);
int decode(const int cc, const int i, const int w);
void emit(const int opr, const int opa, const int opb);
void puncod(const int lb, const int ub, const int mode);
void cvcond(const int s);
void saver();
void reloc();
int loadin();
void emitbf(const int l);
void inldat();
void unary(const int ival);
void exch();
void stack(const int n);
void readcd();
bool operat(int val);
void sydump();
void cmpuse();
void inx(int jp);
void builtin(int bf, int result);
void compare16(int icom, int flag, int iq);
void updateHL(int jp);

/* the following scanner commands are defined */
/* analysis         (12) */
/* bpnf             (13) */
/* count = i        (14) */
/* _delete = i       (15) */
/* eof              (16) */
/* finish           (17)  dump code at finish */
/* generate         (18) */
/* header           (19) */
/* input = i        (20) */
/* jfile (code)= i  (21) */
/* leftmargin = i   (23) */
/* map              (24) */
/* numeric (emit)   (25) */
/* output = i       (26) */
/* print (t OR f)   (27) */
/* quickdump = n    (28)  hexadecimal dump */
/* rightmarg = i    (29) */
/* symbols          (30) */
/* terminal         (31) (0=batch, 1=term, 2=interlist) */
/* usymbol = i      (32) */
/* variables        (33) */
/* width = i        (34) */
/* ypad = n         (36)  blank pad on output */
/* zmargin = i      (37)  sets left margin for i.l. */
/* * = n            (47)  0 - compiler handles stack pointer */
/*                        1 - programmer handles  stack pointer */
/*                        n > 1 (mod 65536) n is base value of sp */

#define errorCnt	contrl[1]
#define C_ANALYSIS	contrl[CHA]
#define C_BPNF		contrl[CHB]
#define C_COUNT		contrl[CHC]
#define C_DELETE	contrl[CHD]
#define C_EOF		contrl[CHE]
#define C_FINISH	contrl[CHF]
#define C_GENERATE	contrl[CHG]
#define C_HEADER	contrl[CHH]
#define C_INPUT		contrl[CHI]
#define C_JFILE		contrl[CHJ]
#define C_LEFTMARG	contrl[CHL]
#define C_MAP		contrl[CHM]
#define C_NUMERIC	contrl[CHN]
#define C_OUTPUT	contrl[CHO]
#define C_PRINT		contrl[CHP]
#define C_QUICKDUMP	contrl[CHQ]
#define C_RIGHTMARG	contrl[CHR]
#define C_SYMBOLS	contrl[CHS]
#define C_TERMINAL	contrl[CHT]
#define C_USYMBOL	contrl[CHU]
#define C_VARIABLES	contrl[CHV]
#define C_WIDTH		contrl[CHW]
#define C_YPAD		contrl[CHY]
#define C_ZMARGIN	contrl[CHZ]
#define C_STACKHANDLING	contrl[STAR]


int iabs(int a) {
    return a < 0 ? -a : a;
}

/*
    although fort.nn is still supported, this routine presets up several files based
    on fname. This allows a command line to specify plm81 file.plm and the fort.nn files
    are named to reflect the file stem
*/
void initFiles(char *fname) {
    FILE *fp;
    char path[_MAX_PATH];
    char *s, *t;

    strcpy(path, fname);
    for (s = path; t = strpbrk(s, "\\/"); s = t + 1)   // find the filename
        ;
    if (t = strrchr(s, '.'))                            // replace extent
        *t = 0;
    else
        t = strchr(s, '\0');
    strcpy(t, ".pol");
    if ((fp = fopen(path, "rt")) == NULL) {
        fprintf(stderr, "can't open %s\n", path);
        exit(1);
    }
    files[C_JFILE] = fp;
    strcpy(t, ".sym");
    if ((fp = fopen(path, "rt")) == NULL) {
        fprintf(stderr, "can't open %s\n", path);
        exit(1);
    }
    files[C_USYMBOL] = fp;

    strcpy(t, ".lst");
    if ((fp = fopen(path, "at")) == NULL) {         // add to existing file
        fprintf(stderr, "can't open %s\n", path);
        exit(1);
    }
    files[C_OUTPUT + 10] = fp;

    strcpy(t, ".hex");
    if ((fp = fopen(path, "wt")) == NULL) {         // add to existing file
        fprintf(stderr, "can't create %s\n", path);
        exit(1);
    }
    files[C_BPNF + 10] = fp;

    strcpy(t, ".cfg");
    if ((fp = fopen(path, "rt")) != NULL)          // set if we have a config file
        files[C_INPUT] = fp;
    files[11] = stdout;
}


// get the file to use
// auto open file if first use
FILE *getfile(int i) {
    if (files[i] == NULL) {
        char fname[8];
        sprintf(fname, "fort.%d", i);
        if ((files[i] = fopen(fname, i < 10 ? "rt" : "wt")) == NULL) {
            if (i != 1) {
                fprintf(stderr, "can't %s %s\n", i < 10 ? "open" : "create", fname);
                exit(1);
            }
        }
    }
    return files[i];
}

void closefiles() {
    int i;
    for (i = 0; i < 20; i++)
        if (files[i] && files[i] != stdout)
            fclose(files[i]);
}

int main(const int argc, char **argv) {
    int i, j, jp, jl, jn, np, n, l, m, k;

    if (argc == 2 && stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        printf("\nUsage: plm82 -v | -V  | [plmfile]\n"
            "Where\n"
            "-v/-V      provide version infomation\n"
            "plmfile    is the same source file name used in plm81 of the form prefix.ext\n"
            "           intermediate files prefix.lst, prefix.pol and prefix.sym are used\n"
            "           optionally prefix.cfg can be used to hold plm82 configuration flags\n"
            "           prefix.lst is updated with pass2 output added and prefix.hex is created\n"
            "           Note the intermediate .pol and .sym files are not deleted automatically\n\n"
            "If plmfile is not specified, input is taken from fort.4 (pol) and fort.7 (sym)\n"
            "The plm81 output files will need to be renamed accordingly and fort.12 saved\n"
            "the created files are fort.12 (lst) and fort.17 (hex)\n");
        exit(0);
    }

    /* initialize memory */
    inital();


    /* contrl(1) holds the error count */
    for (i = 1; i <= 64; i++)
        contrl[i] = -1;
    errorCnt = 0;
    C_ANALYSIS = 0;
    C_BPNF = 7;
    C_COUNT = 0;
    C_DELETE = 120;
    C_EOF = 0;
    C_FINISH = 1;
    C_GENERATE = 1;
    C_HEADER = 0;
    C_INPUT = 1;
    C_JFILE = 4;
    C_LEFTMARG = 1;
    C_MAP = 1;
    C_NUMERIC = 0;
    C_OUTPUT = 2;
    C_PRINT = 0;
    C_QUICKDUMP = 1;
    C_RIGHTMARG = 73;
    C_SYMBOLS = 0;
    C_TERMINAL = 1;
    C_USYMBOL = 7;
    C_VARIABLES = 0;
    C_WIDTH = 120;
    C_YPAD = 1;
    C_ZMARGIN = 2;
    C_STACKHANDLING = 0;
    for (i = 1; i <= 256; i++)
        itran[i] = 1;
    for (i = 1; i <= 52; i++) {
        j = otran[i];
        j = icon(j);
        itran[j] = i;
    }
    if (argc > 1)
        initFiles(argv[1]);

    form("\n8080 PLM2 VERS %d.%d", vers / 10, vers % 10);
    writel(1);
    i = gnc(0);

    /* change margins for reading intermediate language */
    C_LEFTMARG = C_ZMARGIN;
    writel(0);
    codloc = C_HEADER;
    loadsy();
    readcd();
    if (!errflg) {

        /* make sure compiler stack is empty */
        if (sp != 0)
            error(144, 1);

        /* make sure execution stack is empty */
        if (curdep[1] != 0)
            error(150, 1);
        reloc();

        /* may want a symbol table for the simulator */
        writel(0);
        sydump();
        if (C_FINISH != 0) {

            /* dump the preamble */
            i = offset;
            offset = 0;
            if (intpro[1])
                dump(0, 2, 16, 1);
            for (int ii = 2; ii <= 8; ii++) {
                if (intpro[ii])
                    dump(8 * ii - 8, ii * 8 - 6, 16, 1);
            }

            offset = i;

            /* dump the symbol table by segments until codloc-1 */
            i = offset + preamb;

            do {
                jp = 99999;
                jl = 0;

                /* locate next inline data at OR above i */
                jn = 0;
                np = intbas + 1;
                if (np <= sytop)
                    for (n = np; n <= sytop; n++) {
                        l = symbol[n];
                        m = symbol[l - 1];
                        if (m >= 0)
                            if (m % 16 == VARB) {
                                j = iabs(symbol[l]);
                                j = j % 65536;
                                if (j <= jp)
                                    if (j >= i) {

                                        /* candidate at j */
                                        k = m / 16 % 16;
                                        if (k > 2)
                                            k = 1;
                                        k = k * (m / 256);
                                        if (k != 0) {

                                            /* found one at j with length k bytes */
                                            jp = j;
                                            jn = n;
                                            jl = k;
                                        }
                                    }
                            }
                    }

                /* jp is base address of next data stmt, jl is length IN bytes */
                if (i < jp) {

                    /* code is printed below */
                    l = jp - 1;
                    if (l > codloc - 1)
                        l = codloc - 1;
                    dump(i, l, 16, 1);
                }
                if (jp < codloc) {

                    /* then the data segments */
                    if (C_SYMBOLS != 0)
                        form("S%05d", jn);
                    dump(jp, jp + jl - 1, 16, 16);
                }
                i = jp + jl;
            } while (!(i >= codloc));
        }

        int isave = codloc;
        int kval = loadin();

        if (codloc != isave && C_FINISH)
            /* dump the initialized variables */
            dump(kval, codloc - 1, 16, 16);

        if (C_BPNF != 0) {

            /* punch deck */
            writel(0);
            i = C_OUTPUT;
            C_OUTPUT = C_BPNF;
            k = offset;
            offset = 0;

            int mode1 = 1;
            if (intpro[1]) {
                puncod(0, 2, mode1);
                mode1 = 3;
            }
            for (int ii = 2; ii <= 8; ii++)
                if (intpro[ii]) {
                    puncod(ii * 8 - 8, ii * 8 - 6, mode1);
                    mode1 = 3;
                }
            offset = k;
            if (codloc != isave) {
                puncod(offset + preamb, isave - 1, mode1);
                puncod(kval, codloc - 1, 3);
            } else
                puncod(offset + preamb, codloc - 1, mode1);
            puncod(0, 0, 2);

            writel(0);
            C_OUTPUT = i;
        }

        /* write error count */
        j = C_OUTPUT;
        k = j;
        for (;;) {
            writel(0);
            C_OUTPUT = j;
            if (errorCnt == 0)
                form("\nNO");

            else
                form("%d", i);
            form(" PROGRAM ERROR");
            if (errorCnt != 1)
                putch('S');
            form("\n \n");

            /* check for terminal control of a batch job */
            if (j == 1 || C_TERMINAL == 0)
                break;

            /* arrive here if terminal toggle gt 0, AND output NOT console */
            j = 1;
        }
    }
    closefiles();
    cmpuse();
    return errorCnt;
}

void inital() {
    memtop = MAXMEM + 1;
    membot = -1;
    memset(mem, 0, sizeof(mem));
}

int get(int ip) {
    ip -= offset;
    if (ip >= sizeof(mem)) {
        error(101, 5);
        return 0;
    }
    return mem[ip];
}

void put(int ip, const int x) {
    ip -= offset;
    if (ip >= sizeof(mem))
        error(102, 5);

    else
        mem[ip] = x;
}

int alloc(const int i) {
    if (i < 0) {		/* allocation is from top */
        memtop += i;
        if (memtop <= membot)
            error(104, 5);
        return memtop + offset;
    }

    /* allocation is from bottom */
    membot += i;
    if (membot > memtop)
        error(103, 5);
    return membot + offset + 1 - i;
}

int icon(const int i) {

    /* icon is called with an integer variable i which contains a */
    /* character read with an a1 format.  icon must reduce this character */
    /* to a value somewhere between 1 AND 256.  normally, this would be */
    /* accomplished by shifting the character to the rightmost bit posi- */
    /* tions of the word AND masking the right 8 bits.  it is done rather */
    /* inefficiently here, however, to gain some machine independence. */
    const char *s;
    if (s = strchr(otran + 1, i))
        return s - otran;
    return 1;
}

int gnc(const int q) {
    int i, j, k, l, lp, ii;
    FILE *ifile;

    /* get next character from the input stream (OR 0 if */
    /* no character is found) */
    if (ibp > C_RIGHTMARG) {

        /* read another record from command stream */
        if (C_TERMINAL != 0)
            if (C_INPUT == 1) {

                /* input is from terminal, so get rid of last line */
                form("\n \n");
            }
        for (;;) {
            ifile = getfile(C_INPUT);
            if (ifile == NULL || C_EOF == 1)
                return 0;

            int c;
            for (i = 1; i <= 80; i++) {
                c = getc(ifile);
                if (c == '\n' || c == EOF)
                    break;
                ibuff[i] = c;
            }
            if (i == 1 && c == EOF) {
                fprintf(stderr, "EOF reached\n");
                return 0;
            }
            while (c != '\n')
                c = getc(ifile);
            while (i <= 80)
                ibuff[i++] = ' ';
            lp = C_LEFTMARG;
            if (ibuff[lp] != '$')
                break;
            if (ibuff[2] == ' ')
                break;

            int inlen = C_RIGHTMARG;

            /* scanner parameters follow */
            lp = lp + 1;
            for (;;) {
                j = ibuff[lp];
                if (j == '$') {

                    /* display $ parameters */
                    l = 2;
                    k = 64;
                    lp = lp + 1;
                    j = icon(ibuff[lp]);
                    if (j != 1) {
                        l = j;
                        k = j;
                    }
                    for (i = l; i <= k; i++) {
                        j = contrl[i];
                        if (j >= 0)
                            form("$%c=%d", otran[i], j);
                    }
                    if (C_TERMINAL != 0)
                        form("\n ");
                    writel(0);
                } else if (q == 99)
                    return 0;

                else {
                    j = icon(j);
                    lp = lp + 1;
                    for (i = lp; i <= inlen; i++) {
                        ii = i;
                        if (ibuff[i] == '=') {
                            k = 0;
                            ii = ii + 1;
                            for (i = ii; i <= inlen; i++) {
                                l = icon(ibuff[i]);
                                if (l > 1) {
                                    if (l > 11)
                                        break;
                                    k = k * 10 + (l - 2);
                                }
                            }
                            contrl[j] = k;
                            goto L20;
                        } else if (ibuff[i] == '$')
                            break;
                    }
                    k = contrl[j];
                    if (k > 1)
                        error(105, 1);

                    else
                        contrl[j] = 1 - k;
                    if (ii == inlen)
                        break;
                    lp = ii + 1;
                    continue;
                }

                /* may be more $ in input line */
            L20:	ii = lp + 1;
                for (i = ii; i <= inlen; i++) {
                    lp = i;
                    if (ibuff[i] == '$') {
                        lp = lp + 1;
                        goto L30;
                    }
                }
                break;
            L30:;
            }
        }
        ibp = lp;
        if (C_PRINT != 0) {
            if (C_LEFTMARG != 1) {
                form("%.s   ", C_LEFTMARG, ibuff + 1);
            }
            form("%.s", C_RIGHTMARG - C_LEFTMARG, ibuff + C_LEFTMARG);
            if (C_RIGHTMARG != 80) {
                form("   %.s", 80 - C_RIGHTMARG - 1,
                    ibuff + C_RIGHTMARG + 1);
            }
        }
    }
    return icon(ibuff[ibp++]);
}

int imin(const int i, const int j) {
    return i < j ? i : j;
}

void _form(const int cc, const int chars[ /*length+1 */],
    const int start, const int finish, const int length) {
    int i, j;

    /* cc = 0 dump buffer, go to next line */
    /* cc = 1 append to current buffer */
    /* cc = 2 _delete leading blanks AND append */
    j = start;
    i = cc + 1;
    switch (i) {
    case 1:
        break;
    case 2:
        goto L50;
    case 3:
        for (;;) {
            if (j > finish)
                return;
            if (chars[j] != 1)
                goto L50;
            j = j + 1;
        }
    }
    for (;;) {
        writel(0);
    L50:do {
        if (j > finish)
            return;
        obp = obp + 1;
        obuff[obp] = otran[chars[j]];
        j = j + 1;
    } while (!(obp >= C_WIDTH));
    }
    return;
}

void form(char *fmt, ...) {
    va_list marker;
    char buf[128];
    char *s;
    va_start(marker, fmt);
    vsprintf(buf, fmt, marker);
    va_end(marker);
    s = buf;
    while (*s) {
        if (*s != '\n')
            obuff[++obp] = *s;
        if (*s++ == '\n' || obp >= C_WIDTH)
            writel(0);
    }
}

void putch(const int chr) {
    if (chr != '\n')
        obuff[++obp] = chr;
    if (chr == '\n' || obp >= C_WIDTH)
        writel(0);
}

void writel(int nspac) {
    int i, np;
    np = C_YPAD - 1;
    if (obp > np) {
        while (obp > 1 && obuff[obp] == ' ')	// trim off trailling spaces
            obp--;
        obp = imin(C_DELETE, obp);
        FILE *fp = getfile(C_OUTPUT + 10);
        obuff[0] = ' ';
        fwrite(obuff, sizeof(char), obp + 1, fp);
        putc('\n', fp);
        memset(obuff + 1, ' ', obp);
        while (nspac-- > 0) {
            fwrite(obuff, sizeof(char), obp + 1, fp);
            putc('\n', fp);
        }
    }
    if (np > 0)
        for (i = 1; i <= np; i++)
            obuff[i] = ' ';
    obp = np;
    return;
}

void error(const int i, const int level) {

    /* print error message - level is severity code (terminate at 5) */
    errorCnt = errorCnt + 1;
    form("\n(%05d)  ERROR %d\n", C_COUNT, i);

    /* check for severe error - level greater than 4 */
    if (level > 4) {

        /* terminate compilation */
        form("\nCOMPILATION TERMINATED\n");
        errflg = true;
    }
    return;
}

int shr(const int i, const int j) {
    return i / (1 << j);
}

int shl(const int i, const int j) {
    return i << j;
}

int right(const int i, const int j) {
    return i % (1 << j);
}

void _delete(int n)	/* _delete the top n elements from the stack */
{
    int i;
    while (n-- > 0) {
        if (sp <= 0) {
            error(106, 1);
            return;
        }
        if (i = rasn[sp] / 16 % 16) {
            if (regs[RA] == i)
                regs[RA] = 0;
            lock[i] = 0;
            regs[i] = 0;
        }
        if (i = rasn[sp] % 16) {
            if (regs[RA] == i)
                regs[RA] = 0;
            lock[i] = 0;
            regs[i] = 0;
        }
        sp--;
    }
}

void apply(const int op, const int op2, const int com, const int cyflag) {
    int i, j, ip, ia, ib, l, lp, jp, k;

    /* apply op to top elements of stack */
    /* use op2 for high order bytes if double byte operation */
    /* com = 1 if commutative operator, 0 otherwise */
    /* cyflag = 1 if the CARRY is involved IN the operation */

    /* may want to clear the CARRY for this operation */

    /* check for one of the operands IN the stack (only one can be there) */
    i = sp - 1;
    ip = 0;
    for (j = i; j <= sp; j++)
        if (st[j] == 0 && rasn[j] == 0 && litv[j] < 0) {

            /* operand is stacked */
            genreg(-2, &ia, &ib);
            regs[ia] = j;
            if (ip != 0)
                error(152, 1);
            ip = ib;
            if (prec[j] > 1)

                /* double byte operand */
                regs[ib] = j;

            else

                /* single precision result */
                ib = 0;
            rasn[j] = ib * 16 + ia;
            emit(POP, ip, 0);
            ustack();
        }

    /* make a quick check for possible accumulator match */
    /* with the second operand */
    ia = rasn[sp];
    if (ia > 255)
        cvcond(sp);
    ib = rasn[sp - 1];
    if (ib > 255)
        cvcond(sp - 1);
    l = regs[RA];
    if (ia * ib * l * com != 0)

        /* commutative operator, one may be IN the accumulator */
        if (l == ia % 16)

            /* second operand IN gpr's, l.o. byte IN accumulator */
            exch();
    for (;;) {
        ia = 0;
        ib = 0;

        /* is op1 IN gpr's */
        l = rasn[sp - 1];
        if (l == 0) {

            /* is op2 IN gpr's */
            l = rasn[sp];
            if (l != 0) {

                /* yes - can we exchange AND try again */
                /* after insuring that a literal has no regs assigned */
                litv[sp] = -1;
                if (com != 0)
                    goto L70;
            }
        } else {

            /* reg assigned, lock regs containing var */
            i = l % 16;
            if (i == 0)
                goto L120;
            ia = i;
            lock[i] = 1;
            i = l / 16;
            if (i != 0) {
                ib = i;
                lock[i] = 1;
            }

            /* may have to generate one free reg */
            if (prec[sp - 1] < prec[sp])
                ib = ia - 1;

            /* check for pending register store */
            jp = regs[RA];
            if (jp != ia) {
                if (jp != 0)
                    emit(LD, jp, RA);
                regs[RA] = ia;
                emit(LD, RA, ia);
            }
        }

        /* op2 NOT IN gpr's OR op is NOT commutative */
        /* check for literal value - is op2 literal */
        k = litv[sp];
        if (k >= 0)
            break;

        /* op1 NOT a literal,  check for literal op2 */
        if (litv[sp - 1] < 0)
            goto L80;
        if (com != 1)
            goto L80;
    L70:exch();
    }
    if (prec[sp] <= 1 && prec[sp - 1] <= 1)

        /* make special check for possible increment OR decrement */
        if (k == 1)

            /* must be ADD OR subtract without CARRY */
            if (op == AD || op == SU)

                /* first operand must be single byte variable */
                if (prec[sp - 1] == 1) {
                    if (ia <= 1) {

                        /* op1 must be IN memory, so load into gpr */
                        loadv(sp - 1, 0);
                        l = rasn[sp - 1];
                        ia = l % 16;
                        if (ia == 0)
                            goto L120;

                        /* ...may change to inr memory if STD to op1 follows... */
                        lastir = codloc;
                    }
                    jp = ia;
                    if (regs[RA] == ia)
                        jp = RA;
                    if (op == AD)
                        emit(IN, jp, 0);
                    if (op == SU)
                        emit(DC, jp, 0);
                    goto L110;
                }

    /* generate registers to hold results IN loadv */
    /* (loadv will load the low order byte into the ACC) */
L80:	loadv(sp - 1, 1);
    l = rasn[sp - 1];
    ia = l % 16;
    if (ia == 0)
        goto L120;
    lock[ia] = 1;
    ib = l / 16;

    /* is this a single byte / double byte operation */
    if (ib <= 0 && prec[sp] != 1) {

        /* get a spare register */
        ib = ia - 1;
        if (ib == 0)
            goto L120;
        lock[ib] = 1;
    }

    /* now ready to perform operation */
    /* l.o. byte is IN AC, h.o. byte is IN ib. */
    /* result goes to ia (l.o.) AND ib (h.o.) */

    /* is op2 IN gpr's */
    lp = rasn[sp];
    k = -1;
    if (lp > 0)

        /* perform ACC-reg operation */
        emit(op, lp % 16, 0);

    else {

        /* is op2 a literal */
        k = litv[sp];
        if (k < 0) {
            loadv(sp, 2);

            /* perform operation with low order byte */
            emit(op, ME, 0);
        } else

            /* use CMA if op is XR AND op2 is LIT 255 */
            if (op == XR && k % 256 == 255)
                emit(CMA, 0, 0);
            else
                /* perform ACC-immediate operation */
                emit(op, -k % 256, 0);

    }

    /* set up a pending register store */
    /* if this is NOT a compare */
    if (op != CP)
        regs[RA] = ia;
    if (prec[sp] == 2) {

        /* is h.o. byte of op2 IN memory */
        if (k < 0 && lp <= 0) {

            /* point to h.o. byte with h AND l */
            emit(IN, RL, 0);
            regv[RL] = regv[RL] + 1;
        }

        /* do we need to pad with h.o. ZERO for op1 */
        if (prec[sp - 1] > 1) {

            /* is store pending */
            jp = regs[RA];
            if (jp != 0) {
                if (jp == ib)
                    goto L90;
                emit(LD, jp, RA);
                regs[RA] = 0;
            }
            emit(LD, RA, ib);
        } else {

            /* is store pending */
            jp = regs[RA];
            if (jp != 0) {
                if (jp == ib)
                    goto L90;
                emit(LD, jp, RA);
                regs[RA] = 0;
            }
            if (cyflag == 0)
                emit(XR, RA, 0);
            if (cyflag == 1)
                emit(LD, RA, 0);
        }
    L90:if (lp != 0)

        /* op2 IN gpr's - perform ACC-register operation */
        emit(op2, lp / 16, 0);

    else if (k < 0)

        /* perform ACC-memory operation */
        emit(op2, ME, 0);

    else

        /* yes - perform ACC-immediate operation */
        /* use CMA if op1 is XR AND op2 is 65535 */
        if (op2 != XR || k != 65535)
            emit(op2, -(k / 256), 0);

        else
            emit(CMA, 0, 0);
    } else {

        /* second operand is single byte */
        if (prec[sp - 1] < 2)
            goto L110;

        /* may NOT need to perform operations for certain operators, but ... */
        /* perform operation with h.o. byte of op1 */
        /* op1 must be IN the gpr's - perform dummy operation with ZERO */
        jp = regs[RA];
        if (jp != 0) {
            if (jp == ib)
                goto L100;
            emit(LD, jp, RA);
            regs[RA] = 0;
        }
        emit(LD, RA, ib);
    L100:emit(op2, 0, 0);
    }

    /* set up pending register store */
    regs[RA] = ib;

    /* save the pending accumulator - register store */
L110:jp = regs[RA];
    _delete(2);
    regs[RA] = jp;
    sp = sp + 1;
    prec[sp] = 1;
    rasn[sp] = ib * 16 + ia;
    lock[ia] = 0;
    st[sp] = 0;
    litv[sp] = -1;
    regs[ia] = sp;
    regv[ia] = -1;
    if (ib > 0) {
        prec[sp] = 2;
        regs[ib] = sp;
        lock[ib] = 0;
        regv[ib] = -1;
    }
    return;
L120:error(107, 5);
    return;
}

void genreg(const int np, int *ia, int *ib) {
    int n, idump, ip, i, j, k, jp;

    /* generate n free registers for subsequent operation */
    n = iabs(np);

    /* n is number of registers, np negative if no pushing allowed */
    *ib = 0;
    *ia = 0;
    idump = 0;
    for (;;) {

        /* look for free RC OR RE AND allocate IN pairs (RC/RB,RE/RD) */
        k = RC;
        if (regs[k] == 0)
            break;
        k = RE;
        if (regs[k] == 0)
            break;
        if (idump > 0) {
            *ia = 0;
            return;
        } else {
            if (np >= 0) {
                ip = 0;

                /* generate temporaries IN the stack AND RE-try */
                /* search for lowest register pair assignment IN stack */
                if (sp > 0) {
                    for (i = 1; i <= sp; i++) {
                        k = rasn[i];
                        if (k == 0) {
                            if (st[i] == 0 && litv[i] < 0)
                                ip = 0;
                        } else if (k <= 255) {
                            j = k % 16;
                            if (lock[j] == 0) {
                                jp = k / 16;
                                if (jp != 0)

                                    /* otherwise check ho register */
                                    if (lock[jp] != 0 || jp != j - 1)
                                        continue;
                                if (ip == 0)
                                    ip = i;
                            }
                        }
                    }
                    if (ip != 0) {

                        /* found entry to PUSH at ip */
                        j = rasn[ip];
                        jp = j / 16;
                        j = j % 16;
                        regs[j] = 0;
                        if (jp > 0)
                            regs[jp] = 0;

                        /* check pending register store */
                        k = regs[RA];
                        if (k != 0) {
                            if (k == j)
                                emit(LD, j, RA);

                            else {
                                if (k != jp)
                                    goto L130;

                                /* store into ho register */
                                emit(LD, jp, RA);
                            }
                            regs[RA] = 0;
                        }

                        /* free the register for allocation */
                    L130:stack(1);
                        emit(PUSH, j - 1, 0);

                        /* mark element as stacked (st=0, rasn=0) */
                        rasn[ip] = 0;
                        st[ip] = 0;
                        litv[ip] = -1;
                        continue;
                    }
                }
            }
            idump = 1;
            saver();
        }
    }
    *ia = k;
    if (n > 1)
        *ib = *ia - 1;
    return;
}

void loadsy() {
    int SIGN, attrib, m, l, mp, i, j, k;

    /* save the current input file number */
    m = C_INPUT;
    C_INPUT = C_USYMBOL;
    ibp = 9999;

    while ((i = gnc(0)) == SPACE)
        ;

    /* look for initial '/' */
    if (i == SLASH) {
        while ((i = gnc(0)) != SLASH) {
            /* load the interrupt vector */
            if (i < CHZERO || i > SEVEN)
                goto L140;
            i = i - CHZERO + 1;    // convert to index into intpro

            /* get the procedure name corresponding to interrupt i-1 */
            for (j = 0, l = 1; (k = gnc(0)) != SLASH; l *= 32) {
                k -= CHZERO;      // convert to base 32 number
                if (k < 0 || k > 31)
                    goto L140;
                j += k * l;
            }
            intpro[i] = j;
            if (C_SYMBOLS >= 2)
                form("\n I%d=S%05d\n", i - 1, j);
        }

        /* interrupt procedures are handled. */
        while ((i = gnc(0)) == SPACE)
            ;

        if (i == SLASH) {
            while ((i = gnc(0)) != SLASH) {  // process next symbol table entry
                if (++sytop >= syinfo) {
                    error(108, 5);
                    syinfo = symax;
                }
                if (C_SYMBOLS >= 2)     // write symbol number AND symbol table address                 
                    form("\n S%05d", sytop);

                symbol[sytop] = syinfo--;
                attrib = syinfo;
                for (;;) {
                    SIGN = 0;
                    if (i == SPACE)
                        SIGN = 1;
                    if (i == 45)
                        SIGN = -1;
                    if (SIGN == 0)
                        goto L140;
                    for (l = 1, k = 0; CHZERO <= (i = gnc(0)) && i <= CHV; l *= 32)
                        /* get next digit */
                        k += (i - CHZERO) * l;

                    /* END of number */
                    if (syinfo <= sytop) {
                        error(109, 5);
                        syinfo = symax;
                    }
                    if (C_SYMBOLS >= 2)     // write symbol table address AND entry
                        form("\n    %05d %c%08XH", syinfo, SIGN == -1 ? '-' : ' ', k);
                    symbol[syinfo--] = SIGN * k;

                    /* look for '/' */
                    if (i == SLASH) {

                        /* check for special case at END of an entry */
                        attrib = iabs(symbol[attrib]);
                        i = attrib % 16;
                        if (i != PROC && i != VARB) {
                            if (i != LABEL)
                                break;

                            /* check for single reference to the LABEL */
                            j = attrib / 256;
                            if (j != 1)
                                j = 0;

                        }
                        /* allocate a cell AND set to ZERO */

                        // arrive here with proc, varb or label
                        switch (i) {
                        case VARB: j = 1; break;
                        case PROC: j = 3; break;
                        case LABEL: j++; break;
                        }

                        for (i = 1; i <= j; i++)
                            symbol[syinfo--] = 0;

                        break;
                    }
                }
            }

            /* assign relative memory addresses to variables IN symbol table */
            i = sytop;

            /* 65536 = 65280 + 256 */
            lmem = 65280;
            while (i > 0) {

                /* process next symbol */
                mp = symbol[i];
                l = -1;
                k = symbol[mp - 1];

                /* k contains attributes of variable */
                if (k >= 0)
                    if (right(k, 4) == VARB) {

                        /* otherwise type is VARB */
                        k = shr(k, 4);
                        l = right(k, 4);
                        k = shr(k, 4);

                        /* l is element size, k is number of elements */
                        if (l > 2)

                            /* probably an inline data variable */
                            l = -1;

                        else {
                            if (lmem % 2 == 1 && l == 2)
                                lmem = lmem - 1;

                            /* mem is at the proper boundary now */
                            lmem = lmem - l * k;
                            if (lmem < 0) {
                                error(110, 1);
                                lmem = 65280;
                            }
                            l = lmem;
                            if (C_SYMBOLS != 0)
                                if (i > 4 && i != 6)

                                    /* write OUT address assignment */
                                    form("\n S%05d=%05d", i, l);
                        }
                    }
                symbol[mp] = l;
                i = i - 1;
            }
            goto L150;
        }
    }
L140:error(111, 1);

    /* now assign the last address to the variable 'memory' */
    /* ** note that 'memory' must be at location 5 IN the symbol table ** */
L150:i = symbol[5];
    symbol[i] = 65280;
    if (C_SYMBOLS != 0)
        writel(0);
    C_INPUT = m;
    return;
}

void loadv(const int is, const int typv) {
    int s, m, l, lp, jp, i, j, k, typ;
    static int ia, ib;

    /* load value to register if NOT a literal */
    /* typ = 1 if call from 'apply' IN which case the l.o. byte is */
    /* loaded into the accumulator instead of a gpr. */
    /* if typ = 2, the address is loaded, but the variable is NOT. */
    /* if typ = 3, a double byte (address) fetch is forced. */
    /* if typ = 4 then do a quick load into h AND l */
    /* if typ = 5, a double byte quick load into h AND l is forced */
    i = 0;
    s = is;
    typ = typv;
    if (typ != 2) {
        if (rasn[s] > 255)
            cvcond(s);
        if (typ == 4 || typ == 5) {
            m = litv[s];
            i = rasn[s];
            k = st[s];
            if (i != 0) {

                /* registers are assigned */
                j = regs[RA];
                l = i % 16;
                i = i / 16;
                if (j != 0 && j == i)
                    i = RA;
                if (j != 0 && j == l)
                    l = RA;
                if (l == RE && i == RD)
                    emit(XCHG, 0, 0);

                else {

                    /* NOT IN d AND e, so use two byte move */
                    emit(LD, RL, l);

                    /* note that the following may be a lhi 0 */
                    emit(LD, RH, i);
                }
            } else {
                if (k != 0) {

                    /* variable , literal  OR address reference */
                    if (k <= 0) {

                        /* ADR ref - set h AND l with litadd */
                        litadd(sp);
                        goto L170;
                    } else

                        /* simple variable OR literal ref, may use LHLD */
                        /* may want to check for possible INX OR DCX, but now... */
                        if (m < 0) {
                            m = regv[RH];
                            l = regv[RL];
                            if (m != -3 || (-l) != k)
                                if (m == (-4) && (-l) == k)
                                    emit(DCX, RH, 0);

                                else {
                                    j = chain(k, codloc + 1);
                                    emit(LHLD, j, 0);
                                }
                            regv[RH] = -1;
                            regv[RL] = -1;
                            if (prec[s] <= 1 && typ != 5)

                                /* this is a single byte value */
                                emit(LD, RH, 0);

                            else {
                                regv[RH] = -3;
                                regv[RL] = -k;
                            }
                            goto L170;
                        }
                } else if (m < 0) {

                    /* value stacked, so... */
                    ustack();
                    emit(POP, RH, 0);
                    if (prec[s] < 2)
                        emit(LD, RH, 0);
                    goto L160;
                }

                /* literal value to h l */
                emit(LXI, RH, m);
                regv[RH] = m / 256;
                regv[RL] = m % 256;
                return;
            }
        L160:regv[RH] = -1;
            regv[RL] = -1;
        L170:if (rasn[s] == 0)
            rasn[s] = RH * 16 + RL;
        return;
        } else {
            if (rasn[s] > 0)
                return;

            /* check for previously stacked value */
            if (st[s] != 0 || litv[s] >= 0) {

                /* no registers assigned.  allocate registers AND load value. */
                i = prec[s];
                if (typ == 3) {

                    /* force a double byte load */
                    i = 2;
                    typ = 0;
                }
                genreg(i, &ia, &ib);

                /* ia is low order byte, ib is high order byte. */
                if (ia <= 0) {
                    error(112, 5);
                    return;
                }
            } else {
                genreg(2, &k, &i);

                /* check to ensure the stack is in good shape */
                i = s + 1;
                for (;;) {
                    if (i <= sp) {
                        if (st[i] == 0 && rasn[i] == 0 && litv[i] < 0)

                            /* found another stacked value */
                            error(147, 1);
                        i = i + 1;
                    } else {

                        /* available cpu register is based at k */
                        emit(POP, k - 1, 0);
                        regs[k] = s;
                        if (prec[sp] >= 2) {
                            regs[k - 1] = s;
                            k = (k - 1) * 16 + k;
                        }
                        rasn[s] = k;
                        // V4
                        if (typ == 1) {
                            if (regs[RA])
                                emit(LD, regs[RA], RA);
                            emit(LD, RA, k);
                        }


                        /* decrement the stack count for this level */
                        ustack();
                        break;
                    }
                }
                return;
            }
        }
    }

    /* check for literal value (IN arith exp) */
    l = litv[s];
    if (l >= 0 && l <= 65535) {
        //V4
        litv[s] = -1;

        lp = l % 256;
        regs[ia] = s;
        regv[ia] = lp;
        if (typ == 1) {

            /* check for pending register store */
            jp = regs[RA];
            if (jp != 0) {

                /* store ACC into register before continuing */
                emit(LD, jp, RA);
                regs[RA] = 0;
            }
            if (lp == 0)
                emit(XR, RA, 0);
            if (lp != 0)
                emit(LD, RA, -lp);
            if (ib != 0) {
                emit(LD, ib, -l / 256);
                regs[ib] = s;
                regv[ib] = -l;
            }
        } else {
            /* typ = 0, load directly into registers */
            /* may be possible to LXI */
            if (ib != ia - 1) {
                emit(LD, ia, -lp);
                if (ib != 0) {
                    emit(LD, ib, -l / 256);
                    regs[ib] = s;
                    regv[ib] = -l;
                }
            } else {
                emit(LXI, ib, l);
                regs[ib] = s;
                regv[ib] = -l;
            }
        }
        rasn[s] = ib * 16 + ia;
    } else {
        /* otherwise fetch from memory */
        sp = sp + 1;
        j = st[s];
        setadr(j);
        litadd(sp);

        /* address of variable is IN h AND l */
        jp = typ + 1;
        switch (jp) {
        case 1:

            /* call from gensto (typ = 0) */
            emit(LD, ia, ME);
            break;
        case 2:

            /* call from apply to load value of variable */
            jp = regs[RA];

            /* check for pending register store */
            if (jp != 0) {

                /* have to store ACC into register before reloading */
                emit(LD, jp, RA);
                regs[RA] = 0;
            }
            emit(LD, RA, ME);
            break;
        case 3:         // goto removed
            break;
        }

        /* check for double byte variable */
        if (typ != 2 && i > 1) {    // avoids goto when typ == 2

            /* load high order byte */
            emit(IN, RL, 0);
            regv[RL] = regv[RL] + 1;
            emit(LD, ib, ME);
        }

        /* value is now loaded */
        _delete(1);
        if (typ != 2) {
            rasn[s] = ib * 16 + ia;
            if (ib != 0)
                regs[ib] = s;
            regs[ia] = s;
            if (ib != 0)
                regv[ib] = -1;
            regv[ia] = -1;
        }
    }
    return;
}

void setadr(const int val) {    // set top of stack to address reference
    int i, j;

    alter = true;
    if (sp > maxsp) {
        error(113, 5);
        sp = 1;
    } else {    // mark as address reference
        st[sp] = -val;
        i = symbol[val];
        j = iabs(symbol[i - 1]);
        prec[sp] = right(shr(j, 4), 4);
        i = symbol[i];
        if (i < 0) {
            j = 0;
            i = -i;
        } else
            j = 1 << 16;
        litv[sp] = j + right(i, 16);
        rasn[sp] = 0;
    }
    return;
}

void ustack() {
    int i;

    /* decrement curdep AND check for underflow */
    i = curdep[prsp + 1];
    if (i > 0)
        curdep[prsp + 1] = i - 1;

    else
        error(148, 1);
    return;
}

int chain(const int sy, const int loc) {
    int i, j, _chain;

    /* chain IN double-byte refs to symbol sy, if necessary */
    i = symbol[sy];
    j = symbol[i];
    if (j < 0)
        _chain = -j % 65536;    // absolute address already assigned
    else {                      // backstuff required
        i -= 2;
        _chain = symbol[i];
        symbol[i] = loc;
    }
    return _chain;
}

void gensto(const int keep) {
    int l, i1, i2, lp, jp, i, j, k;
    static int iq;

    /* keep = 0 if STD, keep = 1 if STO (value retained) */
    /* generate a store into the address at stack top */
    /* load value if NOT literal */
    l = litv[sp - 1];
    if (l < 0)
        loadv(sp - 1, iq = 0);

    i1 = rasn[sp - 1];
    i2 = i1 % 16;
    i1 = i1 / 16;

    /* check for pending register store */
    jp = regs[RA];
    if (jp != 0) {
        if (jp == i1)
            i1 = 1;
        if (jp == i2)
            i2 = 1;
    }

    /* ** note that this assumes 'stackptr' is at 6 IN sym tab */
    if (-st[sp] != 6) {
        if (litv[sp] < 0) {
            i = rasn[sp];
            if (i <= 0)

                /* registers NOT allocated - check for stacked value */
                if (st[sp] == 0) {
                    emit(POP, RH, 0);    // address is stacked so POP to h AND l
                    ustack();
                    goto L210;
                } else {

                    /* check for ref to simple based variable */
                    i = st[sp];
                    if (i <= intbas) {
                        if (i2 != 0)
                            lock[i2] = 1;
                        if (i1 != 0)
                            lock[i1] = 1;

                        /* force a double byte fetch into gprs */
                        loadv(sp, 3);
                        i = rasn[sp];
                    } else {

                        /* may be able to simplify (OR eliminate) the LHLD */
                        k = regv[RH];
                        lp = regv[RL];
                        if (k != -3 || (-lp) != i)
                            if (k == (-4) && (-lp) == i) {
                                emit(DCX, RH, 0);
                                regv[RH] = -3;
                            } else {
                                j = chain(i, codloc + 1);
                                emit(LHLD, j, 0);
                                regv[RH] = -3;
                                regv[RL] = -i;
                            }
                        goto L220;
                    }
                }
            jp = regs[RA];
            j = i % 16;
            i = i / 16;
            if (i2 == 0 || i != j - 1) {
                if (j == jp)
                    j = RA;
                if (i == jp)
                    i = RA;
                if (i != RD || j != RE) {
                    emit(LD, RL, j);
                    emit(LD, RH, i);
                    goto L210;
                }
            } else

                /* if prevous syllable is XCHG then do another - peep will fix it */
                if (i != RD || lastex != codloc - 1) {

                    /* use STAX - set up accumulator */
                    if (i2 != 1) {
                        if (jp != 0)
                            emit(LD, jp, RA);
                        if (i1 == 1)
                            i1 = jp;
                        emit(LD, RA, i2);
                        regs[RA] = 0;
                    }
                    emit(STAX, i, 0);

                    /* ***** */
                    /* if byte dest we are done */
                    if (prec[sp] >= 2) {

                        /* ***** */
                        emit(INCX, i, 0);
                        if (i1 != 0) {

                            /* ***** */
                            /* store high order byte */
                            if (i2 == 1 && keep != 0) {
                                emit(LD, rasn[sp - 1] % 16, RA);
                                regs[RA] = 0;
                            }
                            emit(LD, RA, i1);
                            emit(STAX, i, 0);
                        } else {

                            /* ***** */
                            /* store high order ZERO */
// V4
                            if (i2 == 1 && keep != 0)

                                emit(LD, rasn[sp - 1] % 16, RA);
                            regs[RA] = 0;
                            emit(XR, RA, 0);
                            emit(STAX, i, 0);
                        }
                    }

                    /* ***** */
                    _delete(1);
                    return;
                }
            emit(XCHG, 0, 0);
        L210:if (i1 != 0)
            lock[i1] = 0;
        if (i2 != 0)
            lock[i2] = 0;
        regv[RH] = -1;
        regv[RL] = -1;
        } else

            /* otherwise this is a literal address */
            /* if possible, generate a SHLD */
            if (i1 != RD || i2 != RE || lastex != codloc - 1
                || prec[sp] != 2)
                litadd(sp);

            else {
                emit(XCHG, 0, 0);
                i = iabs(st[sp]);
                j = chain(i, codloc + 1);
                emit(SHLD, j, 0);
                regv[RH] = -3;
                regv[RL] = -i;
                if (keep != 0)
                    emit(XCHG, 0, 0);
                goto L230;
            }

        /* we may change mov r,m inr r mov m,r to inr m. */
        /* if so, AND this is a non-destructive store, the register */
        /* assignment must be released. */
    L220:iq = lastir;

        /* generate low order byte store */
        if (i2 == 0)

            /* immediate store */
            emit(LD, ME, -(iabs(l) % 256));

        else
            emit(LD, ME, i2);

        /* now store high order byte (if any) */
        if (prec[sp] != 1) {

            /* a double byte store */
            i = 0;

            /* store second byte */
            emit(INCX, RH, 0);

            /* regv(RH) = -3 then LHLD has occurred on symbol -regv(RL) */
            /* regv(RH) = -4 then LHLD AND INCX h has occurred */
            j = regv[RH];
            if (j >= 0)
                regv[RL] = regv[RL] + 1;

            else {
                regv[RH] = -4;
                if (j != -3) {

                    /* RH AND RL have unknown values */
                    regv[RH] = -1;
                    regv[RL] = -1;
                }
            }
            if (prec[sp - 1] >= 2)
                if (i1 == 0)

                    /* second byte is literal */
                    i = l / 256;

                else {

                    /* LD memory from register */
                    emit(LD, ME, i1);
                    goto L230;
                }
            emit(LD, ME, -iabs(i));
        }
    } else if (i2 == 0)

        /* load sp immediate */
        emit(LXI, RSP, l);

    else {
        emit(LD, RL, i2);
        regv[RL] = -1;
        emit(LD, RH, i1);
        regv[RH] = -1;
        emit(SPHL, 0, 0);
    }

    /* now release register containing address */
    /* release register assignment for value */
    /* if mov r,m inr r mov m,r was changed to inr m. */
L230:if (iq != codloc)
_delete(1);

else {
    i = -st[sp];
    _delete(2);
    sp = sp + 1;
    st[sp] = i;
    rasn[sp] = 0;
    prec[sp] = 1;
    litv[sp] = -1;
}
return;
}

void litadd(const int s) {
    int ih, il, ir, kp, it, lp, l, jp, i, j, k;

    /* load h AND l with the address of the variable at s IN */
    /* the stack */
    ih = litv[s];
    il = ih % 256;
    ih = ih / 256;
    ir = RH;
    l = ih;
    if (ih < 0)
        error(114, 1);

    else {

        /* deassign registers */
        i = rasn[s];
        if (i != 103) {

            /* 6*16+7 = 103 */
            jp = regs[RA];
            for (j = 1; j <= 2; j++) {
                k = i % 16;
                i = i / 16;
                if (k != 0) {
                    if (k == jp)
                        regs[RA] = 0;
                    regs[k] = 0;
                    lock[k] = 0;
                    regv[k] = -1;
                }
            }
            rasn[s] = 0;
            for (i = 6; i <= 7; i++) {
                j = regs[i];
                if (j != 0) {
                    k = rasn[j];
                    kp = k % 16;
                    k = k / 16;
                    if (k == i)
                        k = 0;
                    if (kp == i)
                        kp = 0;
                    rasn[j] = k * 16 + kp;
                }
                lp = regv[i];
                if (lp != l)
                    if (lp == l + 1)
                        emit(DC, ir, 0);

                    else {
                        if (lp == l - 1)
                            if (l != 0) {
                                emit(IN, ir, 0);
                                goto L240;
                            }
                        if (i == 6)

                            /* no INC/dec possible, see if l does NOT match */
                            if (il != regv[RL]) {
                                regv[RL] = il;
                                if (l <= 255) {

                                    /* otherwise this is a real address */
                                    emit(LXI, RH, il + ih * 256);
                                    goto L240;
                                } else {

                                    /* the LXI must be backstuffed later */
                                    it = st[s];
                                    if (it >= 0)
                                        goto L250;
                                    it = -it;
                                    it = symbol[it];
                                    j = symbol[it - 2];

                                    /* place reference into chain */
                                    emit(LXI, RH, j);
                                    symbol[it - 2] = codloc - 2;
                                    goto L240;
                                }
                            }
                        if (l <= 255)
                            emit(LD, ir, -l);

                        else {

                            /* the address must be backstuffed later */
                            it = st[s];
                            if (it >= 0)
                                goto L250;
                            it = iabs(it);
                            it = symbol[it];
                            j = symbol[it];
                            if (j <= 0) {
                                error(116, 1);
                                break;
                            } else {

                                /* place link into code */
                                k = shr(j, 16);
                                symbol[it] =
                                    shl(codloc + 1, 16) + right(j, 16);
                                kp = k % 256;
                                k = k / 256;
                                emit(0, k, 0);
                                emit(0, kp, 0);
                            }
                        }
                    }

                /* fix values IN stack AND reg */
            L240:if (i == 7)
                rasn[s] = 103;

            /* 103 = 6*16+7 */
            regs[i] = s;
            regv[i] = l;
            l = il;
            ir = RL;
            }
            return;
        L250:error(115, 1);
        }
    }
    return;
}

/*
 * looking at the callers fa = 16 on all paths
 * fe = 1 OR 16
 * debase = 16 so fr = 16
 */
void dump(const int l, const int u, const int fa, const int fe) {
    bool same;
    int fr, a, b, wr, opcnt, w, rr, nsame, ls, lp, i, j, k;

    lp = l;
    w = C_WIDTH;
    a = 5;
    b = 3;

    //    if (fa == 8)
    //        a = 6;
    if (fe != 1) {

        //        if (fe == 2)
        //            b = 9;
        //        if (fe == 8)
        //            b = 4;
    } else {

        /* symbolic dump */
        b = 6;
        fr = debase;
        if (fr == 2)
            fr = 16;
        wr = 2;
        if (fr == 10)
            wr = 3;
        rr = 6 - wr;
        if (fr != 10)
            rr = rr - 1;
    }
    w = (w - a) / (b + 1);

    /* w is number of entries on each line */
    if (w == 0)
        error(117, 1);

    else {
        if (fa != 10)
            a = a - 1;
        if (fe != 10)
            b = b - 1;

        /* a is the width of the address field */
        /* b is the width of each entry */
        for (i = 1; i <= 29; i++)
            accum[i] = 256;
        nsame = 0;
        opcnt = 0;
        for (;;) {
            same = true;
            ls = lp;
            i = 0;
            for (;;) {
                if (lp > u) {
                    same = false;
                    break;
                } else {
                    i = i + 1;
                    j = get(lp);
                    lp = lp + 1;
                    j = j % 256;
                    if (j != accum[i])
                        same = false;
                    accum[i] = j;
                    if (i >= w)
                        break;
                }
            }
            if (same) {
                nsame = nsame + 1;
                if (nsame <= 1)
                    form("\n \n");
            } else {
                if (i == 0)
                    break;
                form("\n%0*XH", a, ls);
                for (j = 1; j <= i; j++) {
                    putch(' ');
                    k = accum[j];
                    if (opcnt > 0) {
                        opcnt = opcnt - 1;
                        form("%0*XH%*c", wr, k, rr, ' ');
                    } else if (fe != 1)
                        form("%0*XH", b, k);

                    else
                        opcnt = decode(1, k, 6);
                }
                if (lp > u) {
                    writel(0);
                    break;
                }
            }
        }
    }
    return;
}

int decode(const int cc, const int i, const int w) {
    int x, y, insize, ip, m, j, k, l, _decode;

    /*          *****************************************
                *instruction * decoding * using * ctran *
                *****************************************
                the elements of ctran represent the 8080 operation codes IN a
                form which is more usable for instruction decoding IN both the
                decode and interp subroutines.  given an instruction i (between 0
                and 255), ctran(i+1) provides an alternate representation of the
                instruction, as shown below...
                5b    5b     5b       OR     5b     3b  2b    5b
                ------------------          -----------------------
                /    /     /     /          /      /    /    /    /
                / x  /  y  /   i /          / x    / y1 /y2  /  i /
                /    /     /     /          /      /    /    /    /
                ------------------          -----------------------
                where field i specifies a 'category' and the x and y fields
                qualify instructions within the category.
                field i       category         value of x and y fields
                ------   ----------------- ----------------------------------------
                0           mov         the fields indicate the valid operands
                involved...
                ACC=0, b = 1, c = 2, d = 3, e = 4, h = 5,
                l = 6, m = 7, i = 8, sp= 9 (m is memory
                referencing instruction, and i is immed)
                thus, /3/5/0/ is a mov d,h instruction.

                1     increment, decre- the value of x determines the instruc-
                ment, arithmetic, tion within the category..
                OR logical        inr = 1, cdr = 2, add = 3, adc = 4,
                sub = 5, sbc = 6, ana = 7, xra = 8,
                ora = 9, cmp = 10
                the value of y determines the valid
                register involved, as above.  thus,
                /3/4/1/ is an add e instruction.
                ------   ----------------- ----------------------------------------
                2     jump, call, OR    the value of x determines the exact IN-
                return         struction.. jump=1, call=2, return=3
                the subfield y1 determines the orienta-
                tion of the condition.. t=1, f=0
                the value of subfield y2 gives the con-
                dition.. CY=0, z=1, s=2, p=3.
                thus, /3/0/1/2/ is an rfz (return false
                ZERO) instruction.
                ------   - --------------  ----------------------------------------
                3     miscellaneous     the value of the y field determines the
                instruction (the x field gives the value
                of aaa IN the RST instruction)
                rlc  = 1  rrc  = 2  ral  = 3  rar  = 4
                JMP  = 5  call = 6  ret  = 7  RST  = 8
                IN   = 9  OUT  = 10 hlt  = 11 STA  = 12
                LDA  = 13 XCHG = 14 xthl = 15 SPHL = 16
                PCHL = 17 CMA  = 18 STC  = 19 CMC  = 20
                DAA  = 21 SHLD = 22 LHLD = 23 EI   = 24
                DI   = 25 nop  = 26 27 --- 31 undefined
                (ibytes gives number of bytes following
                the first 23 instructions of this group)
                -------   ---------------- ---------------------------------------
                4 - 11   instructions RE  the y field gives a register pair num-
                quiring a regis  ber a = 0, b = 1, d = 3, h = 5, sp = 9
                ter pair
                the instructions IN each category are
                determined by the i field..
                LXI  = 4  PUSH = 5  POP  = 6
                DAD  = 7  STAX = 8  LDAX = 9
                inx  = 10 DCX  = 11
                -------   ---------------- ---------------------------------------
                */
    insize = 284;
    ip = ctran[i + 1];
    x = ip / 1024;
    y = (ip / 32) % 32;
    ip = ip % 32 + 1;
    _decode = 0;

    /* point to the proper category */
    /* (the first two are for condition codes AND register designations) */
    j = insym[ip + 2];

    /* select the proper instruction code within the category */
    switch (ip) {
    case 1:			/* mov */
    case 2:			/* inr ... cmp */
        k = (ip == 1) ? 1 : x;

        /* may be an immediate operation */
        if (y == 8)
            _decode = 1;
        break;
    case 3:

        /* jump call OR return conditionally */
        k = x;
        if (x != 3)
            _decode = 2;
        break;
    case 4:

        /* rlc ... NOP */
        k = y;

        /* check for JMP */
        if (y <= 23)

            /* rlc ... LDA */
            _decode = ibytes[y];
        break;
    case 5:			/* LXI */
        _decode = 2;
    default:			/* LXI ... DCX */
        k = 1;
    }
    j = j + k;
    l = insym[j];
    j = insym[j + 1];
    _form(cc, insym, l, j - 1, insize);
    l = j - l;
    if (ip == 4) {		/* check for RST (if found ADD decimal number) */
        if (y == 8) {

            /* found RST instruction */
            form("%d", x);
            l = l + 2;
        }
    } else if (ip == 3) {	/* conditional */
        j = insym[2] + 1 + y;
        k = insym[j];
        j = insym[j + 1];
        _form(1, insym, k, j - 1, insize);
        l = l + j - k;
    } else {			/* write register reference */

        /* opcode is written.  l characters are IN buffer, check for more */
        putch(' ');
        for (;;) {
            m = y;
            if (ip == 1)
                m = x;
            j = insym[1] + 1 + m;
            k = insym[j];
            j = insym[j + 1];
            _form(1, insym, k, j - 1, insize);
            l = l + j - k + 1;
            if (ip != 1)
                break;
            ip = 0;
        }
    }
    if (l < w)
        form("%*c", w - l, ' ');
    return _decode;
}

void emit(const int opr, const int opa, const int opb) {
    int opcode, operand, n, i;

    unsigned char cbits[43 + 1] =
    { ZPAD, 64, 4, 5, 128, 136, 144, 152, 160, 168, 176, 184, 7, 195, 194,
    205, 196, 201, 192, 199, 219, 211, 118, 50, 58, 235, 249, 233, 47, 55, 63, 39,
    34, 42, 251, 243, 1, 197, 193, 9, 2, 10, 3, 11
    };
    unsigned char cc[2][4] = { { 0x10, 0x00, 0x30, 0x20 }, { 0x18, 0x08, 0x38, 0x28 } };
    // fortan used bits(3)
    // bits(1) -> opcode
    // bits(2) -> operand low
    // bits(3) -> operand high

    /* the following comments are sample calls to the emit */
    /* routine.  note that emit requires three argument at all times */
    /* (the unused arguments are ZERO). */

    /* call emit(LD,RA,RB) */
    /* call emit(LD,RC,-34) */
    /* call emit(LD,RD,ME) */
    /* call emit(LD,ME,RE) */
    /* call emit(IN,RH,0) */
    /* call emit(DC,RL,0) */
    /* call emit(AD,RB,0) */
    /* call emit(AD,ME,0) */
    /* call emit(AD,-5,0) */
    /* call emit(SU,RB,0) */
    /* call emit(SB,ME,0) */
    /* call emit(ND,-5,0) */
    /* call emit(XR,0,0) */
    /* call emit(OR,RB,0) */
    /* call emit(CP,RH,0) */
    /* call emit(ROT,ACC,LFT) */
    /* call emit(ROT,CY,LFT) */
    /* call emit(ROT,CY,RGT) */
    /* call emit(JMP,148,0) */
    /* call emit(JMC,TRU*32+ZERO,148) */
    /* call emit(CAL,1048,0) */
    /* call emit(CLC,FAL*32+PARITY,148) */
    /* call emit(RTN,0,0) */
    /* call emit(RTC,FAL*32+CARRY,255) */
    /* call emit(RST,3,0) */
    /* call emit(INP,6,0) */
    /* call emit(OUT,10,0) */
    /* call emit(HALT,0,0) */
    /* emit a literal between 0 AND 255 */
    /* call emit(0,44,0) */
    /* call emit(STA,300,0) */
    /* call emit(LDA,300,0) */
    /* call emit(XCHG,0,0) */
    /* call emit(SPHL,0,0) */
    /* call emit(PCHL,0,0) */
    /* call emit(CMA,0,0) */
    /* call emit(STC,0,0) */
    /* call emit(CMC,0,0) */
    /* call emit(DAA,0,0) */
    /* call emit(SHLD,300,0) */
    /* call emit(LHLD,300,0) */
    /* call emit(EI,0,0) */
    /* call emit(DI,0,0) */
    /* call emit(LXI,(RB,RD,RH,RSP),300) */
    /* call emit(PUSH,(RB,RD,RH,RA),0) */
    /* call emit(POP,(RB,RD,RH,RA),0) */
    /* call emit(DAD,(RB,RD,RH,RSP),0) */
    /* call emit(STAX,(RB,RD),0) */
    /* call emit(LDAX,(RB,RD),0) */
    /* call emit(INX,(RB,RD,RH,RSP),0) */
    /* call emit(DCX,(RB,RD,RH,RSP),0) */
    n = 1;
    if (C_NUMERIC != 0)

        /* write emitter trace */
        form("\nE(%d,%d,%d)\n", opr, opa, opb);
    if (opr <= 0)
        opcode = opa;

    else {
        opcode = cbits[opr];
        switch (opr) {
        case LD:    /* mov group */
            if (opb <= 0) {	/* lri operation */
#if V4 == 2
                if (lastli == codloc - 2 && lastri / 2 == opa / 2 && lastri < 8 && lastri > 1) {
                    // here we have a lri operation following a lri to the other
                    // register in this pair (b/c,d/e,h/l) or to the same reg.
                    if (lastri == opa) { // here to same register
                        put(codloc - 1, -opb);
                        return;
                    }
                    // here to the other register in the pair
                    // we have to change the opcode to a lxi of the high reg (b,d,h)
                    // also we will only add 1 word and we can clear lastli
                    opcode = get(codloc - 2) / 16 * 16 + 1;
                    put(codloc - 2, opcode);
                    n = 1;
                    lastli = 0;
                    if (opa % 2 != 1) {
                        // here the low register (c,e,l) was already there so we just add
                        // the high reg.
                        opcode = -opb;
                    } else {
                        // here the high register (b,d,h) was already there so we need to
                        // insert the low register between the opcode and it.
                        opcode = get(codloc - 1);
                        put(codloc - 1, -opb);
                    }
                    //
                    //       here the lri didn-t follow a lri that we could change

                } else {
                    lastli = codloc;
                    lastri = opa;

#endif
                    n = 2;							/* 2 byte instruction */
                    opcode = regmap[opa] * 8 + 6;	/* gen MVI instruction */
                    operand = -opb;					/* and the value */
#if V4 == 2
                }
#endif
            } else {

                /* check for possible load register elimination */
                /* is this a lmr OR lrm instruction... */
                if (opa != ME) {

                    /* this is a load register from memory - maybe eliminate */
                    if (opb == ME && lastld == codloc - 1 && lastrg == opa)
                        return;
                }

                /* may change a mov r,m inr r mov m,r to inr m */
                else if (lastir != codloc - 1) {

                    /* this is a load memory from register operation - save */
                    lastld = codloc;
                    lastrg = opb;
                } else {
                    i = right(get(codloc - 1), 3) + 48;

                    /* the register load may have been eliminated... */
                    if (lastld != codloc - 2 || opb != lastrg) {
                        codloc = codloc - 1;
                        membot = membot - 1;
                    }
                    put(codloc - 1, i);
                    lastir = 0;
                    lastrg = 0;
                    lastld = 0;
                    if (lastin == codloc || lastin == codloc + 1)
                        lastin = codloc - 1;
                    return;
                }
                opcode += regmap[opa] * 8 + regmap[opb];
            }
            break;
        case IN: case DC:               /* inr & dcr */
            opcode += regmap[opa] * 8;	/* gen inr / dcr */
            break;
            /* Arith group */
        case AD: case AC: case SU: case SB: case ND: case XR: case OR: case CP:
            if (opa > 0)
                opcode += regmap[opa];
            else {		/* immediate operand */
                n = 2;  // default is 2 byte instruction
#if V4 == 2
                if (opa == 0) {
                    switch (opr) {
                    case AD: case SU: case XR: case OR:
                        // replace adi,0 sui,0 xri,0 or ori 0 with ora,a
                        opcode = cbits[OR] + 7;
                        n = 1;
                        break;
                    case ND:    // replace ani,0 with xra,a
                        opcode = cbits[XR] + 7;
                        n = 1;
                        break;
                    }
                }
#endif
                if (n == 2) {   // gen imm arith instruction
                    opcode += 0x46;	/* gen imm arith instruction */
                    operand = -opa;     /* the immediate value*/
                }
            }
            break;
        case ROT:		/* rotate group */
            i = (opa - CY) * 2 + (opb - LFT);
            opcode += i * 8;
            break;

        case JMC: case CLC: /* condiitonal jmp & call */
            n = 3;
            operand = opb;
            /* fall through to common cc adjustment */
        case RTC:   /* condiitonal return */
            opcode += cc[opa / 32 - FAL][opa % 32 - CARRY];
            break;
        case RST:   /* rst xx */
            opcode += opa % 8 * 8;
            break;
        case INP: case OUT: /* in & out */
            n = 2;
            operand = opa;
            break;
        case JMP: case CAL: /* jmp & call */
        case STA: case LDA: case SHLD: case LHLD:		/* STA LDA SHLD LHLD */
            n = 3;
            operand = opa;
            break;
        case XCHG:
            if (lastex == codloc - 1) {         /* remove double xchg*/
                membot = membot - 1;
                codloc = codloc - 1;
                lastex = 0;
                return;
            }
            lastex = codloc;                    /* mark this xchg */
            break;
        case RTN: case HALT:    /* ret & halt */
        case SPHL: case PCHL: case CMA: case STC: case CMC: case DAA: case EI: case DI:
            break;
        case LXI:		/* LXI (get immediate part) */
            n = 3;                      /* 3 byte instruction */
            opcode += regmap[opa] * 8; /* merge in the register */
            operand = opb;             /* and the location */
            break;
        case PUSH:
            /* PUSH r - check for XCHG PUSH d combination. change to PUSH h */
            if (lastex == codloc - 1 && opa == RD) {
                membot--;
                codloc--;
                lastex = 0;
                opcode += regmap[RH] * 8;
                break;
            }
            /* else drop through... */
        case POP: case DAD: case STAX: case LDAX: case INCX: case DCX:
            /* adjust for push/pop psw */
            i = opa == RA ? 6 : regmap[opa];
            opcode += i * 8;
        }
    }
    i = alloc(n);
    codloc += n;
    put(i, opcode);
    if (n > 1) {
        put(++i, operand % 256);
        if (n > 2)
            put(++i, operand / 256);
    }
    return;
}

void puncod(const int lb, const int ub, const int mode) {
    int isum, up, kp, j, k, lp, l, i;
    int t[4 + 1];

    /* punch code from lower bound (lb) to upper bound (ub) */
    /* mode = 1  - - punch header only */
    /* mode = 2 - - punch trailer only */
    /* mode = 3 - - V2 punch header AND trailer , V4 punch code only */
    up = ub;
    lp = lb;
    writel(0);
    if (C_QUICKDUMP != 0) {


        if (mode == 2) {
            // write end of file record
            form(":00");
            int temp1 = offset + preamb;
            if (offset == 0 && C_STACKHANDLING != 0)
                temp1 = 0;
            form("%04X01", temp1);
            isum = (1 + (temp1 & 0xff) + (temp1 >> 8)) & 0xff;
            isum = (256 - isum) % 256;
            form("%2X", isum);
        } else {
            writel(0);
            l = C_QUICKDUMP;
            if (l < 16)
                l = 16;
            while (lp <= up) {
                kp = up - lp + 1;
                k = imin(kp, l);
                if (k == 0)
                    break;
                form(":%02X%04X00", k, lp);
                isum = k + right(lp, 8) + shr(lp, 8);
                for (i = 1; i <= k; i++) {
                    j = get(lp);
                    isum = isum + j;
                    lp = lp + 1;
                    form("%02X", j);
                }
                isum = right(isum, 8);
                isum = (256 - isum) % 256;
                form("%02X\n", isum);
                writel(0);
            }

        }

    } else {
        t[1] = 25;
        t[2] = 27;
        t[3] = 13;
        t[4] = 17;
        for (i = 1; i <= 4; i++)
            form("********************");
        writel(0);
        if (lp % 8 != 0)
            form("\n%d", lp);
        for (;;) {
            if (lp > up) {
                writel(0);
                for (i = 1; i <= 4; i++)
                    form("********************");
                writel(0);
                break;
            } else {
                if (lp % 4 == 0)
                    if (lp % 8 != 0)
                        form("\n        ");

                    else {
                        if (lp % 256 == 0) {

                            /* ********* */
                            writel(0);
                            for (i = 1; i <= 4; i++)
                                form("********************");
                        }
                        form("\n%d", lp);
                    }

                /* decode a memory location */
                putch(' ');
                _form(1, t, 3, 3, 4);
                k = get(lp);
                for (i = 1; i <= 8; i++) {
                    kp = k / (1 << (8 - i));
                    kp = kp % 2 + 1;
                    _form(1, t, kp, kp, 4);
                }
                _form(1, t, 4, 4, 4);
                lp = lp + 1;
            }
        }
    }
    return;
}

void cvcond(const int s) {
    int i, j, k, ia, jp;

    /* convert the condition code at s IN the stack to a boolean value */
    i = rasn[s];
    j = i / 256;
    k = j % 16;
    j = j / 16;
    ia = i % 16;

    /* j = 1 if true , j = 0 if false */

    /* k = 1 if CARRY, 2 if ZERO, 3 if SIGN, AND 4 if PARITY */

    /* we may generate a short sequence */
    if (k <= 2 && ia != 0 && regs[RA] == ia) {
        if (k != 2) {

            /* short conversion for true OR false CARRY */
            emit(SB, RA, 0);
            if (j == 0)
                emit(CMA, 0, 0);
        } else {

            /* short conversion for true OR false ZERO */
            if (j == 0)
                emit(AD, -255, 0);
            if (j == 1)
                emit(SU, -1, 0);
            emit(SB, RA, 0);
        }
    } else {

        /* do we have to assign a register */
        if (ia == 0) {
            genreg(1, &ia, &jp);
            if (ia != 0) {
                regs[ia] = sp;
                i = ia;
            } else {
                error(118, 5);
                return;
            }
        }

        /* check pending register store */
        jp = regs[RA];
        if (jp != 0)
            if (jp != ia) {
                emit(LD, jp, RA);
                regs[RA] = 0;
            }
        emit(LD, RA, -255);
        j = (FAL + j) * 32 + (CARRY + k - 1);
        emit(JMC, j, codloc + 4);
        emit(XR, RA, 0);
    }

    /* set up pending register store */
    regs[RA] = ia;
    rasn[s] = i % 256;
    return;
}

void saver() {
    int ic1, ic2, i1, i2, i, j, k, l;

    /* save the active registers AND reset tables */
    /* first determine the stack elements which must be saved */
    ic1 = 0;
    ic2 = 0;
    i1 = 0;
    i2 = 0;
    if (sp != 0) {
        for (j = 1; j <= sp; j++) {
            k = rasn[j];
            if (k > 255)
                cvcond(j);
            if (k > 0) {
                k = rasn[j];
                if (k >= 16) {

                    /* double byte */
                    l = k % 16;
                    k = k / 16;
                    if (lock[l] + lock[k] <= 0) {
                        st[j] = i2;
                        i2 = j;
                        ic2 = ic2 + 1;
                    }
                } else

                    /* single byte */
                    if (lock[k] != 1) {
                        st[j] = i1;
                        ic1 = ic1 + 1;
                        i1 = j;
                    }
            }
        }
        lmem = lmem - ic1 - (ic2 * 2);
        if (lmem % 2 * ic2 > 0 && ic1 == 0)
            lmem = lmem - 1;

        /* lmem is now properly aligned. */
        if (lmem < 0) {
            error(119, 1);
            return;
        } else {
            k = lmem;
            while (i1 + i2 != 0) {
                if (k % 2 == 1 || i2 == 0) {

                    /* single byte */
                    i = i1;
                    i1 = st[i];
                } else {

                    /* even byte boundary with double bytes to store */
                    i = i2;
                    i2 = st[i];
                }
                if (i <= 0) {
                    error(120, 1);
                    return;
                } else {

                    /* place temporary into symbol table */
                    sytop = sytop + 1;
                    st[i] = sytop;
                    symbol[sytop] = syinfo;
                    j = rasn[i];
                    l = 1;
                    if (j >= 16)
                        l = 2;
                    symbol[syinfo] = k;
                    k = k + l;
                    syinfo = syinfo - 1;
                    symbol[syinfo] = 256 + l * 16 + VARB;

                    /* length is 1*256 */
                    syinfo = syinfo - 1;

                    /* leave room for LXI chain */
                    symbol[syinfo] = 0;
                    syinfo = syinfo - 1;
                    if (sytop > syinfo) {
                        error(121, 5);
                        return;
                    } else {

                        /* store into memory */
                        l = rasn[i];
                        rasn[i] = 0;
                        sp = sp + 1;
                        setadr(sytop);
                        litadd(sp);
                        for (;;) {
                            i = l % 16;
                            if (i == regs[RA]) {
                                i = 1;
                                regs[RA] = 0;
                                regv[RA] = -1;
                            }
                            emit(LD, ME, i);
                            l = l / 16;
                            if (l == 0) {
                                _delete(1);
                                break;
                            } else {

                                /* double byte store */
                                emit(IN, RL, 0);
                                regv[RL] = regv[RL] + 1;
                            }
                        }
                    }
                }
            }
        }
    }
    for (i = 2; i <= 7; i++)
        if (lock[i] != 1) {
            regs[i] = 0;
            regv[i] = -1;
        }
    return;
}

void reloc() {
    int stloc;
    int stsize;
    int i, j, k, l, m, n, iw, ip;
    if (C_SYMBOLS >= 2) {
        for (i = 1; i <= sytop; i++)
            form("\n%4d=%6d", i, symbol[i]);
        for (i = syinfo; i <= symax; i++)
            form("\n%5d=%c%08XH", i, (symbol[i] < 0) ? '-' : ' ',
                iabs(symbol[i]));
    }

    /* compute max stack depth required for correct execution */
    stsize = maxdep[1];
    for (n = 1; n <= 8; n++) {
        i = intpro[n];
        if (i != 0) {

            /* get interrupt procedure depth */
            // V4
            i = symbol[i] - 4;
            i = symbol[i] + 1;

            /* note that i exceeds depth by 1 since RET may be pending */
            stsize = stsize + i;
        }
    }
    stsize = stsize * 2;
    n = stsize;
    if (C_STACKHANDLING != 0)
        n = 0;

    /* align to even boundary, if necessary */
    if (n != 0 && lmem % 2 == 1)
        lmem = lmem - 1;
    stloc = lmem;
    lmem = lmem - n;

    /* stsize is number of bytes reqd for stack, stloc is addr */
    iw = C_WIDTH / 14;
    n = 0;

    /* compute page to start variables */
    i = 0;
    if (codloc % 256 > lmem % 256)
        i = 1;
    i = i + codloc / 256;
    if (C_VARIABLES > i)
        i = C_VARIABLES;

    /* compute first relative address page */
    j = lmem / 256 - i;
    if (j < 0)
        error(122, 1);

    else {
        for (i = 1; i <= sytop; i++) {
            m = symbol[i];
            k = symbol[m];
            if (k >= 0) {

                /* now fix page number */
                l = right(shr(k, 8), 8) - j;

                /* l is relocated page number */
                symbol[m] = shl(l, 8) + right(k, 8);
                k = shr(k, 16);
                for (;;) {
                    if (k != 0) {

                        /* backstuff lhi l into location k-1 */
                        ip = get(k - 1) * 256 + get(k);
                        put(k - 1, 38);
                        put(k, l);
                        k = ip;
                    } else {

                        /* backstuff LXI references to this variable */
                        k = symbol[m - 2];
                        m = symbol[m];
                        while (k != 0) {
                            l = get(k) + get(k + 1) * 256;
                            put(k, m % 256);
                            put(k + 1, m / 256);
                            k = l;
                        }
                        break;
                    }
                }
            }
        }
        if (C_MAP != 0)
            writel(0);

        /* relocate AND backstuff the stack top references */
        stloc = stloc - j * 256;
        while (lxis != 0) {
            i = lxis;
            lxis = get(i) + get(i + 1) * 256;
            put(i, stloc % 256);
            put(i + 1, stloc / 256);
        }
        form("\nSTACK SIZE ");
        if (C_STACKHANDLING == 1)
            form("OVERRIDDEN");

        else
            form("= %d BYTES", stsize);
        writel(0);

        /* now backstuff all other TRC, TRA, AND PRO addresses */
        for (i = 1; i <= sytop; i++) {
            j = symbol[i];
            k = -symbol[j];
            l = iabs(symbol[j - 1]);
            l = right(l, 4);
            if (l == LABEL || l == PROC) {
                // V4
                l = shr(k, 2);
                n = right(k, 2);
                k = symbol[j - 2];

                
                while (l != 0) {  // V4                        
                    m = get(l) + get(l + 1) * 256;
                    put(l, k % 256);
                    put(l + 1, k / 256);
                    l = m;
                }
                symbol[j] = n;
            }
        }
        if (preamb > 0) {
            for (i = 1; i <= 8; i++) {
                j = intpro[i];
                if (j != 0) {
                    j = symbol[j];
                    //  V4
                    j = symbol[j - 2];
                    intpro[i] = j * 256 + 195;
                }
            }
            if (intpro[1] == 0 && offset == 0 && C_STACKHANDLING != 1) // V4
                intpro[1] = (offset + preamb) * 256 + 195;

            /* ** note that jump inst is 11000011b = 195d ** */
            k = offset;
            offset = 0;
            for (i = 0, j = 1;; j++) {
                for (l = intpro[j];;) {
                    put(i++, l % 256);
                    l = l / 256;
                    if (i >= preamb) {
                        offset = k;
                        return;
                    }
                    if (i % 8 == 0)
                        break;
                }
            }
        }
    }
    return;
}

int loadin()        // modified for V4
{
    int i, j, k, l, m, n, lp;
    int ival = -1;
    /* save the current input file number */
    m = C_INPUT;
    C_INPUT = C_USYMBOL;

    /* get rid of last card image */
    ibp = 99999;

    do {
        i = gnc(0);
    } while (!(i != 1));
    if (i != 41)
        error(124, 1);

    else
        for (;;) {

            /* process next symbol table entry */
            i = gnc(0);
            if (i == 41)
                break;
            i = i - 2;

            /* build address of initialized symbol */
            k = 32;
            for (j = 1; j <= 2; j++) {
                i = (gnc(0) - 2) * k + i;
                k = k * 32;
            }
            j = symbol[i];
            k = symbol[j - 1];
            k = k / 16 % 16;
            j = symbol[j];

            /* j is starting address, AND k is the precision of */
            /* the base variable */
            if (codloc > j)
                error(123, 1);
            while (codloc < j) {
                put(codloc, 0);
                codloc = codloc + 1;
            }

            /* read hex values until next '/' is encountered */
            if (ival < 0)
                ival = j;
            lp = -1;
            for (;;) {
                lp = lp + 1;
                i = gnc(0) - 2;

                /* check for ending / */
                if (i == 39)
                    break;
                l = i / 16;
                i = i % 16 * 16 + (gnc(0) - 2);

                /* i is the next hex value, AND l=1 if beginning of a new bvalue */
                if (k != 2)
                    put(codloc, i);

                else {

                    /* double byte initialize */
                    if (l == 0)

                        /* check for long constant */
                        if (lp < 2) {

                            /* exchange places with h.o. AND l.o. bytes */
                            n = get(codloc - 2);
                            put(codloc - 1, n);
                            put(codloc - 2, i);
                            continue;
                        }
                    lp = 0;
                    put(codloc, i);
                    put(codloc + 1, 0);
                }
                codloc = codloc + k;
            }
        }
    C_INPUT = m;
    return ival;
}

void emitbf(const int l) {
    int i, k, m, n, lp, jp;
    static int kp;

    /* emit code for the built-IN function l.  the biftab */
    /* array is headed by a table which either gives the starting */
    /* location of the BIF code IN biftab (if negative) OR the */
    /* absolute code location of the function if already */
    /* emitted. */
    i = biftab[l];
    if (i < 0) {

        /* code NOT yet emitted */
        i = -i;
        emit(JMP, 0, 0);

        /* backstuff address later */
        biftab[l] = codloc;

        /* get number of bytes to emit */
        k = biftab[i++];

        /* then the number of relative address stuffs */
        kp = biftab[i++];

        /* start emitting code */
        m = i + kp;
        for (jp = 0; jp < k; jp++) {
            if (jp % 3 == 0)
                n = biftab[m++];
            lp = alloc(1);
            put(codloc++, n % 256);
            n = n / 256;
        }

        /* now go back AND replace relative addresses with */
        /* absolute addresses. */
        n = biftab[l];
        for (jp = 0; jp < kp; jp++) {
            m = biftab[i++];
            k = get(n + m) + get(m + n + 1) * 256 + n;
            put(n + m, k % 256);
            put(n + m + 1, k / 256);
        }
        /* backstuff branch around function */
        i = biftab[l];
        put(i - 2, codloc % 256);
        put(i - 1, codloc / 256);
    }

    /* emit call on the function */
    emit(CAL, i, 0);
    return;
}

void inldat() {
    int iq, i, j, k, l, ic;
    static int kp;

    /* emit data inline */
    iq = codloc;
    l = 0;
    for (;;) {
        k = 0;
        if (lapol != 0) {
            for (j = 1; j <= 3; j++)
                for (;;) {
                    i = gnc(0);
                    if (i != SPACE) {
                        if (i < CHZERO || i > CHV)
                            goto L330;
                        k = k * 32 + i - CHZERO;
                        break;
                    }
                }
            i = k;
            k = lapol;
            lapol = i;
            kp = k % 8;
            k = k / 8;

            /* kp is typ AND k is data */
            if (l > 0) {
                if (kp == OPR) {
                    if (k != DAT)
                        goto L330;

                    /* backstuff jump address */
                    /* now fix symbol table entries */
                    k = iabs(ic);
                    l--;
                    k = symbol[k];
                    symbol[k] = -iq;
                    j = symbol[--k];

                    /* check symbol length against count */
                    j /= 256;
                    symbol[k] = l * 256 + 16 + VARB;
                    if (ic < 0) {
                        /* this is an address reference to a constant, so.. */
                        st[++sp] = ic;
                        rasn[sp] = 0;
                        litv[sp] = iq;
                        prec[sp] = 2;
                    } else if (j != l)            /* check size declared against size read */
                        goto L330;
                    return;
                } else {
                    if (kp != LIT)
                        goto L330;
                    emit(0, k, 0);
                }
            } else {                /* define inline data symbol */
                if (kp != DEF)
                    goto L330;
                ic = k;
                if (k <= 0) {       /* inline constant -- set up symbol entry */
                    ic = -(++sytop);
                    symbol[sytop] = syinfo;
                    syinfo -= 2;

                    /* will be filled later */
                    if (syinfo < sytop)
                        goto L330;
                }
            }
            l++;
            continue;
        }
    L330:
        if (kp == LIN)
            C_COUNT = k;
        else {
            error(125, 1);
            return;
        }
    }
}

void unary(const int ival) {
    int val, i, j, k, ip, ia, ib, jp, iq;

    /* 'val' is an integer corresponding to the operations-- */
    /* RTL(1) RTR(2) SFL(3) SFR(4) scl(5) scr(6) HIV(7) LOV(8) */
    /* ** note that the following code assumes the value of RTL = 37 */
    val = ival - 36;
    if (rasn[sp] > 255)
        cvcond(sp);
    ip = prec[sp];
    switch (val) {
    case 1:
    case 2:
        if (ip <= 1) {
            if (rasn[sp] == 0) {
                loadv(sp, 1);
                regs[RA] = rasn[sp] % 16;
            }
            i = rasn[sp] % 16;
            k = regs[RA];
            if (k != 0) {
                if (k != i) {
                    emit(LD, k, RA);
                    emit(LD, RA, i);
                    regs[RA] = i;
                }
            } else {
                emit(LD, RA, i);
                regs[RA] = i;
            }
            i = LFT;
            if (val == 2)
                i = RGT;
            emit(ROT, CY, i);
            return;
        }
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        j = 1;
        if ((val == 4 || val == 6) && ip > 1)
            j = 0;
        i = rasn[sp];
        if (i <= 0) {
            /* load from memory */
            loadv(sp, j);
            i = rasn[sp];
            if (j == 1)
                regs[RA] = i % 16;
        }

        /* may have to store the accumulator */
        ia = i % 16;
        ib = i / 16;
        k = ia;
        if (j != 1)
            k = ib;
        jp = regs[RA];

        /* we want register k to be IN the accumulator */
        if (jp != k) {
            if (jp != 0)
                emit(LD, jp, RA);
            emit(LD, RA, k);
        }
        regs[RA] = k;

        /* SFL AND SFR take separate paths now... */
        if (val == 4 || val == 6) {
            if (val == 4)
                emit(OR, RA, 0);
            emit(ROT, ACC, RGT);
            if (ip >= 2) {
                emit(LD, ib, RA);
                emit(LD, RA, ia);
                emit(ROT, ACC, RGT);
                regs[RA] = ia;
            }
        } else {

            /* SFL - clear CARRY AND shift */
            if (val == 3)
                emit(AD, RA, RA);
            if (val == 5)
                emit(ROT, ACC, LFT);
            if (ip >= 2) {
                emit(LD, ia, RA);
                emit(LD, RA, ib);
                emit(ROT, ACC, LFT);
                regs[RA] = ib;
            }
        }
        return;
        break;
    case 7:
        break;
    case 8:
        if (ip >= 2) {
            if (rasn[sp] <= 0)
                loadv(sp, 0);
            i = rasn[sp];
            ip = i / 16 % 16;
            iq = i % 16;
            if (regs[RA] == iq)
                regs[RA] = 0;
            regs[ip] = 0;
            regv[ip] = -1;
            rasn[sp] = iq;
            prec[sp] = 1;
            if (regs[RA] != ip)
                emit(LD, iq, ip);

            else
                regs[RA] = iq;
            return;
        }
        break;
    case 9:
        prec[sp] = 1;

        /* may have to release register */
        i = rasn[sp];
        rasn[sp] = i % 16;
        i = i / 16;
        if (i != 0) {
            regs[i] = 0;
            regv[i] = -1;
            if (regs[RA] == i)
                regs[RA] = 0;
        }
        return;
    }
    error(126, 1);
    return;
}

void exch() {
    int i, j, ia, ib;

    /* exchange the top two elements of the stack */
    j = sp - 1;
    if (st[j] == 0 && rasn[j] == 0 && litv[j] < 0)

        /* second element is pushed - check top elt */
        if (rasn[sp] == 0 && litv[sp] < 0)

            /* second elt is pushed, top elt is NOT IN cpu */
            if (st[sp] == 0) {

                /* both are pushed, so go thru 20 twice */
                j = sp;
                for (;;) {

                    /* POP element (second if drop thru, top if from 30) */
                    genreg(-1, &ia, &ib);
                    if (ia == 0) {
                        error(107, 5);
                        break;
                    } else {
                        if (prec[j] > 1)
                            ib = ia - 1;
                        emit(POP, ia - 1, 0);
                        ustack();
                        regs[ia] = j;
                        if (ib != 0)
                            regs[ib] = j;
                        rasn[j] = ib * 16 + ia;
                        if (j != sp)
                            break;
                        j = sp - 1;
                    }
                }
            }
    j = sp - 1;
    for (i = 2; i <= 7; i++)
        if (regs[i] == sp)
            regs[i] = j;

        else if (regs[i] == j)
            regs[i] = sp;
    i = prec[sp];
    prec[sp] = prec[j];
    prec[j] = i;
    i = rasn[sp];
    rasn[sp] = rasn[j];
    rasn[j] = i;
    i = st[sp];
    st[sp] = st[j];
    st[j] = i;
    i = litv[sp];
    litv[sp] = litv[j];
    litv[j] = i;
    return;
}

void stack(const int n) {
    int j, k;

    /* ADD n to current depth, test for stacksize exc maxdepth */
    k = prsp + 1;
    j = curdep[k] + n;
    if (j > maxdep[k])
        maxdep[k] = j;
    curdep[k] = j;
    return;
}

void readcd() {
    int lcnt, typ, lline, lloc, polcnt, val, i, j, k, l, m;
    int ibase, ip, kp, lp, ia, ib;
    char *rmap = "-ABCDEFGHIJKLMOP";
    char *polchr = "OPRADRVALDEFLITLIN";
    char *opcval = "NOPADDADCSUBSBCMULDIVMDFNEGANDIORXORNOT"
        "EQLLSSGTRNEQLEQGEQINXTRATRCPRORETSTOSTD"
        "XCHDELDATLODBIFINCCSEENDENBENPHALRTLRTR"
        "SFLSFRHIVLOVCVAORGDRTENADISAX1AX2AX3";


    C_COUNT = 1;
    lline = 0;
    lloc = 0;
    lcnt = C_WIDTH / 12;
    alter = false;
    m = C_INPUT;
    C_INPUT = C_JFILE;
    ibp = 9999;
    polcnt = 0;

    /* reserve space for interrupt locations */
    preamb = 0;
    for (i = 8; i > 0; i--) {
        if (intpro[i] != 0) {
            preamb = (i - 1) * 8 + 3;
            break;
        }
    }

    /* adjust codloc to account for preamble */
    if (codloc < preamb)
        codloc = preamb;

    /* allocate 'preamble' cells at start of code */
    i = alloc(preamb);
    offset = codloc - preamb;

    /* set stack pointer upon program entry */
    if (C_STACKHANDLING != 1) {	/* 1 -> user handled */
        if (C_STACKHANDLING == 0)	/* 0 -> system handled else stack size */

            /* start chain of lxis */
            lxis = codloc + 1;
        emit(LXI, RSP, C_STACKHANDLING);
    }
    while (!errflg) {
        ibase = 0;

        /* may have been stack overflow so... */
        if (sp < 0)
            sp = 0;
        if (C_ANALYSIS != 0)
            if (alter && sp > 0) {

                /* write stack */
                form("\n \n  PR   ST   RASN  LITV\n");
                for (ip = sp; ip > 0; ip--) {
                    form("\n%02d %d ", ip, prec[ip]);
                    if (st[ip] == 0)
                        form("      ");

                    else
                        form("%c%05d", st[ip] < 0 ? 'A' : 'S',
                            iabs(st[ip]));
                    k = rasn[ip];
                    form("  %c %c", rmap[(k >> 4) & 0xf], rmap[k & 0xf]);
                    k = litv[ip];
                    if (k >= 0)
                        form(" %c%05d", (k >> 16) != 0 ? 'R' : ' ',
                            k & 0xffff);
                    writel(0);
                }

                /* write registers */
                if (C_ANALYSIS >= 2) {
                    for (i = 1; i <= 7; i++) {
                        ip = regs[i];
                        kp = lock[i];
                        lp = regv[i];
                        if (kp + ip + lp >= 0) {
                            form(" %c(%c,", rmap[i], kp == 1 ? 'L' : 'U');
                            if (ip == 0)
                                putch('*');

                            else
                                form("%02d", ip);
                            putch(',');
                            if (lp < 0)
                                putch('*');

                            else
                                form("%XH", lp);
                            putch(')');
                        }
                    }
                    writel(0);
                }
            }

        do {
            k = 0;
            if (lapol != 0)
                for (j = 1; j <= 3; j++)
                    for (;;) {
                        i = gnc(0);
                        if (i != 1)
                            if (i >= 2 && i <= 33) {
                                k = k * 32 + (i - 2);
                                break;
                            } else {
                                error(127, 5);
                                C_INPUT = m;
                                return;
                            }
                    }

            /* copy the elt just read to the polish look-ahead, AND */
            /* interpret the previous elt */
            i = k;
            k = lapol;
            lapol = i;
        } while (!(k >= 0));

        /* check for END of code */
        if (k == 0)
            break;
        polcnt = polcnt + 1;
        typ = right(k, 3);
        val = shr(k, 3);

        /* $g=0 for no trace, $g=1 gives lines vs locs, */
        /* $g=2 yields full interlist of i.l. */
        if (C_GENERATE != 0)
            if (C_GENERATE > 1) {
                /* otherwise interlist the i.l. */
                form("\n%05d %04XH %d %.3s ", codloc, codloc, polcnt, polchr + typ * 3);
                switch (typ) {
                case OPR:
                    form("%.3s", opcval + val * 3);
                    break;
                case ADR:
                case VLU:
                case DEF:
                    form("S%05d", val);
                    break;
                case LIT:
                case LIN:;
                    form(" %05d", val);
                }
                writel(0);
            } else
                /* print line number = code location, if altered */
                if (lline != C_COUNT && lloc != codloc) {
                    /* changed completely, so print it */
                    lline = C_COUNT;
                    lloc = codloc;
                    if (lcnt <= 0) {
                        lcnt = C_WIDTH / 12;
                        putch('\n');
                    }
                    lcnt--;
                    form(" %4d=%04XH", lline, lloc);
                }
            if (++sp > maxsp) {    /* stack overflow */
                error(128, 5);
                sp = 1;
            }
            prec[sp] = 0;
            st[sp] = 0;
            rasn[sp] = 0;
            litv[sp] = -1;
            alter = false;
            switch (typ) {
            case OPR:          /* operator */
                sp = sp - 1;
                alter |= operat(val);
                continue;
            case ADR:
                if (sp > 1) {
                    /* check for active condition code which must be changed to boolean */
                    if (rasn[sp - 1] > 255)
                        cvcond(sp - 1);
                }
                i = symbol[val];
                j = symbol[i - 1];
                if (j >= 0) {
                    setadr(val);
                    continue;
                } else {
                    /* load address of based variable.  change to */
                    /* load value of the base, using the variable's precision */
                    ibase = right(shr(-j, 4), 4);
                    val = symbol[i - 2];
                }
                break;
            case VLU:
                break;
            case DEF:          /* mark last register load nil */
                lastrg = 0;
                lastex = 0;
                lastin = 0;
                lastir = 0;
                lastli = 0;     // only used in V4
                sp--;

                /* save registers if this is a PROC OR a LABEL which was */
                /* referenced IN a go-to statement OR was compiler-generated. */
                ip = symbol[val];
                i = iabs(symbol[ip - 1]);

                /* save this DEF symbol number AND the literal values of the */
                /* h AND l registers for possible TRA chain straightening. */
                if (right(i, 4) == LABEL) {
                    defsym = val;
                    defrh = regv[RH];
                    defrl = regv[RL];
                }

                /* we may convert the sequence */

                /* TRC l, TRA/PRO/RET, DEF l */

                /* to an equivalent conditional TRA/PRO/RET... */
                if (i / 256 == 1)
                    if (tstloc == codloc)
                        if (conloc == xfrloc - 3) {
                            j = -symbol[ip];
                            // V4
                            k = shr(j, 2);

                            if (k == conloc + 1) {

                                /* adjust backstuffing chain for JMP OR call */
                                if (xfrsym > 0) {
                                    k = symbol[xfrsym];

                                    /* decrement backstuff location by 3 */
                                    symbol[k] = symbol[k] + 12;
                                }

                                /* arrive here with the configuration TRC...DEF */
// V4
                                symbol[ip] = -right(j, 2);
                                k = iabs(symbol[ip - 1]) % 256;
                                if (symbol[ip - 1] < 0)
                                    k = -k;
                                symbol[ip - 1] = k;
                                j = get(conloc);
                                j = get(conloc);
                                j = shr(j, 3);
                                k = (j % 2 + 1) % 2;
                                k = shl(shr(j, 1), 1) + k;
                                j = get(xfrloc);
                                l = right(shr(j, 1), 2);
                                j = shl(k, 3) + shl(l, 1);
                                for (;;) {
                                    put(conloc, j);
                                    conloc = conloc + 1;
                                    xfrloc = xfrloc + 1;
                                    j = get(xfrloc);
                                    if (xfrloc == codloc) {
                                        codloc = conloc;
                                        membot = membot - 3;
                                        conloc = -1;
                                        xfrloc = -1;
                                        tstloc = -1;

                                        /* notice that defrh AND defrl are now incorrect */
                                        /* defsym=0 prevents use of these variables... */
                                        /* ... if a TRA immediately follows */
                                        defsym = 0;
                                        break;
                                    }
                                }
                            }
                        }
                j = right(i, 4);
                if (j == LABEL) {

                    /* LABEL found.  check for reference to LABEL */
                    i = i / 256;
                    if (i == 0)
                        goto L370;

                    /* check for single reference, no conflict with h AND l */
                    if (i == 1) {
                        // V4
                        i = symbol[ip - 3];
                        /* check for previous reference  forward */
// V4
                        if (i != 0 && i != -1) {
                            l = i % 256;
                            i = i / 256;
                            j = i % 512;
                            i = i / 512;
                            if (i % 2 != 1)
                                l = -1;
                            if ((i / 2) % 2 != 1)
                                j = -1;

                            /* j is h reg, l is l reg */
                            lock[RH] = 1;
                            lock[RL] = 1;
                            saver();

                            /* compare old hl with new hl */
                            lock[RH] = 0;
                            lock[RL] = 0;
                            k = regv[RH];
                            regv[RH] = -1;
                            if (k == (-255) || k == j)
                                regv[RH] = j;
                            k = regv[RL];
                            regv[RL] = -1;
                            if (k == (-255) || k == l)
                                regv[RL] = l;
                            goto L370;
                        }
                    }
                } else if (j == PROC) {

                    /* set up procedure stack for procedure entry */
                    prsp = prsp + 1;
                    if (prsp > prsmax)
                        error(145, 5);

                    else {
                        // V4
                        j = ip - 3;
                        prstk[prsp] = j;

                        /* mark h AND l as unaltered initially */
                        /* /  1b  /  1b  /  1b  /  1b  /  9b  /  8b  / */
                        /* /h unal/l unal/h vald/l vald/h valu/l valu/ */
                        /* ------------------------------------------- */
                        symbol[j] = shl(3, 19);
                        saver();
                        regv[RH] = -254;
                        regv[RL] = -254;
                        k = codloc;

                        /* set up stack depth counters */
                        maxdep[prsp + 1] = 0;
                        curdep[prsp + 1] = 0;
                        for (i = 1; i <= 8; i++)
                            if (val == intpro[i]) {

                                /* interrupt procedure is marked with ho 1 */
                                prstk[prsp] = j + 65536;
                                emit(PUSH, RH, 0);
                                emit(PUSH, RD, 0);
                                emit(PUSH, RB, 0);
                                emit(PUSH, RA, 0);
                                stack(4);
                            }
                        goto L380;
                    }
                }
                saver();

                /* LABEL is resolved.  last two bits of entry must be 01 */
            L370:k = codloc;
            L380:i = -symbol[ip];
                j = i % 4;
                i = i / 4;
                if (j != 1)
                    error(131, 1);
                // V4
                symbol[ip] = -(shl(i, 2) + 3);
                symbol[ip - 2] = k;
                /* now check for procedure entry point */
                i = symbol[ip - 1];
                if (right(i, 4) == PROC) {
                    i = shr(i, 8);

                    /* build receiving sequence for register parameters */
                    if (i >= 1) {
                        k = i - 2;
                        if (k < 0)
                            k = 0;
                        if (i > 2)
                            i = 2;
                        for (j = 1; j <= i; j++) {
                            sp = sp + 1;
                            if (sp > maxsp) {
                                error(113, 5);
                                sp = 1;
                            }

                            /* (RD,RE) = 69    (RB,RC) = 35 */
                            if (j == 1)
                                l = 35;
                            if (j == 2)
                                l = 69;
                            rasn[sp] = l;
                            st[sp] = 0;
                            litv[sp] = -1;
                            prec[sp] = 2;
                            sp = sp + 1;
                            if (sp > maxsp) {
                                error(113, 5);
                                sp = 1;
                            }
                            rasn[sp] = 0;
                            litv[sp] = -1;
                            setadr(val + k + j);
                            alter |= operat(STD);
                        }
                    }
                }
                continue;
                break;
            case LIT:
                if (sp > 1 && rasn[sp - 1] > 255) /* check for active condition code which must be changed to boolean */
                    cvcond(sp - 1);
                alter = true;
                litv[sp] = val;
                prec[sp] = litv[sp] > 255 ? 2 : 1;
                continue;
            case LIN:
                /* line number */
                C_COUNT = val;
                sp--;
                continue;
            }
            i = symbol[val];
            j = symbol[i - 1];
            if (sp > 1)

                /* allow only a LABEL variable to be stacked */
                if (iabs(j) % 16 != LABEL) {
                    /* check for active condition code which must be changed to boolean */
                    if (rasn[sp - 1] > 255)
                        cvcond(sp - 1);
                }
            /* check for condition codes */
            if (val <= intbas)
                if (val <= 4) {

                    /* CARRY ZERO minus PARITY */
                    /* set to true/condition (1*16+val) */
                    rasn[sp] = (16 + val) * 256;
                    st[sp] = 0;
                    prec[sp] = 1;
                    alter = true;
                    continue;
                } else

                    /* may be a call to input OR output */
                    if (val < firsti || val > intbas)

                        /* check for reference to 'memory' */
                        /* ** note that 'memory' must be at location 5 IN the symbol table ** */
                        if (val != 5) {

                            /* ** note that 'stackptr' must be at 6 IN sym tab */
                            if (val != 6)
                                error(129, 1);

                            else {

                                /* load value of stackpointer to registers immediately */
                                genreg(2, &ia, &ib);
                                if (ib == 0)
                                    error(107, 5);

                                else {
                                    rasn[sp] = ib * 16 + ia;
                                    litv[sp] = -1;
                                    st[sp] = 0;
                                    regs[ia] = sp;
                                    regs[ib] = sp;
                                    prec[sp] = 2;
                                    emit(LXI, RH, 0);
                                    emit(DAD, RSP, 0);
                                    emit(LD, ia, RL);
                                    emit(LD, ib, RH);
                                    regv[RH] = -1;
                                    regv[RL] = -1;
                                    alter = true;
                                }
                            }
                            continue;
                        }
                if (j < 0)

                    /* value reference to based variable. first insure that this */
                    /* is NOT a length attribute reference, (i.e., the variable is */
                    /* NOT an actual parameter for a call on length OR last) by */
                    /* insuring that the next polish elt is NOT an address */
                    /* reference to symbol (length+1) OR (last+1) */
                    /* note that this assumes length AND last are symbol numbers */
                    /* 18 AND 19 */
                    if (lapol != 153 && lapol != 161) {

                        /* load value of base variable.  change to load */
                        /* value of base, followed by a LOD op. */
                        ibase = right(shr(-j, 4), 4) + 16;
                        val = symbol[i - 2];
                        i = symbol[val];
                        j = symbol[i - 1];
                    }
                alter = true;

                /* examine attributes */
                st[sp] = val;
                i = right(j, 4);
                j = shr(j, 4);
                k = right(j, 4);
                if (ibase > 0)
                    k = ibase % 16;
                prec[sp] = k;
                if (i >= LITER - 1)
                    if (k > 0 && k < 3)
                        litv[sp] = right(shr(j, 4), 16);

                    else {
                        error(130, 1);
                        continue;
                    }

                /* check for base address which must be loaded */
                if (ibase >= 16) {

                    /* must be a based variable value reference. */
                    /* load the value of the base AND follow it by */
                    /* a load operation. */
                    k = prec[sp];

                    /* mark as a byte load for the LOD operation IN operat */
                    /* leaves 2 if double byte result AND 6 (=2 mod 4) if single byte */
                    prec[sp] = 10 - 4 * k;
                    alter |= operat(LOD);
                }
    }
    emit(EI, 0, 0);
    emit(HALT, 0, 0);

    /* may be line/loc's left IN output buffer */
    if (C_GENERATE != 0)
        writel(0);
    C_INPUT = m;
    return;
}

bool operat(int val) {
    static int it;
    int i, j, k, m, l, icy, icom, iq, iop, iop2, ip, jp;
    int jph, kp, id, ml, mh, ia, ib, il, lp;
    bool skip;

    /* ADD ADC SUB SBC MUL div mod NEG AND IOR */
    /* XOR NOT EQL LSS GTR NEQ LEQ GEQ INX TRA */
    /* TRC PRO RET STO STD XCH DEL cat LOD BIF */
    /* INC CSE END ENB ENP HAL RTL RTR SFL SFR */
    /* HIV LOV CVA ORG AX1 AX2 AX3 */
    icy = 0;
    icom = 0;
    iq = 0;
    switch (val) {
    case INC:       /* place a literal 1 at stack top AND apply ADD operator */
        litv[++sp] = 1;

        if (prec[sp - 1] == 1) { /* check for single byte increment, may be comparing with 255 */
            apply(AD, AC, 1, 1);
            lastin = codloc;
            return true;
        }           // fall through to add
    case ADD:
        /* may do the ADD IN h AND l (using INX operator) */
        if (prec[sp] != 1)
            exch();
        if (prec[sp - 1] != 1)
            inx(1);             // prec=1 for inx
        else {
            exch();
            apply(AD, AC, 1, 1);
        }
        return true;
    case ADC:
        apply(AC, AC, 1, 1);
        return true;
    case SUB:           /* change address value - 1 to address value + 65535 AND apply ADD */
        if (prec[sp - 1] != 1 && litv[sp] == 1) {
            litv[sp] = 65535;
            prec[sp] = 2;
            inx(1);
        } else
            apply(SU, SB, 0, 1);
        return true;
    case SBC:
        apply(SB, SB, 0, 1);
        return true;
    case MUL:
        builtin(1, 2);
        return true;
    case DIV:
        builtin(2, 1);
        return true;
    case MDF:       // MOD
        builtin(2, 2);
        return true;
    case NEG: case BIF: case END: case ENB: case ENP: case AX3:
        return false;
    case AND:
        apply(ND, ND, 1, 0);
        return true;
    case IOR:
        apply(OR, OR, 1, 0);
        return true;
    case XOR:
        apply(XR, XR, 1, 0);
        return true;
    case NOT:
        i = rasn[sp];
        if (i > 255) {      /* condition code - change PARITY */
            j = 1 - (i / 4096);
            rasn[sp] = j * 4096 + i % 4096;
        } else {                  /* perform XOR with 255 OR 65535 (byte OR address) */
            i = prec[sp];
            litv[++sp] = (1 << (i * 8)) - 1;
            prec[sp] = i;
            apply(XR, XR, 1, 0);
        }
        return true;
    case EQL:              /* equal test */
        if (prec[sp] + prec[sp - 1] <= 2) {
            apply(SU, 0, 1, 0);
            rasn[sp] += 18 * 256;   /* mark as true/ZERO (1*16+2) */
        } else
            compare16(1, 18, 1);
        return true;
    case GTR:  /* GTR - change to LSS */
        exch();
    case LSS:  /* LSS set to tru/CARRY (1 * 16 + 1) */
        if (prec[sp] + prec[sp - 1] <= 2) {
            apply(litv[sp] != 1 ? SU : CP, 0, 0, 0);
            rasn[sp] += 17 * 256;   /* mark as condition code */
        } else
            compare16(0, 17, 0);
        return true;
    case NEQ:
        if (prec[sp] + prec[sp - 1] <= 2) {
            apply(SU, 0, 1, 0);
            rasn[sp] += 2 * 256;    /* mark as false/ZERO (0*16 + 2) */
        } else
            compare16(1, 2, 1);
        return true;
    case LEQ:           /* LEQ - change to GEQ */
        exch();
    case GEQ:
        if (prec[sp] + prec[sp - 1] <= 2) {
            apply(litv[sp] != 1 ? SU : CP, 0, icom, 0);
            rasn[sp] += 1 * 256;    /* mark as condition code */
        } else
            compare16(0, 1, 0);
        return true;
    case INX:
        inx(prec[sp - 1]);
        return true;
    case TRC:
        j = sp - 1;
        i = litv[j];
        if (right(i, 1) == 1) { /* this is a do forever (OR something similar) so ignore the jump */
            _delete(2);
            return true;
        }
        iop = 2;                /* NOT a literal '1' */
        /* check for condition code */
        i = rasn[j];
        if (i > 255) {          /* active condition code, construct mask for JMC */
            i /= 256;
            j = i / 16;
            i = i % 16;
            iop2 = (FAL + 1 - j) * 32 + (CARRY + i - 1);
        } else {
            if (i == 0) {      /* load value to accumulator */
                prec[j] = 1;
                loadv(j, 1);
            } else {          /* value already loaded */
                i %= 16;
                j = regs[RA];
                if (j != i) {
                    if (j != 0)
                        emit(LD, j, RA);
                    emit(LD, RA, i);
                }
            }
            regs[RA] = 0;
            emit(ROT, CY, RGT);
            iop2 = FAL * 32 + CARRY;
        }
        break;
    case PRO:
        i = st[sp];
        if (i > intbas) {   /* pass the last two (at most) parameters IN the registers */
            i = right(st[sp], 16);
            i = symbol[i];
            i = shr(symbol[i - 1], 8);
            i = imin(i, 2);
            if (i < 1) {
                lock[RH] = lock[RL] = 1;
                saver();
                lock[RH] = lock[RL] = 0;
                iop = 3;
            } else {
                j = sp - i - i;
                for (k = 1; k <= i; k++) {
                    ip = rasn[j];
                    jp = ip / 16 % 16;
                    ip %= 16;
                    if (ip != 0)
                        lock[ip] = 1;
                    if (jp != 0)
                        lock[jp] = 1;
                    prec[j] = imin(prec[j], prec[j + 1]);
                    if (prec[j] <= 1 && jp != 0) {
                        // bug fix from V4 -- clear pending store if passing address var to byte var
                        if (regs[RA] == jp)
                            regs[RA] = 0;
                        regs[jp] = lock[jp] = 0;
                        jp = 0;
                        if (regs[RA] == ip)
                            lock[RA] = 1;
                        if (regs[RA] == jp)
                            lock[RA] = 1;
                    }
                    rasn[j] = jp * 16 + ip;
                    j += 2;
                }
                j = sp - 1 - 2 * i;
                it = 0;
                /* stack any stuff which does NOT go to the procedure */
                for (k = 1; k <= sp; k++) {     /* check for value to PUSH */
                    jp = rasn[k];
                    if (jp == 0) {              /* registers NOT assigned - check for stacked value */
                        if (st[k] == 0 && litv[k] < 0 && it != 0)
                            error(150, 1);
                    } else if (k <= j) {        /* possible PUSH if NOT a parameter */
                                                /* registers must be pushed */
                        jph = jp / 16;
                        kp = regs[RA];
                        jp = jp % 16;
                        if (kp != 0)        /* pending ACC store, check ho AND lo registers */
                            if (kp == jph) { /* pending ho byte store */
                                emit(LD, jph, RA);
                                regs[RA] = 0;
                            } else if (kp == jp) {  /* check lo byte */
                                emit(LD, jp, RA);
                                regs[RA] = 0;
                            }
                            emit(PUSH, jp - 1, 0);
                            stack(1);
                            st[k] = 0;
                            it = rasn[k];
                            jp = it % 16;
                            if (jp != 0)
                                regs[jp] = 0;
                            jp = it / 16;
                            if (jp != 0)
                                regs[jp] = 0;
                            rasn[k] = 0;
                            litv[k] = -1;
                            it = k;
                    }
                }
                it = RH;
                j = sp - i - i;
                for (k = 1; k <= i; k++) {
                    id = 2 * k + 2;
                    ip = rasn[j];
                    jp = ip / 16 % 16;
                    ip = ip % 16;
                    for (;;) {
                        id--;
                        if (ip == 0)
                            break;
                        if (ip != id) {
                            if (regs[id] != 0) {
                                m = regs[id];
                                mh = rasn[m] / 16 % 16;
                                ml = rasn[m] % 16;
                                if (ml == id)
                                    ml = it;
                                if (mh == id)
                                    mh = it;
                                // Bug fix from V4 -- CLEAR PENDING STORE WHEN REG PAIRS ARE TO BE EXCHANGED ***
                                if (regs[RA] == id) {
                                    emit(LD, it, RA);
                                    regs[RA] = lock[RA] = 0;
                                } else
                                    // end of change
                                    emit(LD, it, id);
                                regs[it] = m;
                                rasn[m] = mh * 16 + ml;
                                it++;
                            }
                            regs[ip] = lock[ip] = 0;
                            if (regs[RA] == ip) {
                                ip = 1;
                                regs[RA] = lock[RA] = 0;
                            }
                            emit(LD, id, ip);
                            regs[id] = j;
                        }
                        lock[id] = 1;
                        ip = jp;
                        if (ip == (-1))
                            break;
                        jp = -1;
                    }
                    j += 2;
                }
                j = sp - 2 * i;
                for (k = 1; k <= i; k++) {
                    if (rasn[j] == 0)
                        loadv(j, 0);
                    ip = 2 * k;
                    regs[ip] = j;
                    lock[ip] = 1;
                    if (prec[j + 1] == 2 && prec[j] == 1)
                        emit(LD, ip, 0);
                    j += 2;
                }
                if (regs[RA] != 0)
                    emit(LD, regs[RA], RA);
                for (k = 1; k <= 7; k++) {
                    regs[k] = lock[k] = 0;
                    regv[k] = -1;
                }
                j = 2 * i;
                for (k = 1; k <= j; k++) {
                    exch();
                    if (st[sp] == 0 && rasn[sp] == 0 && litv[sp] < 0) {
                        emit(POP, RH, 0);
                        ustack();
                        regv[RH] = regv[RL] = -1;
                    }
                    _delete(1);
                }
                iop = 3;
            }
            break;
        } else {        /* this is a built-IN function. */
            _delete(1);
            if (i >= firsti) {
                i = i - firsti + 1;
                switch (i) {
                case 1: case 2: case 3: case 4:
                case 5: case 6: case 8: case 9:
                    val = 36 + i;
                    if (val > 42)       /* ** note that this also assumes only 6 such bifs */
                        unary(val);
                    else {
                        i = litv[sp];
                        if (i > 0) {    /* generate IN-line code for shift counts of */
                                        /* 1 OR 2 for address values */
                                        /* 1 to 3 for shr of byte values */
                                        /* 1 to 6 for all other shift functions on byte values */
                            j = prec[sp - 1] != 1 ? 2 : val == 40 ? 3 : 6;
                            if (i <= j) {
                                _delete(1);
                                for (j = 1; j <= i; j++)
                                    unary(val);
                                return true;
                            }
                        }
                        exch();
                        /* load the value to decrement */
                        loadv(sp - 1, 0);
                        j = rasn[sp - 1] % 16;
                        if (regs[RA] == j) {
                            emit(LD, j, RA);
                            regs[RA] = 0;
                        }
                        lock[j] = 1;

                        /* load the value which is to be operated upon */
                        kp = prec[sp];
                        i = 1;
                        if (kp > 1)
                            i = 0;
                        if (rasn[sp] == 0) {
                            loadv(sp, i);
                            if (i == 1)
                                regs[RA] = rasn[sp] % 16;
                        }
                        k = rasn[sp];
                        m = k % 16;
                        k = k / 16;
                        jp = regs[RA];
                        if (i != 1 || jp != m) {
                            if (jp != 0) {
                                emit(LD, jp, RA);
                                regs[RA] = 0;
                            }
                            if (i != 0) {
                                emit(LD, RA, m);
                                regs[RA] = m;
                            }
                        }
                        i = codloc;
                        unary(val);
                        if (kp != 1) {
                            k = regs[RA];
                            if (k != 0)
                                emit(LD, k, RA);
                            regs[RA] = 0;
                        }
                        emit(DC, j, 0);
                        emit(JMC, FAL * 32 + ZERO, i);

                        /* END up here after operation completed */
                        exch();
                        lock[j] = 0;
                        _delete(1);
                    }
                    return true;
                case 7:
                    if (rasn[sp] > 255)
                        cvcond(sp);
                    /* emit the following code sequence for 100 usec per loop */
                    /* 8080 cpu only */
                    /* (get time parameter into the accumulator) */
                    /*          mvi   b,12   (7 CY overhead) */
                    /* start    mov   c,b    (5 CY * .5 usec = 2.5 usec) */
                    /* -------------------- */
                    /* tim180   dcr   c      (5 CY * .5 usec = 2.5 usec) */
                    /*          jnz   tim180 (10 CY* .5 usec = 5.0 usec) */
                    /* -------------------- */
                    /*              12 *     (15 CY* .5 usec = 7.5 usec) */
                    /*              =        (180 CY* .5 usec = 90 usec) */
                    /*          dcr   a      (5 CY * .5 usec = 2.5 usec) */
                    /*          jnz   start  (10 CY* .5 usec = 5.0 usec) */

                    /*          total time   (200 CY*.5 usec = 100 usec/loop) */
                    j = regs[RA];
                    i = rasn[sp];
                    ip = i / 16;
                    i = i % 16;
                    if (j == 0 || j != i) {
                        /* get time parameter into the accumulator */
                        if (j != 0 && j != ip)
                            emit(LD, j, RA);
                        regs[RA] = 0;
                        if (i == 0)
                            loadv(sp, 1);
                        i = rasn[sp] % 16;
                        if (j != 0)
                            emit(LD, RA, i);
                    }
                    regs[RA] = 0;
                    emit(LD, i - 1, -12);
                    emit(LD, i, i - 1);
                    emit(DC, i, 0);
                    emit(JMC, FAL * 32 + ZERO, codloc - 1);
                    emit(DC, RA, 0);
                    emit(JMC, FAL * 32 + ZERO, codloc - 6);
                    _delete(1);
                    return true;
                case 10:
                    /* input function. get input port number */
                    i = litv[sp];
                    if (i >= 0 && i <= 255) {
                        _delete(1);
                        sp = sp + 1;
                        genreg(1, &j, &k);
                        if (j != 0) {
                            k = regs[RA];
                            if (k != 0)
                                emit(LD, k, RA);
                            regs[RA] = rasn[sp] = j;
                            litv[sp] = -1;
                            st[sp] = 0;
                            prec[sp] = 1;
                            regs[j] = sp;
                            emit(INP, i, 0);
                            return true;
                        }
                    }
                    break;
                case 11:
                    /* check for proper output port number */
                    i = litv[sp];
                    if (i >= 0 && i <= 255) {
                        _delete(1);
                        /* now build an entry which can be recognized by */
                        /* operat. */
                        litv[++sp] = i;
                        rasn[sp] = 0;
                        prec[sp] = 1;
                        st[sp] = outloc;
                        return true;
                    }
                    break;
                case 12:
                case 13:
                    j = st[sp];
                    if (j > 0) {
                        j = symbol[j] - 1;
                        j = iabs(symbol[j]) / 256 + 12 - i;
                        _delete(1);
                        sp = sp + 1;
                        st[sp] = 0;
                        i = 1;
                        if (j > 255)
                            i = 2;
                        prec[sp] = i;
                        rasn[sp] = 0;
                        litv[sp] = j;
                        if (j >= 0)
                            return true;
                    }
                    break;
                case 14:
                    break;
                case 15:
                    if (prec[sp] > 1)
                        return false;
                    if (rasn[sp] == 0)
                        if (litv[sp] < 0) {
                            /* load value to accumulator AND get a register */
                            loadv(sp, 1);
                            regs[RA] = rasn[sp] % 16;
                        } else {
                            prec[sp] = 2;
                            st[sp] = 0;
                            return true;
                        }
                    ia = rasn[sp];
                    prec[sp] = 2;
                    st[sp] = 0;
                    if (ia <= 15) {
                        lock[ia] = 1;
                        ib = ia - 1;
                        regs[ib] = sp;
                        lock[ia] = 0;
                        rasn[sp] = ib * 16 + ia;
                        /* ZERO the register */
                        emit(LD, ib, 0);
                        if (ib == 0)
                            error(133, 5);
                    }
                    return true;
                case 16:
                    j = rasn[sp] % 16;
                    if (j != 0)
                        if (prec[sp] == 1) {
                            i = regs[RA];
                            if (i != j) {       /* may be a pending register store */
                                if (i != 0)
                                    emit(LD, i, RA);
                                emit(LD, RA, j);
                                regs[RA] = j;
                            }
                            emit(DAA, 0, 0);
                            return true;
                        }
                    break;
                }
            }

            /* built IN function error */
            error(136, 1);
            return false;
        }
        break;
    case RET:
        jp = prsp;
        if (jp <= 0) {
            error(146, 1);
            regv[RH] = regv[RL] = -255; /* mark as nil */
            return true;
        }
        // V4 /* check for type AND precision of procedure */
        l = prstk[jp] % 65536 + 2;
        l = symbol[l] / 16 % 16;

        /* l is the precision of the procedure */
        if (l != 0) {
            i = rasn[sp];
            if (i == 0)
                loadv(sp, 1);
            else if (i >= 256)
                cvcond(sp);
            k = rasn[sp];
            jp = regs[RA];
            j = k % 16;
            k = k / 16;
            if (i != 0 && j != jp) {    /* have to load the accumulator.  may have h.o. byte. */
                if (jp != 0 && jp == k)
                    emit(LD, k, RA);
                emit(LD, RA, j);
            }
            if (k != 0 && k != RB)
                emit(LD, RB, k);
            if (l > prec[sp])           /* compare precision of procedure with stack */
                emit(LD, RB, 0);
        }
        _delete(1);
        if (prstk[prsp] > 65535) {      /* interrupt procedure - use the DRT code below */
            emit(POP, RA, 0);
            emit(POP, RB, 0);
            emit(POP, RD, 0);
            emit(POP, RH, 0);
            emit(EI, 0, 0);
            emit(RTN, 0, 0);
            if (prsp <= 0) {
                error(146, 1);
                regv[RH] = regv[RL] = -255; /* mark as nil */
                return true;
            }
        } else
            emit(RTN, 0, 0);
        /* merge values of h AND l for this procedure */
        updateHL(prsp);
        return true;
    case STO:
    case STD:
        /* STO AND STD */
        i = st[sp];

        /* check for output function */
        if (i != outloc) {              /* check for computed address OR saved address */
            if (i < 0) {                /* check for address reference outside intrinsic range */
                i = -i;
                if (i <= intbas)        /* check for 'memory' address reference */
                                        /* ** note that stacktop must be at 6 ** */
                    if (i > 6) {
                        if (val == STD)
                            _delete(1);
                        return true;
                    }
            }
            i = 1;

            /* check for STD */
            if (val == STD)
                i = 0;
            gensto(i);
        } else {
            j = litv[sp];
            i = rasn[sp - 1];
            if (i <= 0 || i >= 256) {           /* load value to ACC */
                i = regs[RA];
                if (i > 0)
                    emit(LD, i, RA);
                loadv(sp - 1, 1);
                i = rasn[sp - 1];
            } else {                           /* operand is IN the gprs */
                i %= 16;
                k = regs[RA];
                if (k > 0 && k != i)
                    emit(LD, k, RA);
                if (k != i)
                    emit(LD, RA, i);
            }
            /* now mark ACC active IN case subsequent STO operator */
            regs[RA] = i % 16;
            emit(OUT, j, 0);
            _delete(1);
        }
        /* * check for STD * */
        if (val == STD)
            _delete(1);
        return true;
    case XCH:
        exch();
        return true;
    case DEL:
        if (st[sp] == 0 && rasn[sp] == 0 && litv[sp] < 0) {

            /* value is stacked, so get rid of it */
            emit(POP, RH, 0);
            regv[RH] = regv[RL] = -1;
            ustack();
        }
        _delete(1);
        return true;
    case DAT:
        inldat();
        return false;
    case LOD:
        il = 0;
        skip = false;
        k = prec[sp];

        /* may be a LOD from a base for a based variable */
        prec[sp] = k % 4;
        ia = rasn[sp];
        if (ia <= 0) {
            /* check for simple based variable case */
            i = st[sp];
            if (i > 0) {               /* reserve registers for the result */
                genreg(2, &ia, &ib);
                regs[ia] = sp;
                regs[ib] = sp;
                rasn[sp] = ib * 16 + ia;

                /* may be able to simplify LHLD */
                lp = regv[RH];
                l = regv[RL];
                if (lp != -3 || (-l) != i)
                    if (lp == (-4) && (-l) == i) {
                        emit(DCX, RH, 0);
                        regv[RH] = -3;
                    } else {
                        j = chain(i, codloc + 1);
                        emit(LHLD, j, 0);
                        regv[RH] = -3;
                        regv[RL] = -i;
                    }
                skip = true;
            } else if (st[sp] == 0) {   /* first check for an address reference */
                loadv(sp, 0);
                ia = rasn[sp];
            } else {                    /* change the address reference to a value reference */
                st[sp] = -st[sp];
                litv[sp] = -1;
                return true;
            }
        }
        if (!skip) {
            ib = ia / 16;
            ia = ia % 16;
            i = regs[RA];
            if (ia == i)
                ia = 1;
            if (ib == i)
                ib = 1;
            if (ib == ia - 1)
                il = ib;
            if (ia * ib == 0) {
                error(138, 5);
                return true;
            }
            /* may be possible to use LDAX OR XCHG */
            if (il != RD) {
                emit(LD, RL, ia);
                emit(LD, RH, ib);
                il = 0;
                regv[RH] = regv[RL] = -1;
            } else if (lastex == codloc - 1 || prec[sp] % 2 != 1) {
                /* double XCHG OR double byte load with addr IN d AND e */
                emit(XCHG, 0, 0);
                il = 0;
                regv[RH] = regv[RL] = -1;
            }
        }

        i = prec[sp] - k / 4;
        prec[sp] = i;

        /* recover the register assignment from rasn */
        ib = rasn[sp];
        ia = ib % 16;
        ib = ib / 16;
        j = regs[RA];
        k = j * (j - ia) * (j - ib);

        /* jump if j=0, ia, OR ib */
        if (k != 0)
            emit(LD, j, RA);

        /* may be able to change register assignment to bc */
        if (ia == RE)
            if (regs[RB] == 0 && regs[RC] == 0) {

                /* bc available, so RE-assign */
                regs[ia] = 0;
                regs[ib] = 0;
                regs[RB] = sp;
                regs[RC] = sp;
                ia = RC;
                ib = RB;
                rasn[sp] = RB * 16 + RC;
            }
        regs[RA] = ia;
        if (il == 0)
            emit(LD, RA, ME);

        else
            emit(LDAX, il, 0);
        if (i > 1) {
            emit(INCX, RH, 0);

            /* may have done a prevous LHLD, if so mark INCX h */
            if (regv[RH] == (-3))
                regv[RH] = -4;
            emit(LD, ib, ME);
        } else {

            /* single byte load - release h.o. register */
            ib = rasn[sp];
            rasn[sp] = ib % 16;
            ib = ib / 16;
            if (ib == regs[RA])
                regs[RA] = 0;
            regs[ib] = 0;
            regv[ib] = -1;
        }
        regs[RH] = regs[RL] = st[sp] = 0;
        return true;
    case CSE:

        /* let x be the value of the stack top */
        /* compute 2*x + codloc, fetch to hl, AND jump with PCHL */
        /* reserve registers for the jump table base */
        genreg(2, &ia, &ib);
        lock[ia] = 1;
        lock[ib] = 1;

        /* index is IN h AND l, so double it */
        emit(DAD, RH, 0);

        /* now load the value of table base, depending upon 9 bytes */
        /* LXI r x y, DAD r, mov em, INX h, mov dm XCHG PCHL */
        emit(LXI, ib, codloc + 9);
        emit(DAD, ib, 0);
        emit(LD, RE, ME);
        emit(INCX, RH, 0);
        emit(LD, RD, ME);
        emit(XCHG, 0, 0);
        emit(PCHL, 0, 0);

        /* phoney entry IN symbol table to keep code dump clean */
        symbol[++sytop] = syinfo;
        symbol[syinfo--] = -codloc;

        /* set entry to len=0/prec=2/type=VARB/ */
        symbol[syinfo] = 32 + VARB;
        casjmp = syinfo;

        /* casjmp will be used to update the length field */
        if (--syinfo <= sytop)
            error(108, 5);
        lock[ib] = 0;
        regv[RH] = regv[RL] = -255; /* mark as nil */
        return true;
    case HAL:
        emit(EI, 0, 0);
        emit(HALT, 0, 0);
        return true;
    case RTL: case RTR: case SFL:
    case SFR: case HIV: case LOV:
        unary(val);
        return true;
    case CVA:
        /* CVA must be immediately preceded by an INX OR ADR ref */
        prec[sp] = 2;

        /* if the address is already IN the gpr's then nothing to do */
        if (rasn[sp] > 0)
            return true;
        if (st[sp] < 0) {       /* check for address ref to data IN rom. */
            jp = litv[sp];
            if (jp <= 65535) {
                if (jp < 0)
                    error(149, 1);
                /* leave literal value */
                st[sp] = 0;
                return true;
            }       /* do LXI r with the address */
            genreg(2, &ia, &ib);
            if (ia <= 0) {
                error(140, 5);
                return false;
            } else {
                j = chain(-st[sp], codloc + 1);
                emit(LXI, ib, j);
                st[sp] = 0;
                rasn[sp] = ib * 16 + ia;
                regs[ia] = sp;
                regs[ib] = sp;
                return true;
            }
        } else if (st[sp] > 0) {

            /* load value of base for address ref to a based variable */
            loadv(sp, 3);
            return true;
        } else {
            error(139, 1);
            return false;
        }
        break;      // not needed no path to here
    case ORG:
        i = litv[sp];
        // V4
        if (i < 0)
            error(154, 1);
        else {
            if (codloc > i)
                error(141, 1);
            j = C_STACKHANDLING;
            k = 3;
            if (j == 1)
                k = 0;
            if (codloc == offset + preamb + k) {

                /* this is the start of program, change offset */
                offset = i - preamb;
                codloc = i + k;
                if (lxis > 0)
                    lxis = codloc - 2;
            } else
                for (;;) {

                    /* some code has been generated, so LXI if necessary */
                    if (codloc < i)
                        emit(0, 0, 0);

                    else {
                        if (j != 1) {
                            if (j <= 1) {
                                j = lxis;
                                lxis = codloc + 1;
                            }
                            emit(LXI, RSP, j);
                        }
                        break;
                    }
                }
        }
        sp--;
        return true;
    case DRT:
        jp = prsp;
        if (prstk[jp] > 65535)

            /* this is the END of an interrupt procedure */
            curdep[jp + 1] = curdep[jp + 1] - 4;
        if (prsp > 0)
            prsp = prsp - 1;

        /* get stack depth for symbol table */
        if (jp > 0) {
            if (curdep[jp + 1] != 0)
                error(150, 1);
            k = maxdep[jp + 1];
            l = prstk[jp] % 65536 - 1;

            /* k is max stack depth, l is symbol table count entry */
            symbol[l] = k;
        }
        k = regv[RH];
        l = regv[RL];
        if (k == (-255) && l == (-255))
            return false;
        if (prstk[jp] > 65535) {    /* POP interrupted registers AND enable interrupts */
            emit(POP, RA, 0);
            emit(POP, RB, 0);
            emit(POP, RD, 0);
            emit(POP, RH, 0);
            emit(EI, 0, 0);
        }
        emit(RTN, 0, 0);
        if (k != -254 || l != -254)
            if (jp > 0) {
                updateHL(jp);
                return true;
            } else
                error(146, 1);
            regv[RH] = regv[RL] = -255; /* mark as nil */
            return true;
    case ENA:
        emit(EI, 0, 0);
        return false;
    case DIS:
        emit(DI, 0, 0);
        return false;
    case AX1:
        /* load case number to h AND l */
        exch();
        loadv(sp, 4);
        _delete(1);
        regv[RH] = -1;
        regv[RL] = -1;
        // fall through to shared code
    case TRA:   /* TRA -   check stack for simple LABEL variable */
        iop = 1;
        lock[RH] = lock[RL] = 1;    /* IN case there are any pending values ... */
        saver();
        lock[RH] = lock[RL] = 0;
        m = litv[sp];
        if (m >= 0) {   /* absolute jump - probably to assembly language subrtne... */

            regv[RH] = regv[RL] = -1;       /* ...so make h AND l registers unknown */
            emit(JMP, m, 0);
            _delete(1);
            return true;
        }
        break;
    case AX2:           /* may NOT be omitted even though no obvious path exists). */
        iop = 4;
        /* casjmp points to symbol table attributes - INC len field */
        symbol[casjmp] += 256;
        break;
    }

    i = st[sp];
    if (i > 0) {
        i = symbol[i];
        j = symbol[i - 1];
        j = right(j, 4);

        /* may be a simple variable */
        if (iop != 1 || j != VARB)
            if ((iop != 3 || j != PROC) && j != LABEL) {
                error(135, 1);
                sp--;
                return true;
            } else {
                j = -symbol[i];
                m = symbol[i - 2];

                if (iop == 1) {
                    it = iabs(symbol[i - 1]);
                    it = right(shr(it, 4), 4);
                    /* it is type of LABEL... */
                    /* 3 is user-defined outer block, 4 is user defined */
                    /* NOT outer block, 5 is compiler defined */
                    if (it == 5)
                        /* this TRA is one of a chain of compiler generated */
                        /* TRA's - straighten the chain if no code has been */
                        /* generated since the previous DEF. */
                        if (defsym > 0) {
                            k = symbol[defsym];
                            if (right(shr(symbol[k - 1], 4), 4) == 5) {
                                l = -symbol[k];
                                jp = symbol[k - 2];
                                if (jp == codloc) {
                                    /* adjust the reference counts AND optimization */
                                    /* information for both DEF's. */
                                    ia = shr(iabs(symbol[k - 1]), 8);
                                    ib = 0;
                                    if (ia == 1)
                                        ib = symbol[k - 3];

                                    if (defrh == (-255))
                                        ia = ia - 1;
                                    symbol[k - 1] = 84;

                                    /* i.e., ZERO references to compiler generated LABEL */
                                    if (shr(iabs(symbol[i - 1]), 8) == 1)
                                        symbol[i - 3] = ib;

                                    symbol[i - 1] =
                                        symbol[i - 1] + ia * 256;
                                    for (;;) {
                                        /* corrected reference count for object of the DEF */
                                        /* merge the backstuffing chains */

                                        ia = shr(l, 2);
                                        if (ia == 0) {

                                            /* equate the defs */
                                            for (ia = 1; ia <= sytop; ia++)
                                                if (symbol[ia] == k)
                                                    symbol[ia] = i;

                                            /* omit the TRA if no path to it */
                                            regv[RH] = defrh;
                                            regv[RL] = defrl;
                                            break;
                                        } else {
                                            ib = get(ia) + get(ia + 1) * 256;
                                            // V4
                                            l = shl(ib, 2) + right(l, 2);

                                            symbol[k] = -l;
                                            // V4
                                            ip = shr(j, 2);
                                            put(ia, ip % 256);
                                            put(ia + 1, ip / 256);
                                            // V4
                                            j = shl(ia, 2) + right(j, 2);
                                            symbol[i] = -j;
                                        }
                                    }
                                }
                                if (regv[RH] == (-255)) {
                                    _delete(1);
                                    return true;
                                }
                            }
                        }
                }
                if (it == 3 && iop == 1) {      // BUG FIX user labels all have 4

                    /* we have a TRA to the outer block... */
                    j = C_STACKHANDLING;
                    if (prsp != 0 && j != 1) {
                        if (j == 0) {
                            j = lxis;
                            lxis = codloc + 1;
                        }
                        emit(LXI, RSP, j % 65536);
                    }
                }
                j = -symbol[i];
                // V4
                m = shr(j, 2);

                /* connect entry into chain */
                k = codloc + 1;
                if (iop == 4)
                    k = codloc;

                /* iop = 4 if we arrived here from case table JMP */
// V4
                symbol[i] = -(shl(k, 2) + right(j, 2));

                /* check for single reference */
                j = symbol[i - 1];
                k = iabs(j) / 256;
                if (k == 1) {
                    // V4
                    //        note in a do case block, an implicit goto is
                    //        generated at the end of each selective statement
                    //        in order to branch out to the next statement
                    //        at the end of block. all these gotos are
                    //        considered a single forward reference for the
                    //        the convienance of the h/l optimization
                    //        ************************************************
                    //        make sure if saved h/l is already marked no good
                    //        /   1b   /   1b   /   9b   /   8b   /
                    //        /h valid /l valid /h value /l value /
                    int lsym = symbol[i - 3];
                    if (lsym != -1) {
                        int ktotal = 0;
                        if (0 <= regv[RH] && regv[RH] < 512)
                            if (lsym == 0 || ((lsym & 0x40000) && lsym / 256 % 512 == regv[RH]))
                                ktotal += 0x40000 + regv[RH] * 256;
                        if (0 <= regv[RL] && regv[RL] < 256)
                            if (lsym == 0 || ((lsym & 0x20000) && lsym % 256 == regv[RL]))
                                ktotal += 0x20000 + regv[RL];
                        symbol[i - 3] = ktotal ? ktotal : -1;
                    }

                }

                /* TRA, TRC, PRO, AX2 (case TRA) */
                switch (iop) {
                case 1:

                    /* may be INC TRA combination IN do-loop */
                    if (lastin + 1 == codloc) {

                        /* change to jfz to top of loop */
                        emit(JMC, FAL * 32 + ZERO, m);
                        _delete(1);
                        return true;
                    } else {
                        xfrloc = codloc;
                        xfrsym = st[sp];
                        tstloc = codloc + 3;
                        emit(JMP, m, 0);
                        _delete(1);
                        regv[RH] = regv[RL] = -255; /* mark as nil */
                        return true;
                    }
                    break;
                case 2:
                    conloc = codloc;
                    emit(JMC, iop2, m);
                    _delete(2);
                    return true;
                    ;
                case 3:
                    xfrloc = codloc;
                    xfrsym = st[sp];
                    tstloc = codloc + 3;
                    emit(CAL, m, 0);

                    /* adjust the maxdepth, if necessary */
// V4
                    j = symbol[i - 4] + 1;
                    /* j is number of double-byte stack elements reqd */
                    stack(j);

                    /* now returned from call so... */
                    curdep[prsp + 1] = curdep[prsp + 1] - j;

                    /* now fix the h AND l values upon return */
// V4
                    j = symbol[i - 3];
                    k = shr(j, 19);

                    /* may be unchanged from call */
                    if (k != 3) {

                        /* compare values */
                        j = right(j, 19);
                        l = j % 256;
                        j = j / 256;
                        k = j % 512;
                        j = j / 512;
                        if (j % 2 != 1)
                            l = -1;
                        if ((j / 2) % 2 != 1)
                            k = -1;
                        regv[RH] = k;
                        regv[RL] = l;
                    }
                    _delete(1);

                    /* may have to construct a returned */
                    /* value at the stack top */
                    j = symbol[i - 1];
                    j = j / 16 % 16;
                    if (j > 0) {

                        /* set stack top to precision of procedure */
                        sp = sp + 1;
                        prec[sp] = j;
                        st[sp] = 0;
                        i = RC;
                        if (j > 1)
                            i = RB * 16 + i;
                        rasn[sp] = i;
                        regs[RA] = RC;
                        regs[RC] = sp;
                        if (j > 1)
                            regs[RB] = sp;
                        litv[sp] = -1;
                    }
                    return true;
                case 4:

                    /* came from a case vector */
                    emit(0, m % 256, 0);
                    emit(0, m / 256, 0);
                    _delete(1);
                    return true;
                }
            }
    } else if (iop != 1 || i != 0) {    /* could be a computed address */
        error(134, 1);
        sp--;
        return true;
    }

    /* jump to computed location */
    loadv(sp, 4);
    _delete(1);
    emit(PCHL, 0, 0);

    /* pc has been moved, so mark h AND l unknown */
// V4
    regv[RH] = regv[RL] = -1;
    return true;

}

void updateHL(int jp) {
    xfrloc = codloc - 1;
    xfrsym = 0;
    tstloc = codloc;
    int i = prstk[jp] % 65536;
    int k = regv[RH];
    int l = regv[RL];
    int j = right(symbol[i], 19);
    jp = shr(symbol[i], 19);
    int lp, kp;

    alter = 1;      // hoisted here to simplify return
    if (jp != 3) {
        /* otherwise merge values of h AND l */
        lp = (j & 0x20000) ? j % 256 : -1;
        kp = (j & 0x40000) ? j / 256 % 512 : -1;
    } else if (k == (-254) && l == (-254))
        return;
    else {  /* h AND l have been altered IN the procedure */
        kp = k;
        lp = l;
    }

    /* compare k with kp AND l with lp */
    j = 0;
    if (l >= 0 && lp == l)
        j = 0x20000 + l;
    if (k >= 0 && kp == k)
        j += 0x40000 + (k * 256);;
    symbol[i] = j;
    regv[RH] = regv[RL] = -255;
}




// update condition code for 16 bit, iq == 1 if zero test
void compare16(int icom, int flag, int iq) {
    apply(SU, SB, icom, 1);
    int ip = rasn[sp] % 16;       /* change to condition code */
    int j = rasn[sp] / 16 % 16;
    if (iq == 1)
        emit(OR, ip, 0);

    /* get rid of high order register in the result */
    regs[RA] = ip;
    rasn[sp] = flag * 256 + ip;
    prec[sp] = 1;
    litv[sp] = -1;
    st[sp] = 0;
    if (j != 0) {
        lock[j] = regs[j] = 0;
        regv[j] = -1;
    }
    return;
}

// genrate call to builtin function bf, result determines result reg pair to use
void builtin(int bf, int result) {
    /* clear condition code */
    if (rasn[sp] > 255)
        cvcond(sp);

    /* clear pending store */
    if (regs[RA] != 0) {
        emit(LD, regs[RA], RA);
        regs[RA] = 0;
    }

    /* lock any correctly assigned registers */
    /* ....AND store the remaining registers. */
    if (rasn[sp] % 16 == RE)
        lock[RE] = 1;
    if (rasn[sp] / 16 == RD)
        lock[RD] = 1;
    if (rasn[sp - 1] % 16 == RC)
        lock[RC] = 1;
    if (rasn[sp - 1] / 16 == RB)
        lock[RB] = 1;
    saver();

    /* mark register c used. */
    if (regs[RC] == 0)
        regs[RC] = -1;

    /* load top of stack into registers d AND e. */
    loadv(sp, 0);
    if (prec[sp] == 1)
        emit(LD, RD, 0);

    /* now deassign register c unless correctly loaded. */
    if (regs[RC] == (-1))
        regs[RC] = 0;

    /* load t.o.s. - 1 into registers b AND c. */
    loadv(sp - 1, 0);
    if (prec[sp - 1] == 1)
        emit(LD, RB, 0);
    _delete(2);

    /* call the built-IN function */
    emitbf(bf);

    /* requires 2 levels IN stack for BIF (call AND temp.) */
    stack(2);
    ustack();
    ustack();

    /* AND then retrieve results */
    for (int k = 1; k <= 7; k++)
        lock[k] = 0;

    /* cannot predict where registers h AND l will END up */
    regv[RL] = -1;
    regv[RH] = -1;
    st[++sp] = 0;
    prec[sp] = 2;
    litv[sp] = -1;
    if (result == 2) {
        rasn[sp] = RD * 16 + RE;
        regs[RD] = regs[RE] = sp;
    } else {
        rasn[sp] = RB * 16 + RC;
        regs[RB] = regs[RC] = sp;
    }
}




/* base may be indexed by ZERO... */
void inx(int jp) {
    int i;

    if (litv[sp] == 0)  /* just _delete the index AND ignore the INX operator */
        _delete(1);
    else {
        if (rasn[sp] > 255)
            cvcond(sp);
        int j = regs[RA];
        int il = rasn[sp] % 16;
        int ih = rasn[sp] / 16;
        int jl = rasn[sp - 1] % 16;
        int jh = rasn[sp - 1] / 16;

        /* check for pending store to base OR index */
        if (j != 0 && (j == jh || j == jl || j == ih || j == il)) {
            emit(LD, j, RA);
            regs[RA] = 0;
        }

        /* make sure that d AND e are available */
        if (regs[RE] != 0 || regs[RD] != 0)
            if (il != RE && jl != RE) { /* mark all registers free */
                int ia, ib, ic;
                if (il != 0)
                    regs[il] = 0;
                if (jl != 0)
                    regs[jl] = 0;
                genreg(2, &ia, &ib);
                regs[ia] = 1;
                genreg(2, &ic, &ib);
                regs[ia] = 0;

                /* all regs are cleared except base AND index, if allocated. */
                if (il != 0)
                    regs[il] = sp;
                if (jl != 0)
                    regs[jl] = sp - 1;
            }

        /* if literal 1 OR -1, use INX OR DCX */
        if (litv[sp] != 1 && litv[sp] != 65535) {
            /* if the index is constant, AND the base an address variable, */
            /* double the literal value at compile time */
            if (litv[sp] >= 0 && jp != 1) {
                litv[sp] *= 2;
                jp = 1;
            }
            i = 0;
            if (litv[sp] >= 0)
                i = 3;
            loadv(sp, i);
        }

        /* if the index was already in the registers, may */
        /* have to extend precision to address. */
        il = rasn[sp] % 16;
        ih = rasn[sp] / 16;
        if (il != 0 && ih == 0) {
            ih = il - 1;
            emit(LD, ih, 0);
        }
        i = DAD;
        i = litv[sp] == 1 ? INCX : litv[sp] == 65535 ? DCX : DAD;
        if (ih == 0)
            ih = RH;
        /* _delete the index.  (note that sp will then point to the base) */
        _delete(1);
        /* load the base into the h AND l registers */
        loadv(sp, 5);
        /* ADD the base AND index */
        emit(i, ih, 0);
        /* AND ADD index again if base is an address variable. */
        if (jp != 1)
            emit(i, ih, 0);
        emit(XCHG, 0, 0);
        /* note XCHG here AND remove with peephole optimization later */
        i = prec[sp];
        _delete(1);
        sp = sp + 1;
        st[sp] = 0;
        prec[sp] = i;
        litv[sp] = regv[RH] = regv[RL] = -1;
        rasn[sp] = RD * 16 + RE;
        regs[RD] = regs[RE] = sp;
    }
}



void sydump() {
    int _char[32 + 1];
    int ichar, addr, i, j, k, l, m, n;

    /* dump the symbol table for the simulator */
    /* clear the output buffer */
    writel(0);
    l = 0;

    /* save the current input file number, point input */
    /* at symbol file. */
    m = C_INPUT;
    C_INPUT = C_USYMBOL;

    /* get rid of last card image */
    ibp = 99999;

    do {
        i = gnc(0);
    } while (!(i != 1));
    if (i != 41)
        error(143, 1);

    else
        for (;;) {

            /* process next symbol table entry */
            i = gnc(0);
            if (i == 41)
                break;

            /* process the next symbol */
            i = i - 2;

            /* build address of initialized symbol */
            k = 32;
            for (j = 1; j <= 2; j++) {
                i = (gnc(0) - 2) * k + i;
                k = k * 32;
            }
            if (i <= 4 || i == 6)

                do {
                    j = gnc(0);
                } while (!(j == 41));

            else {
                /* write symbol number, symbol, AND absolute address (octal) */
                form("%5d ", i);
                ichar = 1;
                for (k = 1; k <= 32; k++)
                    _char[k] = 40;
                for (;;) {

                    /* read until next / symbol */
                    j = gnc(0);
                    if (j == 41)
                        break;
                    _char[ichar] = j;
                    ichar = ichar + 1;

                    /* write next character IN string */
                    putch(otran[j]);
                }

                /* END of symbol */
                putch(' ');

                /* write octal address */
                j = symbol[i];
                // V4
                i = symbol[j - 1];
                i %= 16;
                if (i == PROC || i == LABEL)
                    j -= 2;
                i = iabs(symbol[j]);

                form("%05XH   ", i);
                addr = i;
                if (C_BPNF != 0) {
                    n = C_OUTPUT;
                    C_OUTPUT = C_BPNF;
                    writel(0);
                    l = 1;
                    C_OUTPUT = n;
                }
                obp = C_YPAD - 1;
                if (C_MAP != 0) {
                    _form(1, _char, 1, 32, 32);
                    form("%04XH\n", addr);
                }
            }
        }

    C_INPUT = m;
    return;
}



void cmpuse() {
    printf("table usage in pass 2:\n");
    printf("symbol table - max=%-6d, top=%-6d, info=%-6d\n", symax, sytop, syinfo);
    printf("memory table - max=%-6d, top=%-6d, bot=%-6d\n", maxmem, memtop, membot);
}
