%patches = (
);

# load hex file
my $hex = shift @ARGV;
die "usage: loadAndFix.pl hexfile [binfile]\n" if $hex eq "";
my $bin = shift @ARGV;
$hex =~ /^(.*?)(\.[^\.]*)?$/;
my $patch = "$1.patch";
$bin = "$1.com" if $bin eq "";

open($in, "<", $hex) or die "can't open $hex\n";

my $laddr = 0xffff;
my $haddr = 0;

while (<$in>) {
    next unless /^ :/;
    chomp;
    my $binrec = pack("H*", substr( $_, 2));
    die unless unpack( "%8C*", $binrec ) == 0;
    my( $addr, $type, $data ) = unpack( "x n C X4 C x3 /a", $binrec);
    if ($type == 0 && $addr != 0) {
        $laddr = $addr if $addr < $laddr;
        for my $b (unpack("C*", $data)) {
            $code[$addr++] = $b;
        }
        $haddr = $addr if $addr > $haddr;
    }
}
close $in;

# patch file
if (open $in, "<$patch") {
    while (<$in>) {
        next if /^#/ || /^\s*$/;
        chomp;
        my ($addr, @val) = split;
        my @hval = map(hex, @val);
        $addr = hex($addr);
        $laddr = $addr if $addr < $laddr;
        for my $h (@hval) {
            $code[$addr++] = $h;
        }
        $haddr = $addr if $addr > $haddr;
    }
    close $in;
}

# write the binary file
open $out, ">:raw", $bin or die "can't create $bin\n";
print $out pack("C*", @code[$laddr..$haddr - 1]);
close $out;
