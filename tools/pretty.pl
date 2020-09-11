sub mkaddr
{
    my $loc = $_[0];
    my $addr = hex($code[$loc]) + 256 * hex($code[$loc + 1]);
    if (defined($code[$addr]) && $label[$addr] eq "") {
        $label[$addr] = sprintf "L%04X", $addr;
    } 
    if ($label[$addr] ne "") {
        return sprintf "%s ; %04XH", $label[$addr], $addr;
    } else {
        return sprintf "%04XH", $addr;
    }
}

$dbcnt = 0;
sub printCode
{
    my $addr = $_[0];

    return unless defined($code[$addr]) || defined($label[$addr]);
    if ($dbcnt == 8 || defined($label[$addr]) || $code[$addr] !~ /^..H$/) {
        print $out "\n" if $dbcnt;
        $dbcnt = 0;
    }
    if (!defined($code[$addr])) {
        printf $out "        %-12s  ; %04X\n", $label[$addr] . ':', $addr;
        return;
    }
    print $out "        $label[$addr]:\n" if defined($label[$addr]);
    if ($dbcnt) {
        print $out ", $code[$addr]";
    } else {
        printf $out "          %04X %s", $addr, $code[$addr];
    }
    $dbcnt++ if $code[$addr] =~ /^..H$/;
    print $out "\n" if $dbcnt == 0;
}


die "usage: pretty.pl infile outfile" unless $#ARGV == 1;
$file = shift @ARGV;

open $in, "<$file" or die "can't open $file\n";

$file = shift @ARGV;
open $out, ">$file" or die "can't open $file\n";

$line = 0;

while (($_ = <$in>) && ! /^ 8080 PLM2/) {
    if (/^ (\d{5})/) {
        $line = $1;
        $prog[$line] = $_;
    } elsif ($line != 0) {
        $prog[$line] .= $_;
    }
}

while (($_ = <$in>) && ! /^ STACK/) {
    next if /^\s*$/;
    chomp;
    foreach $interlist (split /\s+/) {
        $interlist =~ /(\d+)=([^H]+)/;
        $loc[$1] = hex($2);
    }

}

$stack = $_;     # add stack size message to end

while (($_ = <$in>) && /^ [A-Z]/) {
    /^ ([^\.]+)\.+(....)/;
    $label[hex($2)] = $1;
}

do {
    chomp;
    if (s/^ (....)H\s*//) {
        $addr = hex($1);
        if (/^..H ..H/) {
            foreach $c (split / /) {
                $code[$addr++] = $c;
            }
        } else {
            while ($_ ne "") {
                my $inst = substr($_, 0, 7);
                substr($_, 0, 7) = "";
                $inst =~ s/\s*$//;      # trim
                if ($inst =~ /MOV (.)(.)/) {
                    if ($2 eq "I") {
                        $inst = "MVI $1,b";
                    } else {
                        $inst = "MOV $1,$2";
                    }
                } elsif ($inst =~ /^LXI/) {
                    $inst .= ",a";
                } elsif ($inst =~ /(ADD|ADC|SUB|SBC|ANA|ORA|XRA) I/) {
                    $inst = substr($inst,0,2) . "I b";
                } elsif ($inst =~ /^(IN|OUT)$/) {
                    $inst .= " b";
                } elsif ($inst =~ /SBC/) {
                    $inst =~ s/SBC/SBB/;
                } elsif ($inst =~ /^(LDA|STA|LHLD|SHLD|JMP|CALL|[JC]N?[CZSP])$/) {
                    $inst .= " a";
                }
                $code[$addr++] = $inst;
            }
        }
    }
} while (($_ = <$in>) && (/^ [\dA-F]/ || /^\s*/));

# clean up the code entries
for ($i = 0; $i < $#code; $i++) {
    if ($code[$i] =~ /b$/) {
        $code[$i] =~ s/b/$code[$i + 1]/e;
        undef $code[++$i];
    } elsif ($code[$i] =~ /a$/) {
        $code[$i] =~ s/a/mkaddr($i + 1)/e;
        undef $code[++$i];
        undef $code[++$i];
    }
}
$prog[$line] .= "PASS2 - " . $_;

$addr = 0;
for ($i = 1; $i < $#prog; $i++) {
    if (defined($loc[$i])) {
        while ($addr < $loc[$i]) {
            printCode($addr++);
      }
   }
   if ($dbcnt) {
       print $out "\n";
       $dbcnt = 0;
   }
   print $out $prog[$i];
}

while ($addr <= $#code) {
    printCode($addr++);
}

print $out "\n" if ($dbcnt);

while ($addr < $#label) {
    my $laddr = $addr;
    next unless defined($label[$addr++]);
    while ($addr < $#label && !defined($label[$addr])) {
        $addr++;
    }
    if ($addr < $#label) {
        printf $out "        %-11s ds %-4d ; %04XH\n", $label[$laddr] . ":", $addr - $laddr, $laddr;
    } else {
        printf $out "        %-11s         ; %04XH\n", $label[$laddr] . ":", $laddr;
    }
}
