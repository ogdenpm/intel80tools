open (SRC, "<isis_t0.plm") || die;

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

