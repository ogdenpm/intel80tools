open(REF, "<isis41r.lst") or die "can't read isis41r.lst\n";

while(<REF>) {
	my($addr, $byte) = (/(....) (..)/);
	$memory{hex($addr)} = hex($byte) unless defined($memory{hex($addr)});
}

close(REF);

die "usage: c.pl file offset\n" unless @ARGV == 2;

$file = shift @ARGV;
$offset = hex(shift @ARGV);
$prev = 64000;
$line = "";

open(FILE, "<$file") or die "can't read $file\n";

while (<FILE>) {
	next unless /^.{22}\d/;
	next if /\sd[swb]\s/;
	next unless /([\dA-F]{4}) ([\dA-F]{2})/;
	my($addr, $byte) = (hex($1), hex($2));
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

