@f = ("ISIS.EXT", "isis.plm", "loc.bas", "loc.lit", "loc1.ext", "loc1.plm",
"loc2a.ext", "loc2a.plm", "loc2b.ext", "loc2b.plm",
"loc3.ext", "loc3.plm", "loc4.ext", "loc4.plm", "loc4a.plm",
"loc5.ext", "loc5a.plm", "loc5b.ext", "loc5b.plm",
"loc6.ext", "loc6.plm", "loc7.ext", "loc7.plm",
"loc8.ext", "loc8.plm", "loc8a.plm", "loc9.ext", "loc9.plm",
"makefile", "memmov.asm");

open(DST, ">locall.plm") || die "can't create locall.plm\n";
foreach  (@f) {
	$out = $_;
	print DST "$out\n";
	open(SRC, "<$_") || die "can't open $_\n";
	print DST <SRC>;
}
close SRC;
close DST;
