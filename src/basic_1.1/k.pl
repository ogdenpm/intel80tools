
open $in, "<kw.txt";

while (<$in>) {
	if (s/^(.)kw://) {
		$let = $1;
	}
	$kw = $let;
	next if (/.db 0$/);
	if (/.db '(.)'/) {
		$kw .= $1;
		$_ = <$in>;
	} elsif (/.db "(.*)"/) {
		$kw .= $1;
		$_ = <$in>;
	} 
	/db (.*)h/;
	$kw .= chr(hex($1) & 0x7f);
	$_ = <$in>;
	/db (.*)h/;
	$map{hex($1)} .= " $kw";
}

foreach $i (sort {$a <=> $b} keys(%map)) {
	printf "%02X :%s\n", $i, $map{$i};
}
