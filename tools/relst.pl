
sub loadMap {
    my $file = $_[0];
    my $cnt = 0;

    open my $in, "<", $file or die "$file: $!";

    while (<$in>) {
        if (/^ {6}MOD\s+(.*)/) {
            $mod = $1;
            next;
        }
        if (/([0-9A-F]{4})H\s+PUB\s+(.*)/) {
            $symbols{$2} = hex($1);
            $cnt++;
        } elsif (/([0-9A-F]{4})H\s+SYM\s+(.*)/) {
            if (defined($symbols{"$mod.$2"})) {
                $exclude{"$mod.$2"}++;          # duplicate so don't use
            } else {
                $symbols{"$mod.$2"} = hex($1);
            }
            $cnt++;
        }
    }
    close $in;
    die "no symbols in map $file\n" if ($cnt == 0)
}

sub loadImage {
    my $file = $_[0];

    open my $in, "<", $file or die "$file: $!";
    binmode $in;

    while (!eof($in)) {
        $type =ord(getc($in));
        $len = ord(getc($in));
        $len += ord(getc($in)) * 256;
        last if ($type == 4 || $type == 14);
        if ($type != 6) {
            seek $in, $len, 1;
        } else {
            read $in, $rec, $len;
            ($seg, $addr, @data) = unpack "CvC*", $rec;
            $crc = pop @data;
            $image[$addr] = 0;
            splice(@image, $addr, $len - 3, @data);
        }
    }
}





$ANY = 0;
$PUB = 1;
$SYM = 2;
$PLM = 0;
$inCseg = 0;
sub getOffset {
    my $file = $_[0];
    my $label;

    open my $in, "<", $file or die "$file $!";
    while (<$in>) {
        last if /^ISIS-II/;
    }
    if (/^ISIS-II 8080\/8085 MACRO ASSEMBLER/) {
        my $method = $ANY;          # assume match on any
        my $mod = "MODULE";
        while (<$in>) {
            if (/^\s+\d+\s+name\s+(\S+)/i) {
                $mod = uc($1);
            } elsif ($method == $ANY && /^\s+\d+\s+public\s+(\S+)/i) {  # we have publics so see if we can sync on these
                $method = defined($symbols{uc($1)}) ? $PUB : $SYM;
            } elsif (/^[^;]+\b([cad])seg/i) {
                $inCseg = lc($1) eq 'c';
            } elsif ($inCseg) {
                next unless (/\s+\d+ (\S+):/);
                $label = uc($1);
                last if defined($symbols{$label})
                     || $method != $PUB && defined($symbols{"$mod.$label"}) && !defined($exclude{"$mod.$label"});
            }
        }
        do {
            if (/^  ([0-9A-F]{4})/) {
                close $in;
                return (defined($symbols{$label}) ? $symbols{$label} : $symbols{"$mod.$label"}) - hex($1);
            }
        } while (<$in>);
    } elsif (/^ISIS-II PL\/M-80/) {
        $PLM = 1;
        my $method = $ANY;
        my $mod;
        while (<$in>) {
            if (/^\s+1\s+([\$0-9A-Za-z]+):/) {
                $mod = uc($1);
            } elsif (/^ {25}(; PROC  (.*)|([^:]+):)/) { # proc or label
                $label = uc($2 || $3);
                next if (defined($exclude{"$mod.$label"})); # skip if ambiguous
                if (defined($symbols{"$mod.$label"})) {     # prefer match on unambigous SYM entry
                    $refloc = $symbols{"$mod.$label"};
                } elsif (defined($symbols{$label})) {       # fall back is public
                    next if (grep(/^$mod\./, keys %symbols));  # can't match if we have sym info for this module
                    $refloc = $symbols{$label};
                    printf "Warning: syncing on public only info in $file\n";
                } else {
                    next;
                }
                my $depth = 1;      # used for PLM to skip nested function definitions
                while (<$in>) {
                    if (/^\s{0,3}\d+\s(\d+)/) {
                        $depth = $1;
                    } elsif ($depth <= 2 && /^ {11}([0-9A-F]{4})/) {
                        close $in;
                        return $refloc - hex($1);
                    }
                }
            }
        }
    }
    close $in;
    return -1;
}

die "usage: relst.pl locfile mapfile lstfiles\n" if $#ARGV < 1;

loadImage(shift @ARGV);
loadMap(shift @ARGV);

while (my $lstwild = shift @ARGV) {
    while ($lstfile = glob($lstwild)) {
        my $offset = getOffset($lstfile);
        open my $in, "<", $lstfile or die "$lstfile: $!";
        $prnfile = $lstfile;
        $prnfile =~ s/(\.lst)?$/.prn/;
        open my $out, ">", "$prnfile" or die "$prnfile: $!";

        if ($offset < 0) {
            local $/;
            print "warning failed to sync to symbols in $lstfile\n";
            print $out "**** warning failed to sync symbols ****\n";
            $all = <$in>;
            print $out $all;
        } else {
            $inCseg = $PLM;
            while (<$in>) {
                if (/!$PLM && ^[^;]+\b([cad])seg\b/i) {
                    $inCseg = lc($1) eq 'c';
                }
                if ($inCseg && /^( {2}| {11})([\dA-F]{4})( |  )([\dA-F]{2})(..)(..)(..)(.*)/) {
                    $addr = hex($2) + $offset;
                    $newline = $1. sprintf("%04X", $addr) . $3;
                    $newline .= sprintf("%02X", $image[$addr]);
                    $newline .= ($5 eq "  ") ? "  " : sprintf("%02X", $image[$addr + 1]);
                    $newline .= ($6 eq "  ") ? "  " : sprintf("%02X", $image[$addr + 2]);
                    $newline .= ($7 eq "  ") ? "  " : sprintf("%02X", $image[$addr + 3]);
                    $newline .= "$8\n";
                    print $out $newline;
                } else {
                    print $out $_;
                }
            }
        }
        close $in;
        close $out;
    }
}


