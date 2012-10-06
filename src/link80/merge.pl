@f = ( "link.ext", "link.plm", "link1a.plm", "link3.plm", "link3a.plm", "linkov.plm");

open(DST, ">linkall.plm") || die "can't create linkall.plm\n";
foreach  (@f) {
	$out = $_;
	print DST "$out\n";
	open(SRC, "<$_") || die "can't open $_\n";
	print DST <SRC>;
}
close SRC;
close DST;
