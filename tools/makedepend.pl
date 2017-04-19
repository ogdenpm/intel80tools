# makedepend.pl target source
# creates file .deps/src.d	where src is the file name part of source

die "usage: makedepend.pl target soruce\n" if $#ARGV != 1;


sub realfile {
	my $isisfile = $_[0];
	my $path;
	$isisfile =~ s/:(..)://;	# remove drive from file if present
	if ($1 ne '') {
		$path = $ENV{"ISIS_".uc($1)};
		$path =~ s/[\/\\]$//;	# remove training / or \ if present
	} else {
		$path = '.';
	}
	return ($path eq '.' || $path eq '') ? $isisfile : $path/$isisfile;
}

# scan the source file for include files and recursively process
# once processed the list of files is stored in a hash based on the source file
# this avoids reprocessing
sub depend {
	my ($src) = realfile($_[0]);
	
	if (!defined($incs{$src})) {
		$incs{$src} = $incCnt++;
		print $depfile " $src";
		if (open(my $inc, "<$src")) {
			while (<$inc>) {
				if (/^\$.*include\s*\(\s*([^\s\)]*)/i) {
					depend($1);
				}
			}
			close $inc;
		} else {
			print "can't open $src\n";
		}
	}
}

$target = realfile(shift @ARGV);
$src = $source = realfile(shift @ARGV);
$src =~ s/.*[\/\\]// if $src =~ /[\/\\]/;
$src =~ s/\.[^\.]*//;

$incCnt = 0;

mkdir ".deps" if (!-d ".deps");
open($depfile, ">.deps/$src.d") or die "can't create .deps/$src.d\n";

# put the target dependency in place
print $depfile "$target:";

depend($source);

print $depfile "\n";
foreach $f (keys(%incs)) {
	print $depfile "$f:\n" if $f ne $source;
}
close $depfile;

unlink ".deps/$src.d" if $incCnt <= 1;
