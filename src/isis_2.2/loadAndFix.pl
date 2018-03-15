# blocks to write
@blks = (0x8, 0xb, 0x3b, 0x2b09, 0x2b26, 0x2ffc);

# load hex file
open($in, "<isis.hex") or die "can't open isis.hex\n";

my $laddr = 0xffff;
my $haddr = 0;

while (<$in>) {
    next unless /^ :/;
    chomp;
    my $binrec = pack("H*", substr( $_, 2));
    die unless unpack( "%8C*", $binrec ) == 0;
    my( $addr, $type, $data ) = unpack( "x n C X4 C x3 /a", $binrec);
    if ($type == 0) {
        for my $b (unpack("C*", $data)) {
            $code[$addr++] = $b;
        }
    }
}
close $in;

# write the binary file
open $out, ">:raw", "isis.bin" or die "can't create isis.bin\n";
for ($i = 0; $i < $#blks; $i += 2) {
    print $out pack("vvC*", $blks[$i + 1] - $blks[$i], $blks[$i], @code[$blks[$i]..$blks[$i+1] - 1]);
}
# write the ending record
print $out pack("vv", 0, 0x3b); # 0x3b is entry point
close $out;
