# asmx.pl
# a pre processor for asm80 files providing the following extensions
# support for names > 6 chars
#   but note public and extern and name arguments should not use this feature
#   as the mangled names will be saved in the object file
#   names in the listing can be converted back to long name form
# support for struc definitions and member access
#   usage
#       strucname   struc
#       member definitions
#       strucname   ends
#   where member defintions are of the form
#       membername:    db  ?                ; note only a check for a number is done so ? is optional
#       membername:    dw  ?
#       membername:    db  number dup ?     ; note only the number is checked
#       membername:    dw  number dup ?
#       membername:    strucname  ?         ; for arrays (dup) and nested structures only ? initialisation is supported
#       membername:    strucname number dup  ?
#  support for struc variables
#   usage
#       varname strucname <initialisers>    ; an initialiser of ? causes ds definitions. A single ?
#                                           ; defines the whole struc as a ds
#   or
#       varname strucname                   ; without this member access is not supported
#       db/dw/ds initialisers

# markers
$tmpComment = ";!! ";                       # comment out line for asm80 processing. unasmx.pl will remove
$maxName = 6;                               # default name width for symbol table
my %linemap;                                # saves original line for grundged lines
my %namemap;                                # saves name map information for long names

die "usage: asmx.pl file.asmx [asm80 options]\n" if $#ARGV < 0;
local $file = shift @ARGV;

open($in, "<", $file) or die "can't open $file\n";
local ($ofile, $oext) = ($file =~ /^(.*?)(?:\.([^\.]*))?$/);
$oext = ($oext eq "asmx") ? "asm" : "asx"; 
open ($out, ">$ofile.$oext") or die "can't create $ofile.$oext\n";

@prog = <$in>;                              # slup in whole program

# pass 1 of the pre-processor
# handles include files, struc definitions and struc var definitions

sub include {
    my $file = $_[0];
    my $root = "./";
    $file =~ s/\s*(.*?)\s*$/\1/;
    if ($file =~ s/^:F(\d)://i) { # isis file
        if (!defined($ENV{"ISIS_F$1"})) {
            if ($1 ne 0) {  # let F0 default to current drive
                print "Warning :$1: not defined. Letting ASM80 process\n";
                return;
            }
        } else {
            $root = $ENV{"ISIS_F$1"};
            $root =~ s/([^\/\\])$/\1\//;
        }
        $file = $root . $file;
    }
    if (! -e $file) {
        print "Warning $file does not exist\n";
        return;
    }
    open (my $inc, "<", $file) or die "oops cannot open $file\n";
    my @inc = <$inc>;
    close $inc;
    splice @prog, $curLine + 1, 0, @inc;         # insert the include file
    $prog[$curLine] = $tmpComment . $prog[$curLine];   # comment out include
}

# parse a line into the label, opcode and parameters (comments are excluded)
#   label and opcode are converted to lower case
#
sub parse {
    my($label, $opcode, $param) = 
        ($prog[$curLine++] =~ /^(\S+)?                       # label
                              \s* (\S*)                      # opcode
                              \s* ((?:[^;']*|'[^']*')*)/x);  # parameters ignore ; in strings
    $param =~ s/\s*$//;              # trim
    $label =~ s/:$//;
    return (lc($label), lc($opcode), $param);
}

# build the struc hash map that is keyed by the structure name with elements
#   member -> an array of the member names
#   size ->  size of the structure
# or by the structure name . member name with elements
#   offset -> offset of this member
#   type -> type of the member one of db, dw or an existing struc
#   dim -> dimension of this member
#
sub strucDef {
    my $name = $_[0];
    my $offset = 0;     # offset of current member
    my $auto;           # used to generate synthetic labels if none provided
    my $prevLabel;      # used to support label and opcode on different lines
    $prog[$curLine - 1] = $tmpComment . $prog[$curLine - 1];     # comment out struc line
    while (1) {
        my($label, $opcode, $param) = parse;
        $prog[$curLine - 1] = $tmpComment . $prog[$curLine - 1];  # comment out definition line
        last if $opcode eq "ends" || $curLine > $#prog;     # all done
        if ($opcode eq "") {                                # label only line
            $prevLabel = $label if $label;
            next;
        }
        $label = $label || $prevLabel || $auto++;           # get a non blank label
        $prevLabel = "";                                    # we have consumed label
        my $dim = ($param =~ /^(\d+)/) || 1;                # look for dim in dim dup ?
        push @{$struc{$name}{member}}, $label;               # add member name to list of members
        $struc{"$name.$label"} = {offset => $offset, type => $opcode, dim => $dim};
        if ($opcode eq "db") {                              # adjust offset
            $offset += $dim;
        } elsif ($opcode eq "dw") {
            $offset += $dim * 2;
        } elsif (defined($struc{$opcode}{size})) {
            $offset += $dim * $struc{$opcode}{size};
        } else {
            my $line = substr($prog[$curLine - 1], 4);
            print "Warning: unknown data type $opcode at line ", $curLine - 1, "\n";
            print "\t$line\n";
        }
    }
    $struc{$name}{size} = $offset;                          # recorded the total size
}

# scan through the program and process struc definitions
# and noting vars that are structures in the hash $strucVar that maps a var name to its structure name
# for simple initialisation vars are declared as
#   varName:  strucName <initialisers>
# for more complex initialisers e.g. arrays, nested struc, large number of members use
#   varName:  strucName
#   initialisation statements
my $prevLabel; 
for ($curLine = 0; $curLine <= $#prog;) {
    if ($prog[$curLine] =~ /^\$\s*include\s*\(([^\)]+)\)/i) {
            include($1);
            $curLine++;
            next;
    }
    my($name, $opcode, $param) = parse;
    if ($opcode eq "") {                   # capture any label on label only lines
        $prevLabel = $name if $name;
        next;
    }
    $name = $name || $prevLabel;           # if no label replace with any previous label
    $prevLabel = "";                       # reset as now used
    if ($opcode eq "struc") {
        strucDef($name);
    } elsif (defined($struc{$opcode})) {   # look for opcode that is a struc name
        $strucVar{$name} = $opcode;
    }
}


# pass 2 of the preprocessor
# handles member vars, initialisation and long name mapping


# convert struc member as follows
#   strucName[.member]+    -> offset of member in struc
#   strucVar[.member]+     -> (strucVar + offset of member in struc)
# Note does not currently support access to nested types
#
# utility function that returns offset of a member in a structure. Can recurse
sub offsetOf {
    my ($type) = $_[0];
    my ($member, $submember) = split /\./, $_[1], 2;
    if (defined($struc{"$type.$member"})) {
        my $offset = $struc{"$type.$member"}{offset};
        return $offset if ($submember eq "");
        return $offset + $subMemberOffset if (my $subMemberOffset = offsetOf($member, $submember) >= 0);
    }
    return -1;
}
sub strucMember {
    my ($type, $member) = @_;
    my ($tbegin, $tend);            # used to add ( ) around var + n to avoid asm80 problems
    if (defined($strucVar{$type})) {    # the type is a struc var
        $tbegin = "($type+";            # prep the (name+ and closing )
        $tend = ")";
        $type = $strucVar{$type};       # get real structure type
    }
    my $offset = offsetOf($type, $member);
    if ($offset >= 0) {
        return $tbegin . $offset . $tend;
    }
    return "$type.$member";             # pass through to assembler; which will error
}

local $mappedId = 0;

# map a long name to an autogenerated name of format @@nnnn
# but note only the first case insensitive name is stored in its original form
# so on post processing all the same mapped names will have the same case as the first
#
sub mapname {
    my $origName = $_[0];
    my $name = lc($origName);
    if (!defined($namemap{$name})) {
        $nameLookup[$mappedId] = $origName;
        $namemap{$name} = sprintf "@@%04d", $mappedId++;
        $maxName = length($name) if length($name) > $maxName;
    }
    return $namemap{$name};             # return mapping
}

# convert an initialiser list into appropriate
# ds, db and dw statements
# initialiser is either
#   <?>
# or
#   <v1, v2...>
# where v1... are the separate initialisers that can be ? for ds, strings or expressions
# the expressions must meet normal asm80 rules with optional long names and member access
# note nested structures and arrays will use ds regardless of initialisation parameter
# 
my $MAXVARS = 48;       # max line length for vars
sub initialiser {
    my ($type, $param) = @_;
    my @expand;

    if ($param eq "?") {                            # simple ? so do ds for whole structure
        push @expand, "        ds $struc{$type}{size}    ; $type";
    } else {
        my @p = split /('(?:[^']*|'')*')|\s*,\s*/, $param;  # split parameters, into strings, non strings and undef for ,

        for my $member (@{$struc{$type}{member}}) {
            my $mtype = $struc{"$type.$member"}{type};
            my $mdim = $struc{"$type.$member"}{dim};
            my $mval = shift @p || shift @p;        # if undef (,) take next parameter component
            $mval =~ s/\s*(\S*)\s*$/\1/;            # trim spaces
            if ($mdim != 1 || defined($struc{$mtype}{size})) {
                    unshift @expand, "; Warning $type.$member - only ? initialiser supported\n";
                    $mval = "?"
            }
            if ($mval eq "?" || $mval eq "")  {      # uninitialsed data
                my $ds = ($mtype eq "db") ? $mdim   # size the member
                                          : ($mtype eq "dw") ? 2 * $mdim
                                          : $struc{$mtype}{size} * $mdim;
                push @expand, "        ds $ds    ; $member";
            } else {                                 # simple initialisation
                push @expand, "        $mtype $mval    ; $member";
            }
        }
    }
    splice @prog, $curLine + 1, 0, @expand;         # insert the expanded lines into the program
}

# process the whole program
#
for ($curLine = 0; $curLine <= $#prog; $curLine++) {
    chomp $prog[$curLine];                          # remove new line

    my($line, $comment) = ($prog[$curLine] =~ /^((?:[^;']|'[^']*')*)(;.*)?/);  # split off comment
    my ($fixup, $expand);
    # process any initialiser
    # note expanded lines are added after this one so they may have further processing
    if ($line =~ /^(\S+:)?\s*(\S+)\s*<\s*(.*)>/) { # expand initialiser
        initialiser(lc($2), lc($3));               # add the db/dw/ds expansions
        $line =~ s/</; </;                         # comment out <...>
        $expand++;                                 # record that we have expanded this line
    }
    my @parts = split /('.*?')/, $line;            # split into string fragments and other
    for my $p (@parts) {
        next if $p =~ /^'/;                        # don't convert string fragments
        # look for name1.name2[.name3]* and process as struc access
        $fixup++ if $p =~ s/([\w\$\?\@_]+)\.([\w\?\$\@_\.]+)/ &strucMember(lc($1), lc($2)) /ieg;
        # finally look for names > 6 chars and map
        $fixup++ if $p =~ s/([a-z\?\@][\w\?\@]{6,})/ mapname($1) /ieg;   # long names
        $fixup++ if $p =~ s/([a-z\?\@] [\w\?\@]*[\$\_] [\w\?\@\$\_]* |      # name with embedded $ or _
                             [\$\_] [\w\?\@\$\_]*)/ mapname($1) /xieg;   # names with $ and _
    }
    $line = join("", @parts);                       # put the line back together
    if ($fixup || $expand) {                        # we either expanded or fixed up line
        $linemap{$curLine} = $prog[$curLine];       # record the fixup line
        $comment = "    ;--$curLine--";             # set up new comment with tag
        if ($expand) {                              # if expand put tag after any label
            $line =~ s/^(\S+:)?.*/\1/;
        } else {
            $line =~ s/\s*$//;                      # otherwise remove trailing spaces
        }
    } 
    print $out "$line$comment\n";                   # generate asm line
}


close $out;
close $in;

unlink "$ofile.lst";
my $ROOT = $ENV{ROOT};
my @args = ("-m", "$ROOT/itools/asm80_4.1/asm80", "$ofile.$oext", 
            "print($ofile.lst)", @ARGV);
my $result = system("$ROOT/thames.exe", @args);

if (-e "$ofile.lst") {
    open($in, "<", "$ofile.lst") or die "can't open $ofile.lst\n";
    open ($out, ">$ofile.lstx") or die "can't create $ofile.lstx\n";

    my $syms = 0;
    while (<$in>) {
        if (/;--(\d+)--/) {
            $_ = substr($_,0,24) . $linemap{$1} . "\n";
        } else {
            s/$tmpComment//;
            s/\@\@(\d{4})/ $nameLookup[$1]/eg;
        }
        $syms++ if /^(PUBLIC|EXTERNAL|USER) SYMBOLS/;
        if ($syms && /^[\w\?\@\.\$]+\s+[A-Z] [0-9A-F]{4}/) { 
            my @item = split;
            for (my $i = 0; $i < $#item; $i += 3) {
                print $out "    " if $i != 0;
                printf $out "%-*s %s %s", $maxName, $item[$i], $item[$i + 1], $item[$i + 2];
            }
            print $out "\n";
        } else {
            print $out $_;
        }
    }
    close $in;
    close $out;
    unlink "$ofile.lst";
    unlink "$ofile.asm";
}
$result;      
