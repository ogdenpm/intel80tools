die "usage: c.pl file offset\n" unless @ARGV == 2;

$file = shift @ARGV;
$offset = hex(shift @ARGV);
$adjust = 0;
$prev = 64000;
$line = "";

open(FILE, "<$file") or die "can't read $file\n";

while (<FILE>) {
	next if / D[BW] /;
	next unless /([\dA-F]{4})  ([\dA-F]{2})/;
	my($addr, $byte) = (hex($1), hex($2));
	$offset -= $addr unless ($adjust++);
	my($raddr) = $addr + $offset;
	if (!defined($memory{$raddr}) || $memory{$raddr} != $byte) {
		if ($raddr < $prev) {
			$line = sprintf "[%04X]: %s", $raddr, $_;
			$prev = $raddr;
		}

	}
}
close(FILE);
print "$file: $line" unless $line eq "";

