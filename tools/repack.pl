use Cwd;

$target = glob("*_all.src");
local $/;

open (SRC, "<$target") || die "can't open $target\n";
$src = <SRC>;
close SRC;


@files = split /\cL(.+)\n/, $src;

$tmp = "$$.tmp";

open(DST, ">$tmp") or die "can't create tmp file\n";

$nested = 0;
for ($i = 1; $i <$#files; $i += 2) {
	next if ($files[$i] eq "---" && --$nested != 0);
	next if ($files[$i] !~ /_all.src$/ && $nested);
	next if ($files[$i] =~ /_all.src$/ && $nested++ != 0);
	print DST "\cL$files[$i]\n";
	next if ($files[$i] eq "---");
	print "repacking $files[$i]\n";
	if (open(FILE, "<$files[$i]")) {
		$file = <FILE>;
		close FILE;
		print DST $file;
	} else {
		close $tmp;
		unlink $tmp;
		die "can't open $files[$i]\n";
	}
}		

close DST;
unlink $target;
rename $tmp,$target;

