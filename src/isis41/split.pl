open (SRC, "<isis41.plm") || die;

while ($_ = <SRC>) {
	if (/^/) {
		chomp;
		s///;
		$file = $_;
		open (DST, ">$file") || die;
	} else {
		print DST;
	}
}

close DST;

