# fixomf: fixes checksums for omf files
# optionally removes @Pnnnn publics from the file
# this is used to support sythesis of old unavailable plm compilers
# that added some of the library routines inline

my $rmpub = 0;
my $rmmain = 0;
if ($#ARGV > 0) {
    while (my $param = shift @ARGV) {
        if ($param eq "-p") {
            $rmpub++;
        } elsif ($param eq "-m") {
            $rmmain++;
        } else {
            unshift @ARGV, $param;
            last;
        }
    }
}
die "usage: fixomf.pl [-p] [-m] infile [outfile]\n" .
    "\t-p removes @Pnnnn publics\n" .
    "\t-m removed main flag\n" unless $#ARGV <= 1;
my $file = shift @ARGV;
my $ofile = ($#ARGV == 0 ? shift @ARGV : $file);
my $overwrite = 0;
if ($ofile eq $file) {
    $overwrite++;
    $ofile .= $$;
}


sub mkRecord {
    my($type, $rec) = @_;
    $rec = pack("Cva*", $type, length($rec) + 1, $rec);
    return $rec . chr((-unpack("%8C*", $rec)) & 0xff);
}

sub stripPublic {
    my ($seg, @pubs) = unpack("C(vC/A*x)*", $_[0]); 
    my $rec = chr($seg);
    for (my $i = 0; $i <= $#pubs; $i += 2) {
        # skip plm80 library names of form @P0nnn
        next if $pubs[$i+1] =~ /^\@P\d{4}$/;
        $rec .= pack("vCA*x", $pubs[$i], length($pubs[$i + 1]), $pubs[$i + 1]);
    }
    return $rec;
}


open my $in, "<:raw", "$file" or die "can't open $file\n";
open my $out, ">:raw", "$ofile" or die "can't create $ofile\n";

sub getRecord {
    my ($hdr, $type, $len, $data);
    $type = $data = "";
    if (read($in, $hdr, 3) != 3) {
        print "premature eof\n";
    } else { 
        ($type, $len) = unpack("Cv", $hdr);
        if (read($in, $data, $len) != $len) {
            print "premature eof\n";
            $type = "";
        } elsif (unpack("%8C*", $hdr . $data) != 0) {
            print "crc error\n";
        } else {
            chop $data;         # remove the crc
        }
    }
    return ($type, $data);
}


while (1) {
    my ($type, $data) = getRecord();
    last if ($type eq "");          # past eof
    my $rec;

    if ($rmpub && $type == 0x16) {   # strip @Pnnnn publics if -p option
        $rec = mkRecord($type, stripPublic($data));
        next if length($rec) <= 5;      # no symbols left
    } elsif ($type == 4 && $rmmain) {   # remove main module flag
        $rec = mkRecord($type, pack("CCv", 0, 0, 0));
    } else {
        $rec = mkRecord($type, $data);  # recreate the original record
    }
    print $out $rec;
    last if $type == 0xE;        # end of file record seen
}
close $in;
close $out;

if ($overwrite) {
    unlink $file;
    rename $ofile, $file;
}
