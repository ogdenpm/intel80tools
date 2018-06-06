# fixomf: fixes checksums for omf files
# optionally removes @Pnnnn publics from the file
# this is used to support sythesis of old unavailable plm compilers
# that added some of the library routines inline

my $rmpub = 0;
my $param = shift @ARGV;
if ($param eq "-p") {
    $rmpub++;
} else {
    unshift @ARGV, $param;
}
die "usage: fixomf.pl [-p] infile [outfile]\n\t-p removes @Pnnnn publics\n" unless $#ARGV <= 1;


sub mkRecord {
    my($type, $rec) = @_;
    $rec = pack("Cva*", $type, length($rec) + 1, $rec);
    return $rec . chr(256 - unpack("%8C*", $rec));
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



# split all records up
# there are two entries per record
# 1 = type
# 2 = data including crc
my $file = shift @ARGV;
my @records;
do {
    local $/;       # slurp the whole omf file into memory
    open $in, "<:raw", "$file" or die "can't open $file\n";

    @records = unpack("(Cv/a*)*", <$in>);
    close $in;
};

my $ofile = ($#ARGV == 0 ? shift @ARGV : $file);
my $overwrite = 0;
if ($ofile eq $file) {
    $overwrite++;
    $ofile .= $$;
}

open $out, ">:raw", "$ofile" or die "can't create $ofile\n";
for (my $i = 0; $i <= $#records; $i += 2) {
    my $crc = chop $records[$i+1];          # original crc
    my $rec = mkRecord($records[$i], $records[$i + 1]); # recreate record
    if ($crc ne substr($rec, -1) || $records[$i] > 0x2C || ($records[$i] & 1)) {    # check crc and type
        printf "bad record: record %d type %d\n", $i / 2 + 1, $records[$i];
    }
    if ($rmpub && $records[$i] == 0x16) {   # strip @Pnnnn publics if -p option
        $rec = mkRecord($records[$i], stripPublic($records[$i + 1]));
        next if length($rec) <= 4;      # empty record
    } 
    print $out $rec;
}
close $out;

if ($overwrite) {
    unlink $file;
    rename $ofile, $file;
}
