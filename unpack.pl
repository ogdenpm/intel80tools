use File::Path qw(make_path);

$sfile = glob("*_all.plm");

{
	open (SRC, "<$sfile") || die "can't open $sfile\n";
	local $/;
	$src = <SRC>;
	close SRC;
}

@files = split /\cL(.+)\n/, $src;

$i = 1;

while ($i < $#files) {
	$userfile = $files[$i++];
	die "Error unexpected --- marker\n" if $userfile eq "---";
	# get the reference file taking into account nested packed files
	$ref = $files[$i++];
	if ($userfile =~ /_all\.plm$/) {
		$nested = 1;
		while ($i < $#files) {
			print "$files[$i]\n";
			if ($files[$i] eq "---") {
				$i += 2;
				last if --$nested == 0;
				$ref .= "\cL---\n";
			} else {
				$nested++ if ($files[$i] =~ /_all\.plm$/); 
				$ref .= "\cL$files[$i++]\n";
				$ref .= $files[$i++];
			}
		}
	}
	if (open(FILE, "<$userfile")) {	# open if it exists
		local $/;
		$file = <FILE>;
		close FILE;
		next if ($file eq $ref);
	}
	make_path($1) if $userfile =~ /(.*(\/|\\))/ && ! -d $1; 	# directory
	print "saving $userfile\n";
	open(FILE, ">$userfile") or die "can't create $userfile\n";
	print FILE $ref;
	close FILE;
}

