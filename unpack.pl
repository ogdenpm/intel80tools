$sfile = glob("*_all.plm");

{
	open (SRC, "<$sfile") || die "can't open $sfile\n";
	local $/;
	undef $/;
	$src = <SRC>;
	close SRC;
}


@files = split /(.*)\n/, $src;

for ($i = 1; $i < $#files; $i += 2) {
	if (open(FILE, "<$files[$i]")) {
		local $/;
		undef $/;
		$file = <FILE>;
		close FILE;
		# skip if not changed
		next if ($file eq $files[$i+1]);
	}
	print "saving $files[$i]\n";
	open(FILE, ">$files[$i]") or die "can't create $files[$i]\n";
	print FILE $files[$i + 1];
	close FILE;
}

print "generating makefile dependencies\n";


open($mk, "<makefile") or die "makefile: $!";
open($tmp, ">$$.tmp") or die "$$.tmp: $!";

my $toolDrive = '$(F1)';	# global used to determine F1 mapping;


# normalise the drive id to $(Fx) format or $(Vx) if specified for the tools drive
sub isisfile {
	my($file) = $_[0];
	$file =~ tr/A-Z/a-z/;
	$file =~ s/(?: : | \$\() ([fv].) [\)\:]/\U\$(\1)/x;
	$file = '$(F0)' . $file unless $file =~ /^\$\(..\)/;	# add missing drive
	return $file;
}

sub dosfile {
	my $file = $_[0];
	
	$file =~ s/\$\((..)\)/ $drivemap{$1} /e;
	return $file;
}

sub trimName {
	my $file = $_[0];
	$file = substr($file, 5) if $file =~ /^\$\((..)\)/ && $drivemap{$1} eq "";
	return $file;
}

sub settools {
	my $file = $_[0];
	if ($file =~ /\.(plm|asm)$/) {
		$toolDrive = '$(V4)';
	} elsif ($file =~ /\.pl3\$/) {
		$toolDrive = '$(V3)';
	} else {
		$toolDrive = '$(F1)';
	}
}

# look for the candidate src file that generates the object file
sub srcfile {
	my $file = $_[0];
	$file =~ s/\.obj$//;
	return "$file.plm" if -e dosfile("$file.plm");
	return "$file.asm" if -e dosfile("$file.asm");
	return "$file.pl3" if -e dosfile("$file.pl3");
	return "";
}

# scan the source file for include files and recursively process
# once processed the list of files is stored in a hash based on the source file
# this avoids reprocessing
sub depend {
	my ($src) = $_[0];
	$src =~ s/^\$\(F1\)/$toolDrive/;
	$src = trimName($src);
	if (!defined($dep{$src})) {
		$dep{$src} = $src;
		if (open(my $inc, "<", dosfile($src))) {
			while (<$inc>) {
				if (/^\$include\(([^)]*)\)/i) {
					$dep{$src} .= " " . depend(isisfile($1));
				}
			}
		} else {
			print "can't open $src\n";
		}
	}
	return $dep{$src};
}



# process the makefile to look for obj target files
# copying the original file up to the auto generated dependency info

while (<$mk>) {
	last if /^# auto generated/;	# marker for auto generated dependencies
	print $tmp $_;		# echo line
	if (/^(V,|V4|F.)\s*=\s*(\S*)/) {	# pick up the drive definitions
		# . and ./ are the same as current directory
		$drivemap{$1} = ($2 eq '.' || $2 eq './') ? '' : $2;
		next;
	}
	next unless /\w\.obj/i;	# look for object files used
	next if /^\./;		# ignore specials e.g. .asm.obj
	chomp;
	# check if we have explicit rule
	# typically these would have specific rules to build
	# ### WARNING only considers single object and source file
	# ### target.obj: source.(plm|pl3|asm)
	# ### WARINIG specifying :f1: as a drive prefix for the object file is unsafe
	if (s/([\$()\w]+\.obj)\s*:\s*([\$()\w]+\.(plm|pl3|asm))//i) {
		my($ofile, $sfile) = ($1, $2);
		$objs{isisfile($ofile)} = isisfile($sfile);
	}
	# find all other .obj files - use a hash variable to avoid duplicates
	foreach $obj (grep(/[\w\\\/:]+\.obj/i, split /[ \t\\,=&]/)) {
		my $ifile = isisfile($obj);
		# if not explicty defined mark object file for auto detect later
		$objs{$ifile} = '?' if !defined($objs{$ifile});
	}
		
}

# reprocess the auto dependency data
# print the marker
print $tmp "# auto generated\n";

# print in alphabetic order
foreach $obj (sort keys %objs) {
	# auto detect the source file if not specified
	$objs{$obj} = srcfile($obj) if $objs{$obj} eq '?';
	settools($objs{$obj});
	my $depends = depend($objs{$obj});
	next unless $depends =~ / /;	# only single dependency so handled naturally
	my $mkobj = trimName($obj);
	print $tmp "$mkobj:";
	# as there may be many dependencies
	# split the list up so that they fit into 80 columns
	# $len keeps the running length
	$len = length($mkobj) + 1;
	foreach my $d (split / /, $depends) {
		if (length($d) + $len > 80) {
			# continue on next line
			# use spaces to align with other dependencies
			$len = length($mkobj) + 1;
			print $tmp " \\\n", " " x $len;
		}
		print $tmp " $d";
		$len += length($d) + 1;
	}
	print $tmp "\n";
}

close $mk;
close $tmp;

unlink "makefile";
rename "$$.tmp", "makefile";

