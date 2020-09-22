/*************************************************************************

           8 0 8 0   p l / m   c o m p i l e r ,   p a s s - 1
                                   plm81
                                version 2.0/4.0
                               January/November, 1975

                            copyright (c) 1975
                            intel corporation
                            3065 bowers avenue
                            santa clara, california 95051

    includes v2 modifications by jeff ogden (um), december 1977

**************************************************************************/



/*              p a s s - 1    e r r o r   m e s s a g e s

    Error                           Message
    Number
    ------  -------------------------------------------------------------
       1    the symbols printed below have been used in the current block
            but do not appear in a declare statement, or LABEL appears in
            a go to statement but does not appear in the block.

       2    pass-1 compiler symbol table overflow.  Too many symbols in
            the source program.  Either reduce the number of variables in
            the program, or re-compile pass-1 with a larger symbol table.

       3    invalid pl/m statement.  The pair of symbols printed below
            cannot appear together in a valid pl/m statement (this error
            may have been caused be a previous error in the program).

       4    invalid pl/m statement.  The statement is improperly formed--
            the parse to this point follows (this may have occurred be-
            cause of a previous program error).

       5    pass-1 parse stack overflow.  The program statements are
            recursively nested too deeply.  Either simplify the program
            structure, or re-compile pass-1 with a larger parse stack.

       6    number conversion error.  The number either exceeds 65535 or
            contains digits which conflict with the radix indicator.

       7    pass-1 table overflow.  Probable cause is a constant string
            which is too long.  If so, the string should be written as a
            sequence of shorter strings, separated by commas.  Otherwise,
            re-compile pass-1 with a larger varc table.

       8    macro table overflow.  Too many literally declarations.
            either reduce the number of literally declarations, or re-
            compile pass-1 with a larger 'macros' table.

       9    invalid constant in initial, data, or in-line constant.
            Precision of constant exceeds two bytes (may be internal
            pass-1 compiler error).

      10    invalid program.  Program syntax incorrect for termination
            of program.  May be due to previous errors which occurred
            within the program.

      11    invalid placement of a declaration within the pl/m program.
            Procedures and variables may only be declared in the outer
            block (main part of the program) or within do-end groups
            (not iterative do's: case  do-while's: case  or do-case's).

      12    improper use of identifier following an end statement.
            Identifiers can only be used in this way to close a procedure
            definition.

      13    identifier following an end statement does not match the name
            of the procedure which it closes.

      14    duplicate formal parameter name in a procedure heading.

      15    identifier following an end statement cannot be found in the
            program.

      16    duplicate LABEL definition at the same block level.

      17    numeric LABEL exceeds cpu addressing space.

      18    invalid call statement.  The name following the call is not
            a procedure.

      19    invalid destination in a go to.  The value must be a LABEL
            or simple variable.

      20    macro table overflow (see error 8 above).

      21    duplicate variable or LABEL definition.

      22    variable which appears in a data declaration has been pre-
            viously declared in this block

      23    pass-1 symbol table overflow (see error 2 above).

      24    invalid use of an identifier as a variable name.

      25    pass-1 symbol table overflow (see error 2 above).

      26    improperly formed based variable declaration.  The form is
            i based j, where i is an identifier not previously declared
            in this block, and j is an address variable.

      27    symbol table overflow in pass-1 (see error 2 above).

      28    invalid address reference.  The dot operator may only
            precede simple and subscripted variables in this context.

      29    undeclared variable.  The variable must appear in a declare
            statement before its use.  (May also be caused by a recursive
            literally declaration.)

      30    subscripted variable or procedure call references an un-
            declared identifier.  The variable or procedure must be
            declared before it is used.

      31    the identifier is improperly used as a procedure or sub-
            scripted variable.

      32    too many subscripts in a subscripted variable reference.
            pl/m allows only one subscript.

      33    iterative do index is invalid. in the form 'do i = e1 to e2'
            the variable i must be simple (unsubscripted).

      34    attempt to complement a $ control toggle where the toggle
            currently has a value other than 0 or 1.  Use the '= n'
            option following the toggle to avoid this error.

      35    input file number stack overflow.  Re-compile pass-1 with
            a larger instk table.

      36    too many block levels in the pl/m program.  Either simplify
            your program (30 block levels are currently allowed) or
            re-compile pass-1 with a larger block table.

       37   The number of actual parameters in the calling sequence
            is greater than the number of formal parameters declared
            for this procedure.

       38   The number of actual parameters in the calling sequence
            is less than the number of formal parameters declared
            for this procedure.

       39   Invalid interrupt number (must be between 0 and 7)

       40   Duplicate interrupt procedure number.  A procedure
            has been previously specified with an identical
            interrupt attribute.


       41   Procedure appears on left-hand-side of an assignment.

       42   Attempted 'call' of a typed procedure.

       43   Attempted use of an untyped procedure as a function
            or a variable.


       44   This procedure is untyped and should not return a value.

       45   This procedure is typed and should return a value.

       46   'return' is invalid outside a procedure definition.

       47   Illegal use of a LABEL as an identifier.

       48   invalid procedure name (procedure names must be identifiers).

       49   warning - a macro expansion has terminated the scope of
            its own name.  (Probably a bad 'literally' declaration.)
*/

/*
     ------  -------------------------------------------------------------
                i m p l e m e n t a t i o n    n o t e s
                - - - - - - - - - - - - - -    - - - - -
      the pl/m compiler is intended to be written in ansi standard
      fortran - iv, and thus it should be possible to compile and
      execute this program on any machine which supports this fortran
      standard.  both pass-1 and pass-2, however, assume the host
      machine word size is at least 31 bits, excluding the sign bit
      (i.e., 32 bits if the sign is included).

      the implementor may find it necessary to change the source program
      in order to account for system dependencies.  these changes are
      as follows

      1)   the fortran logical unit numbers for various devices
           may have to be changed in the 'gnc' and 'writel' subrou-
           tines (see the file definitions below).

       2)   the host machine may not have the pl/m 52 character set
             0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ$=./()+-'*,<>:;
           (the last 15 special characters are
           dollar,  equal,  period,  slash, left paren,
           right paren, plus,   minus,  quote, asterisk,
           comma, less-than, greater-than, colon, semi-colon)
           in this case: case  it is necessary to change the 'otran' vector in
           block data to a character set which the host machine supports

       3)   the computed go to in 'synth' may be too long for some
           compilers.  if you get a compilation error, break the
           'go to' into two sections.

       4)  the host fortran system may have a limitation on the number
           of contiguous comment records (e.g. s/360 level g). if so,
           intersperse the declaration statements integer i1000, integer
           i1001, etc., as necessary to break up the length of comments.
           the symbols i1xxx are reserved for this purpose.

      there are a number of compiler parameters which may have to
      be changed for your installation.  these parameters are defined
      below (see 'scanner commands'), and the corresponding default
      values are set following their definition.  for example, the
                    $rightmargin = i
      parameter determines the right margin of the input source line.
      the parameter is set externally by a single line starting with
      '$r' in columns one and two (the remaining characters up to
      the '=' are ignored).  the internal compiler representation
      of the character 'r' is 29 (see character codes below), and thus
      the value of the $rightmargin parameter corresponds to element 29
      of the 'contrl' vector.

       1)  the parameters $t, $p, $w, $i, $o, and $r
          control the operating mode of pl/m.  for batch processing,
          assuming 132 character (or larger) print line and 80 charac-
          ter card image, the parameters should default as follows
                  $terminal   =  0
                  $print      =  1
                  $width      = 132
                  $input      =  2
                  $output     =  2
                  $rightmargin= 80
          note that it may be desirable to leave $r=72 to allow room
          for an 8-digit sequence number in columns 73-80 of the pl/m
          source card.

      2)  for interactive processing, assuming a console with width
          of 72 characters (e.g., a tty), these parameters should
          default as follows
                  $terminal   =  1
                  $print      =  1
                  $width      = 72
                  $input      =  1
                  $output     =  1
                  $rightmargin= 72

      3)  the characteristics of the intermediate language files
          produced by pass-1 are governed by the $j, $k, $u, $v, and
          $y parameters.  these parameters correspond to the destination
          and width of the intermediate code file ($j and $k), and
          destination and width of the intermediate symbol table ($u
          and $v).  some fortran systems delete the leading character
          of the files produced by other fortran programs.  the $y
          parameter can be used to pad extra blanks at the beginning of
          the intermediate files if this becomes a problem on the host
          system.

          under normal circumstances, these parameters will not
          have to be changed.  in any case: case  experiment with various
          values of the $ parameters by setting them externally be-
          fore actually changing the defaults.

      the implementor may also wish to increase or decrease the size
      of pass-1 or pass-2 tables.  the tables in pass-1 which may be
      changed in size are 'macros' and 'symbol' which correspond to
      the areas which hold 'literally' definitions and program symbols
      and attributes, respectively.  it is impossible to provide an
      exact formula which relates the number of symbols held by either
      of these tables to the table length, since table space is dy-
      namically allocated according to symbol name length and number
      of attributes required for the particular symbol.

      1)  in the case of the macros table: case  the length is related to the
          total number of characters in the macro names plus the total
          number of characters in the macro definitions - at the deep-
          est block level during compilation.  to change the macro
          table size, alter all occurrences of

                           macros(3000)

          in each subroutine to macros(n), where n represents the new
          integer constant size.  in addition, the 'data' statement
          block data (last program segment) must be changed for the
          macro parameters based upon the constant value n to

             data macros /n*0/, curmac /n+1/, maxmac /n/,
            1    mactop /1/

      the macros table size (n above) must never exceed 4094

      2)  if the implementor wishes to increase or decrease the size
          of the symbol table, then all occurrences of

                            symbol(6000)

          must be changed to symbol(m), where m is the desired integer
          constant size.  the 'data' statements for symbol table para-
          meters must also be altered as described in the corresponding
          comment in block data.  in particular, the last item  of
          the data statement for 'symbol' fills the uninitialized por-
          tion of the table with zeroes, and hence must be the evaluation
          of the element
                             (m-120)*0

          (it is currently (6000-120)*0 = 5880*0).  the data statement
          for maxsym and symabs must be changed to initialize these
          variables to the value m.  (In no case should m be made
          greated than 32000.)

      good luck...


       f  i  l  e     d  e  f  i  n  i  t  i  o  n  s
              input                        output
       num   device  unit        num   device    unit
        1     tty     5           1     tty       5
        2     cdr     2           2     ptr       3
        3     pap     6           3     pap       7
        4     mag     16          4     mag       17
        5     dec     9           5     dec       10
        6     disk    20          6     disk      22
        7     disk    21          7     disk      23

     all input records are 120 characters or less.  all
     output records are 132 characters or less.
     the fortran unit numbers can be changed in the
     subroutines gnc, cmptm and writel (these are the only oc-
     currences of references to these units).



      0 1 2 3 4 5 6 7 8 9
      0 0 0 0 0 0 0 0 1 1
      2 3 4 5 6 7 8 9 0 1


      $ = . / ( ) + - ' * , < > : ;
      3 3 4 4 4 4 4 4 4 4 4 4 5 5 5
      8 9 0 1 2 3 4 5 6 7 8 9 0 1 2


      A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
      1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3
      2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7


    seqno              sub/func name
    15410000      subroutine exitb
    16300000      integer function lookup(iv)
    17270000      integer function enter(infov)
    18050000      subroutine dumpsy
    20030000      subroutine recov
    20420000      logical function stack(q)
    20930000      logical function prok(prd)
    21550000      subroutine reduce
    22100000      subroutine cloop
    22740000      subroutine prsym(cc,sym)
    23120000      integer function getc1(i,j)
    23330000      subroutine scan
    25280000      integer function wrdata(sy)
    26460000      subroutine dumpch
    26960000      subroutine synth(prod,sym)
    36310000      integer function gnc(q)
    37980000      subroutine writel(nspace)
    38520000      function icon(i)        no longer used
    38710000      subroutine decibp
    38850000      subroutine conv(prec)
    39020000      function imin(i,j)
    39090000      subroutine form(cc,chars,start,finish)
    39370000      subroutine conout(cc,k,n,base)
    39690000      subroutine pad(cc,chr,i)
    39800000      subroutine stackc(i)
    39950000      subroutine enterb
    40180000      subroutine dumpin
    40880000      subroutine error(i,level)
    41320000      integer function shr(i,j)
    41360000      integer function shl(i,j)
    41400000      integer function right(i,j)
    41440000      subroutine sdump
    41670000      subroutine redpr(prod,sym)
    41900000      subroutine emit(val,typ)
*/
/*************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

void showVersion(FILE *fp, bool full);


FILE *files[20];

#define ASIZE(n)    (sizeof(n)/sizeof(n[0]))

#define ZPAD    0
/* stacks */
#define MSTACK    75
#define MVAR    256
int sp;
int mp = 0;
//int mpp1 = 1;
int vartop = 1;
int pstack[MSTACK + 1];
struct {
    int    loc : 12,
        len : 12;
} var[MSTACK + 1];
//int var[MSTACK + 1];
int varc[MVAR + 1];
int fixv[MSTACK + 1];
int fixc[MSTACK + 1];
//int prmask[5+1];
bool failsf = false;
bool compil = true;
/* sym */
/*     the '48' used in block initialization and in symbol table*/
/*     initialization is derived from the program 'symcs' which*/
/*     builds the initial symbol table.  if this number changes, be*/
/*     sure to alter 'block', 'blksym', 'symtop', and 'symcnt'.*/
/*     two arrays, sym1 and sym2, are equivalenced over the*/
/*     symbol table array in order to limit the number of*/
/*     continuation cards in symbol table initialization*/
/*     below.  the lengths of sym1 and sym2, therefore, must*/
/*     total the length of the symbol table.  currently, these*/
/*     arrays are declared as follows*/

/*         sym1(60) + sym2(5940) = symbol(6000)*/

/*     if you increase (decrease) the size of symbol, you must*/
/*     increase (decrease) the size of sym2 as well.*/

/*     note also that the remaining entries of the symbol*/
/*     table are set to zero at the end of the data statement*/
/*     for sym2.  currently, this is accomplished with the last*/
/*     entry in the data statement*/

/*                   5880*0*/

/*     again, if you change the size of symbol, you must*/
/*     also change this last entry.  if for example, you alter*/
/*     the size of symbol to 3000, the last entry 1880*0 becomes*/

/*                   2880*0*/
#define SYMABS    10000

/*
Symbol format
    i - 2 (opt) => hash << 16 + index to next symbol in chain
    i - 1 => symbolNo << 16 + index ptr to next symbol
    i => # ints used for symbol << 12 + symbol length    (negated when not in scope)
    i+1 => info => length << 8 + prec << 4 + type    (negated for based)
    i+2... => symbol ints

  prec = 3 if user-defined outer block LABEL
  prec = 4 if user-defined LABEL not in outer block
  prec = 5 if compiler-generated LABEL

*/

enum {
    OuterLabel = 3,
    LocalLabel,
    CompilerLabel
};

/*
Symbol table below initial entries are shown below
Note the index refers to the location of the symbol no information - see above
# index    symbol     hash      value   prec    type
1    2    CARRY         83      0       1       1
2    7    ZERO          104     0       1       1
4    17   PARITY        114     0       1       1
5    23   MEMORY        122     0       1       1
6    29   STACKPTR      13      0       2       1
7    35   ROL           59      2       1       2
8    40   ROR           124     2       1       2
9    45   SHL           82      2       1       2
10   50   SHR           20      2       1       2
11   55   SCL           2       2       1       2
12   60   SCR           67      2       1       2
13   65   TIME          82      1       0       2    chains to SHL
14   70   HIGH          113     1       1       2
15   75   LOW           109     1       1       2
16   80   INPUT         81      1       1       2
17   85   OUTPUT        50      1       1       2
18   91   LENGTH        17      1       1       2
19   97   LAST          26      1       1       2
20   102  MOVE          29      3       0       2
21   107  DOUBLE        9       1       2       2
22   113  DEC           7       1       1       2
3    12   SIGN          77      0       1       1
*/

int symbol[SYMABS + 1] = { ZPAD,
    5439488, 65536, 4101, 17, 221103907, 6815744, 131074, 4100, 17,
    608028224, 5046272, 196615, 4100, 17, 491591168, 7471104, 262156,
    8198, 17, 439207134, 587202560, 7995392, 327697, 8198, 17,
    389903964, 587202560, 851968, 393239, 8200, 33, 494449493,
    444186624, 3866624, 458781, 4099, 530, 476405760, 8126464, 524323,
    4099, 530, 476430336, 5373952, 589864, 4099, 530, 491347968,
    1310720, 655405, 4099, 530, 491372544, 131072, 720946, 4099, 530,
    490037248, 4390912, 786487,
    4099, 530, 490061824, 5373996, 852028, 4100, 258, 508392384, 7405568,
    917569, 4100, 274, 307041408, 7143424, 983110, 4099, 274,
    375787520, 5308416, 1048651, 4101, 274, 325167070, 3276800,
    1114192, 8198, 274, 427681439, 503316480, 1114112, 1179733, 8198,
    274, 373130334, 301989888, 1703936, 1245275, 4100, 274, 372103040,
    1900544, 1310817, 4100, 770, 392561600, 589824, 1376358, 8198, 290,
    241562390, 251658240, 458752, 1441899, 4099, 274, 238866432,
    1507441, 0, 1, 117
};



/* some inline definitions to help make the code more readable */
int iabs(int a);

inline int sym_id(int i) { return symbol[i - 1] >> 16; }
inline int sym_infoLen(int i) { return iabs(symbol[i + 1]) >> 8; }
inline int sym_prec(int i) { return (iabs(symbol[i + 1]) >> 4) & 0xf; }
inline int sym_type(int i) { return iabs(symbol[i + 1]) & 0xf; }

inline int symHash(int i) { return i >> 16; }
inline int symChainNext(int i) { return i & 0xffff; }
inline int symId(int i) { return i >> 16; }
inline int symNext(int i) { return i & 0xffff; }
inline int symInts(int i) { return i >> 12; }
inline int symSymLen(int i) { return i % (1 << 12); }
inline int symInfoLen(int i) { return i >> 8; }
inline int symPrec(int i) { return (i >> 4) % 16; }
inline int symType(int i) { return i % 16; }


int symtop = 120;
int maxsym = SYMABS;
int symloc;
int symlen;
int symcnt = 23;
int acnt = 0;
/* cntrl */
int contrl[64 + 1];
/* _data */
enum { OPR = 0, ADR, VLU, DEF, LIT, LIN };
enum {
    NOP = 0, ADD, ADC, SUB, SBC, MUL, DIV, REM, NEG, AND,
    IOR, XOR, NOT, EQL, LSS, GTR, NEQ, LEQ, GEQ, INX,
    TRA, TRC, PRO, RET, STO, STD, XCH, DEL, DAT, LOD,
    BIF, INC, CSE, END, ENB, ENP, HAL, RTL, RTR, SFL,
    SFR, HIV, LOV, CVA, ORG, DRT, ENA, DIS, AX1, AX2, AX3
};
/* types */
enum { VARB = 1, INTR, PROC, LABEL, LITER };
const int VERS = 40;
/* inter */
int intpro[8 + 1];
/* macro */
#define MACBITS    12    /* number of bits to use for macro index */
#define MAXMAC    3000    /* must be less than 1 << MACBITS */
int macros[MAXMAC + 1];
int curmac = MAXMAC + 1;
int mactop = 1;
/* syntax */
/*     syntax analyzer tables*/
const int v[] = { ZPAD,
    18, 49, 16, 29, 29, 26, 29, 51, 1, 31, 26, 22, 16, 25, 1, 39, 1, 2, 50,
    1, 52, 1, 43, 1, 42, 1, 48, 1, 51, 1, 39, 1, 49, 1, 50, 1, 44, 1,
    45, 1, 47, 1, 41, 1, 40, 2, 20, 17, 2, 15, 26, 2, 18, 26, 2, 31,
    26, 2, 26, 29, 2, 13, 36, 3, 16, 26, 17, 3, 16, 25, 15, 3, 35, 26,
    29, 3, 12, 25, 15, 3, 25, 26, 31, 3, 24, 26, 15, 4, 19, 12, 23, 31,
    4, 31, 19, 16, 25, 4, 16, 23, 30, 16, 4, 14, 12, 30, 16, 4, 14, 12,
    23, 23, 4, 18, 26, 31, 26, 4, 15, 12, 31, 12, 4, 13, 36, 31, 16, 4,
    27, 23, 32, 30, 5, 23, 12, 13, 16, 23, 5, 13, 12, 30, 16, 15, 5,
    24, 20, 25, 32, 30, 5, 34, 19, 20, 23, 16, 6, 16, 25, 12, 13, 23,
    16, 6, 29, 16, 31, 32, 29, 25, 7, 15, 20, 30, 12, 13, 23, 16, 7,
    15, 16, 14, 23, 12, 29, 16, 7, 12, 15, 15, 29, 16, 30, 30, 7, 20,
    25, 20, 31, 20, 12, 23, 8, 49, 25, 32, 24, 13, 16, 29, 50, 8, 49,
    30, 31, 29, 20, 25, 18, 50, 9, 20, 25, 31, 16, 29, 29, 32, 27, 31,
    9, 27, 29, 26, 14, 16, 15, 32, 29, 16, 9, 23, 20, 31, 16, 29, 12,
    23, 23, 36, 12, 49, 20, 15,
    16, 25, 31, 20, 17, 20, 16, 29, 50, 813276224, 808598592, 813315727,
    822083584, 813233943, 822083584, 809879135, 449052672, 814032086,
    264503296, 809865246, 432275456, 809337747, 407310336, 812238417,
    472742976, 812709526, 188021824, 812238039, 192035904, 813741843,
    187786225, 808818205, 506300337, 812709259, 508401201, 813032158,
    257750558, 822083584, 810352653, 372111183, 822083584, 813287375,
    6862622, 822083584, 809023371, 5846878, 822083584, 809023371,
    4780750, 822083584, 811136030, 6862622, 822083584, 808310611,
    291599320, 516161536, 809379484, 259380441, 415498240, 809879135,
    436282315, 247726080, 808556504, 234955723, 247726080, 810352669,
    506323927, 258075712, 814032086, 251712907, 527760448, 810386654,
    321740822,
    326495296, 810386654, 321740818, 254602304, 808761167, 7665039,
    226072369, 813741843, 187786176, 405631985, 808818205, 506300288,
    305968049, 813032158, 257750558, 5846878, 822083584, 808760726,
    7725790, 257750558, 822083584, 812238413, 255457039, 4780750,
    822083584, 812238413, 255457039, 6337999, 822083584, 812168971,
    389931996, 5846878, 822083584, 812168971, 389931996, 4780750,
    822083584, 808499023, 235012828, 321701263, 822083584, 811177043,
    221077520, 188081756, 822083584, 813036317, 225523358, 4780750,
    822083584, 808499027, 218224523, 507343832, 516161536, 809865246,
    419551115, 507343832, 516161536, 813032410, 3732499, 407758041,
    415498240, 810345432, 508363983, 469853405, 516161536, 811177043,
    221077530, 474837724,
    600047616, 812709791, 476055390, 192476623, 410718208, 811119375,
    369157072, 325138323, 425922560, 813315727, 3732310, 191936403,
    425922560, 810410972, 192493144, 3511838, 476408896, 811177043,
    221077533, 255170062, 192035904, 811177043, 221077519, 577356765,
    491623985, 809038678, 191936403, 425722838, 257750558, 822083584,
    812238413, 255457039, 3732499, 407758041, 415498240, 809038678,
    191936403, 425723742, 192476623, 410718208, 808305886, 308082579,
    218167450, 473814867, 425922560, 810345432, 508363983, 469882511,
    223151309, 192493144, 822083584
};

const int vloc[] =
{ ZPAD, 1, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 49,
52, 55, 58, 61, 64, 68, 72, 76, 80, 84, 88, 93, 98, 103, 108, 113,
118, 123, 128, 133, 139, 145, 151, 157, 164, 171, 179, 187, 195,
203, 212, 221, 231, 241, 251, 131336, 131337, 196874, 196876,
229646, 229648, 229650, 262420, 295190, 295192, 295194, 327964,
327966, 327968, 360738, 360741, 360744, 360747, 360750, 360753,
393524, 393527, 393530, 393533, 459072, 459075, 459078, 459081,
491852, 491855, 491858, 524629, 524633, 524637, 524641, 524645,
524649, 524653, 524657, 524661, 557433, 557437, 557441, 557445,
557449, 590221, 590225, 590229, 623001, 623005, 655777, 688549,
721322, 754095, 754100, 852409
};


const char *tokens[] = { "null",
/* 1 */ ";", ")", "(", ",", ":", "=", "<", ">", "+", "-", "*", "/",
    ".",
    /*14 */ "IF", "DO", "GO", "TO", "OR", "BY",
    /*20 */ "EOF", "END", "XOR", "AND", "NOT", "MOD",
    /*26 */ "HALT", "THEN", "ELSE", "CASE", "CALL", "GOTO", "DATA", "BYTE",    "PLUS",
    /*35 */ "LABEL", "BASED", "MINUS", "WHILE",
    /*39 */ "ENABLE", "RETURN", "DISABLE", "DECLARE", "ADDRESS", "INITIAL",
    /*45 */ "<NUMBER>", "<STRING>",
    /*47 */ "INTERRUPT", "PROCEDURE", "LITERALLY",
    /*50 */ "<IDENTIFIER>",
    /*51 */ "<TO>", "<BY>",
    /*53 */ "<TYPE>", "<TERM>",
    /*55 */ "<GROUP>", "<WHILE>", "<GO TO>",
    /*58 */ "<ENDING>", "<PROGRAM>",
    /*60 */ "<REPLACE>", "<PRIMARY>",
    /*62 */ "<VARIABLE>", "<CONSTANT>", "<RELATION>",
    /*65 */ "<STATEMENT>", "<IF CLAUSE>", "<TRUE PART>", "<DATA LIST>", "<DATA HEAD>", "<LEFT PART>",
    /*71 */ "<ASSIGNMENT>", "<EXPRESSION>", "<GROUP HEAD>", "<BOUND HEAD>",
    /*75 */ "<IF STATEMENT>", "<WHILE CLAUSE>", "<INITIAL LIST>", "<INITIAL HEAD>",
    /*79 */ "<CASE SELECTOR>", "<VARIABLE NAME>", "<CONSTANT HEAD>",
    /*82 */ "<STATEMENT LIST>", "<CALL STATEMENT>", "<PROCEDURE HEAD>",
    /*85 */ "<PROCEDURE NAME>", "<PARAMETER LIST>", "<PARAMETER HEAD>",
    /*88 */ "<BASED VARIABLE>", "<LOGICAL FACTOR>", "<SUBSCRIPT HEAD>",
    /*91 */ "<BASIC STATEMENT>", "<GO TO STATEMENT>", "<STEP DEFINITION>",
    /*94 */ "<IDENTIFIER LIST>", "<LOGICAL PRIMARY>",
    /*96 */ "<RETURN STATEMENT>", "<LABEL DEFINITION>", "<TYPE DECLARATION>",
    /*99 */ "<ITERATION CONTROL>", "<LOGICAL SECONDARY>",
    /*101*/ "<LOGICAL EXPRESSION>",
    /*102*/ "<DECLARATION ELEMENT>",
    /*103*/ "<PROCEDURE DEFINITION>",
    /*104*/ "<DECLARATION STATEMENT>", "<ARITHMETIC EXPRESSION>",
    /*106*/ "<IDENTIFIER SPECIFICATION>"
};

/* token ids */
enum {
    SEMIV = 1, DOV = 15, EOFILE = 20, ENDV = 21, CALLV = 30, DECL = 42,
    NUMBV = 45, STRV = 46, PROCV = 48, IDENTV = 50, GROUPV = 55,
    STMTV = 65, SLISTV = 82
};

const unsigned char vindx[] =
{ ZPAD, 1, 14, 20, 26, 35, 39, 41, 45, 47, 50, 50, 50, 51 };



const unsigned char c1[][13] = {
    /*   1 */ {0x08, 0x00, 0x00, 0xA0, 0x02, 0x0A, 0x20, 0xA2, 0x00, 0x80, 0x2A, 0x08, 0x20},
    /*   2 */ {0xA8, 0xAA, 0xAA, 0x02, 0xA8, 0xA0, 0x88, 0x00, 0xEC, 0x08, 0xC0, 0x02, 0x00},
    /*   3 */ {0xC0, 0x00, 0x30, 0x0C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x30},
    /*   4 */ {0xC0, 0x00, 0x20, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x28, 0x30},
    /*   5 */ {0x08, 0x10, 0x00, 0xA0, 0x02, 0x08, 0x20, 0xA0, 0x00, 0x80, 0x2A, 0x08, 0x22},
    /*   6 */ {0xC0, 0x00, 0x30, 0x0C, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x30},
    /*   7 */ {0x80, 0x10, 0x21, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x20},
    /*   8 */ {0x80, 0x10, 0x20, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x20},
    /*   9 */ {0x40, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  10 */ {0x40, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  11 */ {0x40, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  12 */ {0x40, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  13 */ {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  14 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  15 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x10, 0x00, 0x00, 0x10},
    /*  16 */ {0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  17 */ {0x80, 0x00, 0x20, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x28, 0x20},
    /*  18 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  19 */ {0x80, 0x00, 0x20, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x28, 0x20},
    /*  20 */ {0x04, 0x00, 0x00, 0x50, 0x01, 0x00, 0x10, 0x50, 0x00, 0x40, 0x15, 0x04, 0x10},
    /*  21 */ {0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10},
    /*  22 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  23 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  24 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  25 */ {0x40, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  26 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  27 */ {0x08, 0x00, 0x00, 0xA0, 0x02, 0x00, 0x20, 0xA0, 0x00, 0x80, 0x2A, 0x08, 0x20},
    /*  28 */ {0x08, 0x00, 0x00, 0xA0, 0x02, 0x00, 0x20, 0xA0, 0x00, 0x80, 0x2A, 0x08, 0x20},
    /*  29 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  30 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10},
    /*  31 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x20},
    /*  32 */ {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  33 */ {0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00},
    /*  34 */ {0x40, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  35 */ {0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00},
    /*  36 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20},
    /*  37 */ {0x40, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  38 */ {0x80, 0x00, 0x20, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x28, 0x20},
    /*  39 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  40 */ {0x48, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  41 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  42 */ {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10},
    /*  43 */ {0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00},
    /*  44 */ {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  45 */ {0x3C, 0xA6, 0xAA, 0x02, 0xA8, 0xA0, 0x88, 0x00, 0x20, 0x08, 0x00, 0x00, 0x00},
    /*  46 */ {0x28, 0xA2, 0xAA, 0x02, 0xA8, 0xA0, 0x88, 0x00, 0x20, 0x08, 0x00, 0x00, 0x00},
    /*  47 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00},
    /*  48 */ {0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x80, 0x80, 0x00},
    /*  49 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00},
    /*  50 */ {0xF8, 0xAF, 0xAA, 0x02, 0xA8, 0xA0, 0x88, 0x00, 0xA9, 0x09, 0x80, 0x00, 0x04},
    /*  51 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  52 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  53 */ {0x0C, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00},
    /*  54 */ {0x28, 0xA2, 0x6A, 0x01, 0xA8, 0xA0, 0x84, 0x00, 0x20, 0x08, 0x00, 0x00, 0x00},
    /*  55 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  56 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  57 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x10},
    /*  58 */ {0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  59 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  60 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  61 */ {0x28, 0xA2, 0xAA, 0x02, 0xA8, 0xA0, 0x88, 0x00, 0x20, 0x08, 0x00, 0x00, 0x00},
    /*  62 */ {0x28, 0xB7, 0xAA, 0x02, 0xA8, 0xA0, 0x88, 0x00, 0x20, 0x08, 0x00, 0x00, 0x00},
    /*  63 */ {0x38, 0xA3, 0xAA, 0x02, 0xA8, 0xA0, 0x88, 0x00, 0x20, 0x08, 0x00, 0x00, 0x00},
    /*  64 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  65 */ {0x08, 0x00, 0x00, 0xA0, 0x02, 0x0A, 0x20, 0xA0, 0x00, 0x80, 0x2A, 0x08, 0x20},
    /*  66 */ {0x04, 0x00, 0x00, 0x50, 0x01, 0x00, 0x10, 0x50, 0x00, 0x40, 0x15, 0x04, 0x10},
    /*  67 */ {0x04, 0x00, 0x00, 0x50, 0x01, 0x00, 0x10, 0x50, 0x00, 0x40, 0x15, 0x04, 0x10},
    /*  68 */ {0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  69 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00},
    /*  70 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10},
    /*  71 */ {0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  72 */ {0x18, 0x01, 0x00, 0x00, 0x44, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  73 */ {0x04, 0x00, 0x00, 0x50, 0x01, 0x04, 0x10, 0x50, 0x00, 0x40, 0x15, 0x04, 0x10},
    /*  74 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00},
    /*  75 */ {0x08, 0x00, 0x00, 0xA0, 0x02, 0x0A, 0x20, 0xA0, 0x00, 0x80, 0x2A, 0x08, 0x20},
    /*  76 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  77 */ {0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00},
    /*  78 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00},
    /*  79 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  80 */ {0x90, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x80, 0x00, 0x00},
    /*  81 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00},
    /*  82 */ {0x04, 0x00, 0x00, 0x50, 0x01, 0x06, 0x10, 0x50, 0x00, 0x40, 0x15, 0x04, 0x10},
    /*  83 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  84 */ {0x04, 0x00, 0x00, 0x50, 0x01, 0x00, 0x10, 0x50, 0x00, 0x40, 0x15, 0x04, 0x10},
    /*  85 */ {0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x40, 0x40, 0x00},
    /*  86 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x40, 0x00, 0x00},
    /*  87 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10},
    /*  88 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10},
    /*  89 */ {0x28, 0x02, 0x00, 0x00, 0xA8, 0x60, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  90 */ {0x40, 0x00, 0x10, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0x10},
    /*  91 */ {0x08, 0x00, 0x00, 0xA0, 0x02, 0x0A, 0x20, 0xA3, 0x00, 0x80, 0x2A, 0x08, 0x20},
    /*  92 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  93 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  94 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10},
    /*  95 */ {0x28, 0x02, 0x00, 0x00, 0xA8, 0xA0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  96 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /*  97 */ {0x04, 0x00, 0x00, 0x50, 0x01, 0x04, 0x10, 0x50, 0x00, 0x40, 0x15, 0x04, 0x11},
    /*  98 */ {0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00},
    /*  99 */ {0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* 100 */ {0x28, 0x02, 0x00, 0x00, 0xA8, 0xA0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* 101 */ {0x28, 0x02, 0x00, 0x00, 0x98, 0x10, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* 102 */ {0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* 103 */ {0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* 104 */ {0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* 105 */ {0x28, 0x52, 0x15, 0x00, 0xA8, 0xA0, 0x80, 0x00, 0x10, 0x04, 0x00, 0x00, 0x00},
    /* 106 */ {0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x00, 0x40, 0x00, 0x00},
    /* 107 */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};


#define TRI(a,b,c) (((a) << 16) + ((b) << 8) + c)

const int c1tri[] = {
        TRI(3,3,3), TRI(3,3,10), TRI(3,3,13), TRI(3,3,24), TRI(3,3,45),
        TRI(3,3,46), TRI(3,3,50), TRI(3,50,3), TRI(5,6,3), TRI(5,6,10),
        TRI(5,6,13), TRI(5,6,24), TRI(5,6,45), TRI(5,6,46), TRI(5,6,50),
        TRI(6,3,3), TRI(6,3,10), TRI(6,3,13), TRI(6,3,24), TRI(6,3,45),
        TRI(6,3,46), TRI(6,3,50), TRI(6,50,3), TRI(9,3,3), TRI(9,3,10),
        TRI(9,3,13), TRI(9,3,24), TRI(9,3,45), TRI(9,3,46), TRI(9,3,50),
        TRI(9,50,3), TRI(10,3,3), TRI(10,3,10), TRI(10,3,13), TRI(10,3,24),
        TRI(10,3,45), TRI(10,3,46), TRI(10,3,50), TRI(10,50,3), TRI(11,3,3),
        TRI(11,3,10), TRI(11,3,13), TRI(11,3,24), TRI(11,3,45), TRI(11,3,46),
        TRI(11,3,50), TRI(11,50,3), TRI(12,3,3), TRI(12,3,10), TRI(12,3,13),
        TRI(12,3,24), TRI(12,3,45), TRI(12,3,46), TRI(12,3,50), TRI(12,50,3),
        TRI(13,50,3), TRI(14,3,3), TRI(14,3,10), TRI(14,3,13), TRI(14,3,24),
        TRI(14,3,45), TRI(14,3,46), TRI(14,3,50), TRI(14,50,3), TRI(15,50,3),
        TRI(15,62,6), TRI(18,3,3), TRI(18,3,10), TRI(18,3,13), TRI(18,3,24),
        TRI(18,3,45), TRI(18,3,46), TRI(18,3,50), TRI(18,50,3), TRI(20,50,3),
        TRI(20,50,5), TRI(20,62,4), TRI(20,62,6), TRI(20,71,1), TRI(22,3,3),
        TRI(22,3,10), TRI(22,3,13), TRI(22,3,24), TRI(22,3,45), TRI(22,3,46),
        TRI(22,3,50), TRI(22,50,3), TRI(23,3,3), TRI(23,3,10), TRI(23,3,13),
        TRI(23,3,24), TRI(23,3,45), TRI(23,3,46), TRI(23,3,50), TRI(23,50,3),
        TRI(24,3,3), TRI(24,3,10), TRI(24,3,13), TRI(24,3,24), TRI(24,3,45),
        TRI(24,3,46), TRI(24,3,50), TRI(24,50,3), TRI(25,3,3), TRI(25,3,10),
        TRI(25,3,13), TRI(25,3,24), TRI(25,3,45), TRI(25,3,46), TRI(25,3,50),
        TRI(25,50,3), TRI(29,3,3), TRI(29,3,10), TRI(29,3,13), TRI(29,3,24),
        TRI(29,3,45), TRI(29,3,46), TRI(29,3,50), TRI(29,50,3), TRI(30,50,3),
        TRI(34,3,3), TRI(34,3,10), TRI(34,3,13), TRI(34,3,24), TRI(34,3,45),
        TRI(34,3,46), TRI(34,3,50), TRI(34,50,3), TRI(37,3,3), TRI(37,3,10),
        TRI(37,3,13), TRI(37,3,24), TRI(37,3,45), TRI(37,3,46), TRI(37,3,50),
        TRI(37,50,3), TRI(40,3,3), TRI(40,3,10), TRI(40,3,13), TRI(40,3,24),
        TRI(40,3,45), TRI(40,3,46), TRI(40,3,50), TRI(40,50,3), TRI(45,2,33),
        TRI(45,2,35), TRI(45,2,43), TRI(47,45,1), TRI(51,3,3), TRI(51,3,10),
        TRI(51,3,13), TRI(51,3,24), TRI(51,3,45), TRI(51,3,46), TRI(51,3,50),
        TRI(51,50,3), TRI(52,3,3), TRI(52,3,10), TRI(52,3,13), TRI(52,3,24),
        TRI(52,3,45), TRI(52,3,46), TRI(52,3,50), TRI(52,50,3), TRI(56,3,3),
        TRI(56,3,10), TRI(56,3,13), TRI(56,3,24), TRI(56,3,45), TRI(56,3,46),
        TRI(56,3,50), TRI(56,50,3), TRI(60,3,3), TRI(60,3,10), TRI(60,3,13),
        TRI(60,3,24), TRI(60,3,45), TRI(60,3,46), TRI(60,3,50), TRI(60,50,3),
        TRI(64,3,3), TRI(64,3,10), TRI(64,3,13), TRI(64,3,24), TRI(64,3,45),
        TRI(64,3,46), TRI(64,3,50), TRI(64,50,3), TRI(66,50,3), TRI(66,50,5),
        TRI(66,62,4), TRI(66,62,6), TRI(66,71,1), TRI(66,91,28), TRI(67,50,3),
        TRI(67,50,5), TRI(67,62,4), TRI(67,62,6), TRI(67,71,1), TRI(69,63,2),
        TRI(69,63,4), TRI(70,50,3), TRI(70,62,4), TRI(70,62,6), TRI(73,50,3),
        TRI(73,50,5), TRI(73,62,4), TRI(73,62,6), TRI(73,71,1), TRI(74,45,2),
        TRI(78,63,2), TRI(78,63,4), TRI(81,63,2), TRI(81,63,4), TRI(82,50,3),
        TRI(82,50,5), TRI(82,62,4), TRI(82,62,6), TRI(82,71,1), TRI(84,50,3),
        TRI(84,50,5), TRI(84,62,4), TRI(84,62,6), TRI(84,71,1), TRI(85,53,1),
        TRI(86,53,1), TRI(87,50,2), TRI(87,50,4), TRI(90,3,3), TRI(90,3,10),
        TRI(90,3,13), TRI(90,3,24), TRI(90,3,45), TRI(90,3,46), TRI(90,3,50),
        TRI(90,50,3), TRI(97,50,3), TRI(97,50,5), TRI(97,62,4), TRI(97,62,6),
        TRI(97,71,1), TRI(104,4,3), TRI(104,4,50)
};


const int prtb[] =
{ 0, 5592629, 5582637, 21813, 21846, 3933, 3916, 3919, 85, 15,
71, 55, 103, 96, 83, 92, 104, 26, 39, 41, 0, 17727, 20031, 22322,
24144, 20799, 840, 23112, 32, 106, 44, 13, 50, 0, 0, 22322, 17727,
24144, 20031, 20799, 23112, 62, 50, 45, 7, 8, 0, 0, 0, 7, 0, 16, 0,
0, 0, 3656, 91, 0, 0, 0, 50, 0, 0, 0, 57, 0, 12849, 0, 97, 21, 57,
88, 0, 0, 4861186, 106, 26889, 26890, 26914, 26917, 10, 0, 21586,
97, 73, 13835, 13836, 13849, 0, 30, 13, 0, 13, 0, 16963, 82, 73,
66, 0, 50, 70, 3360820, 15932, 51, 56, 29, 40, 97, 0, 98, 0, 0,
25874, 25878, 0, 97, 0, 24, 0, 0, 4078664, 22807, 0, 4064518, 0,
26628, 42, 26944, 0
};

const unsigned char prdtb[] =
{ 0, 38, 39, 36, 37, 25, 26, 27, 35, 24, 6, 7, 8, 9, 10, 11, 12,
13, 14, 15, 16, 61, 78, 41, 72, 114, 117, 121, 62, 70, 79, 118,
122, 42, 73, 43, 63, 74, 80, 119, 123, 84, 47, 48, 100, 101, 96,
83, 97, 99, 98, 54, 126, 127, 44, 21, 22, 55, 67, 69, 77, 128, 49,
68, 53, 125, 59, 124, 40, 45, 52, 76, 75, 120, 65, 64, 103, 104,
105, 106, 107, 102, 34, 46, 23, 109, 110, 111, 108, 51, 116, 115,
113, 112, 19, 3, 28, 18, 2, 60, 82, 31, 81, 30, 32, 33, 50, 20, 5,
66, 71, 1, 88, 89, 87, 17, 4, 93, 92, 58, 29, 91, 90, 86, 85, 57,
56, 95, 94
};


const unsigned char hdtb[] =
{ 0, 84, 84, 84, 84, 73, 73, 73, 84, 73, 91, 91, 91, 91, 91, 91,
91, 91, 91, 91, 91, 68, 77, 86, 106, 61, 61, 62, 69, 74, 78, 81,
90, 87, 94, 87, 69, 94, 78, 81, 90, 70, 97, 97, 64, 64, 64, 60, 64,
64, 64, 57, 51, 52, 58, 66, 67, 57, 53, 53, 88, 56, 96, 53, 92, 63,
102, 63, 85, 58, 92, 80, 80, 62, 98, 98, 105, 105, 105, 105, 105,
105, 103, 58, 55, 54, 54, 54, 54, 83, 61, 61, 61, 61, 75, 82, 73,
75, 82, 102, 71, 99, 71, 99, 76, 79, 96, 75, 65, 98, 106, 59, 101,
101, 101, 91, 65, 100, 100, 102, 93, 89, 89, 72, 72, 104, 104, 95,
95
};
const unsigned char prlen[] =
{ 0, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 3, 3, 3, 3, 3, 3, 2, 2,
2, 2, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 3, 2, 1, 1, 1, 2, 1, 1, 1, 2,
1, 3, 1, 2, 2, 2, 2, 1, 1, 4, 2, 3, 3, 3, 3, 2, 1, 3, 2, 2, 3, 3,
3, 1, 2, 2, 1, 2, 1, 3, 2, 2, 2, 1, 2, 2, 4, 3, 2, 2, 2, 2, 2, 1,
2, 1, 1, 3, 3, 1, 2, 1, 2, 1, 1, 4, 3, 1, 4, 1, 3, 2, 3, 1
};

const unsigned char contc[] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
const unsigned char leftc[] = { ZPAD, 105, 4, 42, 94, 85 };
const unsigned char lefti[] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5,
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5
};


const unsigned char prind[] =
{ 1, 21, 28, 35, 42, 44, 48, 49, 51, 51, 51, 51, 51, 51, 51, 51,
51, 53, 53, 54, 54, 55, 55, 55, 55, 55, 55, 56, 57, 57, 57, 58, 58,
59, 59, 60, 61, 61, 62, 62, 63, 63, 63, 64, 64, 66, 68, 68, 69, 69,
74, 74, 74, 76, 82, 82, 82, 82, 85, 85, 85, 89, 92, 94, 94, 99, 99,
99, 100, 100, 100, 101, 107, 107, 107, 109, 109, 110, 110, 110,
111, 111, 112, 112, 112, 112, 112, 112, 112, 115, 115, 117, 117,
117, 117, 119, 119, 119, 120, 121, 123, 125, 127, 127, 127, 129,
129
};





int nt = 50;
int vil = 12;
//const int nc1tri = sizeof(c1tri) / sizeof(c1tri[0]) - 2;    // - ZPAD - 1

#define PACK 5                // number of packed chars per int
#define MAXBLK 30
int token = 0;
/* blk */
int block[MAXBLK + 1] = { ZPAD, 1, 120 };
int dopar[MAXBLK + 1];
int macblk[MAXBLK + 1];
int curblk = 2;
int blksym = 120;
int proctp[MAXBLK + 1];
/* files */
char obuff[132 + 1];
int ibp = 81;
int obp = 0;
int inptr = 0;
int instk[7 + 1];
char itran[256];
const char otran[] =
" 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ$=./()+-'*,<>:;            ";
/* asc */
/*     translation table from internal to ascii*/
const unsigned char ascii[64] =
{ 32, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 65, 66, 67, 68, 69,
70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86,
87, 88, 89, 90, 36, 61, 46, 47, 40, 41, 43, 45, 39, 42, 44, 60, 62,
58, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
/* pole */
int poltop = 0;
/* scanc */
int acclen;
int accum[32 + 1];
int type;
enum { EOFLAG = 1, IDENT, NUMB, SPECL, STR };
int stype = 0;
const int CONT = 1;
int value;
/* hash */
int hentry[127 + 1] =
{ ZPAD, 0, 54, 0, 0, 0, 0, 112, 0, 106, 0, 0, 0, 28, 0, 0, 0, 90, 0, 0,
49, 0, 0, 0, 0, 0, 96, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 0, 34, 0, 0, 0,
0, 0, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 79, 64, 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0,
0, 0, 0, 74, 0, 0, 0, 69, 16, 0, 0, 0, 0, 0, 0, 0, 22, 0, 39, 0, 0,
0
};
int hcode;


/* function declarations */
int main(int argc, char **argv);
int iabs(int a);
void exitb();
int lookup(const int iv);
int enter(int info);
void dumpsy();
void recov();
bool stack( /*const int q */);
bool prok(const int prd);
void reduce();
void cloop();
//void prsym(const int cc, const int sym);
int getc1(int i, int j);
void scan();
int wrdata(const int sy);
void dumpch();
void synth(const int prod, const int symm);
int gnc();
void parseOptions(char *s);
void writel(int nspac);
void decibp();
void conv(const int prec);
int imin(const int i, const int j);
void form(char *fmt, ...);        // ascii version using printf formats
//void conout(const int cc, const int k, const int n, const int base);
//void pad(const int cc, const int chr, const int i);
void putch(const int chr);
void stackc(const int i);
void enterb();
void dumpin();
void error(const int i, const int level);
//int shr(const int i, const int j);
//int shl(const int i, const int j);
int right(const int i, const int j);
void sdump();
void redpr(const int prod, const int sym);
void emit(const int val, const int typ);
void cmpuse();

int iabs(int a)
{
    return a < 0 ? -a : a;
}

enum {
    SPACE = 1, ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE,
    CHA, CHB, CHC, CHD, CHE, CHF, CHG, CHH, CHI, CHJ, CHK, CHL, CHM,
    CHN, CHO, CHP, CHQ, CHR, CHS, CHT, CHU, CHV, CHW, CHX, CHY, CHZ,
    DOLLAR, EQUALS, DOT, SLASH, LPAREN, RPAREN, PLUS, MINUS, QUOTE,
    STAR, COMMA, LESS, GREATER, COLON, SEMICOLON
};

void dumpTable()
{
    FILE *fp = fopen("sym.dmp", "at");
    for (int j = 0; j <= symtop; j++) {
        fprintf(fp, "%d,%c", symbol[j], (j % 6) == 0 ? '\n' : ' ');
    }
    fprintf(fp, "\nentry=%d\n\n", symtop);
    fclose(fp);
}



/*     syntax analyzer tables */
/*      global tables */
/*     global variables */
/*     the following scanner commands are defined */
/*     analyze = i      (12)  print syntax analysis trace */
/*     bypass           (13)  bypass stack dump on error */
/*     count = i        (14)  begin line count at i */
/*     delete = i       (15) */
/*     eof              (16) */
/*     generate         (18) */
/*     input = i        (20) */
/*     jfile (code)= i  (21) */
/*     kwidth (cd)= i   (22) */
/*     leftmargin = i   (23) */
/*     memory = i       (24) */
/*     output = i       (26) */
/*     print (t or f)   (27) */
/*     rightmarg = i    (29) */
/*     symbols          (30) */
/*     terminal         (31) (0=batch, 1=term, 2=interlist) */
/*     usymbol = i      (32) */
/*     vwidth (sym) = i (33) */
/*     width = i        (34) */
/*     ypad = n         (36)  blank pad on output */
/*     contrl(1) is the error count */
#define errorCnt    contrl[1]
#define C_ANALYZE    contrl[CHA]
#define C_BYPASS    contrl[CHB]
#define C_COUNT        contrl[CHC]
#define C_DELETE    contrl[CHD]
#define C_EOF        contrl[CHE]
#define C_GENERATE    contrl[CHG]
#define C_INPUT        contrl[CHI]
#define C_JFILE        contrl[CHJ]
#define C_KWIDTH    contrl[CHK]
#define C_LEFTMARG    contrl[CHL]
#define C_MEMORY    contrl[CHM]
#define C_OUTPUT    contrl[CHO]
#define C_PRINT        contrl[CHP]
#define C_RIGHTMARG    contrl[CHR]
#define C_SYMBOLS    contrl[CHS]
#define C_TERMINAL    contrl[CHT]
#define C_USYMBOL    contrl[CHU]
#define C_VWIDTH    contrl[CHV]
#define C_WIDTH        contrl[CHW]
#define C_YPAD        contrl[CHY]


// get the file to use
// auto open file if first use


/*
    although fort.nn is still supported, this routine presets up several files based
    on fname. This allows a command line to specify plm81 file.plm and the fort.nn files
    are named to reflect the file stem
*/
void initFiles(char *fname) {
    char path[_MAX_PATH];
    char *s, *t;

    FILE *fp;
    if ((fp = fopen(fname, "rt")) == NULL) {
        fprintf(stderr, "can't open %s\n", fname);
        exit(1);
    }
    files[C_INPUT] = fp;
    strcpy(path, fname);
    for (s = path; t = strpbrk(s, "\\/"); s = t + 1)   // find the filename
        ;
    if (t = strrchr(s, '.'))                            // replace extent
        *t = 0;
    strcpy(t, ".pol");
    if ((fp = fopen(path, "wt")) == NULL) {
        fprintf(stderr, "can't create %s\n", path);
        exit(1);
    }
    files[C_JFILE + 10] = fp;
    strcpy(t, ".sym");
    if ((fp = fopen(path, "wt")) == NULL) {
        fprintf(stderr, "can't create %s\n", path);
        exit(1);
    }
    files[C_USYMBOL + 10] = fp;

    strcpy(t, ".lst");
    if ((fp = fopen(path, "wt")) == NULL) {
        fprintf(stderr, "can't create %s\n", path);
        exit(1);
    }
    files[C_OUTPUT + 10] = fp;
}


FILE *getfile(int i, int direction)
{
    char fname[8];

    if (i > 9) {
        fprintf(stderr, "logical file number > 9\n");
        exit(1);
    }
    if (direction > 0) i += 10;
    if (files[i] == NULL) {
        if (i == 1)
            files[i] = stdin;
        else if (i == 11)
            files[i] = stdout;
        else {
            sprintf(fname, "fort.%d", i);
            if ((files[i] = fopen(fname, i < 10 ? "rt" : "wt")) == NULL) {
                fprintf(stderr, "can't %s %s\n", i < 10 ? "open" : "create", fname);
                exit(1);
            }
        }
    }
    return files[i];
}

void closefiles()
{
    int i;
    for (i = 0; i < 20; i++)
        if (files[i] != NULL && i != 1 && i != 11)
            fclose(files[i]);
}

int main(int argc, char **argv)
{
    int i, j, k;

    if (argc == 2 && stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        printf("\nUsage: plm81 -v | -V  | [plmfile]\n"
            "Where\n"
            "-v/-V      provide version infomation\n"
            "plmfile    is the source file, of the form prefix.ext e.g. m80.plm\n"
            "           intermediate files prefix.lst, prefix.pol, prefix.sym are created\n\n"
            "If plmfile is not specified, source is taken from fort.2\n"
            "the created files are fort.12 (lst), fort.16 (pol) file and fort.17 (sym)\n");
        exit(0);
    }



    for (i = 1; i <= 64; i++)
        contrl[i] = -1;
    errorCnt = 0;
    C_ANALYZE = 0;
    C_BYPASS = 1;
    C_COUNT = 0;
    C_DELETE = 132;    /* changed from original 120 */
    C_EOF = 0;
    C_GENERATE = 0;
    C_INPUT = 2;
    C_JFILE = 6;
    C_KWIDTH = 72;
    C_LEFTMARG = 1;
    C_MEMORY = 1;
    C_OUTPUT = 2;    /* changed from original 1 */
    C_PRINT = 1;
    C_RIGHTMARG = 120;
    C_SYMBOLS = 0;
    C_TERMINAL = 1; 
    C_USYMBOL = 7;
    C_VWIDTH = 72;
    C_WIDTH = 132;    /* changed from original 132 */
    C_YPAD = 1;

    if (argc > 1)
        initFiles(argv[1]);

    // setup input translation table
    memset(itran, SPACE, 256);
    for (i = SPACE; i <= SEMICOLON; i++) {
        itran[otran[i - 1]] = i;
        if (isalpha(otran[i - 1]))
            itran[tolower(otran[i - 1])] = i;
    }
    time_t now;
    time(&now);
    form("\n         pl/m-8080  version 4.0 - %s", ctime(&now));
    writel(1);
    for (i = 1; i <= 3; i++)
        pstack[i] = 0;
    pstack[4] = EOFILE;
    sp = 4;
    scan();
    cloop();
    emit(NOP, OPR);
    while (poltop != 0)
        emit(NOP, OPR);
    /*     print error count */
    i = errorCnt;
    j = C_OUTPUT;
    k = j;
    for (;;) {
        writel(0);
        C_OUTPUT = j;
        if (i == 0)
            form("\nNO");
        else
            form("%d", i);
        form(" PROGRAM ERROR");
        if (i != 1)
            form("S");
        form("\n \n");
        /*     check for terminal control of a batch run */
        if (j != 1 && C_TERMINAL != 0)
            /*     arrive here if terminal toggle gt 0, and output not console */
            j = 1;
        else {
            C_OUTPUT = k;
            dumpsy();
            /*     may want a symbol table for the simulator */
            if (C_MEMORY == 0)
                symbol[2] = 0;
            dumpch();
            dumpin();
            closefiles();
            cmpuse();
            return errorCnt;
        }
    }
}

void exitb()
{
    int np, kp, lp, i, j, k, l, n;
    bool erred;
    /*     goes through here upon block exit */
    /*      global tables */
    erred = false;
    if (curblk > 2) {
        i = block[curblk];
        /* de-allocate those macro definitions whose scope we are leaving,
         * and check if any of these are currently in expansion.
         */
        mactop = macblk[curblk];

        for (n = curmac; n <= MAXMAC; n++) {
            j = right(macros[n], MACBITS);
            if (mactop <= j) {
                macros[n] = j + (j << MACBITS);
                error(49, 1);
            }
        }
        curblk--;
        for (j = symbol[symtop]; j >= i; j = right(symbol[j], 16)) {
            if (symbol[j + 1] >= 0) {
                k = iabs(symbol[j + 2]);    // info
                kp = symType(k);
                lp = symInfoLen(kp);        // always 0!. possibly fortran should be lp = shr(k, 8)
                if (kp < LITER) {
                    if (kp == VARB || kp == LABEL) {
                        if (symPrec(k) == 0) {
                            if (kp == LABEL && curblk > 1)    // labels may be non local
                                continue;            // only fail if not defined at all
                            if (!erred) {
                                error(1, 1);
                                erred = true;
                            }
                            form("\n     ");
                            if ((n = symInts(symbol[j + 1])) != 0) {
                                for (kp = 1; kp <= n; kp++) {
                                    l = symbol[j + 2 + kp];
                                    for (lp = 1; lp <= PACK; lp++)
                                        putch(otran[right(l >> (30 - lp * 6), 6)]);
                                }
                                putch('\n');
                            }
                        }
                    }
                    symbol[j + 1] = -symbol[j + 1];    // negate length fields
                                                        // to mark out of scope
                    /* may want to fix the hash code chain */
                    if (lp > 0) {
                        /* find match on the entry */
                        k = j - 1;
                        kp = symbol[k];
                        hcode = symHash(kp);;
                        kp = symChainNext(kp);
                        n = hentry[hcode];
                        if (n == k)
                            /* this entry is directly connected */
                            hentry[hcode] = kp;
                        else {
                            /* look through some literals in the symbol table above */
                            while ((np = right(symbol[n], 16)) != k)
                                n = np;
                            symbol[n] = (hcode >> 16) + kp;
                        }
                    }
                }
            }

        }
        blksym = block[curblk];
    }
}

int lookup(const int iv)
{
    int jp, lp, j, k, m;
    /*     syntax analyzer tables */
    /*      global tables */
    bool sflag = pstack[iv] != NUMBV;

    symlen = var[iv].len;
    symloc = var[iv].loc;
    j = symloc;
    k = PACK * 6;
    m = 0;
    if (varc[j] <= 52) {
        for (jp = 0; jp < symlen; jp++) {
            if ((k -= 6) < 0) {
                varc[j++] = m;
                m = 0;
                k = PACK * 6 - 6;
            }
            m = ((varc[jp + symloc] - 1) << k) + m;
        }
        varc[j] = m;
    }
    /*     varc is now in packed form ready for lookup */
    /*     compute hash code (reduce numbers mod 127, use first 5 chars of */
    /*     identifiers and strings ) */
    hcode = sflag ? varc[symloc] : fixv[iv];
    hcode = hcode % 127 + 1;
    /*     hcode is in the range 1 to 127 */
    lp = (symlen - 1) / PACK + 1;
    for (k = hentry[hcode]; k > 0; k = symChainNext(symbol[k])) {
        if (!sflag) {
            /*     compare numbers in internal form rather than characters */
            if (symType(j = symbol[k + 3]) > LITER && symInfoLen(j) == fixv[iv])
                break;
        } else if (symSymLen(symbol[k + 2]) == symlen) {
            j = k + 3;
            jp = symloc;
            bool match = true;
            for (m = 1; m <= lp && match; m++, jp++) {
                match = varc[jp] == symbol[j + m];
            }
            /*     symbol found */
            if (match) {
                /*     make sure the types match. */
                jp = pstack[iv];
                m = right(iabs(symbol[k + 3]), 4);
                if (jp == STRV && m == LITER || jp == IDENTV && m < LITER)
                    break;
            }
        }
    }
    /*     jp is identifier, m is variable, LABEL, or procedure. */
    return (k <= 0) ? 0 : k + 2;
}

int enter(int info)
{
    int ihash, i, j, q, link;
    /*     syntax analyzer tables */

    /*      global tables */
    /*      enter assumes a previous call to lookup (either that, or set up */
    /*      the values of symloc and symlen in the varc array). */
    /*         also set-up hash code value (see lookup), if necessary */
    i = symtop;
    link = symbol[i];
    if (info >= 0) {
        /*     compute hash table entry */
        /*     fix collision chain */
        symbol[i] = (hcode << 16) + hentry[hcode];
        hentry[hcode] = i++;
        ihash = 1;
        q = (symlen - 1) / PACK + 1;
    } else {
        /*     entry with no external name */
        info = -info;
        q = symlen = hcode = ihash = 0;
    }

    symtop = symtop + q + ihash + 3;

    if (symtop > maxsym) {
        i = ihash;
        symtop = q + ihash + 3;
        error(2, 5);
    }
    symbol[symtop] = i;
    symbol[i] = (++symcnt << 16) + link;
    symbol[++i] = (q << 12) + symlen;
    symbol[i + 1] = info;
    for (j = 0; j < q; j++)
        symbol[i + 2 + j] = varc[symloc + j];

    return i;
}



void dumpsy()
{
    int kp, lp, ic, mc, ip, it, i, j, k, _k, l, m, n;
    /*      global tables */
    ic = C_SYMBOLS;
    if (ic != 0) {
        writel(0);
        if (ic > 1)
            form("\nSYMBOL  ADDR WDS CHRS   LENGTH PR TY");
        for (i = symbol[symtop] & 0xffff, it = symtop; i > 0; it = i, i = symbol[i] & 0xffff) {
            /*     quick check for zero length name */
            if (ic >= 2 || iabs(symbol[i + 1]) >> 12 > 0)
                form("\nS%05d", symId(symbol[i]));

            if (ic >= 2) {
                k = symbol[i + 1];
                form("%c ", k < 0 ? '*' : ' ');    // * if now out of scope
                k = iabs(k);
                form("%04d %3d %4d ", i + 1, symInts(k), symSymLen(k));

                k = symbol[i + 2];
                form("%c ", k < 0 ? 'B' : 'R');    // based or regular
                k = iabs(k);
                form("%06d%3d%3d", symInfoLen(k), symPrec(k), symType(k));
            }
            putch(' ');
            ip = i + 2;
            k = iabs(symbol[i + 1]);
            n = symInts(k);
            if (n != 0) {                        // has size
                mc = symSymLen(k);
                m = symType(iabs(symbol[i + 2]));
                if (m == LITER)
                    putch('\'');
                for (kp = 1; kp <= n; kp++) {
                    l = symbol[kp + ip];
                    for (lp = 1; lp <= PACK && mc-- > 0; lp++)
                        putch(otran[(l >> (30 - lp * 6)) & 0x3f]);
                }
                if (m == LITER)
                    putch('\'');
            }
            ip = ip + n;
            if (ic >= 2)
                while (++ip < it) {
                    k = symbol[ip];
                    form(" %c%08XH", (k < 0) ? '-' : ' ', iabs(k));
                }
        }
        writel(0);
    }
    writel(0);
    _k = C_OUTPUT;            // bug in original code
    C_OUTPUT = C_USYMBOL;
    kp = C_WIDTH;
    C_WIDTH = C_VWIDTH;
    /*     write the interrupt procedure names */
    form("/");
    for (i = 1; i <= 8; i++) {
        if ((j = intpro[i]) > 0) {     /* write intnumber symbolnum (4 base-32 digits) */
            putch(otran[i]);
            for (l = 0; l < 3; l++) {
                putch(otran[j % 32 + 1]);
                j /= 32;
            }
            form("/");
        }
    }
    form("/");
    writel(0);


    /*     reverse the symbol table pointers */
    /*     set the length field of compiler-generated labels to 1 */

    i = symtop;
    j = symbol[i];
    symbol[i] = 0;
    while (j != 0) {
        k = symbol[j + 2];
        /* set length to 1 and prec to 5 (for comp generated labels) */
        if (symType(k) == LABEL && symInfoLen(k) == 0)
            symbol[j + 2] = (1 << 8) + (CompilerLabel << 4) + LABEL;
        m = symbol[j];
        symbol[j] = i;
        i = j;
        j = m & 0xffff;
    }

    putch('/');
    for (j = 2; symbol[j]; j = symbol[j]) {
        lp = symbol[j + 2];    /* sym info */
        putch(lp < 0 ? '-' : ' ');
        lp = iabs(lp);
        do {
            putch(otran[lp % 32 + 1]);
        } while ((lp /= 32) != 0);

        if (symbol[j + 2] < 0) {
            lp = symbol[j + symInts(iabs(symbol[j + 1])) + 3];
            putch(lp < 0 ? '-' : ' ');
            lp = iabs(lp);
            do {
                putch(otran[lp % 32 + 1]);
            } while ((lp /= 32) != 0);
        }
        putch('/');
    }

    putch('/');
    writel(0);
    C_OUTPUT = _k;
    C_WIDTH = kp;
}

void recov()
{
    int i;
    /*      global tables */
    for (;;) {
        /*     find something solid in the text */
        if (token == DECL || token == PROCV || token == ENDV
            || token == DOV || token == SEMIV || token == EOFILE)
            for (;;) {
                /*     and in the stack */
                i = pstack[sp];
                if (failsf && getc1(i, token) != 0) {
                    failsf = false;
                    return;
                }
                if (i == EOFILE && token == EOFILE) {
                    failsf = compil = false;
                    return;
                }
                if ((i == GROUPV || i == SLISTV || i == STMTV
                    || i == DOV || i == PROCV) && token != EOFILE)
                    break;
                /*         but don't go too far */
                if (sp <= 4)
                    break;
                vartop = var[sp--].loc;
            }
        scan();
    }
}

// simple compare for bsearch
int intcmp(const int *a, const int *b) { return *a - *b; }

bool stack( /*const int q */)
{
    int j;

    for (;;) {
        ;
        switch (getc1(pstack[sp], token)) {
        case 0:
            /*     illegal symbol pair */
            error(3, 1);
            form("\n%s %s", tokens[pstack[sp]], tokens[token]);
            sdump();
            recov();
            /*     recover may have set compiling false */
            if (!compil)
                return true;
            break;
        case 1:
            return true;
        case 2:
            return false;
        case 3:
            j = TRI(pstack[sp - 1], pstack[sp], token);
            return bsearch(&j, c1tri, ASIZE(c1tri), sizeof(c1tri[0]),
                (int(*)(const void*, const void*)) intcmp) != NULL;
        }
    }
}

bool prok(const int prd)
{
    int lp, i, j, k, l;
    /*      context check of equal or imbedded right parts */
    switch (contc[prd]) {
    case 0:
        /*     no check required */
        return true;
    case 2:
        /*     left context check */
        k = hdtb[prd] - nt;
        i = pstack[sp - prlen[prd]];
        l = lefti[k - 1] + 1;
        lp = lefti[k];
        if (l <= lp)
            for (j = l; j <= lp; j++)
                if (leftc[j] == i)
                    return true;
        break;
    default:
        assert(0);                // will force alert shouldn't happen
    }
    return false;
}

void reduce()
{
    int prd, i, j, k, l, m, ltemp;
    /*     pack stack top */
    j = 0;
    for (i = sp - 4; i <= sp - 1; i++)
        j = (j << 8) + pstack[i];
    ltemp = pstack[sp];
    k = prind[ltemp - 1];
    l = prind[ltemp];

    for (prd = k; prd < l; prd++) {
        m = right(j, 8 * (prlen[prd] - 1));    // m = significant items to reduce by
        if (m == prtb[prd] && prok(prd)) {
            mp = sp - prlen[prd] + 1;    // mp -> lowest item on stack
            synth(prdtb[prd], hdtb[prd]);    // do the action
            sp = mp;                    // do the reduce
            pstack[sp] = hdtb[prd];
            vartop = var[sp].loc;
            return;
        }
    }
    /*     no applicable production */
    error(4, 1);
    failsf = false;
    sdump();
    recov();
    return;
}

void cloop()
{
    int i, j;
    /*      global tables */
    compil = true;
    while (compil)
        if (!stack())
            reduce();
        else {
            /*     stack may have set compiling false */
            if (!compil)
                break;
            if (++sp >= MSTACK) {
                error(5, 5);
                break;
            } else {
                pstack[sp] = token;
                /*     insert accum into varc here */
                if (token == NUMBV) {
                    conv(16);
                    if (value < 0) {
                        error(6, 1);
                        value = 0;
                    }
                    fixv[sp] = value;
                }
                var[sp].loc = vartop;
                for (;;) {
                    if (acclen != 0)
                        for (j = 1; j <= acclen; j++) {
                            varc[vartop] = accum[j];
                            if (++vartop > MVAR) {
                                error(7, 5);
                                vartop = 1;
                            }
                        }
                    /* string constatncs may be continued over several buffersful */
                    if (token != STRV || stype != CONT)
                        break;
                    scan();
                }
                i = vartop - var[sp].loc;
                var[sp].len = i < 0 ? 1 : i;
                scan();
            }
        }
        return;
}

/*
    getc1(stackitem, token)
    returns action code
    0 - illegal combination
    1 - stack
    2 - reduce
    3 - look at top three items is match in c1tri then stack else reduce
*/
int getc1(int i, int j)
{
    return (c1[i - 1][j / 4] >> ((j % 4) << 1)) & 3;

}

void scan()
{
    int lp, i, j, k, l, m, n;
    /*      global tables */
    /*     scan finds the next entity in the input stream */
    /*     the resulting item is placed into accum (of length */
    /*     acclen).  type and stype identify the item as shown */
    /*     below -- */
    /*     type     stype         item           variable */
    /*       1        na        end of file       EOFLAG */
    /*       2       cont       identifier        IDENT */
    /*       3       radix      number            NUMB */
    /*       4        na        spec char         SPECL */
    /*       5        cont      string            STR */

    failsf = true;
    for (;;) {
        i = gnc();
        acclen = 0;
        if (stype == CONT)
            switch (type) {
            case EOFLAG:
                type = EOFLAG; token = EOFILE; return;
            case IDENT:
                goto L80;
            case NUMB:
            case SPECL:
                break;
            case STR:
                /*     continue with string */
                decibp();
                goto L70;
            }
        for (;;) {
            if (i == 0) { type = EOFLAG; token = EOFILE; return; }
            if (i == 1)    // space
                ;
            else if (ZERO <= i && i <= NINE) { // digit
                /*     number */
                type = NUMB;
                stype = 0;
                for (;;) {
                    accum[++acclen] = i;
                    if (acclen != 32) {
                        while ((i = gnc()) == DOLLAR)    // gobble $ in number
                            ;
                        if (i < 2 || i > 17)    // not a hex digit
                            break;
                    } else {
                        stype = CONT;
                        while (ZERO <= (i = gnc()) && i <= CHF)
                            ;
                        decibp();
                        break;
                    }
                }
                if (stype == CONT)
                    break;
                /*     check radix */
                if (i == CHH)    // H
                    stype = 16;
                if (i == CHQ || i == CHO)    // Q or O
                    stype = 8;
                if (stype == 0) {
                    if (accum[acclen] == CHB) {    // B
                        stype = 2;
                        acclen = acclen - 1;
                    } else if (accum[acclen] != CHD) //D
                        stype = 10;
                    else {
                        stype = 10;
                        acclen = acclen - 1;
                    }
                    decibp();
                }
                for (i = 1; i <= acclen; i++) {
                    j = accum[i] - 2;
                    if (j >= stype) {
                        stype = 1;
                        break;
                    }
                }
                break;
            } else if (CHA <= i && i <= CHZ) {    // alpha character
            L80:            type = IDENT;                    /*     identifier */
                for (;;) {
                    accum[++acclen] = i;
                    if (acclen >= 32) {
                        stype = CONT;
                        break;
                    } else {
                        while ((i = gnc()) == DOLLAR)    // gobble up $ in name
                            ;
                        if (i < ZERO || i > CHZ) {        // not alphanumeric
                            decibp();
                            stype = 0;
                            break;
                        }
                    }
                }
                break;
            } else if (CHZ < i && i <= 64) {    // non alpha numeric
                if (i == QUOTE) {        // quote
                L70:                type = STR;        /*     string quote */
                    accum[1] = 1;
                    for (;;) {
                        i = gnc( /*0 */);
                        if (i == QUOTE && (i = gnc()) != QUOTE) {    // end quote or double quote
                            decibp();        // backup one
                            stype = 0;        // no more to collect
                            break;
                        }
                        accum[++acclen] = i;    // stuff char (double quote reduced to single)
                        if (acclen >= 32) {
                            stype = CONT;
                            break;
                        }
                    }
                    break;
                }
                type = SPECL;
                acclen = 1;
                accum[1] = i;
                if (i != SLASH)        // slash
                    break;
                i = gnc( /*0 */);
                /*     look for comment */
                if (i != STAR) {        // star
                    decibp();
                    break;
                } else
                    for (;;) {
                        /*     comment found */
                        i = gnc( /*0 */);
                        if (i == 0) {
                            type = EOFLAG; token = EOFILE; return;
                        }
                        if (i == STAR) {    // star
                            i = gnc( /*0 */);
                            if (i != SLASH)    // slash
                                decibp();
                            else {
                                acclen = 0;
                                break;
                            }
                        }
                    }
            }
            /*     deblank input */
            i = gnc( /*0 */);
        }
        /*     the code below is here to satisfy the syntax analyzer */
        if (type == EOFLAG) {
            token = EOFILE;
            return;
        }
        if (type == STR) {
            token = STRV;
            return;
        }
        token = 0;
        if (acclen <= vil) {
            /*     search for token in vocabulary */
            j = vindx[acclen] + 1;
            k = vindx[acclen + 1];
            for (i = j; i <= k; i++) {
                l = vloc[i];
                lp = l + v[l];
                l++;
                bool match = true;
                for (m = l, n = 1; m <= lp && match; m++, n++)
                    match = accum[n] == v[m];

                if (match) {
                    token = i - 1;
                    return;
                }
            }
        }
        if (type != IDENT) {
            if (type == NUMB)
                token = NUMBV;
            return;
        }

        l = mactop;
        bool match = false;
        while (!match) {
            if ((l = macros[l]) == 0) {        // end of macros
                token = type == NUMB ? NUMBV : IDENTV;
                return;
            }
            if ((k = macros[l + 1]) == acclen) {    // is the length the same
                match = true;
                for (j = 1, i = l + 2; j <= k && match; j++, i++)
                    match = accum[j] == macros[i];
            }
        }
        /*     macro found, set-up macro table and rescan */
        curmac = curmac - 1;
        if (curmac <= mactop) {
            error(8, 5);
            curmac = MAXMAC + 1;
        } else {
            j = i + macros[i];
            macros[curmac] = (i << MACBITS) + j;    /* merge the begin end and indexes */
            macros[l + 1] = -k;
        }
    }
    token = i - 1;
    return;
}

int wrdata(const int sy)
{
    bool dflag;
    int np, ip, i, j, k, l, m, n, _wrdata, kp, nbytes, lp;
    /*     if sy is negative, the call comes from synth -- data is inserted */
    /*     inline by calling LIT with each byte value. */

    /*     if sy is positive, the call comes from dumpin -- */
    /*     wrdata writes data into the output file from symbol at location */
    /*     'sy'  each byte value is written as a pair of base 32 digits. */
    /*     the high order bit of the first digit is 1, and all remaining high */
    /*     order digits are zero. the value returned by wrdata is the total */
    /*     number of bytes written. */
    /*      global tables */
    nbytes = 0;
    j = iabs(sy);

    /*     check precision of value */
    k = symbol[j + 1];
    /*     set dflag to true if we are dumping a variable or LABEL name */
    l = right(k, 4);
    dflag = (l == LABEL) || (l == VARB) || (l == PROC);
    l = right(k >> 4, 4);
    if (l <= 2 && !dflag) {

        /*     single or double byte constant */
        kp = k >> 8;
        k = 16;
        nbytes = l;
        while (l > 0) {
            /*     process next byte */
            l = l - 1;
            n = right(kp >> (l * 8), 8);
            if (sy < 0)

                /*     otherwise emit data inline */
                emit(n, LIT);
            else
                /*         n is then written in two parts */
                for (i = 1; i <= 2; i++) {
                    k = right(n >> ((2 - i) * 4), 4) + k + 2;
                    putch(otran[k - 1]);
                    //                                      pad(1,k,1);
                    k = 0;
                }
        }
    } else {
        l = right(iabs(symbol[j]), 12);
        j = j + 1;
        k = 16;
        n = -1;
        np = (PACK - 1) * 6;
        lp = 1;
        while (lp <= l) {
            if (n < 0) {
                n = np;
                j = j + 1;
                m = symbol[j];
            }
            nbytes = nbytes + 1;
            kp = right(m >> n, 6) + 1;
            if (dflag)

                /*     write out the variable or LABEL name */
                putch(otran[kp - 1]);
            else {
                kp = ascii[kp - 1];

                /*    write out both hex values */
                if (sy < 0)

                    /*     emit string data inline */
                    emit(kp, LIT);
                else

                    for (ip = 1; ip <= 2; ip++) {
                        k = right(kp >> ((2 - ip) * 4), 4) + k + 2;
                        putch(otran[k - 1]);
                        k = 0;
                    }
            }
            n = n - 6;
            lp = lp + 1;
        }
    }
    _wrdata = nbytes;
    return _wrdata;
}

void dumpch()
{
    int i, j, kq, k, l, m, kt;
    /*     dump the symbolic names for the simulator */
    writel(0);
    kt = C_OUTPUT;
    C_OUTPUT = C_USYMBOL;
    kq = C_WIDTH;
    C_WIDTH = C_VWIDTH;

    putch('/');
    i = symbol[2] == 0 ? 0 : 2;
    for (k = 1; i; k++, i = symbol[i]) {
        if ((j = symbol[i + 2]) >= 0) {
            if ((j = symType(j)) == LABEL || j == VARB || j == PROC) {
                /* check if real symbol */
                if (symInts(iabs(symbol[i + 1]))) {
                    /* write symbol number */
                    m = k;
                    for (l = 0; l < 3; l++) {
                        putch(otran[m % 32 + 1]);
                        m = m / 32;
                    }
                    /* now write the string */
                    wrdata(i + 1);
                    putch('/');
                }
            }
        }
    }
    putch('/');
    writel(0);
    C_OUTPUT = kt;
    C_WIDTH = kq;
}

void synth(const int prod, const int symm)
{
    int ip, i, j, k, l, m, n, kp, ltemp;

    /*    mp == left ,  sp == right */

    /*      global tables */
    if (C_ANALYZE != 0)
        redpr(prod, symm);
    switch (prod) {
    case   1: // <PROGRAM> ::= <STATEMENT LIST>
        if (mp != 5)
            error(10, 1);    /* invalid program */
        compil = false;
        exitb();
        return;
    case   2: // <STATEMENT LIST> ::= <STATEMENT>
    case   3: // <STATEMENT LIST> ::= <STATEMENT LIST> <STATEMENT>
    case   4: // <STATEMENT> ::= <BASIC STATEMENT>
    case   5: // <STATEMENT> ::= <IF STATEMENT>
        return;
    case   6: // <BASIC STATEMENT> ::= <ASSIGNMENT> ';'
        while (acnt > 0) {
            i = symbol[maxsym - acnt];
            acnt--;
            if (i <= 0)
                emit(XCH, OPR);
            else {
                emit(sym_id(i), ADR);
            }
            if (acnt > 0)
                emit(STO, OPR);
        }
        emit(STD, OPR);
        return;
    case   7: // <BASIC STATEMENT> ::= <GROUP> ';'

        return;

    case   8: // <BASIC STATEMENT> ::= <PROCEDURE DEFINITION> ';'
        goto case12;
    case   9: // <BASIC STATEMENT> ::= <RETURN STATEMENT> ';'
    case  10: // <BASIC STATEMENT> ::= <CALL STATEMENT> ';'
    case  11: // <BASIC STATEMENT> ::= <GO TO STATEMENT> ';'
        return;
    case  12: // <BASIC STATEMENT> ::= <DECLARATION STATEMENT> ';'
    case12:
        if (right(dopar[curblk], 2) != 0)
            error(11, 1);
        return;
    case  13: // <BASIC STATEMENT> ::= HALT ';'
        emit(HAL, OPR);
        return;
    case  14: // <BASIC STATEMENT> ::= ENABLE ';'
        emit(ENA, OPR);
        return;
    case  15: // <BASIC STATEMENT> ::= DISABLE ';'
        emit(DIS, OPR);
        return;
    case  16: // <BASIC STATEMENT> ::= ';'
    case  17: // <BASIC STATEMENT> ::= <LABEL DEFINITION> <BASIC STATEMENT>
        return;
    case  18: // <IF STATEMENT> ::= <IF CLAUSE> <STATEMENT>
        // fall through to common code
    case  19: // <IF STATEMENT> ::= <IF CLAUSE> <TRUE PART> <STATEMENT>
        i = fixv[mp];
        emit(sym_id(i), DEF);
        symbol[i + 1] = 64 + LABEL;
        return;
    case  20: // <IF STATEMENT> ::= <LABEL DEFINITION> <IF STATEMENT>
        return;
    case  21: // <IF CLAUSE> ::= IF <EXPRESSION> THEN
        i = enter(-LABEL);
        emit(sym_id(i), VLU);
        emit(TRC, OPR);
        fixv[mp] = i;
        return;
    case  22: // <TRUE PART> ::= <BASIC STATEMENT> ELSE
        i = enter(-LABEL);
        emit(sym_id(i), VLU);
        emit(TRA, OPR);
        j = fixv[mp - 1];
        fixv[mp - 1] = i;
        i = j;
        emit(sym_id(i), DEF);
        symbol[i + 1] = 64 + LABEL;
        return;
    case  23: // <GROUP> ::= <GROUP HEAD> <ENDING>
        if (fixv[sp] > 0)
            error(12, 1);
        else if (fixc[sp] < 0)
            fixc[mp] = 0;
        i = dopar[curblk + 1];
        j = right(i, 2);
        i = i >> 2;
        switch (j) {
        case 0:
            emit(END, OPR);
            return;
        case 1:
            /*     end of iterative statement */
            k = fixv[mp];
            if (k != 0) {
                /*     otherwise increment variable */
                emit(k, VLU);
                emit(INC, OPR);
                emit(k, ADR);
                emit(STD, OPR);
            }
            break;
        case 2:
            break;
        case 3:
            /*     generate destination of case branch */
            j = right(i, 14);
            k = sym_id(j);
            emit(k, DEF);
            m = symbol[j + 1] >> 8;
            symbol[j + 1] = right(symbol[j + 1], 8);
            /*     m is symbol number of LABEL at end of jump table */
            emit(CSE, OPR);
            /*     define the jump table */
            i = i >> 14;
            /*     reverse the LABEL list */
            l = 0;
            for (;;) {
                if (i != 0) {
                    k = symbol[i + 1];
                    symbol[i + 1] = (l << 8) + right(k, 8);
                    l = i;
                    i = k >> 8;
                } else {
                    for (;;) {
                        /*     emit list starting at l */
                        i = symbol[l + 1];
                        symbol[l + 1] = 64 + LABEL;
                        if ((j = i >> 8) == 0) {
                            /*     define end of jump table */
                            emit(m, DEF);
                            break;
                        } else {
                            k = sym_id(l);
                            emit(k, VLU);
                            emit(AX2, OPR);
                            l = j;
                        }
                    }
                    break;
                }
            }
            return;
        }
        /*     define end of while statement */
        emit((i >> 14), VLU);
        emit(TRA, OPR);
        emit(right(i, 14), DEF);
        return;
    case  24: // <GROUP HEAD> ::= DO ';'
        enterb();
        emit(ENB, OPR);
        return;
    case  25: // <GROUP HEAD> ::= DO <STEP DEFINITION> ';'
        enterb();
        dopar[curblk] = 1 + (fixv[mp + 1] << 2);
        return;
    case  26: // <GROUP HEAD> ::= DO <WHILE CLAUSE> ';'
        enterb();
        dopar[curblk] = 2 + (fixv[mp + 1] << 2);
        return;
    case  27: // <GROUP HEAD> ::= DO <CASE SELECTOR> ';'
        enterb();
        k = enter(-(64 + LABEL));
        k = (symbol[k - 1] >> 16);
        /*     k is LABEL after case jump table */
        i = enter(-((k << 8) + 64 + LABEL));
        j = sym_id(i);
        emit(j, VLU);
        emit(AX1, OPR);
        dopar[curblk] = (i << 2) + 3;
        i = dopar[curblk];
        k = (i >> 16);
        j = enter(-((k << 8) + 64 + LABEL));
        dopar[curblk] = (j << 16) + right(i, 16);
        emit(sym_id(j), DEF);
        return;
    case  28: // <GROUP HEAD> ::= <GROUP HEAD> <STATEMENT>
        i = dopar[curblk];
        if (right(i, 2) != 3)
            return;
        /*     otherwise case stmt */
        j = right((i >> 2), 14);
        j = symbol[j + 1];
        j = (j >> 8);
        emit(j, VLU);
        emit(TRA, OPR);
        i = dopar[curblk];
        k = (i >> 16);
        j = enter(-((k << 8) + 64 + LABEL));
        dopar[curblk] = (j << 16) + right(i, 16);
        emit(sym_id(j), DEF);
        return;
    case  29: // <STEP DEFINITION> ::= <VARIABLE> <REPLACE> <EXPRESSION> <ITERATION CONTROL>
        i = fixv[mp];
        j = fixv[mp + 3];
        if (j >= 0)
            i = 0;
        /*     place <variable> symbol number into do slot */
        fixv[mp - 1] = i;
        fixv[mp] = iabs(j);
        return;
    case  30: // <ITERATION CONTROL> ::= <TO> <EXPRESSION>
        emit(LEQ, OPR);
        i = enter(-(64 + LABEL));
        i = sym_id(i);
        emit(i, VLU);
        emit(TRC, OPR);
        fixv[mp] = -((fixv[mp] << 14) + i);
        return;
    case  31: // <ITERATION CONTROL> ::= <TO> <EXPRESSION> <BY> <EXPRESSION>
        i = fixv[mp - 3];
        /*     i = symbol number of indexing variable */
        emit(i, VLU);
        emit(ADD, OPR);
        emit(i, ADR);
        emit(STD, OPR);
        /*     branch to compare */
        i = fixv[mp + 2];
        emit((i >> 14), VLU);
        emit(TRA, OPR);
        /*     define beginning of statements */
        emit(right(i, 14), DEF);
        return;
    case  32: // <WHILE CLAUSE> ::= <WHILE> <EXPRESSION>
        i = enter(-(64 + LABEL));
        j = fixv[mp];
        i = sym_id(i);
        fixv[mp] = (j << 14) + i;
        /*     (back branch number/end loop number) */
        emit(i, VLU);
        emit(TRC, OPR);
        return;
    case  33: // <CASE SELECTOR> ::= CASE <EXPRESSION>
        return;
    case  34: // <PROCEDURE DEFINITION> ::= <PROCEDURE HEAD> <STATEMENT LIST> <ENDING>
        k = (fixv[mp] >> 15);
        j = fixv[sp];
        if (j < 0)
            j = -j + 1;
        if (j != 0 && right(fixv[mp], 15) != j)
            error(13, 1);
        emit(END, OPR);
        /*     emit a ret just in case he forgot it */
        emit(DRT, OPR);
        emit((symbol[k - 1] >> 16), DEF);
        return;
    case  35: // <PROCEDURE HEAD> ::= <PROCEDURE NAME> ';'
        proctp[curblk] = 1;
        i = fixv[mp];
        symbol[i + 1] = (0 << 8) + (0 << 4) + PROC;
        j = enter(-(64 + LABEL));
        fixv[mp] = (j << 15) + i;
        emit(sym_id(j), VLU);
        emit(TRA, OPR);
        emit(sym_id(i), DEF);
        return;
    case  36: // <PROCEDURE HEAD> ::= <PROCEDURE NAME> <TYPE> ';'
        k = fixv[sp - 1];
        proctp[curblk] = 2;
        i = fixv[mp];
        symbol[i + 1] = (0 << 8) + (k << 4) + PROC;
        j = enter(-(64 + LABEL));
        fixv[mp] = (j << 15) + i;
        emit(sym_id(j), VLU);
        emit(TRA, OPR);
        emit(sym_id(i), DEF);
        return;
    case  37: // <PROCEDURE HEAD> ::= <PROCEDURE NAME> <PARAMETER LIST> ';'
        l = fixv[mp + 1];
        k = 0;
        proctp[curblk] = 1;
        i = fixv[mp];
        symbol[i + 1] = (l << 8) + (k << 4) + PROC;
        j = enter(-(64 + LABEL));
        fixv[mp] = (j << 15) + i;
        emit(sym_id(j), VLU);
        emit(TRA, OPR);
        emit(sym_id(i), DEF);
        return;
    case  38: // <PROCEDURE HEAD> ::= <PROCEDURE NAME> <PARAMETER LIST> <TYPE> ';'
        l = fixv[mp + 1];
        k = fixv[sp - 1];
        proctp[curblk] = 2;
        i = fixv[mp];
        symbol[i + 1] = (l << 8) + (k << 4) + PROC;
        j = enter(-(64 + LABEL));
        fixv[mp] = (j << 15) + i;
        emit(sym_id(j), VLU);
        emit(TRA, OPR);
        emit(sym_id(i), DEF);
        return;
    case  39: // <PROCEDURE HEAD> ::= <PROCEDURE NAME> INTERRUPT <NUMBER> ';'
        /*     get symbol number */
        i = fixv[mp];
        i = sym_id(i);
        /*     get interrupt number */
        j = fixv[sp - 1];
        if (j > 7)
            error(39, 1);
        else {
            k = intpro[++j];
            /*     is interrupt duplicated */
            if (k <= 0)
                intpro[j] = i;
            else
                error(40, 1);
        }
        proctp[curblk] = 1;
        i = fixv[mp];
        symbol[i + 1] = (0 << 8) + (0 << 4) + PROC;
        j = enter(-(64 + LABEL));
        fixv[mp] = (j << 15) + i;
        emit(sym_id(j), VLU);
        emit(TRA, OPR);
        emit(sym_id(i), DEF);
        return;
    case  40: // <PROCEDURE NAME> ::= <LABEL DEFINITION> PROCEDURE
    /* check for numeric label */
        if (fixc[mp] >= 0) {
            fixv[mp] = fixc[mp];
            error(48, 1);
        }
        enterb();
        emit(ENP, OPR);
        return;
    case  41: // <PARAMETER LIST> ::= <PARAMETER HEAD> <IDENTIFIER> ')'
    case  43: // <PARAMETER HEAD> ::= <PARAMETER HEAD> <IDENTIFIER> ','
        i = lookup(sp - 1);
        if (i >= blksym)
            error(14, 1);
        /*i = */ enter(VARB);
        fixv[mp]++;
        return;
    case  42: // <PARAMETER HEAD> ::= '(' Left context check(<PROCEDURE NAME>)
        fixv[mp] = 0;
        return;
    case  44: // <ENDING> ::= END
        exitb();
        fixv[mp] = 0;
        return;
    case  45: // <ENDING> ::= END <IDENTIFIER>
        exitb();
        i = lookup(sp);
        if (i == 0)
            error(15, 1);
        fixv[mp] = i;
        return;
    case  46: // <ENDING> ::= <LABEL DEFINITION> <ENDING>
        fixv[mp] = fixv[sp];
        return;
    case  47: // <LABEL DEFINITION> ::= <IDENTIFIER> ':'
        i = lookup(mp);
        if (curblk == 2)
            ip = 48;
        if (curblk != 2)
            ip = 64;
        if (i < blksym)

            /*         prec = 3 if user-defined outer block LABEL */
            /*         prec = 4 if user-defined LABEL not in outer block */
            /*         prec = 5 if compiler-generated LABEL */
            i = enter(ip + LABEL);
        else {
            j = symbol[i + 1];
            j = right((j >> 4), 4);
            k = i + 1;
            if (j != 0) {
                error(16, 1);
                symbol[k] = symbol[k] - j * 16;
            }
            symbol[k] = symbol[k] + ip;
        }
        fixv[mp] = i;
        /* indicate that this is an identifier label */
        fixc[mp] = -1;
        if (token != PROCV) {
            emit(sym_id(i), DEF);
        }
        return;
    case  48: // <LABEL DEFINITION> ::= <NUMBER> ':'
        k = fixv[mp];
        if (k > 65535) {
            error(17, 1);
        } else {
            if ((l = lookup(mp)) == 0) {
                /*     enter number */
                j = (k > 255) ? 2 : 1;
                l = enter((k << 8) + (j << 4) + LITER + 1);
            }
            /* indicate that this is a numeric label */
            fixc[mp] = l;
            emit(sym_id(l), VLU);
            emit(ORG, OPR);
        }
        return;
    case  49: // <RETURN STATEMENT> ::= RETURN
        emit(0, LIT);
        if (proctp[curblk] == 2)
            error(45, 1);
        else if (proctp[curblk] == 0)
            error(46, 1);
        emit(RET, OPR);
        return;
    case  50: // <RETURN STATEMENT> ::= RETURN <EXPRESSION>
        if (proctp[curblk] == 1)
            error(44, 1);
        else if (proctp[curblk] == 0)
            error(46, 1);
        emit(RET, OPR);
        return;
    case  51: // <CALL STATEMENT> ::= CALL <VARIABLE>
        if ((i = fixv[sp]) == 0)
            return;
        if (i > 0) {
            j = right(symbol[i + 1], 4);
            emit(sym_id(i), ADR);
            if (j == PROC) {
                emit(PRO, OPR);
                return;
            } else if (j == INTR) {
                emit(BIF, OPR);
                return;
            }
        }
        error(18, 1);
        return;
    case  52: // <GO TO STATEMENT> ::= <GO TO> <IDENTIFIER>
        if ((i = lookup(sp)) == 0)
            i = enter(LABEL);
        j = right(symbol[i + 1], 4);
        if (j != LABEL && j != VARB) {
            error(19, 1);
        } else {
            /*     increment the reference counter (use length field) */
            if (j == LABEL)
                symbol[i + 1] = symbol[i + 1] + 256;
            emit(sym_id(i), VLU);
            emit(TRA, OPR);
        }
        return;
    case  53: // <GO TO STATEMENT> ::= <GO TO> <NUMBER>
        j = sp;
        k = fixv[j];
        if (k > 65535) {
            error(17, 1);
        } else {
            if ((l = lookup(j)) == 0) {
                /*     enter number */
                j = (k > 255) ? 2 : 1;
                l = enter((k << 8) + (j << 4) + LITER + 1);
            }
            emit(sym_id(l), VLU);
            emit(TRA, OPR);
        }
        return;
    case  54: // <GO TO> ::= GO TO
    case  55: // <GO TO> ::= GOTO
    case  56: // <DECLARATION STATEMENT> ::= DECLARE <DECLARATION ELEMENT>
    case  57: // <DECLARATION STATEMENT> ::= <DECLARATION STATEMENT> ',' <DECLARATION ELEMENT>
    case  58: // <DECLARATION ELEMENT> ::= <TYPE DECLARATION>
        return;
    case  59: // <DECLARATION ELEMENT> ::= <IDENTIFIER> LITERALLY <STRING>
        l = mp;
        k = mactop;
        for (m = 1; m <= 2; m++) {
            ip = var[l].len;
            i = var[l].loc - 1;

            k = k + 1;
            if (k >= curmac) {
                error(20, 5);
                return;
            }
            macros[k] = ip;
            for (j = 1; j <= ip; j++) {
                if (++k >= curmac) {
                    error(20, 5);
                    return;
                }
                macros[k] = varc[i + j];
            }
            l = sp;
        }

        if (++k < curmac) {
            macros[k] = mactop;
            mactop = k;
        } else
            error(20, 5);
        return;
    case  60: // <DECLARATION ELEMENT> ::= <IDENTIFIER> <DATA LIST>
        i = fixv[mp] + 1;
        j = fixv[mp + 1];
        l = right(j, 16);
        symbol[i] = (l << 8) + symbol[i];
        emit(DAT, OPR);
        emit((j >> 16), DEF);
        return;
    case  61: // <DATA LIST> ::= <DATA HEAD> <CONSTANT> ')'
    case  63: // <DATA HEAD> ::= <DATA HEAD> <CONSTANT> ','
        fixv[mp] += wrdata(-fixv[mp + 1]);
        return;
    case  62: // <DATA HEAD> ::= DATA '('
        j = enter(-(64 + LABEL));
        j = sym_id(j);
        emit(j, VLU);
        emit(TRA, OPR);
        fixv[mp] = (j << 16);
        i = lookup(mp - 1);
        if (i > blksym)
            error(22, 1);
        /*     set precision of inline data to 3 */
        i = enter(48 + VARB);
        fixv[mp - 1] = i;
        emit(DAT, OPR);
        emit(sym_id(i), DEF);
        return;
    case  64: // <TYPE DECLARATION> ::= <IDENTIFIER SPECIFICATION> <TYPE>
        n = 1;
        goto L200;
    case  65: // <TYPE DECLARATION> ::= <BOUND HEAD> <NUMBER> ')' <TYPE>
        n = fixv[mp + 1];

    L200:    i = fixv[mp];
        j = i >> 15;
        i = right(i, 15);
        k = fixv[sp];
        for (l = j; l <= i; l++) {
            m = symbol[l] + 1;
            ip = symbol[m];
            if (k == 0) {
                if (ip != 1)
                    error(21, 1);
                ip = LABEL;
            }
            symbol[m] = (n << 8) + (k << 4) + right(iabs(ip), 4);
            if (ip < 0)
                symbol[m] = -symbol[m];
        }
        maxsym = i;
        fixv[mp] = symbol[i];
        return;
    case  66: // <TYPE DECLARATION> ::= <TYPE DECLARATION> <INITIAL LIST>
        return;
    case  67: // <TYPE> ::= BYTE
        fixv[mp] = 1;
        return;
    case  68: // <TYPE> ::= ADDRESS
        fixv[mp] = 2;
        return;
    case  69: // <TYPE> ::= LABEL
        fixv[mp] = 0;
        return;
    case  70: // <BOUND HEAD> ::= <IDENTIFIER SPECIFICATION> '('
        return;
    case  71: // <IDENTIFIER SPECIFICATION> ::= <VARIABLE NAME>
        symbol[maxsym] = fixv[mp];
        fixv[mp] = (maxsym << 15) + maxsym;
        return;
    case  72: // <IDENTIFIER SPECIFICATION> ::= <IDENTIFIER LIST> <VARIABLE NAME> ')'
    case  74: // <IDENTIFIER LIST> ::= <IDENTIFIER LIST> <VARIABLE NAME> ','
        if (symtop >= maxsym) {
            error(23, 5);
            maxsym = SYMABS;
        }
        symbol[maxsym] = fixv[mp + 1];
        fixv[mp] = (maxsym << 15) + right(fixv[mp], 15);
        maxsym--;
        return;
    case  73: // <IDENTIFIER LIST> ::= '('
        fixv[mp] = maxsym;
        return;
    case  75: // <VARIABLE NAME> ::= <IDENTIFIER> Left context check(',' | DECLARE | <IDENTIFIER LIST>)
    case  77: // <BASED VARIABLE> ::= <IDENTIFIER> BASED
        i = lookup(mp);
        if (i <= blksym)
            i = enter(VARB);
        else {
            if (right(symbol[i + 1], 8) != VARB)
                error(24, 1);
        }
        fixv[mp] = i;
        return;
    case  76: // <VARIABLE NAME> ::= <BASED VARIABLE> <IDENTIFIER>
        i = fixv[mp];
        j = symtop;
        symtop = symtop + 1;
        if (symtop > maxsym) {
            symtop = symtop - 1;
            error(25, 5);
        } else {
            symbol[symtop] = symbol[j];
            k = lookup(sp);
            if (k == 0)
                k = enter(VARB);
            else {
                if (right(symbol[k + 1], 4) != VARB) {
                    error(26, 1);
                    return;
                }
            }
            symbol[j] = sym_id(k);    // stuff the symId of the base var
            i++;
            symbol[i] = -symbol[i];    // mark the info as based
        }
        return;
    case  78: // <INITIAL LIST> ::= <INITIAL HEAD> <CONSTANT> ')'
        goto case80;
    case  79: // <INITIAL HEAD> ::= INITIAL '('
        i = fixv[mp - 1];
        fixv[mp] = maxsym;
        j = maxsym--;
        if (maxsym <= symtop) {
            error(23, 5);
            maxsym = SYMABS;
            symbol[maxsym] = fixv[mp + 1];
            fixv[mp] = (maxsym << 15) + right(fixv[mp], 15);
            maxsym--;
        } else {
            i = sym_id(i);
            symbol[j] = (i << 15);
        }
        return;
    case  80: // <INITIAL HEAD> ::= <INITIAL HEAD> <CONSTANT> ','
    case80:
        i = fixv[mp];
        if (maxsym <= symtop) {
            error(23, 5);
            maxsym = SYMABS;
            symbol[maxsym] = fixv[mp + 1];
            fixv[mp] = (maxsym << 15) + right(fixv[mp], 15);
        } else {
            symbol[i]++;
            i = fixv[mp + 1];
            i = (sym_id(i) << 16) + i;
            symbol[maxsym] = i;
        }
        maxsym--;
        return;
    case  81: // <ASSIGNMENT> ::= <VARIABLE> <REPLACE> <EXPRESSION>
    case  82: // <ASSIGNMENT> ::= <LEFT PART> <ASSIGNMENT>
        i = maxsym - ++acnt;
        if (i <= symtop) {
            error(27, 5);
            acnt = 0;
        } else {
            symbol[i] = fixv[mp];
            /*      check for procedure on lhs of assignment. */
            /*     ****note that this is dependent on symbol number of output=17**** */
            if (fixv[mp] == 0 && fixc[mp] != 17)
                error(41, 1);
        }
        return;
    case  83: // <REPLACE> ::= '='
    case  84: // <LEFT PART> ::= <VARIABLE> ','
    case  85: // <EXPRESSION> ::= <LOGICAL EXPRESSION>
        return;
    case  86: // <EXPRESSION> ::= <VARIABLE> ':' '=' <LOGICAL EXPRESSION>
        j = fixv[mp];
        if (fixv[mp] == 0)
            error(41, 1);
        if (j < 0)
            emit(XCH, OPR);
        else {
            j = symbol[j - 1];
            emit(j >> 16, ADR);
        }
        emit(STO, OPR);
        return;
    case  87: // <LOGICAL EXPRESSION> ::= <LOGICAL FACTOR>
        return;
    case  88: // <LOGICAL EXPRESSION> ::= <LOGICAL EXPRESSION> OR <LOGICAL FACTOR>
        emit(IOR, OPR);
        return;
    case  89: // <LOGICAL EXPRESSION> ::= <LOGICAL EXPRESSION> XOR <LOGICAL FACTOR>
        emit(XOR, OPR);
        return;
    case  90: // <LOGICAL FACTOR> ::= <LOGICAL SECONDARY>
        return;
    case  91: // <LOGICAL FACTOR> ::= <LOGICAL FACTOR> AND <LOGICAL SECONDARY>
        emit(AND, OPR);
        return;
    case  92: // <LOGICAL SECONDARY> ::= <LOGICAL PRIMARY>
        return;
    case  93: // <LOGICAL SECONDARY> ::= NOT <LOGICAL PRIMARY>
        emit(NOT, OPR);
        return;
    case  94: // <LOGICAL PRIMARY> ::= <ARITHMETIC EXPRESSION>
        return;
    case  95: // <LOGICAL PRIMARY> ::= <ARITHMETIC EXPRESSION> <RELATION> <ARITHMETIC EXPRESSION>
        emit(fixv[mp + 1], OPR);
        return;
    case  96: // <RELATION> ::= '=' Left context check(<ARITHMETIC EXPRESSION>)
    case  97: // <RELATION> ::= '<'
    case  98: // <RELATION> ::= '>'
    case  99: // <RELATION> ::= '<' '>'
    case 100: // <RELATION> ::= '<' '='
    case 101: // <RELATION> ::= '>' '='

        /*     * note that the code that follows depends upon fixed production # */
        fixv[mp] = (prod - 96) + EQL;
        return;
    case 102: // <ARITHMETIC EXPRESSION> ::= <TERM>
        return;
    case 103: // <ARITHMETIC EXPRESSION> ::= <ARITHMETIC EXPRESSION> '+' <TERM>
    case 104: // <ARITHMETIC EXPRESSION> ::= <ARITHMETIC EXPRESSION> '-' <TERM>
    case 105: // <ARITHMETIC EXPRESSION> ::= <ARITHMETIC EXPRESSION> PLUS <TERM>
    case 106: // <ARITHMETIC EXPRESSION> ::= <ARITHMETIC EXPRESSION> MINUS <TERM>
        /*     * note that the following code dpends upon fixed prod numbers */
        i = (prod - 103) + ADD;
        /*     *** the values of adc and sub were accidentily reversed *** */
        if (i == ADC || i == SUB)
            i = 5 - i;
        emit(i, OPR);
        return;
    case 107: // <ARITHMETIC EXPRESSION> ::= '-' <TERM>
        emit(0, LIT);
        emit(XCH, OPR);
        emit(SUB, OPR);
        return;
    case 108: // <TERM> ::= <PRIMARY>
        return;
    case 109: // <TERM> ::= <TERM> '*' <PRIMARY>
    case 110: // <TERM> ::= <TERM> '/' <PRIMARY>
    case 111: // <TERM> ::= <TERM> MOD <PRIMARY>

        /*     <term> ::= <primary> */
        /*     * note that the following code depends upon fixed prod numbers */
        i = (prod - 109) + MUL;
        emit(i, OPR);
        return;
    case 112: // <PRIMARY> ::= <CONSTANT>
        i = fixv[mp];
        emit(sym_id(i), VLU);
        return;
    case 113: // <PRIMARY> ::= '.' <CONSTANT>
        i = enter(-(64 + LABEL));
        i = sym_id(i);
        fixv[mp] = i;
        emit(i, VLU);
        emit(TRA, OPR);
        emit(DAT, OPR);
        emit(0, DEF);
        /*     drop through to next production */
    case 114: // <PRIMARY> ::= <CONSTANT HEAD> <CONSTANT> ')'
        wrdata(-fixv[mp + 1]);
        emit(DAT, OPR);
        emit(fixv[mp], DEF);
        return;
    case 115: // <PRIMARY> ::= <VARIABLE>
        i = fixv[mp];
        if (i > 0) {
            /*     simple variable */
            emit(sym_id(i), VLU);
            j = sym_type(i);
            if (j == PROC)
                emit(PRO, OPR);
            else if (j == INTR)
                emit(BIF, OPR);
        } else if (i != 0)
            /*     subscripted variable */
            emit(LOD, OPR);
        return;
    case 116: // <PRIMARY> ::= '.' <VARIABLE>
        i = fixv[sp];
        if (i > 0) {
            if (sym_type(i) == VARB) {
                emit(sym_id(i), ADR);
                emit(CVA, OPR);    /*     subscripted - change precision to 2 */
                return;
            }
        } else if (i != 0) {    /*     subscripted - change precision to 2 */
            emit(CVA, OPR);
            return;
        }
        error(28, 1);
        return;
    case 117: // <PRIMARY> ::= '(' <EXPRESSION> ')'
        return;
    case 118: // <CONSTANT HEAD> ::= '.' '('
        i = enter(-(64 + LABEL));
        i = sym_id(i);
        fixv[mp] = i;
        emit(i, VLU);
        emit(TRA, OPR);
        emit(DAT, OPR);
        emit(0, DEF);
        return;
    case 119: // <CONSTANT HEAD> ::= <CONSTANT HEAD> <CONSTANT> ','
        wrdata(-fixv[mp + 1]);
        return;
    case 120: // <VARIABLE> ::= <IDENTIFIER>
        if ((i = lookup(mp)) == 0) {
            error(29, 1);
            i = enter(VARB);
        }
        fixv[mp] = i;
        j = sym_type(i);
        if (j == LABEL)
            error(47, 1);
        if (j != PROC && j != INTR)
            return;
        if (sym_infoLen(i) != 0)
            error(38, 1);
        j = sym_prec(i);
        if (pstack[mp - 1] == CALLV && j != 0)
            error(42, 1);
        if (pstack[mp - 1] != CALLV && j == 0)
            error(43, 1);
        i = sym_id(i);
        i = ((i << 15) + i + 1);
        fixc[mp] = 0;
        emit(i >> 15, VLU);
        fixc[mp] = i >> 15;
        fixv[mp] = 0;
        emit(PRO, OPR);
        return;
    case 121: // <VARIABLE> ::= <SUBSCRIPT HEAD> <EXPRESSION> ')'
        i = fixv[mp];
        if (i >= 0) {
            fixv[mp] = -i;
            emit(INX, OPR);
            return;
        } else {
            i = -i;
            emit(right(i, 15), ADR);
            if (fixc[mp] != 1)
                emit(STD, OPR);
            if (iabs(fixc[mp]) == 0)
                error(37, 1);
            if (iabs(fixc[mp]) > 1)
                error(38, 1);
        }
        emit(i >> 15, VLU);
        fixc[mp] = i >> 15;
        fixv[mp] = 0;
        emit(PRO, OPR);
        return;
    case 122: // <SUBSCRIPT HEAD> ::= <IDENTIFIER> '('
        if ((i = lookup(mp)) == 0) {
            error(30, 1);
            i = enter(VARB);
        }
        j = right(iabs(symbol[i + 1]), 4);
        if (j != VARB)
            if (j != PROC && j != INTR)
                error(31, 1);
            else {
                fixc[mp] = sym_infoLen(i);
                if (j == INTR)
                    fixc[mp] = -fixc[mp];
                j = sym_prec(i);
                /*     in the statements below, 30 is the token for 'call' */
                if (pstack[mp - 1] == 30 && j != 0)
                    error(42, 1);
                if (pstack[mp - 1] != 30 && j == 0)
                    error(43, 1);
                i = sym_id(i);
                fixv[mp] = -((i << 15) + i + 1);
                return;
            }
            fixv[mp] = i;
            emit(sym_id(i), ADR);
            return;
    case 123: // <SUBSCRIPT HEAD> ::= <SUBSCRIPT HEAD> <EXPRESSION> ','
        i = -fixv[mp];
        if (i <= 0)
            error(32, 1);
        else {
            fixv[mp] = -(i + 1);
            j = right(i, 15);
            emit(j, ADR);
            if (fixc[mp] == 0)
                error(37, 1);
            else {
                if (fixc[mp] != 2)
                    emit(STD, OPR);
                fixc[mp] += (fixc[mp] < 0) ? 1 : -1;
            }
        }
        return;
    case 124: // <CONSTANT> ::= <STRING>
        /*     may wish to treat this string as a constant later */
        i = var[sp].len;

        l = 3;
        k = 0;
        if (i > 0 && i <= 2) {
            /*         convert internal character form to ascii */
            j = var[sp].loc;
            k = 0;
            for (l = 1; l <= i; l++) {
                ltemp = j + l - 1;
                kp = varc[ltemp];
                k = k * 256 + ascii[kp - 1];
            }
            l = i;
        }
        i = lookup(sp);
        if (i == 0)
            i = enter((k << 8) + (l << 4) + LITER);
        fixv[mp] = i;
        return;
    case 125: // <CONSTANT> ::= <NUMBER>
        i = lookup(sp);
        if (i == 0) {
            /*     enter number into symbol table */
            i = fixv[mp];
            j = 1;
            if (i > 255)
                j = 2;
            i = enter((i << 8) + (j << 4) + LITER + 1);
        }
        fixv[mp] = i;
        return;
    case 126: // <TO> ::= TO
        i = fixv[mp - 3];
        if (i <= 0) {
            error(33, 1);
            fixv[mp] = 1;
        } else {
            i = sym_id(i);
            fixv[mp - 3] = i;
            emit(i, ADR);
            emit(STD, OPR);
            j = enter(-(64 + LABEL));
            j = sym_id(j);
            emit(j, DEF);
            fixv[mp] = j;
            emit(i, VLU);
        }
        return;
    case 127: // <BY> ::= BY
        emit(LEQ, OPR);
        i = enter(-(64 + LABEL));
        /*     save symbol number at <to> (end loop number) */
        i = sym_id(i);
        j = fixv[mp - 2];
        fixv[mp - 2] = i;
        emit(i, VLU);
        emit(TRC, OPR);
        i = enter(-(64 + LABEL));
        i = sym_id(i);
        fixv[mp] = (j << 14) + i;
        /*     <by> is (to number/statement number) */
        emit(i, VLU);
        emit(TRA, OPR);
        /*     now define by LABEL */
        i = enter(-(64 + LABEL));
        i = sym_id(i);
        /*     save by LABEL in <to> as branch back number */
        fixv[mp - 2] = (i << 14) + fixv[mp - 2];
        emit(i, DEF);
        return;
    case 128: // <WHILE> ::= WHILE
        i = enter(-(64 + LABEL));
        i = sym_id(i);
        emit(i, DEF);
        fixv[mp] = i;
        return;
    }
    assert(0);
}

#define INMAX    120
static char ibuff[INMAX + 1];

int gnc( /*const int q */)
{


    int _gnc, lp, i, j;

    /*     get next character from the input stream (or 0 if */
    /*     no character is found) */

    while (curmac <= MAXMAC) {
        i = macros[curmac];
        j = i >> MACBITS;
        i = right(i, MACBITS);
        if (j < i) {
            _gnc = macros[++j];        /* pick up the char */
            macros[curmac] = (j << MACBITS) + i;    /* merge back the current and end indexes */
            return _gnc;
        }
        curmac++;
        j = macros[i + 1];
        macros[j + 1] = -macros[j + 1];
    }
    if (ibp > C_RIGHTMARG) {
        /*     read another record from command stream */
        if (C_TERMINAL != 0) {
            if (C_INPUT == 1)
                form("\n ");
            writel(0);
        }
        for (;;) {
            int c;
            FILE *fp = getfile(C_INPUT, 0);

            memset(ibuff, ' ', INMAX);
            ibuff[INMAX] = 0;

            for (i = 0; i < INMAX; i++) {
                c = getc(fp);
                if (c == '\n' || c == EOF)
                    break;
                c = toupper(c);
                if (strchr(otran, c) == NULL)        // check a valid character
                    c = ' ';
                ibuff[i] = c;
            }
            if (i == 0 && c == EOF) {
                writel(0);
                if (inptr >= 1) {
                    C_INPUT = instk[--inptr];
                    continue;
                }
                C_EOF = 1;
                return 0;
            }

            while (c != '\n' && c != EOF)        // gobble up rest of line
                c = getc(fp);


            lp = C_LEFTMARG - 1;
            if (ibuff[lp] != '$' || ibuff[1] == ' ')    // suspect ibuff[1] should be ibuff[lp + 1] 
                break;
            parseOptions(ibuff + lp + 1);
        }
        ibp = lp;
        emit(++C_COUNT, LIN);
        if (C_PRINT != 0) {
            form("\n%05d%3d   %s", C_COUNT, curblk - 1, ibuff);
        }
    }
    return itran[ibuff[ibp++]];    // map to internal char set
}

void parseOptions(char *s)
{
    int j, k;

    ibuff[C_RIGHTMARG] = 0;        // truncate line at right  margin
    while (*s) {            // process all of line
        if (*s == '$') {        // display $ paramters
            if (*++s == ' ') {    // display all parameters
                s++;
                for (j = 2; j <= 64; j++)
                    if (contrl[j] >= 0)
                        form("\n$%c=%d", otran[j - 1], contrl[j]);
            } else if (*s) {    // make sure not off end of line
                j = itran[*s++];
                if (contrl[j] >= 0)
                    form("\n$%c=%d", otran[j - 1], contrl[j]);
            }
            if (C_TERMINAL != 0)
                form("\n ");
            writel(0);
        } else if ((j = *s)) {
            j = itran[j];        // map to local char set
            while (*++s && *s != '=' && *s != '$')
                ;
            if (*s == '=') {
                k = 0;
                while (*++s == ' ' || isdigit(*s)) {
                    if (*s != ' ')
                        k = k * 10 + *s - '0';
                }
                if (j == CHI)
                    stackc(k);
                else
                    contrl[j] = k;
            } else {
                k = contrl[j];
                if (k < 0 || k > 1)
                    error(34, 1);
                else
                    contrl[j] = 1 - k;
            }
        }
        while (*s && *s++ != '$')
            ;

    }

}

void writel(int nspac)
{
    int i, np;

    np = C_YPAD - 1;
    if (obp > np) {

        while (obp > 1 && obuff[obp] == ' ')    // trim off trailling spaces
            obp--;

        obp = imin(C_DELETE, obp);

        FILE *fp = getfile(C_OUTPUT, 1);

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

void decibp()
{
    if (curmac > MAXMAC)
        ibp--;
    else
        macros[curmac] -= (1 << MACBITS);    /* back up index */
}

void conv(const int prec)
{
    int i;
    if (stype > 1) {
        value = 0;
        for (i = 1; i <= acclen; i++) {
            value = value * stype + accum[i] - 2;
        }
        if (prec <= 0 || value < (1 << prec))
            return;
    }
    value = -1;
    return;
}

int imin(const int i, const int j)
{
    return i < j ? i : j;
}

void form(char *fmt, ...)
{
    va_list marker;
    char buf[200];
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

void stackc(const int i)
{
    if (++inptr > 7)
        error(35, 5);
    else {
        instk[inptr] = C_INPUT;
        C_INPUT = i;
    }
}

void putch(const int chr)
{
    if (chr != '\n')
        obuff[++obp] = chr;
    if (chr == '\n' || obp >= C_WIDTH)
        writel(0);
}

void enterb()
{
    /*     entry to block goes through here */
    curblk++;
    proctp[curblk] = proctp[curblk - 1];
    if (curblk > MAXBLK) {
        error(36, 5);
        curblk = 1;
    }
    block[curblk] = symtop;
    dopar[curblk] = 0;
    /*     save the state of the macro definition table */
    macblk[curblk] = mactop;
    //    macblk[curblk] = (mactop << MACBITS) + curmac;

    blksym = symtop;
    return;
}

void dumpin()
{
    int jp, i, kq, j, k, kt;
    /*     dump the initialization table */
    /*     wrdata(x) writes the data at location x in symbol table */
    /*     and returns the number of bytes written */
    if (C_SYMBOLS == 2) {
        for (i = SYMABS; i > maxsym; i--) {
            form("\n \n");
            form("\nSYMBOL S%05d =", (symbol[i] >> 15));
            for (jp = right(symbol[i], 15); jp > 0; jp--) {
                /*         get the symbol number */
                form(" S%05d", (symbol[--i] >> 16));
            }
        }
    }
    putch('\n');
    kt = C_OUTPUT;
    C_OUTPUT = C_USYMBOL;
    kq = C_WIDTH;
    C_WIDTH = C_VWIDTH;
    /*     ready to write the initialization table */
    putch('/');
    for (i = SYMABS; i > maxsym; i--) {
        j = (symbol[i] >> 15);
        /*     write symbol numbers */
        for (k = 0; k < 3; k++) {
            putch(otran[j % 32 + 1]);
            j = j / 32;
        }
        jp = right(symbol[i], 15);
        while (jp-- > 0) {
            wrdata(right(symbol[--i], 16));
        }
        putch('/');

    }
    putch('/');
    putch('\n');
    C_OUTPUT = kt;
    C_WIDTH = kq;
    return;
}

void error(const int i, const int level)
{
    /*     i is error number, level is severity code */
    errorCnt++;
    form("\n(%05d)  ERROR %d  NEAR ", C_COUNT, i);
    for (int j = 1; j <= acclen; j++)
        putch(otran[accum[j] - 1]);
    putch('\n');
    /*     check for terminal error - level greater than 4 */
    if (level > 4) {
        /*         terminate compilation */
        form("\nCOMPILATION TERMINATED\n");
        compil = false;
    }
}

/*
int shr(const int i, const int j)
{
    return i / (1 << j);
}
*/
int right(const int i, const int j)
{
    return i % (1 << j);
}

void sdump()
{
    int i;
    /*     check for stack dump bypass */
    if (C_BYPASS == 0) {
        form("\nPARSE STACK: ");
        for (i = 5; i <= sp; i++)            // fortran guard not needed
            form("%s ", tokens[pstack[i]]);
        putch('\n');
    }
}

void redpr(const int prod, const int sym)
{
    int i;
    form("\n%5d  %s ::=", prod, tokens[sym]);
    for (i = mp; i <= sp; i++)
        form(" %s", tokens[pstack[i]]);
    putch('\n');
    return;
}

void emit(const int val, const int typ)
{
    int lcode, kp, i, j, k;

#define MAXPOL 30
    static int polish[MAXPOL + 1];
    static int polcnt = 0;
    /*     opradrvaldeflitlin*/
    static char *polchr[] = { "OPR", "ADR", "VAL", "DEF", "LIT", "LIN" };
    static char *opcval[] = {
        "NOP", "ADD", "ADC", "SUB", "SBC", "MUL", "DIV", "REM",
        "NEG", "AND", "IOR", "XOR", "NOT", "EQL", "LSS", "GTR",
        "NEQ", "LEQ", "GEQ", "INX", "TRA", "TRC", "PRO", "RET",
        "STO", "STD", "XCH", "DEL", "DAT", "LOD", "BIF", "INC",
        "CSE", "END", "ENB", "ENP", "HAL", "RTL", "RTR", "SFL",
        "SFR", "HIV", "LOV", "CVA", "ORG", "DRT", "ENA", "DIS",
        "AX1", "AX2", "AX3"
    };

    /*     typ      meaning */
    /*      0      operator */
    /*      1      load address */
    /*      2      load value */
    /*      3      define location */
    /*      4      literal value */
    /*      5      line number */
    /*      6      unused */
    /*      7        " */
    assert(val >= 0);

    if (++poltop > MAXPOL) {
        error(37, 1);
        poltop = 1;
    }
    polcnt++;
    if (C_GENERATE != 0) {
        form("\n%5d %s ", polcnt, polchr[typ]);
        switch (typ) {
        case OPR:
            form(opcval[val]);
            break;
        case ADR:
        case VLU:
        case DEF:
            form("S%05d", val);
            break;
        case LIT:
        case LIN:
            form(" %05d", val);
        }
        /*     now store the polish element in the polish array. */

        writel(0);
    }
    polish[poltop] = (val << 3) + typ;
    lcode = C_KWIDTH / 3;
    if (poltop >= lcode) {
        /*     write the current buffer */
        putch('\n');
        kp = C_WIDTH;
        C_WIDTH = C_KWIDTH;
        k = C_OUTPUT;
        C_OUTPUT = C_JFILE;

        putch('\n');
        for (i = 1; i <= lcode; i++) {
            j = polish[i];
            putch(otran[(j >> 10) % 32 + 1]);
            putch(otran[(j >> 5) % 32 + 1]);
            putch(otran[j % 32 + 1]);
        }

        writel(0);
        C_WIDTH = kp;
        C_OUTPUT = k;
        poltop = 0;
    }
    return;
}

void cmpuse() {
    printf("table usage in pass 1:\n");
    printf("symbol table - max=%-6d, top=%-6d, loc=%-6d\n", maxsym, symtop, symloc);
    printf("               len=%-6d, cnt=%-6d, abs=%-6d\n", symlen, symcnt, SYMABS);
    printf("macro table - max=%-6d, top=%-6d, cur=%-6d\n", MAXMAC, mactop, curmac);
}
