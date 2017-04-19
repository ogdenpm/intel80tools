
sub loadMap {
	my $file = $_[0];
	my $cnt = 0;

	open my $in, "<", $file or die "$file: $!";

	while (<$in>) {
		next unless /([0-9A-F]{4})H\s+(PUB|SYM)\s+(.*)/;
		$symbols{$3} = hex($1);
		++$cnt;
	}
	close $in;
	die "no symbols in map $file\n" if ($cnt == 0)
}


sub getOffset {
	my $file = $_[0];

	open my $in, "<", $file or die "$file $!";
	my $cmd = <$in>;	# command line
	my $isAsm = ($cmd =~ /ASM80/) ? 1 : 0;
	my $label;

	# look for a reference to align
	while (<$in>) {
		if ($isAsm) {
			next unless (/\d+ (\w+):/);
			$label = $1;
			$label =~ tr/a-z/A-Z/;
		} else {	# plm - sync on symbols procedure or label
			next unless /^ {25}(; PROC  (.*)|([^:]+):)/;
			$label = $2 || $3;
		}
		next unless defined($symbols{$label});
		my $realLoc = $symbols{$label};
		my $depth = 1;		# used for PLM to check not in nested function
		do {
			$depth = $1 if (!$isAsm && /^\s{0,3}\d+\s+(\d+)/);
			if (($isAsm && /^  ([0-9A-F]{4})/)
			 || ($depth <= 2 && /^ {11}([0-9A-F]{4})/)) {
				close $in;
				return $realLoc - hex($1) 
			}
		} while (<$in>);
	}
	close $in;
	return -1;
}

die "usage: relst.pl mapfile lstfiles\n" if $#ARGV < 1;

loadMap(shift @ARGV);

while (my $lstwild = shift @ARGV) {
	while ($lstfile = glob($lstwild)) {
		my $offset = getOffset($lstfile);
		if ($offset < 0) {
			print "warning failed to sync to symbols in $lstfile\n";
		} else {
			open my $in, "<", $lstfile or die "$lstfile: $!";
			open my $out, ">", "$lstfile.prn" or die "$lstfile.prn: $!";
			while (<$in>) {
				s/^( {2}| {11})([\dA-F]{4})/ $1 . sprintf("%04X", hex($2) + $offset) /e;
				print $out $_;
			}
			close $in;
			close $out;
		}
	}
}


