$sfile = glob("*_all.plm");

{
	open (SRC, "<$sfile") || die "can't open $sfile\n";
	local $/;
	undef $/;
	$src = <SRC>;
	close SRC;
}


@files = split /(.*)\n/, $src;

for ($i = 1; $i < $#files; $i += 2) {
	if (open(FILE, "<$files[$i]")) {
		local $/;
		undef $/;
		$file = <FILE>;
		close FILE;
		# skip if not changed
		next if ($file eq $files[$i+1]);
	}
	print "saving $files[$i]\n";
	open(FILE, ">$files[$i]") or die "can't create $files[$i]\n";
	print FILE $files[$i + 1];
	close FILE;
}

