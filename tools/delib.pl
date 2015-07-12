
$#ARGV == 1 or die "usage: delib.pl library targetdir\n";
my $file = shift;
my $dir = shift;

open my $in, "<$file" or die "$file: $!";
binmode $in;

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
	if ($type == 2) {
		$inmod = 1;
		my $name = substr($rec, 4, ord(substr($rec, 3, 1)));
		open $out, ">$dir/$name.obj" or die "$dir/$name.obj: $!";
		binmode $out;
		print "$name\n";
	}
	next unless $inmod;
	print $out $rec;
	if ($type == 4) {
		$inmod = 0;
		print $out "\x0e\x01\x00\xf1";	# end of file
		close $out;
	}
}

