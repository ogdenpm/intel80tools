










�++========================================++
�||�||
�||�||
�||�I�S�I�S�-�I�I�||
�||�-------------�||
�||�||
�||�S�O�F�T�W�A�R�E�||
�||�---------------�||
�||�||
�||�T�O�O�L�B�O�X�||
�||�-------------�||
�||�||
�||�||
�++========================================++







�Order�Number:�121727-002


























�Copyright�1980,1983�Intel�Corporation
�Intel�Corporation,�3065�Bowers�Avenue
�Santa�Clara,�California�95051






�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�v


















�Intel�Corporation�has�carefully�reviewed�this�Vendor
�Supplied�Product�for�its�suitability�and�operational
�characteristics�when�used�with�Intel�products,�and�believes
�that�the�product�will�operate�and�perform�according�to�its
�published�user�manuals.�HOWEVER,�INTEL�MAKES�NO�WARRANTIES
�WITH�RESPECT�TO�THE�OPERATION�AND�USE�OF�THIS�PRODUCT.
�Successful�use�depends�solely�on�customer's�ability�to
�install�and�use�this�product.

�This�Product�is�licensed�on�an�"as�is"�basis,�and�Intel
�Corporation�does�not�guarantee�any�future�enhancements�or
�extensions�to�this�product.�The�existence�of�this�product
�does�not�imply�its�adaptation�in�any�form�as�an�Intel
�standard,�nor�its�compatibility�with�any�other�Intel
�product,�except�as�specifically�stated�in�the�published
�user�manuals.

�In�addition,�if�a�problem�is�encountered�which�the�user
�diagnosis�indicates�is�caused�by�a�defect�in�the�Product,
�the�user�is�requested�to�fill�out�a�Problem�Report�form,
�and�mail�it�to:


�Intel�Corporation
�MCSD,�Marketing
�3065�Bowers�Avenue
�Santa�Clara,�CA�95051



















�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�vii


�++==============================================++
�||�||
�||�ISIS-II�SOFTWARE�TOOLBOX�||
�||�VERSION�2.0�||
�||�PREFACE�||
�||�||
�++==============================================++



�This�manual�defines�and�describes�the�use�of�the
�ISIS-II�Software�Toolbox.�The�Toolbox�is�a�collection�of
�utilities�which�enhance�the�ISIS-II�operating�system�to
�improve�programmer�productivity.

�The�manual�is�organized�as�follows:


�*�QUICK�REFERENCE�SUMMARY

�The�following�section�contains�a�"quick�reference"
�summary�of�what�each�tool�does.


�*�EXPLANATION�OF�SOURCE�MANAGEMENT�SYSTEM

�An�explanation�of�the�philosophy�behind�the�Source
�Management�System,�and�a�description�of�how�the�Source
�Management�tools�work�together,�is�given�in�Source
�Management�Overview�chapter.�The�appropriate�tools�are
�described�immediately�after�the�overview.


�*�OVERVIEW�OF�CONDITIONAL�SUBMIT�CONTROL

�A�brief�description�of�conditional�Submit�execution,
�plus�an�example�using�most�of�the�tools,�is�contained�in
�the�Submit�File�Control�Overview.�The�individual�Submit
�tools�follow�the�overview.


�*�OTHER�TOOLS

�Those�tools�not�associated�with�Submit�Control�or
�Source�Management�are�listed�alphabeticallly,�following
�those�two�sections.



�IDENTIFYING�THE�TOOLS

�The�version�numbers�of�the�programs�constituting�the
�Software�Toolbox�can�be�ascertained�through�the�Software
�Toolbox�tool,�"Which".








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�ix


�++==============================================++
�||�||
�||�ISIS-II�SOFTWARE�TOOLBOX�||
�||�VERSION�2.0�||
�||�TABLE�OF�CONTENTS�||
�||�||
�++==============================================++


�QUICK�REFERENCE�SUMMARY�........................�1

�SOURCE�MANAGEMENT�OVERVIEW�.....................�3

�XLATE2�.....................................�4
�MRKOBJ�.....................................�7
�CHKLOD�.....................................�9
�CLEAN�.....................................�13
�LATEST�....................................�14

�SUBMIT�FILE�CONTROL�OVERVIEW�..................�16

�IF�........................................�18
�ELSE�......................................�22
�ENDIF�.....................................�22
�GOTO�......................................�23
�RETURN�....................................�24
�EXIT�......................................�24
�LOOP�......................................�25
�RESCAN�....................................�26
�WAIT�......................................�27
�NOTE�......................................�28

�OTHERS�TOOLS:

�COMPAR�....................................�29
�CONSOL�....................................�34
�DCOPY�.....................................�36
�DSORT�.....................................�41
�ERRS�......................................�43
�GANEF�.....................................�44
�GENPEX�....................................�50
�HDBACK�....................................�56
�LAST�......................................�60
�LOWER/UPPER�...............................�61
�MERG80�....................................�62
�MERG86�....................................�64
�PACK/UNPACK�...............................�66
�PASSIF�....................................�70
�RELAB�.....................................�76
�SORT�......................................�77
�STOPIF�....................................�79
�WHICH�.....................................�80










�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�1


�++==============================================++
�||�||
�||�ISIS-II�SOFTWARE�TOOLBOX�||
�||�VERSION�2.0�||
�||�QUICK�REFERENCE�SUMMARY�||
�||�||
�++==============================================++


�Related�programs�are�indicated�by�indentation.


�SOURCE�MANAGEMENT

�XLATE2�--�submit-like�tool�with�intelligent�parameter
�substitution�(for�version�control)
�MRKOBJ�--�"marks"�object�modules�with�source�version
�information
�CHKLOD�--�lists�source�version�data�put�in�load�modules
�by�Mrkobj
�CLEAN�--�deletes�all�old�versions�off�a�specified�disk
�LATEST�--�displays�latest�version�numbers�of�specified
�files


�SUBMIT�FILE�EXECUTION�CONTROL

�IF�--�conditional�submit�file�execution�based�on�file
�ELSE�existence,�program�errors,�pattern�matching,�plus
�ENDIF�several�other�conditions.
�GOTO�--�causes�execution�to�resume�at�a�given�"label"
�LOOP�--�causes�execution�to�resume�at�the�beginning�of
�the�submit�file�(like�a�"GOTO�start")
�NOTE�--�allows�"progress�report"�messsages�to�be�displayed
�to�the�cold�boot�console
�WAIT�--�displays�a�message,�and�waits�for�user�input�to
�continue�or�abort
�RETURN�--�causes�execution�of�the�last�line�of�a�Submit�file
�causing�immediate�return�to�the�"submitter."
�RESCAN�--�allows�submit�file�to�be�"started�up"�in�the�middle
�EXIT�--�halts�submit�file�execution
�STOPIF�--�halts�submit�file�execution�if�an�error�is�found�in
�a�specified�program�listing.


�OPERATING�SYSTEM�FUNCTIONS

�CONSOL�--�reassigns�console�input�and�console�output
�DSORT�--�alphabetically�sorts�floppy�and�5440�hard�disk
�directories
�RELAB�--�changes�disk�name�to�specified�name











�page�2�ISIS-II�SOFTWARE�TOOLBOX,�V2.0





�PROGRAM�DEVELOPMENT�AND�DEBUGGING

�ERRS�--�fast�display�of�program�errors�in�PLM80,�PLM86,
�and�ASM86�listings
�MERG80�--�merges�debug�data�from�locate�maps�into�PLM80
�listings
�MERG86�--�merges�debug�data�from�symbol�maps�into�PLM86
�or�Pasc86�listings
�GENPEX�--�produces�include�file�for�PLM�external
�declarations�(source�level)
�PASSIF�--�general�purpose�assertion�checking�and�reporting
�tool


�DISK�RECOVERY

�GANEF�--�interactively�reads�and�writes�disk�data�blocks


�DISK�BACKUP�AND�FILE�PROCESSING

�DCOPY�--�fast�track�by�track�diskette�copying
�HDBACK�--�sophisticated�hard�disk�to�floppy�disk�backup
�program
�PACK�--�compacts�text�files
�UNPACK�--�reconstitutes�"packed"�files


�TEXT�PROCESSING

�COMPAR�--�performs�line-oriented�text�file�comparison
�(shows�souce�code�changes)
�UPPER�--�changes�all�letters�in�a�file�to�uppercase
�LOWER�--�changes�all�uppercase�letters�to�lowercase
�LAST�--�displays�the�last�512�bytes�of�a�file
�SORT�--�sophisticated�line-oriented�text�file�sorting�tool


�PROGRAM�IDENTIFICATION

�WHICH�--�displays�version�number�of�Software�Toolbox
�Programs

















�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�3


�++==============================================++
�||�||
�||�ISIS-II�SOFTWARE�TOOLBOX�||
�||�VERSION�2.0�||
�||�SOURCE�MANAGEMENT�OVERVIEW�||
�||�||
�++==============================================++



�The�central�feature�of�the�Source�Management�System�is
�that�all�source�modules�have�a�wholly�numeric�filename
�extension�consisting�of�exactly�three�ascii�decimal�digits;
�e.g.,�"003",�"057",�"671",�etc.

�The�utilities�which�comprise�the�Source�Management
�System�interpret�these�numeric�extensions�as�being�version
�numbers.�A�numerically�greater�version�number�is�deemed�to
�indicate�a�more�recent�version�of�a�given�file.

�The�Source�Management�System�can�mark�object�modules
�with�the�version�number�of�the�source�file�from�which�it
�was�generated.�The�8080�and�8086�linkers�and�locators�pass
�this�information�all�the�way�through�to�the�load�module,
�allowing�the�Chklod�program�to�display�and�process�it.




�The�utilities�comprising�the�Source�Management�System�are:


�XLATE2�--�like�submit�except�that�it�can�pick�up�the
�"latest"�version�of�a�given�filename,�and
�substitute�it�into�the�Command�Sequence�File�it
�is�building

�MRKOBJ�--�"marks"�object�modules�with�the�name�of�the�source
�file�from�which�they�were�produced.�Mrkobj�works
�on�both�8080�and�8086�object�modules.

�CHKLOD�--�examines�load�modules;�finds,�processes,�and
�displays�the�source�version�information
�previously�placed�there�by�Mrkobj

�CLEAN�--�gets�rid�of�all�the�old�versions�of�all�the�files
�on�a�specified�disk

�LATEST�--�displays�latest�version�numbers�of�specified�files













�page�4�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�XLATE2�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY


�Xlate2�is�a�part�of�the�source-management�family�of
�tools.�It�is�much�like�the�Submit�cusp,�but�has
�"intelligent�parameter�substitution"�capability.

�It�can�determine�the�greatest�numeric�extension�of�a
�particular�file�on�a�particular�disk,�and�substitute�the
�filename�plus�extension�into�the�Command�Sequence�file.



�SAMPLE�INVOCATION

�XLATE2�compil.tpl�5�MAIN�PARSE�GEN�TOKENS



�SAMPLE�ACTIONS�PERFORMED

�Given�that�the�"template"�file�(compil.tpl)�looks�like
�the�one�below:


�TEMPLATE�FILE
�+---------------------------------------------------------+
�|�|
�|�:f1:plm86�:f?:#0�print(:f1:%0.lst)�debug�xref�|
�|�|
�+---------------------------------------------------------+


�And
�:f5:main�has�numeric�extensions�001�002�and�006
�:f5:parse�has�numeric�extension�000
�:f5:gen�has�numeric�extension�531
�:f5:tokens�has�numeric�extension�211


�Then�XLATE2�will�create�a�Command�Sequence�file�(shown
�below),�with�filename�:f1:xlate2.cs,�and�transfer�control
�to�it:











�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�5




�COMMAND�SEQUENCE�FILE
�+--------------------------------------------------------------+
�|�|
�|�:f1:plm86�:f5:main.006�print(:f1:main.lst�)�debug�xref�|
�|�:f1:plm86�:f5:parse.000�print(:f1:parse.lst�)�debug�xref�|
�|�:f1:plm86�:f5:gen.531�print(:f1:gen.lst�)�debug�xref�|
�|�:f1:plm86�:f5:tokens.211�print(:f1:tokens.lst)�debug�xref�|
�|�|
�|�xlate2�RESTORE�(:VI:)�(�0,�0)�|
�|�|
�+--------------------------------------------------------------+


�After�the�command�sequence�file�has�been�executed,
�control�will�return�to�the�caller,�via�the�the�final
�"xlate2�RESTORE"�command.

�Xlate2�has�fairly�different�parameter�substitution�than
�Submit�in�that�it�only�passes�file�names,�yet�has�the
�capability�to�retrieve�the�latest�version�of�a�particular
�file.�The�best�of�both�worlds�may�be�realized�by�including
�Xlate2�commands�within�Submit�files,�and�utilizing�the
�extended�parameter�passing�capability�of�Submit.



�FORMAL�SYNTACTIC�SPECIFICATION

�The�invocation�syntax�for�XLATE2�is�:

�XLATE2�[?]�[HALT!]�<filename>�<digit>�<filename_list>

�<filename>�is�the�name�of�a�template�file,�the�function�of
�which�is�described�later.

�<digit>�is�a�decimal�digit�from�0�to�9�,�giving�the
�device_number�from�which�numeric�filename
�extensions�should�be�taken.�Only�one�such�device
�is�allowed�per�invocation�of�xlate2.

�<filename_list>�is�a�list�of�ISIS-II�filenames�separated�by
�blanks,�(NOT�commas).�Additionally,�filenames�of
�the�form�namepart�dot�(no�extension,�but�a�dot)
�are�allowed�and�have�an�interpretation�given
�below.�Note�that�the�device�can�be�respecified�but
�this�will�be�ignored.�Continuation�is�allowable
�by�the�use�of�&�as�the�last�non-blank�character�on
�a�line.�**�,�the�standard�prompt,�will�be�issued
�after�continued�lines.

�?�causes�xlate2�to�print�its�version�number�and�exit.









�page�6�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�HALT!�causes�the�consol�file�to�be�created�but�not
�executed,�allowing�verification.�The�consol�file
�created�will�be�xlate2.cs�on�the�drive�from�which
�xlate2�was�invoked.


�The�template�file�uses�the�following�substitution:

�substitution�signs:�substitutions�are�of�the�form�%0�or�#0.

�the�substitution�matrix:�don't�get�confused�here,�all�you
�probably�want�is�%0.ext�and�#0�in�the�template�and
�a�<filename_list>�with�name�parts�only�(no�dot,
�no�extension)�on�the�command�line.


�foo�foo.�foo.abc
�+-------------------------------------------+
�|�|
�%0�|�foo�foo.�foo.abc�|
�|�|
�%0.�|�foo�foo�foo�|
�|�|
�%0.ext�|�foo.ext�foo.ext�foo.ext�|
�|�|
�#0�|�foo.#xt�foo�foo.#xt�|
�|�|
�+-------------------------------------------+


�foo.#xt�is�the�greatest�numeric�extension�of�foo�found�in
�ISIS.DIR�on�the�device_number.�If�no�numeric
�extension�of�foo�is�found�then�a�warning�will�be
�issued�and�foo.�will�appear�in�place�of�foo.#xt�in
�the�consol�file.

�In�the�above�example�it�is�assumed�that�ext�is�<=�3
�characters.�If�not�then�a�warning�will�be�given.

�Note:�In�all�the�template�substitutions�it�is�wise�to
�----�follow�them�with�a�blank.


�Note:�Additionally�:fn:<subtitution_parameter>�will
�----�preserve�the�n�in�the�template�substitution,�at
�least�for�0�<=�n�<=�9.�Furthermore�for�:f?:�,�the
�<device_number>�will�be�substituted�for�the�?.

�Note:�Warnings�will�be�printed�to�both�the�cold-boot
�----�console�and�to�the�current�console�if�different.












�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�7


�+==================================+
�|�|
�|�MRKOBJ�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�This�program�is�a�member�of�the�PL/M�Source�Management
�Family.�It�"marks"�object�modules�with�the�name�of�the
�source�file�from�which�it�was�produced.�Mrkobj�works�on
�both�8080�and�8086�object�modules.

�The�program�name�"mrkobj"�is�intended�to�be�a�mnemonic
�for�"mark�object�module".

�The�version�number�information�within�the�"marked"
�object�module�will�be�used�later,�by�the�Chklod�program,
�which�will�perform�source�program�version�determination�and
�checking.



�SAMPLE�PROGRAM�INVOCATION

�MRKOBJ�:f1:main.obj�WITH�:f1:main.003


�SAMPLE�ACTIONS�PERFORMED

�The�object�module�":f1:main.obj"�will�be�marked�with
�the�source�file�name�":f1:main.003".


�TYPICAL�USE�ENVIRONMENT

�Mrkobj�is�intended�to�be�used�in�conjunction�with�the
�xlate2�tool,�since�xlate2�can�automatically�determine�the
�latest�version�of�the�source�file,�avoiding�errors�that
�could�occur�with�manual�invocations�of�Mrkobj.

�A�sample�"xlate2-type"�submit�file�which�uses�Mrkobj
�is:

�+---------------------------------------------------+
�|�|
�|�PLM86�:f?:#0�print(:f1:%0.lst)�|
�|�|
�|�MRKOBJ�:f1:%0.obj�WITH�:f?:#0�|
�|�|
�+---------------------------------------------------+









�page�8�ISIS-II�SOFTWARE�TOOLBOX,�V2.0



�ACTIONS�PERFORMED

�The�object�modules�are�marked�by�the�insertion�of�extra
�records.


�RECORDS�USED

�For�the�8086,�a�comment�record�is�used.�For�the�8080,�a
�debug�record�is�used.�Both�types�of�records�are�purgeable.


�NOPURGE�OPTION

�The�keyword�"NOPURGE",�or�its�abbreviation�"NP",
�appearing�in�the�command�tail,�causes�Mrkobj�to�create�a
�non-purgeable�comment�record�as�the�marking�record.
�This�applies�only�to�8086�object�modules.

�ex.�MRKOBJ�:f1:baz.obj�WITH�:f1:baz.011�NOPURGE


�ERROR�CONDITIONS

�TARGET�OBJECT�MODULE�IS�LIBRARY

�If�the�object�module�is�a�library,�then�Mrkobj�will
�print:

�***�<object_module_name>�IS�AN�8080�LIBRARY�***
�or
�***�<object_module_name>�IS�AN�8086�LIBRARY�***

�to�the�console�output�device,�and�exit.

�TARGET�OBJECT�MODULE�IS�SEQUENCE

�If�the�object�module�specified�is�a�sequence,�then
�Mrkobj�will�print

�***�CANNOT�MARK�OBJECT�MODULE�SEQUENCE�***

�to�the�console�output�device,�and�exit.


�FORMAL�SYNTACTIC�SPECIFICATION

�Mrkobj�is�invoked�by:

�MRKOBJ�<object_filename>�WITH�<source_filename>
�[�<nopurge_option>�]

�<nopurge_option>�:==�(�NOPURGE�|�NP�)








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�9


�+==================================+
�|�|
�|�CHKLOD�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�Chklod�is�a�member�of�the�PL/M�Source�Management
�Family.�It�examines�8080�or�8086�loadable�object�modules
�for�source�file�version�number�information.�The�version
�numbers�were�previously�placed�in�the�object�modules�(by
�the�Mrkobj�Software�Toolbox�program)�before�being�linked.

�Using�the�version�information,�and�examining�a�program
�source�disk�directory�or�directories,�Chklod�ascertains
�whether�the�source�files�used�to�produce�the�load�module,
�are�the�most�"recent"�source�modules.

�Chklod�can�also�produce�a�"listing"�of�a�specified�load
�module.�The�listing�is�a�formatted,�human-readable
�compendium�of�the�source�version�information�found�in�the
�load�module.



�SAMPLE�INVOCATIONS�WITH�SAMPLE�ACTIONS�PERFORMED:


�CHKLOD�:f1:prog

�the�load�module�":f1:prog"�is�checked�against�the
�corresponding�source�files�in�drive�1�(which�is�the
�default�"drive�to�be�searched"�if�none�is
�specified).


�CHKLOD�:f1:prog�1�2�3�1

�:f1:prog�is�checked�against�the�corresponding
�source�files�found�on�drives�1,�2,�and�3.�Chklod
�pauses�and�prints�a�prompting�message�before
�searching�the�diskettes.�Notice�that�drive�1�will
�be�searched�twice.


�CHKLOD�:f1:prog�LISTING�TO�:f1:prog.arc�NOSORT

�In�case�three�Chklod�dumps�a�formatted�listing�of
�the�source�version�number�information�to�the�file
�":f1:prog.arc".�The�source�version�information�is
�listed�in�the�same�order�in�which�it�appeared�in
�the�load�module.







�page�10�ISIS-II�SOFTWARE�TOOLBOX,�V2.0



�CHKLOD�:f1:prog�LT

�In�case�four�Chklod�sorts�the�listing,�and�dumps�it
�to�the�default�output�device,�which�is�the�console
�output.


�CHKLOD�:f1:prog�LT�2�3�4

�In�case�five�Chklod�sorts�the�listing,�and�dumps�it
�to�the�default�output�device,�which�is�the�console
�output,�and�then�checks�:f1:prog�against�the
�corresponding�source�files�on�drives�2,�3,�and�4.


�ACTIONS�PERFORMED


�LOAD�MODULE�CHECKING�AND�INTERPRETATION


�NO�SOURCE�VERSION�INFORMATION�IN�LOAD�MODULE

�If�Chklod�cannot�find�any�source�version�information
�records,�then�Chklod�writes�"NO�MARKED�RECORDS�FOUND"�to
�console�output�and�exits.


�DUPLICATE�SOURCE�VERSION�INFORMATION�IN�A�SINGLE�LOAD�MODULE

�If�Chklod�finds�that�the�source�version�information,
�for�a�single�source�file,�has�been�specified�more�than�once
�in�a�single�load�module,�then�the�message:

�"WARNING:�respecification�of�source�version�information
�encountered"
�SOURCE�PROGRAM�NAME:�<name_part_of_source_program_1>"
�LOAD�MODULE�VERSION�LEVELS:
�<three_digit_ascii_numeric_extension_1>
�<three_digit_ascii_numeric_extension_2>
�.
�.
�.
�etc.

�will�be�output�to�the�console�output�device.�In�these
�cases,�Chklod�will�choose�the�source�file�with�the�highest
�numeric�extension�as�being�correct,�in�performing�its
�subsequent�checking.


�EXPLICIT�DEVICE�SPECIFICATION�CAUSES�A�PAUSE�AND�PROMPT

�When�the�disk�drives�to�be�searched�for�source�files







�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�11


�are�explicitly�specified�in�Chklod's�command�tail,�then,
�before�reading�the�specified�disk�directory,�Chklod�will
�halt�and�write:

�"insert�source�disk�in�drive�n�and�type�<cr>..."

�to�the�console�output,�and�then�wait�until�<CRLF>�is
�typed.


�SOURCE�VERSION�DISCREPANCIES

�For�source�filenames�where�the�greatest�numeric
�extension�gleaned�from�disk�is�different�than�the�numeric
�extension�found�in�the�load�module,�the�message(s):

�VERSION�DISCREPANCY�FOUND

�SOURCENAME:�<source_file_name_part_1>
�LOAD�MODULE�VERSION�LEVEL:�nnn
�SOURCE�MODULE�VERSION�LEVEL:�nnn

�SOURCENAME:�<source_file_name_part_2>
�LOAD�MODULE�VERSION�LEVEL:�nnn
�SOURCE�MODULE�VERSION�LEVEL:�nnn
�.
�.
�.
�etc.

�will�be�written�to�the�console�output.�(nnn�is�a�three-
�digit�numeric�extension.)

�If�there�is�more�than�one�numeric�extension�entry�in
�either�the�directory�or�the�load�module,�then�the�above
�error�message(s)�refer�to�the�largest�ones�found.


�EXPECTED�SOURCE�MODULE�NOT�FOUND

�If�there�is�no�file�in�the�source�disk(s)�which
�corresponds�with�a�numeric�extension�record�in�the�load
�module,�then�the�form�of�discrepancy�listing�is:

�SOURCENAME:�<source_file_name_part>
�LOAD�MODULE�VERSION�LEVEL:�nnn
�SOURCE�MODULE�VERSION�LEVEL:�(NONE)

�If�the�greatest�numeric�extension�gleaned�from�disk�is
�equal�to�the�numeric�extension�found�in�the�load�module,
�then�no�discrepancy�message�is�produced.

�If�Chklod�finds�no�discrepancies�of�any�kind,�including
�doubly�marked�load�modules,�then�the�message:








�page�12�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�NO�DISCREPANCIES�FOUND

�will�be�output�to�the�system�console.


�INVALID�LOAD�MODULE

�If�the�object�module�is�a�library�then�Chklod�will�print

�"SPECIFIED�LOAD�MODULE�IS�A�LIBRARY"

�to�the�console�output,�and�exit.

�------------------------------------------------------------

�LOAD�MODULE�LISTING�PRODUCTION

�The�keyword�"LISTING",�or�its�abbreviation�"LT",�appearing
�in�the�command�tail�causes�Chklod�to�produce�a�formatted�listing
�of�the�source�program�version�level�information�found�in�the
�specified�load�module.�If�no�output�device�is�specified,�then
�the�listing�goes�to�the�console�output�device.

�The�listing�will�be�sorted,�unless�the�NOSORT�option�is
�specified.

�A�sample�listing�follows:

�LISTING�OF�<load_module_filename>

�ARITHM.003
�MAIN�.015
�PARSE�.227
�RMATH�.004

�-------------------------------------------------------------

�FORMAL�SYNTACTIC�SPECIFICATION

�Chklod�is�invoked�with�the�command:

�CHKLOD�<loadable_isis_file>�[�<command_tail>�]

�<command_tail>�:==
�<command_tail_element>�[�<command_tail_element>...�]

�<command_tail_element>�:==�<directory_specification>�|
�<listing_specification>�|
�<nosort_specification>

�<directory_specification>�:==�{ascii_decimal_digit}

�<listing_specification>�:==
�(�LISTING�|�LT�)�[�TO�<isis_output_device>�]

�<nosort_specification>�:==�NOSORT






�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�13


�+==================================+
�|�|
�|�CLEAN�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�Clean,�is�one�of�the�Source�Management�Tools.�It�gets
�rid�of�all�the�"old"�versions�of�a�file�on�the�specified
�disk.�An�old�file�is�one�with�the�same�name�part�as
�another�file,�but�with�a�lesser�numeric�extension.



�SAMPLE�INVOCATION

�clean�1



�ACTIONS�PERFORMED

�The�single�decimal�digit�specifies�which�device�Clean
�is�to�operate�upon.�Clean�reads�in�the�directory,�and
�determines�which�files�with�numeric�extensions�are�not�the
�most�recent�versions�of�that�file.�Clean�then�deletes�all
�the�old�versions�of�the�file.�Clean�also�deletes�all�files
�with�extensions�"tmp",�and�"bak".



�EXAMPLE

�If�the�files

�main.003,main.004,main.005

�pars.009

�token.bak

�appear�on�the�same�drive,�then�Clean�will�delete�files

�main.003,main.004

�token.bak


�SYNTAX

�Clean�<single$decimal$digit>








�page�14�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�LATEST�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�Latest�is�a�tool�which,�given�a�list�of�filenames,
�lists�the�greatest�numeric�extension�for�each.

�Latest�is�one�of�the�family�of�source�management�tools.



�SAMPLE�INVOCATION�WITH�OUTPUT

�Given�the�command:

�:f1:latest�5�main�filnam�&
�prog�&
�errs


�and�assuming�that

�:f5:main�has�numeric�extensions�001�002�and�006
�:f5:filnam�has�numeric�extension�000
�:f5:prog�has�numeric�extension�531
�:f5:errs�has�numeric�extension�211

�then�LATEST�will�report

�main�has�greatest�numeric�extension�main.006
�filnam�has�greatest�numeric�extension�filnam.000
�prog�has�greatest�numeric�extension�prog.531
�errs�has�greatest�numeric�extension�errs.211



�FORMAL�SYNTACTIC�AND�SEMANTIC�SPECIFICATION

�LATEST�<decimal_digit>�<filename_list>�|
�<question_mark>

�<decimal_digit>�is�a�decimal�digit�from�0�to�9,
�giving�the�device_number�from�which�numeric�filename
�extensions�should�be�taken.�Only�one�such�device�is
�allowed�per�invocation�of�latest.











�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�15


�<filename_list>�is�a�list�of�ISIS-II�filenames
�separated�by�blanks,�(NOT�commas).�Additionally,
�filenames�of�the�form�namepart�dot�(no�extension,�but
�just�a�dot)�are�allowed�and�have�an�interpretation
�given�below.�Note�that�the�device�can�be�respecified
�but�the�respecification�will�be�ignored.�Continuation
�is�allowable�by�the�use�of�an�ampersand�as�the�last
�non-blank�character�on�a�line.�Two�asterisks,�the
�standard�continuation�line�prompt,�will�be�issued
�before�continued�lines.

�?�Causes�LATEST�to�print�its�version�number�and�exit.




�WARNINGS�AND�ERRORS

�Warnings�will�be�printed�to�both�the�cold�boot�console
�and�to�the�current�system�console,�if�different.










































�page�16�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�++==============================================++
�||�||
�||�ISIS-II�SOFTWARE�TOOLBOX�||
�||�VERSION�2.0�||
�||�SUBMIT�FILE�CONTROL�OVERVIEW�||
�||�||
�++==============================================++



�Structured�programming�has�finally�arrived�for�the
�submit�file�user.�It�is�now�possible�to�structure�your
�command�files�to�make�them�more�readable,�more�manageable,
�and�more�meaningful.�The�package�of�commands�will�allow
�you�to�make�decisions�and�branches�within�a�command�file.

�The�"IF"�family�of�tools�contains�a�series�of
�commands�which�work�together�to�provide�this�total
�capability:�IF,�ELSE,�ENDIF,�GOTO,�LOOP,�RETURN,�RESCAN,
�EXIT,�WAIT�and�NOTE.

�As�an�example,�the�following�Submit�file�will�compile�a
�program,�and�if�there�are�no�errors,�link�and�locate�the
�object.

�IF�NOT�PLM80�THEN
�WAIT;�Put�the�PLM80�disk�in�drive�0
�LOOP;
�ENDIF;
�;;�RECOMP
�PLM80�%0.PLM�NOLIST
�IF�'�0�PROGRAM�ERROR'�IN�%0.LST�THEN
�LINK�%0.OBJ,SYSTEM.LIB,PLM80.LIB�TO�%0.LNK
�LOCATE�%0.LNK�MAP�PURGE
�ELSE
�EDIT�%0.PLM
�NOTE;�Enter�a�'1'�if�you�want�to�recompile�%0.PLM
�IF�'1'�=�:VI:�THEN
�GOTO�RECOMP;
�ENDIF;
�ENDIF;

�Line�1�of�the�example�is�a�test�to�see�if�the�file
�PLM80�exists�on�the�disk�on�drive�0,�if�it�does�not,�then
�the�WAIT�command�will�be�executed,�and�wait�for�the�user
�to�enter�a�carriage-return�at�the�console.�Then�the�file-
�existence�test�is�repeated�until�the�file�"PLM80"�is
�detected�on�drive�0.

�Line�5�is�a�comment�line,�but�it�is�of�the�format�(two
�consecutive�leading�semicolons)�that�the�GOTO�command�uses
�for�declaring�labels.�Next�comes�the�compile�itself.�Line
�7�is�to�check�if�the�compilation�has�errors�by�looking�for
�the�string�'�0�PROGRAM�ERROR'�in�the�program�listing.�If








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�17


�the�string�is�in�the�listing,�then�no�errors�were�detected
�and�it�is�OK�to�link�and�locate.�(The�'THEN'�after�the
�condition�is�treated�as�a�comment.)�If�the�condition�is�not
�met,�the�editor�is�invoked,�and�the�user�can�edit�the
�source�and�recompile.

�Finally,�the�NOTE�command�tells�the�user�what�can�be
�done�to�cause�the�modified�source�program�to�be�recompiled.
�The�next�IF�command�allows�the�user�to�interactively
�control�the�flow�of�the�commands.

�This�example�illustrates�almost�all�the�features�of�the
�IF�family�of�tools.�The�individual�commands�are�explained
�in�the�following�sections.
















































�page�18�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�IF�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�The�IF�command�is�the�most�powerful�command�of�the
�package.�It�ascertains�the�conditions�that�are�most�likely
�used:�string�comparison,�file�comparison,�string�in�file,
�and�file�existance.�There�are�six�types�of�assertions.

�The�syntax�of�the�IF�command�is:

�IF�[NOT]�<string>�=�<string>�[comment]
�IF�[NOT]�<string>�=�<file>�[comment]
�IF�[NOT]�<string>�IN�<string>�[comment]
�IF�[NOT]�<string>�IN�<file>�[comment]
�IF�[NOT]�<file>�=�<file>�[comment]
�IF�[NOT]�<file>�[comment]


�1.�STRING�COMPARISON

�The�first�form,�string�comparison,�allows�conditional
�Submit�file�execution�based�on�submit�parameters.�One
�compares�a�fixed�string�with�a�string�defined�by�a�submit
�parameter.�An�example:

�;copy�files�from�:F3:�to�:F2:,�IDISK�first�if�desired.
�IF�NOT�'%0'�=�''�THEN
�IDISK�:F2:NONSYS.DSK
�ENDIF;
�COPY�:F3:*.SRC�TO�:F2:�Q
�^E

�The�two�strings�being�compared�are�both�converted�to
�upper-case�first,�and�the�length�of�the�two�strings�must
�be�the�same.�Otherwise�they�are�considered�unequal.

�Note�that�the�command�syntax�allows�optional�comments
�after�the�conditional�expression.�The�word�'THEN'�is
�treated�as�a�comment�by�the�IF�command.

�2.�STRING�=�FILE

�The�second�form�of�the�IF�command�is�most�often�used
�for�interactive�control.�It�tests�if�the�given�string
�exactly�matches�the�first�part�of�the�specified�file,�the
�remainder�of�the�file,�if�any,�is�ignored.�An�example:









�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�19


�NOTE;�select�the�compiler�to�use�by�typing�'1'�when�desired
�IF�'1'�=�:VI:�THEN�/*�PASCAL�*/
�PASC86�%0
�ELSE�IF�'1'�=�:VI:�THEN�/*�PLM86�*/
�PLM86�%0
�ELSE�IF�'1'�=�:VI:�THEN�/*�FORTRAN�*/
�FORT86�%0
�ELSE�IF�'1'�=�:VI:�THEN�/*�ASM86�*/
�ASM86�%0
�ELSE
�NOTE;�You�didn't�select�any�compile,�&
�type�'1'�if�you�want�to�start�over�again.
�IF�'1'�=�:VI:�THEN
�LOOP�/*�Start�over�again�*/
�ENDIF;
�ENDIF;


�This�example�also�shows�how�nested�IF�commands�and�ELSE�IF
�commands�(which�are�explained�later)�are�used.�IF�commands
�can�be�nested�to�an�arbitrary�depth.

�*�CAUTION�*

�Be�careful�not�to�use�:CI:�as�the�input�file,�because
�it�will�cause�the�next�command�line�to�be�swallowed.�Note
�that�the�input�file�is�opened�for�read.�Therefore�when�:VI:
�is�specified,�there�is�no�echoing,�and�no�indication�that
�the�system�is�waiting�for�input�is�given;�so�it�is�best�to
�limit�the�input�to�a�single�character,�and�to�provide�a
�note�to�the�user

�Also,�IF�does�does�not�do�any�parity�checking,�so
�PARITY�MAY�BE�CAUSING�IMPROPER�CONDITIONAL�EXECUTION.�If
�the�parity�is�either�odd,�even,�or�always�1,�improper
�comparisons�may�take�place.�To�avoid�this,�MAKE�SURE
�PARITY�IS�SET�TO�ZERO�ON�THE�INPUT�DEVICE.

�3.�STRING�IN�STRING

�The�third�form�of�the�IF�command�allows�a�list�of
�options�to�be�specified�in�only�one�parameter.�The
�following�command�sequence�will�compile,�link�and�locate,
�and�optionally�run�a�program�depending�on�the�parameter
�input.

















�page�20�ISIS-II�SOFTWARE�TOOLBOX,�V2.0




�IF�'C'�IN�'%1'�THEN�/*�Compile�it�first�*/
�PLM80�%0.PLM
�IF�NOT�'�0�PROGRAM�ERROR'�IN�%0.LST�THEN
�EXIT;�/*�Skip�the�rest�of�the�commands�*/
�ENDIF;
�ENDIF;
�IF�'L'�IN�'%1'�THEN�/*�Link�and�locate�the�object�*/
�LINK�%0.OBJ,SYSTEM.LIB,PLM80.LIB�TO�%0.LNK
�LOCATE�%0.LNK�PURGE
�ENDIF;
�IF�'G'�IN�'%1'�THEN�/*�Go�run�it�*/
�%0
�ENDIF;

�SUBMITting�the�above�command�file�with�the�parameters
�(:F3:FOO,�CLG)�will�cause�the�program�:F3:FOO.PLM�to�be
�compiled,�linked�and�located,�and�executed.�However,�the
�parameters�(:F3:FOO,�CL)�will�not�run�the�final�program.
�(:F3:FOO,�C)�will�only�compile�it.

�4.�STRING�IN�FILE

�The�fourth�form�of�the�IF�command�is�used�to�test�the
�presence�of�a�particular�string�in�a�file.�The�typical�use
�is�to�detect�compilation�errors,�"unresolved�externals"�in
�a�Link,�etc.�Note�that�when�doing�this,�be�careful�to�put
�a�blank�before�the�'0',�otherwise�you�will�think�there�are
�no�errors�when�there�may�be�10�or�20�errors�detected.
�It�also�works�if�the�NOLIST�option�is�used�to�cut�down�on
�the�size�of�the�listing�file.

�This�form�is�slightly�different�from�the�second�form
�(explained�above)�when�used�for�interactive�control.
�Although�the�test�for�the�presence�of�a�string�will�work�if
�the�input�file�is�a�disk�file,�it�will�take�forever�if
�input�is�coming�from�an�interactive�terminal.�That's
�because�the�IN�form�of�the�command�will�try�to�fill�the
�buffer�completely�before�starting�the�scan,�the�buffer�is
�about�50K�bytes.�The�second�form,�on�the�other�hand,�only
�reads�in�as�many�bytes�as�the�length�of�the�string,
�allowing�interactive�input.

�5.�FILE�COMPARISON

�The�fifth�form�of�the�IF�command�is�to�compare�two
�files.�The�comparison�is�a�byte�by�byte�comparison.�If
�the�two�file�names�are�exactly�the�same,�then�the�files�are
�considered�the�same,�even�if�they�don't�exist.�However,�if
�the�file�names�are�not�the�same,�then�the�files�are
�considered�the�same�only�if�their�contents�are�exactly�the
�same.�It�is�possible�to�use�'NOT'�as�the�first
�file�name.








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�21


�6.�FILE�EXISTENCE

�The�final�form�of�the�IF�command�is�used�to�test�the
�existence�of�a�particular�file.�Sometimes�it�is�useful�to
�make�sure�that�all�the�files�are�available�before�starting
�a�long�complicated�job.�Also,�the�existence�or�absence�of
�files�can�be�used�as�"boolean�variables"�vis-a-vis�the�"IF"
�family.�With�this�form,�since�the�'NOT'�may�actually�be�a
�file�name,�be�very�careful�with�the�trailing�comments;�if
�the�word�'NOT'�is�there,�then�either�follow�it�with�a�file
�name�before�the�comments,�or�else�make�sure�that�'NOT'�is
�the�last�item�on�the�command�line.


�STRINGS

�Strings�are�character�sequences�enclosed�in�single�or
�double�quote�characters.�Non-printable�characters�are
�allowed.�If�a�string�is�enclosed�in�single�quotes,�then
�the�single�quote�character�cannot�appear�in�the�string.
�Double-quoted�strings�cannot�contain�a�double-�quote
�character.�Unfortunately,�because�the�:CI:�file�is�line
�edited,�several�characters�are�not�permitted�in�the�string�-
�-CR,�LF,�ESC,�rubout,�CNTL-X,�or�CNTL-Z.�The�strings�are
�always�converted�to�upper-case�first,�so�looking�for�a
�lower-case�string�in�a�file�will�not�work.�Also�since�the
�'&'�is�used�to�denote�line�continuation,�it�cannot�appear
�in�the�string�either.�Continuation�lines�work�like�this:
�the�first�'&'�encountered�on�a�line�signals�continuation,
�the�rest�of�the�line�is�discarded�and�the�'&'�is�replaced
�with�a�space.�Therefore�it�is�possible�to�have�a�string�of
�arbitrary�length,�so�long�as�it�is�all�upper�case�and�does
�not�contain�CR/LF.

�Null�strings�are�allowed.�In�the�string�comparison,
�null�strings�are�handled�like�any�other�string.�In�the�IN
�conditions,�if�the�first�string�is�a�null�string,�then�the
�condition�is�automatically�true,�no�matter�what�the�right
�operand�is�(either�another�string�or�a�file).�If�the�right
�operand�of�the�IN�is�a�null�string,�then�the�condition�is
�also�true�by�default.

�The�IF�command�evaluates�the�condition�specified.�The
�NOT�reverses�this�condition.�If�the�condition�is�true,
�then�the�sequence�of�commands�following�the�IF�command�are
�executed.�If�the�condition�is�false,�then�it�will�skip�the
�following�commands�until�either�an�ELSE�command�or�an�ENDIF
�command�is�encountered.�Because�of�this�skipping�action,
�if�an�ELSE�command�or�an�ENDIF�command�is�not�found,�then
�it�is�possible�to�read�past�the�end�of�the�command�file�and
�cause�an�ISIS�error�29.�The�commands�that�are�skipped�are
�printed�(by�ISIS,�because�:CI:�automatically�echoes�to
�:CO:),�but�they�will�be�prefixed�with�an�'#'�instead�of�the
�normal�CLI�prompt.�IMBEDDED�CNTL-E's�WILL�CAUSE�A�BREAK,
�EVEN�IF�THEY�ARE�IN�COMMANDS�THAT�WON'T�BE�EXECUTED.







�page�22�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�ELSE�|
�|�v1.1�|
�|�|
�+==================================+


�The�ELSE�command�is�a�part�of�the�IF�command,�the
�syntax�of�the�ELSE�command�is:

�ELSE�[IF�command]

�The�ELSE�command�can�have�an�optional�IF�clause�on�the
�same�line.�If�the�initial�IF�command�evaluates�to�false,
�then�it�will�skip�forward�looking�for�the�ELSE�part.�If
�an�ELSE�IF�command�is�encountered�while�skipping,�then�the
�second�IF�condition�is�evaluated,�end�interpreted�the�same
�as�the�original�IF.�This�is�used�to�eliminate�nesting�IF
�command�structures�too�deeply,�it�is�also�quicker�because
�the�IF�program�does�not�need�to�be�reloaded.

�If�the�ELSE�command�is�encountered�while�executing�the
�commands�in�the�true�part�of�the�initial�IF�command�(or
�the�true�part�of�an�ELSE�IF�command),�then�it�will�skip
�all�other�commands�until�an�ENDIF�command�(explained
�below)�is�encountered.�As�with�the�IF�command,�all�the
�commands�that�are�skipped�will�be�prefixed�with�'#'
�instead�of�the�normal�CLI�prompt.






�+==================================+
�|�|
�|�ENDIF�|
�|�v1.1�|
�|�|
�+==================================+


�The�ENDIF�command�is�also�a�part�of�the�IF�command.�It
�only�serves�to�designate�the�end�of�an�IF�statement.�The
�ENDIF�command�is�required�for�the�semantic�integrity�of�the
�IF�and�ELSE�commands.�However,�when�loaded�and�executed,
�it�immediately�exits.














�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�23


�+==================================+
�|�|
�|�GOTO�|
�|�v1.1�|
�|�|
�+==================================+



�The�GOTO�command�is�used�to�transfer�control�to�any
�arbitrary�point�in�a�command�file.�The�syntax�of�the�GOTO
�command�is:

�GOTO�<label>

�The�<label>�has�the�form�of�a�valid�file�name�except
�that�no�':'�is�allowed;�i.e.,�6�alphanumeric�characters
�followed�by�a�period�followed�by�3�alphanumeric�characters
�followed�by�a�delimiter�character.


�The�GOTO�command�will�"rewind"�the�current�command
�file,�then�read�each�line,�looking�for�a�label
�declaration�that�matches�the�label�specified�on�the�GOTO
�command.�A�label�declaration�is�a�comment�line�starting
�with�TWO�CONSECUTIVE�SEMI-COLONS,�then�followed�by�the
�<label>.�When�the�label�declaration�is�found,�command
�execution�will�continue�with�the�following�command�line.
�As�with�the�IF�command,�care�must�be�taken�that�the�label
�is�declared,�otherwise�the�command�file�will�be�exhausted
�and�the�user�will�get�an�ISIS�error�29.�All�lines�that�are
�skipped�will�be�prefixed�with�'#'�instead�of�the�normal�CLI
�prompt.

�Note�that�the�GOTO�command�always�rewinds�the�file
�first,�then�starts�skipping.�Therefore�your�:CO:�file�may
�have�a�long�list�of�commands�if�you�are�many�lines�into�a
�command�file�and�do�a�forward�GOTO.�Since�GOTO�commands
�are�not�very�efficient�and�command�files�containing�them
�are�not�very�readable,�you�should�use�the�IF,�RETURN,
�LOOP,�and�EXIT�commands�whenever�possible�to�avoid�using
�the�GOTO�command.

�Example:

�;do�some�initial�commands
�;;�LOOP.1:
�IF�'0'�=�:VI:�THEN
�EXIT;�/*�Abort�it�all.�*/
�ENDIF;
�;do�some�stuff.
�;do�some�more�stuff.
�GOTO�LOOP.1;









�page�24�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�RETURN�|
�|�v1.1�|
�|�|
�+==================================+


�The�RETURN�command�is�used�to�return�control�to�the
�command�file�which�submitted�the�current�command�file.�It
�does�this�by�skipping�all�the�remaining�commands�until�the
�last�one�in�the�current�command�file.�Then�the�last
�command�is�executed.�All�commands�that�are�skipped�will�be
�prefixed�with�'#'�instead�of�the�normal�CLI�prompt.�The
�RETURN�command�does�not�take�any�parameters.

�If�the�last�command�line�starts�with�the�string�"END�JOB"
�then�RETURN�backs�up�to�the�previous�line�and�executes�that
�instead.�This�is�for�the�case�when�executing�an�IMPORT
�command�file,�in�which�the�second�to�last�line�is�the�RESTORE
�command.







�+==================================+
�|�|
�|�EXIT�|
�|�v1.1�|
�|�|
�+==================================+


�The�EXIT�command�is�used�to�abort�the�rest�of�the
�command�sequence�in�the�command�file.�It�resets�both�the
�:CI:�and�:CO:�files�to�the�cold�start�console.�It�can�be
�used�when�nested�several�levels�deep�in�submits.�The�EXIT
�command�does�not�take�any�parameters.





















�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�25


�+==================================+
�|�|
�|�LOOP�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�Loop�is�a�program�which�allows�the�user�a�measure�of
�control�over�submit�file�execution.

�When�loop�is�invoked�in�a�submit�file,�it�forces
�command�execution�to�begin�at�the�beginning�of�the�file.


�EXAMPLE

�There�is�a�need�to�search�through�a�stack�of�diskettes
�for�the�program�"foo".�The�following�submit�file�is�used:


�WAIT;�put�disk�in�drive�4
�DIR�4I�FOR�FOO*.*
�LOOP


�After�each�invocation�of�Wait,�a�new�diskette�is
�inserted�in�drive�4,�and�carriage-return�is�hit�to�re-
�execute�the�submit�file.�This�is�much�faster�and�easier
�than�retyping�the�directory�command�for�each�diskette�to�be
�searched.





























�page�26�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�RESCAN�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�This�program�allows�the�user�to�(re)start�execution�of
�a�Command�Sequence�(CS)�file�at�any�line�within�it.


�SAMPLE�INVOCATION

�RESCAN�:F3:TEST.CS


�ACTIONS�PERFORMED

�The�action�performed�by�this�program�is�to�read
�and�display�the�Command�Sequence�file,�line-by-line;
�and�after�each�line�is�displayed,�read�a�character
�from�the�keyboard�to�determine�whether�to:�exit,�begin
�Command�Sequence�file�execution,�or�return�to
�interactive�mode.

�The�command�characters�and�actions�associated�with
�them�are:


�ESCAPE�--the�submit�file�is�restarted�at�the
�currently�displayed�line

�CONTROL$C�--Rescan�returns�the�system�to
�interactive�mode�and�exits

�TILDA�--ignored

�ANYTHING�ELSE�--the�next�line�of�the�file�will�be
�printed



�CAVEAT

�Due�to�the�way�ISIS�handles�the�console�device,
�reaching�the�end�of�the�file�will�cause�an�ISIS�error
�29�with�a�subsequent�reboot.












�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�27


�+==================================+
�|�|
�|�WAIT�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�WAIT�allows�one�a�modicum�of�interactive�control�over
�the�execution�of�one's�submit�files.


�SAMPLE�INVOCATION

�WAIT�insert�diskette�source.all�in�drive�5


�INTENDED�USE�ENVIRONMENT

�The�Wait�program�is�intended�to�be�used�in�a�submit
�file�any�time�pauses�are�needed.


�ACTIONS�PERFORMED

�When�invoked�within�a�submit�file,�Wait�will�display�its
�command�tail�to�the�cold-boot�console,�if�different�from�the
�current�console,�and�wait�for�input�from�the�user.�The�user
�will�type�a�carriage-return�to�continue�execution,�or�CNTL-C
�to�exit.�Any�other�character�will�cause�the�bell�to�ring.

�Wait�has�ampersand�continuation,�so�the�command�tail
�message�may�be�spread�over�many�lines.


�TYPICAL�USE�ENVIRONMENT

�Wait�is�useful�in�conjunction�with�LOOP�and�the�IF
�family�of�programs.

�Another�possible�use�of�wait�is�within�a�submit�file
�which�requires�that�a�number�of�different�diskettes�be�placed
�in�the�same�drive,�at�different�times.�Wait�may�be�used�to
�halt�the�submit�file�execution�until�the�user�has�placed�the
�necessary�diskette�in�the�drive�and�hit�a�<CR>.�The�user
�can�be�prompted�by�a�comment�placed�in�Wait's�command�tail.


�FORMAL�SYNTAX�SPECIFICATION

�wait�<message>�[�{&�<ignored�text>�<crlf>�<message>}...�]

�where�<message>�is�a�string�of�characters.







�page�28�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�NOTE�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�The�Note�program�keeps�the�user�informed�as�to�the
�progress�of�an�executing�submit�file,�when�the�consol
�output�(:co:)�has�been�re-directed�to�a�disk�file.


�SAMPLE�INVOCATION

�NOTE�...�beginning�compilation�of�prog.src�...



�ACTIONS�PERFORMED

�Note�prints�its�command�tail�to�the�"cold-boot-
�console";�which�is�video-output�(:vo:)�in�the�case�of�a
�CRT,�or�teletype-output�(:to:)�for�a�teletype.�The�mapping
�of�:co:�to�the�disk�file�is�not�affected.

�If�:co:�has�not�been�re-directed,�then�note�does�no
�printing,�but�merely�exits.

�Note�has�ampersand�continuation,�so�the�command�tail
�message�may�be�spread�over�many�lines.



�FORMAL�SYNTAX�SPECIFICATION

�note�<message>�[�{&�<ignored�text>�<crlf>�<message>}...�]

�where�<message>�is�a�string�of�characters.





















�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�29


�+==================================+
�|�|
�|�COMPAR�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�Compar�compares�two�text�files�and�displays�the
�differences�between�them.

�Compar�may�be�used�to�compare�source�files�which
�contain�substantial�differences.�It�is�fast�enough�that
�finding�a�single�character�mismatch�is�feasible.



�SAMPLE�INVOCATION

�Compar�:f1:good.fil�to�:f1:bad.fil�&
�print(:f1:output.cmp)�sync(3)

�In�this�example,�the�two�files�are�compared,�and�the
�portions�which�miscompare�are�displayed�in�file
�":f1:output.cmp".�"Sync(3)"�means�that�3�lines�must�match
�before�Compar�deems�that�the�files�have�been
�resynchronized.



�ACTIONS�PERFORMED

�Compar�prints�out�the�mismatching�blocks�of�lines.



�PERFORMANCE

�Approximately�10�seconds�of�processing�time�are
�required�to�synchronize�with�sync(50)�and�200�lines
�inserted�into�one�file.�200�lines�with�the�default�sync(3)
�takes�4�seconds.




�OUTPUT�FORMAT


�LINE�IDENTIFICATION

�Each�line�is�printed�with�a�file�designation�number�(1
�or�2)�and�a�line�number�prefix.�The�pound-�sign�(#)�in�the







�page�30�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�prefix�indicates�the�line�is�the�first�in�a�mismatched
�block.�The�equals_sign�(=)�prefix�indicates�the�line�is�the
�first�matching�line�following�the�mismatched�block.



�FIRST�MISMATCHING�CHARACTER

�The�first�mismatching�character�in�any�given�block�is
�indicated�by�an�up-arrow�(^)�which�appears�beneath�it.

�If�the�original�line�wraps�around�the�screen,�so�will
�the�up-arrow�line.�In�this�case,�the�up-arrow�will�appear
�2�lines�below�the�corresponding�character.



�TABS�AND�UNPRINTABLE�CHARACTERS

�In�the�output,�tabs�echo�as�single�spaces.�Other
�unprintables�(including�form�feeds)�echo�as�a�question
�mark.



�EXAMPLE

�Given�the�"sample"�command�line�specified�above,�and
�given�that�the�text�files�to�be�compared�contain:

�good.fil

�+------------------------+
�|�good�line�1�|
�|�good�line�2�|
�|�good�line�3�|
�|�good�line�4�|
�|�good�line�5�|
�|�good�line�6�|
�|�good�line�7�|
�+------------------------+

�bad.fil

�+------------------------+
�|�bad�line�1�|
�|�good�line�2�|
�|�good�line�3�|
�|�good�line�4�|
�|�good�bad�line�5�|
�|�good�line�6�|
�|�good�line�7�|
�|�extra�line�|
�+------------------------+








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�31



�then�the�following�output�will�be�placed�in�file
�":f1:output.cmp":


�============================================================

�ISIS-II�SOURCE�COMPARE�v1.1
�PRINT(:F1:OUTPUT.CMP�)�SYNC(�3)
�FILE�1:�:F1:GOOD
�FILE�2:�:F1:BAD

�##########�MISMATCH�AT�RECORD�NO.�1�FILE1:�:F1:GOOD
�##########�AT�RECORD�NO.�1�FILE2:�:F1:BAD

�#1:�1:good�line�1
�^
�=1:�2:good�line�2

�-------------------------------------------------------------

�#2:�1:bad�line�1
�^
�=2:�2:good�line�2

�=============================================================

�##########�MISMATCH�AT�RECORD�NO.�5�FILE1:�:F1:GOOD
�##########�AT�RECORD�NO.�5�FILE2:�:F1:BAD

�#1:�5:good�line�5
�^
�1:�6:good�line�6
�1:�7:good�line�7
�=1:�8:^Z

�-------------------------------------------------------------

�#2:�5:good�bad�line�5
�^
�2:�6:good�line�6
�2:�7:good�line�7
�2:�8:extra�line
�=2:�9:^Z

�=============================================================


�FILES�DIFFER

�=============================================================











�page�32�ISIS-II�SOFTWARE�TOOLBOX,�V2.0



�PROGRAM�SYNTAX


�PRINT�CONTROL

�If�the�print�control�is�present�in�the�command�line,
�then�output�is�directed�to�the�specified�file.�If�the
�printfile�is�not�:co:,�then�the�output�will�contain�the
�invocation�information.

�If�an�output�file�is�not�specified�with�the�print
�control,�the�output�is�written�to�:CO:.


�SYNCHRONIZATION�(SYNC)�CONTROL

�The�sync�control�specifies�the�number�of�lines�that
�must�match�before�Compar�deems�that�the�files�have
�synchronized.�The�default�synchronization�value�is�3.

�A�sync(1)�may�be�used�to�compare�files�which�are
�expected�to�be�identical.�A�larger�value�for�the�sync
�parameter�should�be�used�when�differences�are�expected�in
�the�files.



�AMPERSAND�COMMAND�LINE�CONTINUATION

�An�ampersand�can�be�used�to�continue�the�command�line.
�It�may�not�be�within�a�filename�or�command�word.






�COMPAR�LIMITATIONS�AND�RESTRICTIONS


�INPUT�FILES�MUST�BE�TEXT�FILES

�The�files�to�be�compared�must�be�text�files.�Lines
�longer�than�254�characters�will�cause�Compar�to�hang.�A
�line�is�defined�as�a�sequence�of�characters�terminated�by�a
�line�feed.















�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�33




�SPACE�CONSTRAINTS

�If�the�files�mismatch,�and�synchronization�does�not
�occur�in�254�lines�or�before�the�buffers�are�used�up,�then
�the�message�"not�enough�space�to�synchronize"�is�given,�and
�Compar�exits.


�NAKED-LINE�FEEDS�IN�OUTPUT�FILE

�If�blank�lines�follow�a�blank�line�in�the�output
�file,�Compar�uses�naked�line-feeds�(sans�preceeding
�carriage-returns)�to�save�space.




�FORMAL�COMMAND�SYNTAX�SPECIFICATION

�Compar�<filename>�to�<filename>
�[print(�<filename>�)]�[sync(�<number>�)]







































�page�34�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�CONSOL�|
�|�v2.1�|
�|�|
�+==================================+


�SUMMARY



�The�Consol�program�uses�the�ISIS-II�system�call�of�the
�same�name�to�assign�the�"logical�devices"�:ci:�and�:co:�to
�physical�devices.


�SAMPLE�INVOCATIONS�WITH�EXPLANATIONS


�The�command

�CONSOL�,:F1:TEST.CSL

�redirects�the�console�output�(:co:)�to�the�disk�file
�:f1:test.csl.


�The�command

�CONSOL�:F1:JOB.CSD

�redirects�the�console�input�to�:f1:job.csd.�The�system
�will�begin�executing�commands�out�of�the�file.


�The�command

�CONSOL�:VI:,:VO:

�forces�the�system�back�into�interactive�mode.





















�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�35





�SOME�APPLICATIONS



�*�Execute�a�long�job�from�a�submit�file�and�save�the
�consol�output,�which�would�have�gone�to�the�screen,�in
�a�disk�file,�to�be�subsequently�examined�and
�processed.


�*�Use�Consol�to�assign�:ci:�to�an�already�existing
�Command�Sequence�file.�This�is�faster�than�using�the
�submit�program,�which�produces�a�new�Command�Sequence
�file�from�a�command�Sequence�Definition�File,�each
�time�it�is�invoked.�If�Consol�is�used�in�this�manner,
�there�should�be�another�consol�command�at�the�end�of
�the�file�(consol�:vi:,:vo:),�to�avoid�an�ISIS�error
�29.



�SYNTAX:

�CONSOL�<new$ci$device>�|
�[<new$ci$device>],<new$co$device>


































�page�36�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�DCOPY�|
�|�v2.2�|
�|�|
�+==================================+


�SUMMARY

�The�DCOPY�program�copies�entire�diskettes�to�entire
�diskettes.�The�copying�is�done�track-by-track,�bypassing
�the�ISIS-II�file�structure.�For�a�medium-full�to�full�disk,
�this�results�in�much�higher�speed,�for�the�entire�job,�than
�using�the�Copy�cusp.

�DCOPY�has�optional�formatting�and/or�verify�capability.

�DCOPY�will�NOT�work�with�hard�disks.�To�copy�hard
�disks,�use�HDCOPY.

�Since�DCOPY�creates�an�exact�copy�of�a�disk,�with�all
�disk�data�and�pointer�blocks�the�same�as�on�the�source
�disk,�if�the�file�system�integrity�on�a�disk�has�been
�damaged,�a�duplicate�disk�created�with�DCOPY�will�contain
�the�same�errors.


�SAMPLE�INVOCATIONS�WITH�EXPLANATIONS

�DCOPY�F=4�T=5

�copy�the�diskette�in�drive�4�to�the�diskette�in
�drive�5.


�DCOPY�I�F=5�T=6,7

�copy�the�diskette�in�drive�5�to�the�diskettes�in
�drives�6�and�7,�first�initializing�(formatting)�them.


�DCOPY�IV�F=0�T=1,2,3

�copy�the�diskette�in�drive�0�to�the�diskettes�in
�drives�1,2,�and�3,�with�verification,�first
�initializing�(formatting)�them.



�WORKS�ON�SINGLE�AND�DOUBLE�DENSITY

�The�DCOPY�program�operates�on�both�single�and�double
�density�diskettes�and�is�not�influenced�by�the�drive�from
�which�it�was�loaded.







�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�37




�SEMANTIC�DESCRIPTIONS


�NULL�COMMAND

�If�the�command�tail�is�null,�DCOPY�followed�by
�<CR><LF>,�the�defaults�set�is:�COPY�track�0�thru�76�from
�drive�0�to�drive�1.


�CHANGING�THE�INTERLEAVE�WHILE�COPYING

�If�the�item�interleave�is�absent�from�the�command�tail,
�the�interleave�from�the�source�diskette�will�be�duplicated
�on�the�destination�diskette.�If�the�item�"interleave"�(I=)
�is�in�the�command�tail,�the�integers�following�the�equals
�sign�will�be�the�new�interleave�numbers,�1�per�track,�for
�the�destination�diskette.

�If�interleave�integers�are�not�given�for�the�entire�77
�tracks,�the�last�interleave�integer�present�will�be�the
�interleave�used�for�the�remaining�tracks�of�the�destination
�diskette.�A�continuation�item�(&)�can�only�be�used�after�a
�comma�and�before�the�integer�in�the�interleave�integer
�list,�e.g.,�I=1,1,4,6,&�<CRLF>.


�DISKETTE�COPY�VERIFICATION

�The�item�verify�(V)�in�the�command�tail�will�copy�the
�source�diskette�to�the�destination�diskette�and�verify�all
�tracks�copied.�Verification�means�immediately�re-reading
�what�was�just�written�to�confirm�that�the�writing�was
�correctly�executed.


�VERIFICATION�ONLY

�The�item�verify�only�(VO)�in�the�command�tail�will
�verify�the�source�diskette�against�the�destination�diskette
�without�copying.


�SOURCE�DISKETTE�DESIGNATION

�The�item�FROM�(F=)�in�the�command�tail�followed�by�a
�disk�drive�number�designates�which�drive�contains�the
�source�diskette.












�page�38�ISIS-II�SOFTWARE�TOOLBOX,�V2.0




�DESTINATION�DISKETTE�DESIGNATION

�The�item�TO�(T=)�in�the�command�tail,�followed�by�a
�disk�drive�number,�or�a�series�of�up�to�4�drive�numbers,
�signifies�which�drive�contains�the�destination�diskettes.

�The�FROM�and�TO�devices�must�all�be�either�single�or
�double�density.�They�may�not�be�mixed.


�LOTRAK

�An�item�entered�as�the�first�free�standing�integer
�becomes�the�low�track�integer,�this�is�the�1st�track�acted
�upon�by�DCOPY.


�HITRAK

�The�next�integer�separated�by�a�blank�after�the�low
�trak�integer�becomes�the�high�track�integer.�This�is�the
�last�track�acted�upon�by�DCOPY.�If�HITRAK�is�not�entered,
�only�a�single�track�specified�by�low�track�is�acted�upon.


�COMMAND�LINE�CONTINUATION

�The�continuation�(&)�item�in�a�command�tail�can�be�used
�to�extend�the�command�tail�to�another�line.�The�ampersand
�must�not�appear�within�an�item�or�its�attributes.�The
�continuation�may�only�occur�between�other�complete�items.
�All�characters�past�the�continuation�are�not�processed�and
�are�considered�comments.�Exceptions�are�as�above�with�the
�interleave�item.�Several�lines�may�be�entered,�each
�separated�by�ampersands.




�DCOPY�FUNCTIONS

�DCOPY�is�used�to�duplicate�and/or�verify�single�or
�double�density�diskettes.�The�interleave,�verify�and�verify
�only�option�may�only�be�mixed�in�one�way.�Any�other�mix
�will�cause�a�syntax�error.�The�I�and�V�option�may�be
�entered�simultaneously.�This�default�causes�the�destination
�diskette�to�be�formatted,�copied�and�verified�against�the
�source�diskette.












�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�39






�The�DCOPY�program�will�type�one�or�both�of�the�words
�COPY�and�VERIFY,�depending�on�the�verify�option�selected,
�and�will�then�print�out�the�message�"LOAD�DISKETTES�AND
�TYPE�'CR'�".�At�this�point,�the�human�operator�places�the
�desired�diskettes�into�the�source�drive�and�destination
�drives,�and�hits�the�carriage�return�key�on�the�console.�To
�exit�DCOPY�and�return�to�ISIS�type�the�'E'�key�to�exit.


�DCOPY�prints�the�following�(for�example)�on�the
�console:

�----------�-----6789�012

�where�"-"�means�the�track�was�not�operated�upon,�and
�the�digit�is�the�last�digit�in�the�(decimal)�number�of�the
�track�operated�upon.�The�example�would�result�from�copying
�tracks�16-22�inclusive.

�If�tracks�are�found�to�be�not�identical�by�the�compare
�(verify)�operation,�DCOPY�prints�diagnostic�information�and
�enters�interactive�mode,�as�described�below:


�First,�DCOPY�indicates�which�track�and�sectors
�miscompare:

�----456
�track�4�0002H�SECTOR�MISCOMPARES
�track�4�0003H�SECTOR�MISCOMPARES
�track�4�001AH�SECTOR�MISCOMPARES
�*


�This�indicates�that�sectors�2,�3�&�26�of�track�0
�miscompare,�and�the�'*'�is�the�prompt�for�human�input.
�Acceptable�input�at�this�point�is�one�of�three�lines:

�E
�C
�<track-number>�<sector-number>�<drive-number>


�where�the�track-,sector-,and�drive�numbers�may�be�in
�any�valid�PL/M�number�syntax.�(Any�number�not�specified�is
�taken�as�zero)












�page�40�ISIS-II�SOFTWARE�TOOLBOX,�V2.0





�'E'�causes�DCOPY�to�exit�interactive�mode�and�prompts
�user�to�insert�new�disks.

�'C'�causes�DCOPY�to�continue,�starting�with�the�next
�track.�The�track�identification�output�(as�shown
�above)�is�repeated�,�with�tracks�already�operated
�upon�indicated�by�'-�'�instead�of�by�digits.

�6�3�1�(for�example),�will�cause�DCOPY�to�print�the�data
�in�sector�three�of�track�6�of�the�diskette�in
�drive�1.

�5�5�(for�example)�will�cause�DCOPY�to�print�the�data�in
�sector�5�of�track�5�of�the�diskette�in�drive�0.

�In�either�case,�DCOPY�will�prompt�with�another�'*',�and
�any�of�the�three�commands�above�is�again�acceptable�input.



�FORMAL�SYNTAX�SPECIFICATION

�DCOPY�<command�tail>

�<command�tail>�::=�<null>|<item>|�<command�tail>
�<item>�::=�<interleave>|<verify>|<verify�only>|
�<from>|<to>|<lotrak>|<hitrak>|<continuation>

�<interleave>�::=�$I|I|$I=<integer�list>|I=<integer�list>
�<verify>�::=�$V|V
�<verify�only>::=�$VO|VO
�<from>�::=�F=<disk�drive�number>
�<to>�::=�T=<disk�drive�number>[,<disk�drive�number>
�[,<disk�drive�number>]]
�<lotrak>�::=�<integer>
�<hitrak>�::=�<integer>
�<continuation>�::=�&

�<integer�list>�::=�<integer>|<integer>,<integer�list>|
�<integer>,<continuation><integer�list>

�<disk�drive�number>�::=�0|1|2|3|4|5|6|7|8|9



�KNOWN�PROBLEMS

�When�DCOPY�enters�interactive�mode,�and�displays�a�user-
�specified�disk�block,�subsequent�display�requests�will�show
�the�same�block,�even�if�the�request�is�different.









�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�41


�+==================================+
�|�|
�|�DSORT�|
�|�v3.3�|
�|�|
�+==================================+


�SUMMARY

�DSORT�is�an�ISIS-II�program�which�modifies�(re-�writes)
�the�directory�of�an�ISIS-II�floppy�or�hard�disk�(i.e.,�the
�ISIS.DIR�file),�so�that�the�files�on�the�disk�appear�in
�alphabetical�order�in�the�directory.


�SAMPLE�INVOCATIONS�WITH�EXPLANATIONS

�DSORT�1
�The�directory�of�drive�1�is�alphabetically�sorted.

�DSORT�4E
�The�directory�of�drive�4�is�sorted�by�EXTENSION.

�DSORT�7�S
�The�directory�of�drive�7�is�sorted�by�SIZE.

�DSORT�0�L
�The�directory�of�drive�0�is�sorted�by�LOCATION.


�PERFORMANCE

�DSORT�takes�about�5�seconds�to�sort�a�full�floppy
�directory,�it�is�much�faster�for�directories�that�are
�relatively�empty;�and�it�is�much�faster�yet�for�the�hard
�disk.



�LIMITATIONS�OF�DSORT

�DSORT�WILL�NOT�WORK�WITH�WINCHESTER�OR�REMOTE�DIRECTORIES.

�If�execution�of�DSORT�is�interrupted,�either�by
�pressing�interrupt�switch�1�or�reset,�the�disk�directory
�may�be�clobbered.

�DSORT�will�function�properly�for�both�floppy�disks�and
�hard�disks.�However,�the�memory�size�available�must�be�large
�enough�to�hold�the�entire�directory;�DSORT�verifies�that�the
�entire�directory�is�read�in�before�it�goes�into�the�sort.
�DSORT�requires�a�32K�or�larger�system.�In�general,�there
�should�be�no�problems�with�a�64K�system.








�page�42�ISIS-II�SOFTWARE�TOOLBOX,�V2.0



�DSORT�may�be�incompatible�with�future�releases�of�ISIS.


�ACTIONS�PERFORMED

�DSORT�will�identify�itself�by�version�number.�Failure
�to�properly�identify�a�disk�drive�or�specifying�a�non-
�supported�drive�(e.g.,�drive�9�while�running�ISIS�V3.4)
�will�cause�DSORT�to�exit.

�If�the�"E"�option�is�specified,�then�the�entries�of�the
�specified�disk�directory�are�sorted�by�extension,�with�a
�name�sub-sort�within�the�extension�sort.

�If�the�"S"�option�is�specified,�then�the�entries�of�the
�specified�disk�directory�are�sorted�by�file�size.�The
�entries�of�the�smallest�files�appear�at�the�beginning�of
�the�directory.


�If�the�"L"�option�is�specified,�then�the�entries�of�the
�specified�disk�directory�are�sorted�into�the�order�in�which
�their�header�blocks�appear�upon�the�disk.



�FORMAT�ATTRIBUTE

�Any�file�with�the�Format�Attribute�set�is�deemed�to
�"alphabetically�precede"�any�file�without�the�Format
�Attribute�set.


�DIRECTORY�COMPACTION

�The�directory�is�compacted;�so�the�DIR�command�and�file
�searches�may�be�faster�than�previously.�Note�that�even�the
�ISIS.*�files�are�sorted.



�FORMAL�SYNTACTIC�SPECIFICATION

�valid_command�:==�DSORT�<drive_num>�[options]
�drive_num�:==�ascii_decimal_digit
�options�:==�(�E�|�L�|�S�)


�KNOWN�PROBLEMS

�Dsort�may�not�work�correctly�on�a�disk�directory�which
�is,�or�ever�has�been,�totally�full;�i.e.,�200�files�on�a
�double�density�floppy�disk,�or�992�files�on�a�hard�disk.








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�43


�+==================================+
�|�|
�|�ERRS�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�ERRS�is�a�program�which�displays�errors�and�warnings
�in�PL/M�listings.



�PROGRAM�INVOCATION

�Invoke�ERRS�by�typing:

�ERRS�<filename>



�ACTIONS�PERFORMED

�ERRS�issues�a�"clear�screen"�for�most�terminals.�You
�may�get�a�couple�of�characters�of�garbage�at�the�top�of
�your�display,�which�is�the�visible�part�of�clear�screen
�codes�for�other�terminals.

�ERRS�displays�the�next�portion�of�the�file�that�contains
�an�error�message�or�warning�message�line,�and�waits�for
�keyboard�input.�Control-C�aborts,�any�other�character
�continues.



�PROGRAM�LIMITATIONS

�ERRS�occasionally�hangs�up�on�very�large�listings.






















�page�44�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�GANEF�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�GANEF�is�an�ISIS-II�program.�It�provides�a�collection
�of�routines�to�read,�examine,�modify,�and�write�sectors�on
�a�disk,�bypassing�any�file�structure�that�may�be�on�the
�disk.�Using�GANEF,�the�user�may�recover�files,�destroy
�files,�patch�files,�and�achieve�other�wondrous�effects.


�LIMITATIONS

�GANEF�WILL�NOT�WORK�WITH�REMOTE�DIRECTORY�DISKS.

�Recovering�bad�disks�can�be�a�difficult�task.�It�is
�recommended�that�the�entire�contents�of�a�disk�with
�"recovered"�files�be�COPYed�to�a�freshly�formatted�disk,
�to�insure�the�integrity�of�the�file�structure,�before�any
�further�use�is�made�of�said�files.


�INVOCATION

�To�invoke�GANEF,�enter�the�command�"GANEF".�Any
�command�tail�is�ignored.�GANEF�will�identify�itself�by
�version�number,�and�prompt�for�a�command.


�OVERVIEW�OF�COMMANDS

�GANEF�commands�are�one�character,�followed�sometimes�by
�parameters.�GANEF's�current�repertoire�includes:�A,�B,�D,
�E,�F,�M,�Q,�R,�S,�T,�W,�and�<CR>,�as�described�below.
�After�completing�a�command,�GANEF�will�prompt�for�another.

�In�the�following�sections,�when�the�term�<block>�is
�used,�it�refers�to�the�location�of�a�specific�128�byte
�sector�on�a�disk.�The�format�is:


�<block>�::=�<track>�<sector>�[<disk>]


�where�the�<track>,�<sector>�and�<disk>�are�numbers�in
�the�proper�range,�or�the�minus�sign�('-').�If�the�minus�is
�entered,�then�the�value�of�the�field�is�not�changed�from
�the�previous�value.�Likewise�for�any�field�that�is�not








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�45




�entered,�the�previous�value�is�used�(except�for�<disk>,
�which�is�discussed�below.)�For�floppy�disks,�the�<track>
�must�be�in�the�range�0�..�76.�For�hard�disks,�it�must�be�in
�the�range�0�..�199.�For�Winchester�disks,�it�must�be�in�the
�range�0�..�211.�The�<sector>�must�be�in�the�range�1�..
�26�for�single�density�floppy,�1�..�52�for�double�density
�floppy,�1�..�144�for�hard�disk,�and�1�..�210�for�Winchester
�disk.�The�value�for�<disk>�must�be�in�the�range�0�..�9.
�These�values�are�validated�using�the�ISIS-II�SPATH�procedure.
�This�is�done�to�minimize�the�chance�of�entering�a�wrong�value
�and�get�an�error�24.



�DEFAULT�COMMAND�PARAMETERS

�GANEF�always�remembers�the�<track>,�<sector>,�and
�<disk>�of�the�last�sector�it�operated�on.�Therefore�if�any
�of�the�parameters�are�not�entered,�then�the�values�from�the
�last�operation�will�be�used.



�ASSIGNING�DEFAULT�DISKS

�It�is�possible�to�select�a�particular�disk�as�the
�default�to�use�when�a�<disk>�parameter�is�not�entered.
�There�are�two�default�disks,�a�disk�to�read�from,�and�a
�disk�to�write�to.�These�defaults�are�established�with�the
�Assign�command.�When�a�command�that�causes�a�sector�to�be
�read�is�entered�without�the�<disk>�parameter,�then�the
�default�"read"�disk�is�selected.�Likewise,�when�a�command
�that�causes�a�sector�to�be�written�is�entered�without�the
�<disk>�parameter,�then�the�default�"write"�disk�is
�selected.�This�is�done�to�facilitate�the�copying�of
�sectors�from�one�disk�to�another.




�GANEF�COMMANDS
�-----�--------

�============================================================

�ASSIGN

�A�[R=<disk>]�[W=<disk>]

�This�command�assigns�the�specified�disks�as�the
�default�disks�to�read�and�write.

�------------------------------------------------------------







�page�46�ISIS-II�SOFTWARE�TOOLBOX,�V2.0



�------------------------------------------------------------

�BIT

�B�[<block>]

�This�command�displays�the�bit�number�of�the�specified
�sector�in�the�bit-map�(ISIS.MAP).

�------------------------------------------------------------

�DISPLAY

�D�[<block>]

�This�command�displays�the�specified�sector�on�the
�console�in�hex�and�ASCII�format.

�------------------------------------------------------------

�EXIT

�E

�This�command�exits�the�GANEF�program.

�------------------------------------------------------------

�FIND

�F�[<block>]

�This�command�finds�a�sector�containing�a�specified
�string,�the�scan�starts�at�the�specified�sector.�If�a�block
�parameter�is�specified,�then�GANEF�will�ask�for�an�input
�string�to�search�for.�If�just�the�command�is�given,�then
�the�string�entered�previously�will�be�used,�and�the�block
�number�is�incremented�by�one�before�starting�the�scan.
�GANEF�maintains�an�internal�buffer�to�hold�the�search�string,
�this�buffer�looks�exactly�like�a�disk�sector,�it�is�128�bytes
�long,�and�initially�it�contains�zeros.�When�asked�to�enter
�the�string,�you�are�actually�modifying�the�contents�of�the
�search�buffer�in�the�same�manner�as�the�S�command�(described
�below).

�------------------------------------------------------------

�MARK�SECTOR

�M�[<block>]

�This�command�marks�the�specified�sector.�It�is
�possible�to�recall�the�block�at�a�later�time.�This�is








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�47





�particularly�useful�when�following�the�pointer�chains�of�an
�ISIS-II�file.�The�header�block�is�"marked",�then�the�data
�sectors�are�examined,�and�to�find�the�next�data�sector,
�just�recall�the�header�block�again.

�------------------------------------------------------------

�QUIT

�Q

�This�command�quits�the�GANEF�program,�same�as�the
�E�command.

�------------------------------------------------------------

�RECALL�MARKED�SECTOR

�R

�This�command�recalls�the�block�that�was�marked�and
�displays�it.

�------------------------------------------------------------

�SUBSTITUTION�MODE

�S�[<offset>]

�This�command�initiates�the�substitute�mode.�In�this
�mode,�you�can�modify�the�contents�of�the�internal�buffer�by
�entering�byte�values�in�hex�or�strings�in�ASCII,�or�hit
�<CR>�to�leave�the�byte�unchanged.�ASCII�strings�are
�enclosed�in�single�quotes,�all�characters�are�translated�to
�upper�case,�and�no�embedded�single�quotes�are�allowed.

�If�the�value�is�not�a�string�(does�not�begin�with�a
�single�quote),�then�it�is�assumed�to�be�a�hex�number�(i.e.,
�it�does�not�have�to�begin�with�a�digit,�and�it�does�not
�have�to�be�followed�by�the�letter�'H').�Hex�byte�values�can
�be�optionally�followed�by�a�repeat�factor.�The�format�of
�the�repeat�factor�is�HH*R,�where�HH�is�the�hex�value�to�be
�entered,�and�R�is�the�number�of�times�the�value�is�to�be
�repeated,�default�is�1�if�*R�is�not�entered.�R�is�entered
�in�the�PL/M�number�format,�i.e.,�it�is�assummed�to�be
�decimal�unless�followed�by�the�letter�'H'.

�Many�bytes�can�be�changed�in�a�single�line.�Also,
�strings�and�hex�values�can�be�intermixed�on�a�line.�If�a
�byte�is�to�remain�unchanged�when�changing�many�bytes�on�a
�single�line,�then�enter�the�character�'-',�this�will�leave








�page�48�ISIS-II�SOFTWARE�TOOLBOX,�V2.0




�the�byte�unchanged.�GANEF�will�display�the�offset�of�the
�byte�to�be�modified�next,�and�its�hex�value.�The�offset
�value�wraps�around�at�127�back�to�0.�To�exit�the
�substitute�mode,�end�a�line�with�the�<ESC>�key�instead�of
�the�<CR>,�GANEF�will�re-display�the�content�of�the�buffer.

�------------------------------------------------------------


�TRANSFER�FILE

�T�[<block>]

�This�command�transfers�the�file�from�a�disk�that
�probably�has�a�clobbered�directory�to�a�new�ISIS-II�file.
�The�<block>�specifies�the�location�of�the�header�block�of
�the�file�being�recovered.

�It�is�not�recommended�to�recover�a�file�from�a�disk�and
�put�the�new�file�on�the�same�disk,�because�sectors�of�the
�old�file�may�get�clobbered�by�the�new�file.�The�last
�sector�of�the�new�file�will�contain�128�bytes,�it�is�the
�user's�responsibility�to�edit�and�delete�the�unused�bytes.

�The�T�command�will�prompt�for�the�new�file�name.

�------------------------------------------------------------


�WRITE�SECTOR�TO�DISK

�W�[<block>]

�This�command�writes�the�contents�of�the�internal�sector
�buffer�to�the�specied�<block>.�If�W�is�followed
�immediately�by�a�<CR>,�then�<block>�is�assummed�to�be�the
�same�as�the�<block>�that�was�read,�and�a�verification�is
�requested.

�------------------------------------------------------------


�DISPLAY�NEXT�SECTOR

�<CR>

�This�command�will�display�the�next�sector�following�the
�one�previously�read.�If�the�previous�sector�was�the�last
�one�on�a�track,�then�the�first�sector�of�the�next�track�is
�displayed.

�============================================================








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�49





�ARITHMETIC�FUNCTIONS
�----------�---------

�ADDITION

�+�<operand1>�[<operand2>]

�This�command�will�add�<operand2>�to�<operand1>�and
�display�the�result�in�both�hex�and�decimal.



�SUBTRACTION

�-�<operand1>�[<operand2>]

�This�command�will�subtract�<operand2>�from�<operand1>
�and�display�the�result�in�both�hex�and�decimal.



�MULTIPLICATION

�*�<operand1>�[<operand2>]

�This�command�will�multiply�<operand1>�by�<operand2>�and
�display�the�result�in�both�hex�and�decimal.



�INTEGER�DIVISION

�/�<operand1>�[<operand2>]

�This�command�will�perform�an�integer�divide�of
�<operand1>�by�<operand2>�and�display�the�result�in�both�hex
�and�decimal.



�MOD�OPERATION

�%�<operand1>�[<operand2>]

�This�command�will�perform�an�integer�MOD�of�<operand1>
�with�<operand2>�and�display�the�result�in�both�hex�and
�decimal.












�page�50�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�GENPEX�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�GENPEX�allows�one�to�create�a�large�"database"�from
�which�all�kinds�of�external�declarations�can�be�created.
�Genpex�will�create�only�those�external�declarations�that
�are�needed�by�a�particular�file.

�Given�a�plm�source�file,�and�a�template�file�containing
�all�the�external�declarations�that�might�be�needed�by�that
�source�file,�Genpex�produces�a�PL/M�INCLUDE�file�containing
�only�those�declarations�that�the�source�file�actually
�needs.�This�helps�the�user�save�symbol�table�space�during
�compilation,�and�it�maintains�a�level�of�control�over
�external�references.�For�any�given�source�file,�the�PEX
�file�created�gives�a�list�of�all�external�procedures,
�variables,�and�LITERALLYs.�The�template�file�contains�all
�PUBLIC�procedures,�variables�and�LITERALLYs�for�a�given
�source�file.


�SAMPLE�INVOCATION

�GENPEX�<pex_file_list>�TO�<src_file>


�SEMANTICS

�src_file�is�any�PL/M�program�source�file�and�must
�contain�the�command

�$�INCLUDE�(<src_file_root>.IPX).


�<pex_file_list>�is�a�list�of�<pex_file>s,�separated�by
�commas.�<pex_file>�contains�text�that�will�define�global
�symbols�for�a�PL/M�program�source�file.

�Genpex�has�ampersand�continuation,�so�the�command�line
�may�be�spread�over�many�lines.



�ACTIONS�PERFORMED

�After�GENPEX�has�been�run,�all�symbols�used�in�the
�<src_file>�and�its�include�files�that�match�symbols�defined








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�51



�in�the�<pex_file>s�will�have�a�PL/M�declaration�inserted�for
�those�symbols�in�the�file�<src_file_root>.IPX.�Any�previous
�file�with�that�name�will�be�overwritten.�Public�symbols
�include�Procedures,�Variables,�and�Literallys.

�Genpex�will�find�$INCLUDE�lines�in�the�<src_file>,�even
�if�the�"$"�is�not�the�first�character�in�the�PL/M�line�(i.e.
�if�LEFTMARGIN�>�1),�as�long�as�the�"$"�is�not�preceded�by�the
�characters�"/","*",�or�any�letter�or�number.



�SAMPLE�TEMPLATE�FILE

�These�are�the�pex�declarations�for�the�Universal
�Development�Interface�(UDI).�NOTE�THAT�ALL�LINES�IN�THE
�PEXFILE�MUST�BEGIN�AT�THE�LEFT�MARGIN.


�/*�UDI�types�*/
�/*�these�become�LITERALLY�declarations�*/
�TOKEN�'word'
�CONNECTION�'word'


�/*�UDI�procedural�interface�*/
�/*�these�become�PROCEDURE�declarations�*/
�DQ$ALLOCATE�P(W@)"TOKEN"
�DQ$ATTACH�P(@@)"CONNECTION"
�DQ$CHANGE$EXTENSION�P(@@@)
�DQ$CLOSE�P("CONNECTION"@)
�DQ$CREATE�P(@@)"CONNECTION"
�DQ$DECODE$EXCEPTION�P(W@@)
�DQ$DELETE�P(@@)
�DQ$DETACH�P("CONNECTION"@)
�DQ$EXIT�P(W)
�DQ$FREE�P("TOKEN"@)
�DQ$GET$ARGUMENT�P(@@)B
�DQ$GET$CONNECTION$STATUS�P(W@@)
�DQ$GET$EXCEPTION$HANDLER�P(@@)
�DQ$GET$SIZE�P("TOKEN"@)
�DQ$GET$SYSTEM$ID�P(@@)
�DQ$GET$TIME�P(@@)
�DQ$OPEN�P(WBB@)
�DQ$OVERLAY�P(@@)
�DQ$READ�P("CONNECTION"@W@)W
�DQ$RENAME�P(@@@)
�DQ$SEEK�P("CONNECTION"BWW@)
�DQ$SPECIAL�P(B@@)
�DQ$SWITCH$BUFFER�P(@@)W
�DQ$TRAP$CC�P(@@)
�DQ$TRAP$EXCEPTION�P(@@)
�DQ$TRUNCATE�P(W@)
�DQ$WRITE�P("CONNECTION"@W@)







�page�52�ISIS-II�SOFTWARE�TOOLBOX,�V2.0




�HOW�TO�CREATE�A�PEXFILE�(TEMPLATE�FILE)

�The�pexfiles�contain�symbol�definition�information�for
�all�possible�declarations�in�a�system.�Therefore,�the�user
�must�categorize�the�public�definitions�by�source�file.�One
�way�to�do�this�is�to�have�a�separate�pexfile�for�each
�source�module,�with�each�pexfile�containing�the�pex
�definitions�of�the�publics�in�that�module.�Alternately,
�symbols�from�multiple�source�modules�can�be�combined�into
�one�pexfile.�The�pexfile�contains�file�definitions,�and
�then�the�public�symbol�declarations�for�that�file.�This
�eliminates�having�both�PUBLIC�and�EXTERNAL�declarations�for
�the�same�symbol�in�the�same�source�file.�A�pexfile�may�also
�contain�comments.


�COMMENTS

�A�comment�line�is�any�line�that�does�not�begin�with�a
�letter�or�a�dollar�sign�($).

�FILE�DEFINITIONS

�File�definitions�in�a�pexfile�have�the�syntax:

�'$'�'FILE'�'('�<filename>�')'

�For�example:

�$�file�(absio.src)
�$�FILE�(FOO.PLM)


�SYMBOL�DEFINITIONS:

�1.�LITERALLYs�--�A�literally�declaration�takes�the�form

�<symbol>�<literal_expansion>

�where�the�literal_expansion�is�any�string�enclosed�in
�single�quotes.�It�is�clear,�then�that�the�declaration
�in�a�pexfile�is�the�same�as�in�the�PL/M�source,�without
�the�keywords�DECLARE�and�LITERALLY.


�2.�SIMPLE�VARIABLES�--�A�simple�variable�is�is�the�same�as
�a�PL/M�variable�declaration�without�the�keyword
�DECLARE,�so�it�takes�the�form

�<symbol>�<type>

�where�<type>�is�one�of�the�following:








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�53



�A�for�address
�B�for�byte
�W�for�word
�D�for�dword
�S�for�selector
�I�for�integer
�R�for�real
�@�for�pointer
�L�for�label
�any�string�in�double-quotes

�a�double-quoted�string�is�a�user-defined-type,�and�must
�be�defined�in�a�literally�for�successful�compilation.

�3.�BASED�VARIABLES�--�A�based�variable�definition�takes
�the�form

�<symbol>�<type>�'..'�<base>

�where�<base>�is�the�variable�name�of�the�variable�that
�<symbol>�is�based�on.�For�example,

�fieldptr�@
�field�B..fieldptr

�would�expand�to

�declare�FIELDPTR�pointer�external;
�declare�FIELD�BASED�FIELD�POINTER�byte;

�in�a�typical�Genpex�execution.

�4.�ARRAYS�--�An�array�declaration�is�the�same�as�a�simple
�variable,�followed�by�the�letter�'S'�(not�in�quotes).

�array�"token"S
�BIGARRAY�BS

�The�'S'�stands�for�"subscripted"�and�is�expanded�to�a
�'1'�in�the�actual�external�declaration.

�5.�STRUCTURES�--�Structures�in�PL/M�must�be�declared�as
�literallys�when�using�GENPEX.�Since�literallys�may�be
�nested�(only�once),�structures�longer�than�120
�characters�may�be�accomplished�similar�to�this�example:

�linked_list�'STRUCTURE�(info_part,�link_part)'
�info_part�'name�(40)�BYTE,�phone�(9)�BYTE,�birth�WORD'
�link_part�'last_ptr�POINTER,�next_ptr�POINTER'

�Incidentally,�Genpex�will�generate�external�declarations
�for�symbols�used�in�other�Genpex�declarations�(in�this
�case,�info_part�and�link_part),�even�if�they�are�not
�used�anywhere�else�in�the�program.







�page�54�ISIS-II�SOFTWARE�TOOLBOX,�V2.0



�6.�PROCEDURES�--�Procedures�take�the�form

�<symbol>�'P'�'('�<type>...�')'�<type>

�where�the�type�definition(s)�in�the�parentheses
�correspond�to�procedure�parameters.�The�parenthetical
�clause�may�be�left�out�if�the�procedure�has�no
�parameters.�The�<type>�after�the�parameter�clause�is
�for�a�typed�procedure;�it,�too,�is�optional.�As�in�the
�case�of�variables,�any�user-defined�type�(double-quoted
�string)�may�be�used.�Some�more�examples:

�sort�P(@@)
�dq$allocate�P(W@)"token"
�OUTBUFFER�P("BOOLEAN"@BBBW)

�In�the�last�example,�the�expansion�in�the�Include�file
�would�be�:

�OUTBUFFER:�procedure(ZZ1,ZZ2,ZZ3,ZZ4,ZZ5,ZZ6)�external;
�declare�(ZZ3,ZZ4,ZZ5)�byte,�(ZZ6)�word,�(ZZ2)�pointer,
�ZZ1�BOOLEAN;�end;

�Note:�a�'1'�or�a�'2'�may�be�used�in�place�of�<type>
�for�PLM80�users.�Genpex�will�expand�this�to�declare�a
�procedure�of�type�Byte�or�Address.

�Genpex�will�generate�dummy�parameters�and�variable
�declarations�of�the�form�'ZZn'�(n�is�an�integer),
�according�to�the�parenthesized�types�in�the�parameter
�clause.

�Genpex�will�attempt�to�create�external�declaration�lines
�no�longer�than�80�characters.�Subsequent�lines�created
�for�a�single�declaration�will�be�indented�two�spaces.
�Genpex�will�also�attempt�to�indent�all�lines�according
�to�the�last�seen�leftmargin�specification�in�the�source.
























�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�55





�FORMAL�SYNTACTIC�SPECIFICATION�OF�PEXFILE�FORMAT

�Data�in�the�pexfile�has�the�following�syntax:


�<pexline>�::=�(<comment_line>
�|�<file_line>
�|�<data_line>)
�CRLF.


�<file_line>�::=�'$'�'file'�'('�<filename>�')'.


�<comment_line>�::=�/*�any�line�not�beginning�with�a
�letter�or�a�'$'�*/.


�<data_line>�::=�<valid_PLM_symbol>�(<proc>
�|<var>
�|�<literally>).


�<proc>�::=�'P'�[�'('�<type>+�')'�]
�|�['1'
�|�'2']�[<type>].


�<literally>�::=�<single-quoted-string>


�<var>�::=�<type>
�['S'�/*�an�array�*/]
�['..'�<valid_PLM_symbol>�].
�/*�based�*/


�<type>�::=�(�'A'�/*�address�*/
�|�'B'�/*�byte�*/
�|�'I'�/*�integer�*/
�|�'R'�/*�real�*/
�|�'W'�/*�word�*/
�|�'D'�/*�dword�*/
�|�'S'�/*�selector�*/
�|�'L'�/*�label�*/
�|�'@'�/*�pointer�*/
�|�'"'�<valid_PLM_symbol>�'"'�)
�/*�user�defined�<type>�*/.











�page�56�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�HDBACK�|
�|�v2.3�|
�|�|
�+==================================+


�SUMMARY

�The�Hdback�program�was�written�to�simplify�the�process
�of�backing�up�hard-disk�platters�to�floppy�disks.


�AVOIDS�OVER-FILLING�DISKETTES

�The�program�avoids�ERROR�7's�(DISK�FULL)�and�ERROR�9's
�(DIRECTORY�FULL)�if�NEWLY�FORMATTED�NON-SYSTEM�disks�are
�used�as�the�targets�for�the�copies,�or�if�the�DELETE�switch
�is�used�to�purge�all�of�the�target�disk�files.


�ALLOWS�GROUPING�OF�FILES�BY�ATTRIBUTE

�This�program�also�allows�further�discrimination�of�the
�files�to�be�copied�by�allowing�the�file�selection�process
�to�include�the�attributes�of�the�file.�It�also�allows�the
�attributes�of�the�source�files�to�be�modified�AFTER�the
�copy�is�performed.�All�files�with�the�FORMAT�attribute�are
�ignored.�It�also�allows�the�attributes�of�the�target�files
�to�be�set�the�same�as�the�source�files,�if�the�"C"�switch�is
�used.



�DEFAULT�FLOPPY�DRIVE

�The�default�target�device�is�:F5:.�This�may�be�altered
�by�using�the�"TO"�device_list�option.


�============================================================

�The�actions�of�HDBACK�will�be�described�through�the
�following

�EXAMPLES:

�HDBACK�:F0:

�will�copy�all�files�from�:F0:�to�:F5:�with�the
�appropriate�breaks�to�allow�for�disk�changes.

�------------------------------------------------------------








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�57




�------------------------------------------------------------


�HDBACK�:F0:�w1�i0

�will�copy�all�files�on�:F0:�to�:F5:�which�have�the�W
�(write�protect)�attribute�set�and�the�I�(invisible)
�attribute�reset.

�------------------------------------------------------------

�HDBACK�:F0:*.SRC�w0�=�1

�will�copy�all�of�the�.SRC�files�which�have�the�W
�attribute�reset�to�:F5:.�After�the�copy�is�completed,�the
�source�file's�W�attribute�will�be�set.

�------------------------------------------------------------

�HDBACK�:F0:*.*�to�4,5

�will�copy�all�of�the�files�on�:F0:�to�:F4:�and�when
�that�drive�is�full,�to�:F5:.�The�pause�to�change�disks
�will�occur�after�:F5:�is�filled.

�------------------------------------------------------------

�HDBACK�:F0:*.PLM�w*�=�1�to�4567

�will�copy�all�.PLM�files�from�:F0:�to�drives�4,5,6�and
�7.�Note�that�the�W�attribute�is�ignored�during�the�copy
�selection�because�of�the�use�of�the�wildcard.�After�each
�copy,�the�W�attribute�of�the�source�file�will�be�set.

�------------------------------------------------------------

�HDBACK�:F0:*.*�to�4,5�C�DELETE

�will�copy�all�of�the�files�on�:F0:�to�:F4:�and�when
�that�drive�is�full,�to�:F5:.�The�pause�to�change�disks
�will�occur�after�:F5:�is�filled.�HDBACK�will�delete�all
�files�on�the�drive�4�and�5�diskettes,�except�those�with�the
�format�attribute�set,�before�copying.�The�copied�files�will
�get�the�same�attributes�as�the�drive�0�source�files.

�============================================================


�Specifying�only�the�device,�:F#:,�is�equivalent�to
�typing�:F#:*.*










�page�58�ISIS-II�SOFTWARE�TOOLBOX,�V2.0



�When�only�a�single�target�device�is�used,�(e.g.�the
�default�:F5:),�HDBACK�will�open�a�file�named�TMP.TMP�on�the
�source�drive�for�write�mode.�This�is�necessary�since�ISIS
�does�not�contain�a�DISMOUNT�command.�TMP.TMP�will�be
�deleted�immediately�after�it�is�created.

�If�the�DELETE�switch�is�used,�all�files�without�the
�format�attribute�on�the�target�disk�will�be�deleted�BEFORE
�the�backup.



�SPECIFIES�NUMBER�OF�DISKETTES�NEEDED

�When�HDBACK�is�invoked,�it�will�search�the�source
�device's�directory�to�determine�the�number�of�disks�that
�will�be�needed.�This�number�is�then�printed�to�the
�screen.�HDBACK�will�then�pause�to�allow�the�loading�of�the
�first�set�of�target�disks.

�Typing�an�<ESC>�while�HDBACK�is�waiting,�will�cause
�HDBACK�to�return�to�ISIS.�Typing�<CR>�will�allow�HDBACK�to
�continue�its�copying.�If�the�DELETE�switch�is�used,�all
�files�on�the�target�drive�will�be�deleted�before�the
�copying�continues.

�As�HDBACK�is�copying�the�files,�the�filename�of�the
�current�source�file�is�listed�on�the�screen.�This�will�be
�followed�by�a�series�of�one�or�more�dots,�with�each�dot
�representing�4096�characters�transferred�(except�the�last
�one�which�is�between�1�and�4096,�inclusive).�After�this,
�if�the�attributes�are�to�be�set�or�reset,�the�new�attribute
�value�will�be�displayed.�After�this,�if�the�attributes�are
�to�be�retained�on�the�target�file,�the�set�attribute�is
�displayed.�Only�the�"W","S",�and�"I"�attributes�are�copied.



�BACKING�UP�FROM�REMOTE�DIRECTORIES

�If�the�source�files�reside�in�a�remote�directory,
�HDBACK�will�perform�with�the�following�differences:

�The�attribute�settings�of�the�files�are�ignored.�Files
�will�be�backed�up�or�not�only�according�to�file�name.

�Directory�files�will�not�be�copied.�The�message
�"REMOTE�DIRECTORY,�NOT�COPIED"�will�follow�the�filename�in
�the�display.

�Files�without�read�access�will�not�be�copied.�The
�message�"INSUFFICIENT�ACCESS�RIGHTS,�NOT�COPIED"�will
�follow�the�filename�in�the�display.








�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�59


�Subdirectories�will�not�be�backed�up.�HDBACK�can�only
�back�up�a�single�level�of�directory�at�a�time.

�The�calculation�of�the�number�of�target�diskettes�needed
�may�be�slower.�The�message�"REMOTE�SOURCE"�will�be
�displayed,�followed�be�dots,�one�for�every�10�files,�during
�the�remote�directory�search.



�SYNTAX:

�[:F#:]�HDBACK�filename�[{attr_spec}]
�[�"TO"�device_list�]�[C]�[DELETE]

�where:

�filename�=�device�|
�=�device�file�|
�=�file.�|

�device�=�":F"�digit�":".

�file�=�name�["."�ext].

�name�=�{(digit�|�alpha�|�wildcard)}.�/*�<=�6�chars�*/

�ext�=�{(digit�|�alpha�|�wild�card)}.�/*�<=�3�chars�*/

�attr_spec�=�"W"�("0"|"1"|"*"|"?")�[�"="�("0"|"1")]
�=�"S"�("0"|"1"|"*"|"?")�[�"="�("0"|"1")]
�=�"I"�("0"|"1"|"*"|"?")�[�"="�("0"|"1")].
�/*�each�attribute�may�appear�only�once�*/

�device_list�=�digit�[�[","]�digit�].�/*�<=�4�digits�*/

�digit�=�"0"�..�"9".

�alpha�=�"A"�..�"Z"�|�"a"�..�"z".

�wildcard�=�"*"�|�"?".



�KNOWN�PROBLEMS

�Hdback�may�not�work�correctly�on�a�disk�with�a
�directory�that�is,�or�ever�has�been,�totally�full.














�page�60�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�LAST�|
�|�v1.1�|
�|�|
�+==================================+



�SUMMARY

�LAST�outputs�to�the�console�the�last�512
�characters�of�a�file.�It�is�the�quickest�way�to�view
�the�end�of�a�large�file.


�SAMPLE�INVOCATION

�last�:f1:prog.lst


�SAMPLE�USE

�Ascertain�whether�there�were�errors�in�a�given
�compilation�by�inspecting�the�module�summary�at�the
�end�of�the�program�listing.



�SYNTAX�SPECIFICATION

�LAST�<filename>






























�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�61


�+==================================+
�|�|
�|�LOWER�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�This�program�converts�a�file�to�lower�case�ascii.


�SAMPLE�INVOCATION

�lower�:f1:upcase.txt�:f1:lccase.txt


�SYNTAX

�LOWER�<file1>�<file2>

�wherein

�<file1>�is�the�source�file,�and
�<file2>�is�<file1>�translated�to�lower�case.






�+==================================+
�|�|
�|�UPPER�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�UPPER�converts�all�lower�case�letters�in�a�file
�to�upper�case.


�SAMPLE�INVOCATION

�UPPER�:f1:low.txt

�SYNTAX

�UPPER�<filename>









�page�62�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�MERG80�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�MERG80�is�a�program�which,�given�a�PLM80�listing�and�a
�locate�map,�merges�the�line�numbers�and�symbol�names�from
�the�map�into�the�listing.

�With�MERG80,�it�is�easier�to�ascertain�where
�breakpoints�should�be�set�in�a�program�being�debugged.�The
�listing�will�contain�absolute�locations�for�both�PL/M
�statements,�and�symbols.


�SAMPLE�INVOCATION

�MERG80�:F1:PROG.MP2�(PROGMOD)�INTO�:F1:PROG.LST

�where�PROG.MP2�is�the�locate�map�(which�must
�contain�"lines"�and�"symbol"�information�in�single
�column�format);

�PROGMOD�is�the�module�name;�and

�PROG.LST�is�the�file�containing�the�program�listing.



�INTENDED�USE�ENVIRONMENT

�MERG80�was�designed�for�use�in�program�debugging,�in
�conjunction�with�the�ISIS-II�monitor.


�LIMITATIONS

�The�line�and�symbol�information�in�the�locate�map�must
�be�in�a�single�column.


�MULTI-MODULE�PROGRAMS

�Observe�that,�when�developing�multi-module�programs,
�the�user�will�invoke�MERG80�once�for�every�module�in�the
�program;�there�is�no�way�to�merge�all�the�listings�with
�one�command.










�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�63




�OUTPUT�FORMAT


�"Line-number"�data�(absolute�memory�addresses�which
�mark�the�starting�points�of�the�machine�instructions
�generated�for�the�particular�lines�of�high-level�source)
�go�right�on�the�lines�which�they�correspond�to,
�overwriting�the�block�depth�numbers�in�PL/M�listings.

�Symbol�values�are�presented�in�an�alphabetized�table
�tacked�on�to�the�end�of�the�listing.




�FORMAL�SYNTAX�SPECIFICATION


�MERG80�<loc_list_name>�"("�<module_name>�")"
�INTO�<plm_list_name>�[N]�[L]


�where�<loc_list_name>�is�the�name�of�the�locate
�listing;�<module_name>�is�the�name�of�the�module,�or�enough
�of�the�first�part�of�the�name�of�the�module�to�distinguish
�the�module;�and�<plm_list_name>�is�the�name�of�the�PL/M
�listing.


�OPTIONS

�There�are�two�optional�flags�that�may�be�included�at
�the�end�of�the�command�invocation.

�N�--which�suppresses�sorting�of�the�symbols

�L�--causes�the�program�to�emit�a�linefeed�between
�the�PL/M�listing�and�the�symbol�table;�normally�a
�formfeed�is�emitted.�This�is�used�when�one�has�a
�very�short�listing�and�wants�everything�on�one�page.




















�page�64�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�MERG86�|
�|�v1.7�|
�|�|
�+==================================+


�SUMMARY

�MERG86�is�a�program�which,�given�a�PLM86�or�Pasc86
�listing�and�a�link86�or�loc86�map�with�symbol�information,
�merges�the�line�numbers�and�symbol�names�from�the�map�into
�the�listing.

�With�Merg86,�it�is�easier�to�ascertain�where
�breakpoints�should�be�set�in�a�program�being�debugged.�The
�listing�will�then�contain�absolute�locations�for�both�PL/M
�statements,�and�symbols.



�SAMPLE�INVOCATION

�MERG86�:F1:PROG.MP2�(PROGMOD)�INTO�:F1:PROG.LST

�where�PROG.MP2�is�the�locate�map�(which�must�contain
�"lines"�and�"symbol"�information�in�single�column�format);

�PROGMOD�is�the�module�name;�and

�PROG.LST�is�the�file�containing�the�program�listing.



�INTENDED�USE�ENVIRONMENT

�Merg86�was�designed�for�use�in�program�debugging,�in
�conjunction�with�a�monitor�or�software�debugger�(e.g.,
�Deb86)�which�accepts�absolute�addresses�as�program
�breakpoints�or�symbol�locations.�Merg86�is�particularly
�useful�when�a�given�software�debugger�cannot�accept�the
�volume�of�symbol�information�which�may�be�present�in�a
�large�8086�load�module.


�LIMITATIONS

�The�line�and�symbol�information�in�the�symbol�map�must
�be�in�a�single�column.












�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�65



�MERGING�MULTI-MODULE�PROGRAMS

�Observe�that,�when�developing�multi-module�programs,
�the�user�will�invoke�Merg86�once�for�every�module�in�the
�program.�There�is�no�way�to�process�all�the�listings�with
�one�merg86�command.



�OUTPUT�FORMAT

�"Line-number"�data�(absolute�memory�addresses�or
�offsets�which�mark�the�starting�point�of�the�machine
�instructions�generated�for�the�particular�lines�of�high-
�level�source)�go�right�on�the�lines�which�they�correspond
�to,�overwriting�the�PLM86�block�depth�numbers,�or�the
�Pasc86�line�numbers.

�Symbol�addresses�are�presented�in�an�alphabetized�table
�appended�to�the�end�of�the�listing.



�FORMAL�SYNTACTIC�SPECIFICATION


�MERG86�<map_filename>�"("�<module_name>�")"
�INTO�<listing_name>�[N]�[L]


�where�<map_filename>�is�the�name�of�the�symbol
�listing;�<module_name>�is�the�name�of�the�module,�or�enough
�of�the�first�part�of�the�name�of�the�module�to�distinguish
�the�module;�and�<listing_name>�is�the�name�of�the�program
�listing.


�COMMAND-TAIL�OPTIONS

�There�are�two�optional�flags�that�may�be�included�at
�the�end�of�the�command�invocation.

�N�--which�suppresses�sorting�of�the�symbols

�L�--causes�the�program�to�emit�a�line-feed�between�the
�program�listing�and�the�symbol�table.�normally�a
�formfeed�is�emitted.�This�is�used�when�one�has�a
�very�short�listing,�and�wants�everything�on�one�page.













�page�66�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�PACK�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�This�utility�accepts�an�ASCII�text�file�as�input,�and
�produces�a�file�in�which�strings�of�blank�characters�have
�been�compressed�into�"special"�single�byte�quantities.
�Programs�which�deal�with�these�compressed�files�(such�as
�UNPACK)�can�distinguish�these�special�bytes,�because�their
�high-order�bit�is�"on",�which�is�not�the�case�with�any�other
�Ascii�character.

�Another�program,�called�UNPACK,�can�reverse�the�process
�and,�thereby�reconsitute�a�packed�file.



�USE�ENVIRONMENT

�Pack's�purpose�is�to�squeeze�compiler�program�listings,
�and�other�"blank-intensive"�files,�down�to�a�minimal�form,
�so�that�as�many�as�possible�can�be�put�on�a�given�disk.



�SAMPLE�INVOCATIONS

�pack�:f1:*.lst
�pack�:f1:*.src�to�:f1:*.prc
�pack�:f1:*.mp?�nodelete



�SYNTAX

�:F#:PACK�<from_filename>�[�TO�<to_filename>�]
�[�DELETE�|�NODELETE�]


�<from_filename>�may�be�any�standard�ISIS-II
�pathname�and�may�contain�the�wildcard�characters�"*"
�and�"?".�There�is�no�default�for�from_filename.














�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�67




�<to_filename>�may�be�any�standard�ISIS-II
�pathname,�may�contain�wildcard�characters�or�may�be
�omitted.�If�<to_filename>�is�omitted,�the�default�of
�:Fn:source_file.PCK�is�assumed.�Also�note�that�if
�<to_filename>�is�a�single�filename�(i.e.�no�wildcard
�characters),�then�the�outputs�will�be�concatenated
�into�the�single�file.

�the�DELETE/NODELETE�switch�is�used�to�determine�if
�the�source�file�is�to�be�deleted�after�the�output�file
�is�generated.�The�default�is�DELETE.



�SEE�ALSO

�unpack.doc


�**�CAUTION�**

�Pack�opens�output�files�before�opening�input�files,�so,
�if�one�tries�to�explicitly�pack�from�a�non-existent�file,
�to�a�file�which�already�exists,�the�existing�file�will�be
�destroyed�(overwritten�with�nothing),�before�it�is
�determined�that�the�input�file�does�not�exist.

�This�is�most�likely�to�occur�when�one�of�the�tools�has
�been�invoked�with�wildcard,�and�the�inverse�tool�is�invoked
�on�specific�files.

�This�situation�can�always�be�avoided�by�confirming�the
�existence�of�the�file�to�be�operated�upon,�with�the
�conditional�submit�tools,�before�PACKing�or�UNPACKing.


























�page�68�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�UNPACK�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�This�utility�accepts�a�file�produced�by�the�PACK
�utility�and�expands�the�special�characters�into�strings�of
�ASCII�blank�characters.

�Blanks�are�reconstituted�by�converting�bytes�with�the
�high�bit�on�into�a�string�of�blanks�whose�length�is�the
�binary�number�represented�by�the�lower�seven�bits�of�the
�byte.�All�other�characters�are�passed�through�without
�alteration.



�SYNTAX


�:F#:UNPACK�<from_file_name>�[�TO�<to_file_name>�]
�[�DELETE�|�NODELETE�]

�<from_file_name>�may�be�any�standard�ISIS-II
�pathname�and�may�contain�the�wildcard�characters�"*"
�and�"?".�There�is�no�default�for�<from_file_name>.

�<to_file_name>�may�be�any�standard�ISIS-II
�pathname,�may�contain�wildcard�characters�or�may�be
�omitted.�If�<to_file_name>�is�omitted,�the�default
�:Fn:source_file.PCK�is�assumed.�Also�note�that�if
�<to_file_name>�is�a�single�filename�(i.e.�no�wildcard
�characters)�the�outputs�will�be�concatenated�into�the
�single�file.

�If�the�spool�queue�is�used�as�the�<to_file>,�then
�a�filename�must�be�used,�with�or�without�wildcard.
�That�is,�the�spool�file�must�be�specified�as�a�single
�file�(e.g.�TO�:SP:FILUNP)�or�as�individual�files
�(e.g.�TO�:SP:*.UNP).�Specifying�:SP:�without�a
�filename�causes�an�abort.

�the�DELETE/NODELETE�switch�is�used�to�determine�if
�the�source�file�is�to�be�deleted�after�the�output�file
�is�generated.�The�default�is�DELETE.


�SEE�ALSO

�pack.doc







�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�69




�**�CAUTION�**

�Unpack�opens�output�files�before�opening�input�files,
�so,�if�one�tries�to�explicitly�unpack�from�a�non-existent
�file,�to�a�file�which�already�exists,�the�existing�file
�will�be�destroyed�(overwritten�with�nothing),�before�it�is
�determined�that�the�input�file�does�not�exist.

�This�is�most�likely�to�occur�when�one�of�the�tools�has
�been�invoked�with�wildcard,�and�the�inverse�tool�is�invoked
�on�specific�files.

�This�situation�can�always�be�avoided�by�confirming�the
�existence�of�the�file�to�be�operated�upon,�with�the
�conditional�submit�tools,�before�PACKing�or�UNPACKing.













































�page�70�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�PASSIF�|
�|�v1.2�|
�|�|
�+==================================+



�I.�INTRODUCTION

�A.�PURPOSE

�Passif�was�developed�to�automate�the�software�testing
�process.

�Such�testing�involves�(among�other�things)�searching
�files�for�specific�strings,�or�the�lack�thereof,�checking
�whether�two�files�(usually�object�modules)�are�identical,
�and�checking�whether�specified�files�exist.

�In�the�past,�performing�and�checking�this�style�of�test
�required�the�use�of�many�different�tools�and�utilities,�and
�the�examination�and�processing�of�huge�disk�files.

�Passif�obviates�this�by�directly�performing�the�test-
�checking�functions�needed,�and�reporting�the�results�in�a
�processed�and�compact�form.

�B.�PHILOSOPHY

�It�is�intended�that�Passif�be�able�to�adequately
�process�the�great�majority�of�test�cases�encountered�in�a
�normal�test�suite.�It�is�not�intended�to�handle�all
�conceivable�requirements�of�this�style�of�software�testing.

�It�is�assumed�that�Passif�will�typically�be�used�to
�quality�assure�a�large�test�base,�run�as�a�night�job.
�Passif,�therefore,�is�not�a�performance�crucial�program�but
�its�speed�should�be�adequate�for�occasional�interactive
�use.

�Under�exceptional�conditions,�Passif�will�attempt�to
�continue�processing,�if�there�is�reasonable�method�of�doing
�so.

�The�design�of�Passif�emphasizes�simplicity�and
�maintainability.

�C.�EXECUTION�ENVIRONMENT

�Passif�runs�under�the�ISIS-II�operating�system,�on�an
�MDS-800�or�Series�II.�Passif�should�also�run�under�any
�later�version�of�ISIS,�on�any�ISIS�supported�hardware
�environment.







�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�71




�II.�SPECIFICATION

�A.�GENERAL�METHOD�OF�OPERATION

�1.�Overview

�Passif�ascertains�what�assertion�to�check�by�reading
�its�command�tail.�Passif�then�performs�whatever�functions
�are�necessary�to�check�the�verity�of�the�assertion.

�The�results�of�the�assertion�checking�are�then�reported
�in�a�disk�file�called�":f1:report.log".�This�file�has�a
�"banner"�at�the�top�with�three�decimal�ascii�numbers,
�representing�tests�executed,�tests�passed,�and�tests
�failed,�respectively.�The�numbers�are�updated�by�Passif
�during�each�of�its�invocations.

�The�report�file�is�always�opened�in�update�mode.�If
�the�assertion(s)�are�true,�then�Passif�merely�increments
�the�"TESTS�PASSED"�number�and�the�"TESTS�EXECUTED"�number.

�If�the�assertion�is�false,�then�the�"TESTS�EXECUTED"
�and�"TESTS�FAILED"�numbers�are�incremented,�and�a�"failed
�assertion�entry"�for�the�test�which�failed,�is�appended�to
�the�end�of�the�report�file.�A�sample�of�a�typical�report
�file,�and�a�detailed�description�of�what�type�of
�information�appears�in�a�"failed�assertion�entry"�appears
�below.

�Each�time�it�is�invoked,�Passif�dumps�some�test�status
�information�to�the�cold�boot�console.�As�soon�as�it�comes
�up,�it�prints�a�carriage-return�(but�no�line-feed).�When
�Passif�is�finished�processing,�it�prints�out�the�new�banner
�it�has�generated,�but�leaves�off�the�carriage-return�line-
�feed�at�the�end.

�Thus,�the�user�sees�a�periodically�updated�display�of
�test�status�information�on�his�console.�He�knows,�in
�real�time,�when�Passif�is�running�(cursor�at�the�beginning
�of�the�line),�and�when�the�rest�of�the�test�suite�is
�running�(cursor�at�the�end�of�the�line).�The�display�of
�this�information�only�costs�one�line�of�screen,�which�is
�painted�over�and�over�again,�so�the�user�won't�lose�status
�information�which�any�other�of�his�programs�may�have�dumped
�to�the�console.















�page�72�ISIS-II�SOFTWARE�TOOLBOX,�V2.0



�EXAMPLE�OF�A�TYPICAL�REPORT�FILE

�==============================================================

�13517�TESTS�EXECUTED�13499�TESTS�PASSED�00018�TESTS�FAILED

�--------------------------------------------------------------

�test�00533�failed�:CI:=:F3:FOOBAR.BAS
�assertion:�string_found("ERROR�28")�in�file(:f1:loc86.mp2)

�--------------------------------------------------------------

�test�00534�failed�:CI:=:F3:FOOBAR.BAS
�assertion:�string_found("ERROR�29")�in�file(:f1:loc86.mp2)

�--------------------------------------------------------------

�test�00714�failed�:CI:=:F3:FOOBAR.BAS
�syntax�error�in�command�tail:
�"FILE_EXISTS(FOOOBAR.CS)#"

�--------------------------------------------------------------

�test�00998�failed�:CI:=:F0:T28.CS
�assertion:�string_found("ERROR�28")�in�file(:f1:loc86.mp2)

�--------------------------------------------------------------

�test�00999�failed�:CI:=:F0:T28.CS
�assertion:�string_found("ERROR�28")�in�file(:f1:loc86.mp2)
�isis�error�13

�--------------------------------------------------------------
�==============================================================



�2.�Test�Failure�Reporting

�When�a�test�fails,�Passif�appends�an�entry�to�the
�report�file�specifying:

�a.�the�name�of�the�console�input�file
�b.�the�ordinal�number�of�the�test
�c.�the�assertion�which�caused�the�failure


�3.�Test�Result�Report�File

�The�results�of�the�tests�performed�by�Passif�will�be
�reported�in�file�":F1:REPORT.LOG".









�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�73


�The�use�of�a�"hardwired"�output�file�contributes�to�the
�parallelism�and�understandability�of�testing�procedures
�between�users�of�Passif.

�Other�methods�of�specifying�the�report�file�are�less
�desirable.�If�the�report�file�is�specified�in�a
�"macrofile"�which�is�read�by�Passif�upon�each�of�its
�invocations,�a�substantial�performance�penalty�is�incurred,
�especially�with�a�hard�disk�system.

�If�the�report�file�is�specified�in�Passif's�command
�tail�with�a�control�or�switch,�then�the�command�tail
�becomes�cluttered,�and�loses�its�"readable-English"�flavor.

�The�user�may�change�the�report�file�specification�in
�Passif,�if�absolutely�necessary,�by�editing�the�load
�module,�since�the�report�file�is�specified�therein�by�the
�string�"REPORT_FILE=:F1:REPORT.LOG".�The�difficulty�of
�changing�the�report�file�specification�is�deliberate.


�4.�Exceptional�Conditions

�a.�Unreadable�Report�File

�Since�Passif�must�read�the�report�file�banner�to
�"orient"�itself�(ascertain�the�current�ordinal�test
�number),�an�unreadable�banner�constitutes�an�exceptional
�condition.

�In�such�cases�Passif�will�"re-initialize"�the�report
�file�by�inserting�a�new�banner�at�the�beginning�of�the
�report�file.�Whatever�was�already�in�the�report�file�will,
�therefore,�not�be�destroyed.

�The�values�of�"TESTS�PASSED",�"TESTS�FAILED",�and
�"TOTAL�TESTS�EXECUTED"�are�all�"initialized"�to�zero�in�the
�new�banner.



�b.�Syntax�Error�in�Command�Tail

�If�there�is�a�syntax�error�in�Passif's�command�tail,
�then�a�"test�failed"�entry�is�made�in�the�report�file.�The
�string�"syntax�error�in�command�tail:"�appears�in�the
�report�file,�followed�by�the�offending�command�tail,�up�to
�the�point�where�the�error�was�detected,�followed�by�a�hash-
�mark.�This�is�the�standard�command-tail�error�reporting
�mechanism�used�by�most�Intel�products.












�page�74�ISIS-II�SOFTWARE�TOOLBOX,�V2.0




�B.�EXAMPLES�OF�LEGAL�COMMANDS�WITH�EXPLANATIONS

�1.�File�Exists

�Example:

�PASSIF�FILE_EXISTS(:F1:T23.MP2)

�Action:

�Passif�checks�to�see�whether�file�":f1:t23.mp2"�exists.
�If�the�file�does�not�exist,�or�the�file�exists,�but�has
�zero�length,�then�a�"failed�assertion"�response�is
�triggered;�else,�the�test�passes.�A�file�of�zero�length�is
�considered�to�be�a�failure�because�the�existence�of�such�a
�file�is�almost�always�an�indication�of�an�exceptional
�condition.�However,�if�the�file�is�a�spool�queue�file,�e.g.
�:SP:T45.LST,�the�length�of�the�file�is�not�considered.

�2.�File�Absent

�Example:

�PASSIF�FILE_ABSENT(:F0:ERRORS)

�Action:

�Passif�checks�that�file�":f0:errors"�does�not�exist.
�Detection�of�a�file�of�zero�length,�or�greater,�triggers�a
�"failed�assertion"�response.

�3.�Files�Match

�Example:

�PASSIF�FILES_MATCH(:F1:T31A.OBJ,:F6:T31A.OUT)

�Passif�checks�whether�the�specified�files�are
�identical.

�4.�String�Found

�Example:

�PASSIF�STRING_FOUND("***�ERROR�#217")�IN�FILE�(:F1:P11T01.LST)

�PASSIF�STRING_FOUND�&
�("this�is�quite�a�long�string�which�contains�a�""")�&
�IN�FILE�(:F1:P11T01.LST)

�Passif�checks�whether�the�specified�string�appears�in
�the�specified�file.�Notice�that�ampersand�continuation�is
�allowed�between�tokens,�as�per�usual.







�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�75



�5.�String�Absent

�Example:

�PASSIF�STRING_ABSENT("ERROR")�IN�FILE(:F1:T22A.MP1)

�Passif�checks�whether�the�specified�string�is�absent
�from�the�specified�file.


�C.�ADDITIONAL�SEMANTICS�FOR�COMMAND�TAIL�SPECIFICATION


�1.�Ampersand�Continuation

�Ampersand�continuation�is�allowed�between�any�command
�tail�tokens;�that�is,�anywhere�except�embedded�within
�string�specifications�or�keywords.


�2.�Keyword�Abbreviations

�The�keywords�may�be�abbreviated�as�follows:

�FILE_EXISTS�==>�FE
�FILE_ABSENT�==>�FA
�FILES_MATCH�==>�FM
�STRING_FOUND�==>�SF
�STRING_ABSENT�==>�SA


�3.�Multiple�Commands

�Multiple�commands�are�not�allowed�in�a�single
�invocation�of�Passif.�The�only�multiple�entities�(lists)
�allowed�in�a�single�command�are�the�two�filenames�required
�in�the�"files_match"�command.


�4.�Null�Command�Tail

�If�the�command�tail�to�Passif�consists�only�of�blanks
�and�tabs�then�Passif�signs�on�and�exits.


















�page�76�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�RELAB�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�Relab�allows�the�user�to�change�a�disk�name.

�RELAB�WILL�NOT�WORK�WITH�REMOTE�DISKS.


�SAMPLE�INVOCATION

�relab�:f7:newsrc.dsk

�The�syntax�is�like�the�syntax�for�the�format�program,
�but�only�the�disk�label�is�affected.


�USE�ENVIRONMENT

�Relab�works�on�both�floppy�and�hard�disks.


�SYNTAX

�RELAB�[<device>]<name>

�where

�<device>�::=�:F<n>:
�<n>�::=�0�..�9
�<name>�::=�<standard_isis_filename>

























�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�77


�+==================================+
�|�|
�|�SORT�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�The�Sort�program�allows�the�user�to�sort�a�file�of
�ascii�text,�line-by-line,�according�to�specified�fields�or
�columns�of�characters.


�SAMPLE�USE

�The�user�plans�to�use�a�file-manipulation�program,�with
�a�limited�capacity,�on�his�source�files,�and�wishes�to
�determine�the�size�of�the�largest�one.�He�therefore�sends
�a�directory�of�the�disk�containing�the�source�files,�to�a
�disk�file,�and�sorts�it.

�+------------------------------------------------------+
�|�|
�|�>dir�1io�for�*.src�to�tmp�|
�|�>sort�tmp�to�tmp.srt�17,25�|
�|�|
�+------------------------------------------------------+



�LIMITATIONS

�The�Sort�program�is�limited�to�"in-memory"�sorting,
�only.�A�very�large�input�file�will�cause�Sort�to�abort.




�SYNTAX

�INVOCATION

�SORT�file1�[TO�file2]�[col1[,col2]]�...

�If�"TO�file2"�is�not�specified,�then�file1�is�replaced�by
�the�sorted�file1.














�page�78�ISIS-II�SOFTWARE�TOOLBOX,�V2.0




�DEFINING�THE�FIELDS

�A�field�is�a�single�decimal�column�number,�or�a�pair�of
�decimal�column�numbers�separated�by�a�comma.�Fields�are
�separated�by�blanks.

�The�default�value�for�an�unspecified�ending�column
�number�is�<end-of-line>.�So,�to�use�a�single�column�as�a
�sort�field,�that�column�must�be�specified�as�both�the
�starting�and�ending�columns.

�examples:
�3,10�defines�a�sort�field�starting�in�column
�3�and�ending�in�column�10.

�24�defines�a�sort�field�starting�in�column
�24�and�ending�at�the�end�of�line.

�7,7�defines�a�single�column�sort�field�in
�column�7.


�If�no�fields�are�defined,�then�by�default�a�single�sort
�field�is�defined�starting�in�column�1�and�ending�at�<eol>.

�A�maximum�of�10�fields�may�be�defined�on�the�command
�line.



�SORT�FIELD�PRECENDENCE

�The�precedence�of�the�sort�fields�is�the�same�as�their
�order�of�entry;�ie,�the�first�field�specified�is�the�last
�sort�that�takes�place.


�example:

�SORT�:F1:NAMES.DIR�20,29�10,18�1,8

�will�sort�the�file�:f1:names.dir�with�the�primary
�sort�field�being�columns�20�through�29,�the
�secondary�sort�field�being�columns�10�thru�18,�and
�the�least�significant�sort�field�being�columns�1
�through�8.�The�resultant�sorted�file�will�be
�written�back�to�the�file�:f1:names.dir,�overwriting
�the�original.












�ISIS-II�SOFTWARE�TOOLBOX,�V2.0�page�79


�+==================================+
�|�|
�|�STOPIF�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�Stopif�stops�submit�file�execution�if�an�error�or
�warning�is�found�in�the�specified�program�listing.



�INVOCATION

�STOPIF�<:f1:prog.lst>�[�IW�|�IGNOREWARNING�]

�The�IGNOREWARNING�command�will�cause�Stopif�to�only
�abort�submit�file�execution�if�errors�are�found�in�the
�listing.



�TYPICAL�USAGE

�Stopif�is�usually�used�in�a�submit�file�which�generates
�a�program.�Stopif�saves�the�link�and�locate�time�if�there
�were�translator�program�errors.�If�the�user�wishes�the
�program�generation�to�continue,�despite�translator�errors,
�then�the�Passif�program�is�recommended.



�TRANSLATORS�SUPPORTED

�Stopif�will�work�with�any�translator�list�file�that�has
�at�the�end�an�error�and�warning�summary�message�that�uses
�decimal�numbers�for�the�error�or�warning�counts,�or�"NO"�if
�the�count�is�zero.�Stopif�determines�whether�errors�or
�warnings�occured�from�this�summary�information�only.


�SEE�ALSO

�Passif.doc















�page�80�ISIS-II�SOFTWARE�TOOLBOX,�V2.0


�+==================================+
�|�|
�|�WHICH�|
�|�v1.1�|
�|�|
�+==================================+


�SUMMARY

�This�program�ascertains�and�displays�the�version�number
�associated�with�a�given�Software�Toolbox�tool.


�SAMPLE�INVOCATION

�which�:f1:passif


�SAMPLE�ACTION�PERFORMED

�Given�that�the�current�version�of�Passif�is
�version�1.1,�then�Which�will�write:

�"program�version�number�is�V1.1"

�to�the�console�output�device.


�FORMAL�SYNTACTIC�SPECIFICATION

�WHICH�<file_name_of_tool_box_tool>


�ACTIONS�PERFORMED

�WHICH�searches�the�specified�load�module�file�for�a
�string�of�ascii�bytes�of�the�form:

�"program_version_number=VX.Y"

�where�X�and�Y�above�are�ascii�decimal�digits.�If�the
�string�is�found,�Which�writes

�"program�version�number�is�VX.Y"

�to�the�console�output�device,�where�the�VX.Y
�corresponds�to�the�VX.Y�described�above.


�If�the�string�is�not�found,�Which�writes

�"file�does�not�contain�a�program�version�number"

�to�the�console�output�device.




