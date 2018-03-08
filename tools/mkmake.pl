for $f (glob("*.csd")) {
	open($in, "<$f") or die "can't open $f\n";
	while (<$in>) {
		next unless /^LINK/;
		chomp;
		$link = $_;
		while ($link =~ s/[&]$//) {
			$_ = <$in>;
			chomp;
			$link .= $_;
		}
		$link =~ s/^LINK\s+//;
		$link =~ s/\%0//g;
		$link =~ s/\s+TO\s+(\w+)\.LNK//;
		$target = lc($1);
		$link =~ s/,\s*/ /g;
		$link =~ s/(\w+.LIB)/\$(\L\1)/g;
		$link =~ s/(\w+.OBJ(\s+\w+.OBJ)+)/\$(call objdir,\L\1)/g;
		$link =~ s/(\w+.OBJ)/\$(OBJ)\/\L\1/g;
		close $in;

		print "\$(OBJ)\/$target.rel: $link\n";
		print "\t\$(call link,\$@,\$^)\n\n";

	}
}

