die "usage: rebase.pl lstfile lstaddr realaddr\n" unless @ARGV == 3;

$file = shift @ARGV;

$offset = hex(shift @ARGV);
$offset = hex(shift @ARGV) - $offset;

open (FILE, "<$file") or die "can't read $file\n";

while (<FILE>) {
	s/^( {11})([\dA-F]{4})/ $1 . sprintf("%04X", hex($2) + $offset) /e;
	print;
}
