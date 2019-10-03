# makedepend.pl target source
# creates file .deps/src.d	where src is the file name part of source

die "usage: makedepend.pl target source\n" if $#ARGV != 1;


sub realfile {
	my $isisfile = $_[0];
	my $path = "";

	return $isisfile if $isisfile =~ /\/\\/;	# passed in dos/unix filename. Assume user has correct char case

	$isisfile = lc($isisfile);
	if ($isisfile =~ s/:(..)://) {	# remove drive from file if present
		$device = uc($1);
		if (($path = $ENV{"ISIS_".uc($1)}) eq "") {
            print "warning ISIS_$device not defined\n";
        } elsif ($path !~ s/^([A-Z]:)?\.$/\1/) {    # drive:. or . only remove .
            $path =~ s/([^\\\/])$/\1\//;            # append any missing / or \ with a /
        }
	} 
	return "$path$isisfile";
}

# scan the source file for include files and recursively process
# once processed the list of files is stored in a hash based on the source file
# this avoids reprocessing
sub depend {
	my ($src) = realfile($_[0]);
	
	if (!defined($incs{$src})) {
		$incs{$src}++;
		$deps .= " $src";
		if (open(my $inc, '<', $src)) {
			while (<$inc>) {
				next unless /^\$/;
				if (/include\s*\(\s*([^\s\)]*)/i) {
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
$src =~ s/.*[\/\\]//;					# remove any path
$src =~ s/\.[^\.]*?$// unless $src =~ /^\.+[^\.]*?$/;	# remove any ext

$incCnt = 0;
$deps = '';

depend($source);
unlink ".deps/$src.d" if -f ".deps/$src.d";

if ($deps ne " $source") {
	mkdir ".deps" if (!-d ".deps");
	open($depfile, ">.deps/$src.d") or die "can't create .deps/$src.d\n";

	# put the target dependency in place
	print $depfile "$target:$deps\n";

	foreach $f (keys(%incs)) {
		print $depfile "$f:\n" if $f ne $source;
	}
	close $depfile;
}
