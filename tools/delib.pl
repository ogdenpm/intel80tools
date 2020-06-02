
$#ARGV == 1 or die "usage: delib.pl library targetdir\n";
my $file = shift;
my $dir = shift;

open my $in, "<:raw", $file or die "$file: $!";

sub mkRecord {
    my($type, $rec) = @_;
    $rec = pack("Cva*", $type, length($rec) + 1, $rec);
    return $rec . chr(256 - unpack("%8C*", $rec));
}

sub mkName {
    my ($name) = $_[0];
    return pack("CA*", length($name), $name);
}

sub getRec {
	my $rec;
	read $in, $rec, 3;
	my ($type, $len) = unpack("CS<", $rec);
	read $in, $rec, $len, 3;
	return ($type, $rec);
}

my $inmod = 0;
while (!eof($in)) {
	my ($type, $rec) = getRec;
	if (!$inmod && ($type == 0x6e || $type == 0x80 || $type == 2)) {
		$inmod = 1;
		my $name = substr($rec, 4, ord(substr($rec, 3, 1)));
		open $out, ">:raw", "$dir/$name.obj" or die "$dir/$name.obj: $!";
		print "$name\n";
        if ($type == 0x6e) {
            $rec = mkRecord(0x80, mkName($name));
        }
        print $out $rec;
        next;
	}
	next unless $inmod;
	print $out $rec unless $type == 0x80;
	if ($type == 0x8a || $type == 4) {
		$inmod = 0;
        print $out "\x0e\x01\x00\xf1" if $type == 4;	# end of file
		close $out;
	}
}

