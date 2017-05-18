use Cwd;

my $dir = getcwd;	# use directory name as prefix for merged file
my $target = (split '/', $dir)[-1] . "_all.plm";
$target =~ tr/A-Z/a-z/;

sub addfiles {
	my $dir = $_[0];
	my @f = glob("$dir/*");

	foreach  (@f) {
		tr/A-Z/a-z/;
		next if $_ eq "./$target";
		next if $_ eq "./makefile";
		if (-d) {
			addfiles($_);
		} elsif (-f && -T) {
			$out = $_;
			$out =~ s/^\.\///;
			print "add $out\n";
			print DST "$out\n";
			open(SRC, "<$_") || die "$_:$!";
			print DST <SRC>;
			print DST "\cL---\n" if $out =~ /_all\.plm$/; # mark end of embedded packed file
			close SRC;
		} else {
			print "skipping file $_\n";
		}
	}
}
open(DST, ">", $target) || die "$target:$!";
addfiles(".");
close DST;
