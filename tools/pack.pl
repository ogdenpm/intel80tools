use Cwd;


sub usage {
    print <<"END";
create a packed text file

usage: pack.pl [-h] [-a pattern | -c pattern | -d pattern | -l | -u] [-f] [-m] [file]

where -h            prints simple help and exits
      -a pattern    add text files matching pattern - also updates changed files
      -A            as -a with implied pattern of "*"
      -c pattern    create new packed file from text files matching pattern
      -C            as -c with implied pattern of "*"
      -d pattern    remove text files matching pattern
      -l            list names of included files
      -u            update files in existing packed file 
      -f            files only no directories
      -m            for -a and -c include makefile in top level directory
      file          an optional target file - default is {curdir}_all.src

      default operation is -h

      patterns are case insensitive ? matches any char * matches any number of chars
      multiple patterns are separated by |
      [..] matches ranges of chars
      to match a file name with a space use "* *"

END
    exit();
}

my $dst;
my $recurse = 1;
my $pattern = "*";
my $makefile = 0;
my %fileList;
my %fileStatus;
my $operation;
my ($ADD, $CREATE, $DELETE, $UPDATE, $LIST, $COPY) = (1, 2, 3, 4, 5, 6);
my @status = ("", "Added", "", "Deleted", "Updated", "List", "Copy");
my $operation = 0;

my $target = (split /\/|\\/, getcwd)[-1] . "_all.src";
$target =~ tr/A-Z/a-z/;


sub getExisting {
    if (open my $src, "<", $target) {
        my @files = split /\cL([^\?]\S*)\n/, do { local $/; <$src> };
        close $src;

        for (my $i = 1; $i < $#files; $i += 2) { 
            $files[$i] =~ s/`/ /g;                     # spaced stored as ` in packed file
            $fileList{$files[$i]} = $files[$i + 1];
            $fileStatus{$files[$i]} = $COPY;
        }
        close $src;
    }
}

sub getFiles {
    my $makefile = $_[0];
    my $ndir = $_[1] || "";
	opendir my $dir, ($ndir || '.');

    while (my $file = readdir $dir) {
        next if $file eq '.' || $file eq '..';          # ignore . and ..
        $file = $ndir eq "" ? $file : "$ndir/$file";
		if (-d $file) {
            if ($recurse) {
                getFiles(1, $file);
            } else {
                print "skipping directory $file\n";
            }
		} else {
            next unless $file =~ /^$pattern$/xi;            # match those to include
            if ($file =~ /^makefile$/i && $makefile == 0) {
                print "skipping $file\n";
                next;
            }
            next if lc($file) eq $target;                   # avoid add of target
            if (-f $file && -T $file) {
                next if $operation == $UPDATE && !defined($fileStatus{$file});
                if (open my $src, "<", $file) {
                    my $content = do { local $/; <$src> };
                    close $src;
                    $content =~ s/\cL(\S+)$/\cL?\1/mg;  # nested ^Lfile to ^L?file
                    if (defined($fileStatus{$file})) {
                        if ($fileList{$file} ne $content) {
                            $fileStatus{$file} = $UPDATE;
                        }
                    } else {
                        $fileStatus{$file} = $ADD;
                    }
                    $fileList{$file} = $content;
                } else {
                    print "warning could not read $file\n";
                }
            } else {
                print "skipping binary file $file\n";
            }
        }
    }
    closedir $dir;
}





sub deleteFiles {
    $pattern = "(.*\/)?$pattern" if ($recurse);
    for $f (keys %fileList) {
        $fileStatus{$f} = $DELETE if $f =~ /^$pattern$/xi;
    }
}


# use current directory as a prefix for the generated file
my $opt;

while ($opt = shift @ARGV) {
    last unless $opt =~ /^-/;
    if ($opt eq "-h") {
        usage();
    } elsif ($opt eq "-a") {
        usage() unless ($pattern = shift @ARGV) && !$operation;
        $operation = $ADD;
    } elsif ($opt eq "-C") {
        usage() unless !$operation;
        $operation = $ADD;
        $pattern = "*";
    } elsif ($opt eq "-c") {
        usage() unless ($pattern = shift @ARGV) && !$operation;
        $operation = $CREATE;
    } elsif ($opt eq "-C") {
        usage() unless !$operation;
        $operation = $CREATE;
        $pattern = "*";
    } elsif ($opt eq "-d") {
        usage() unless ($pattern = shift @ARGV) && !$operation;
        $operation = $DELETE;
    } elsif ($opt eq "-l") {
        usage() unless !$operation;
        $operation = $LIST;
    } elsif ($opt eq "-u") {
        usage() unless !$operation;
        $operation = $UPDATE;
    } elsif ($opt eq "-f") {
        $recurse = 0;
    } elsif ($opt eq "-m") {
        $makefile = 1;
    } else {
        usage();
    }
}

usage() unless $operation;

$target = $opt if $opt ne "";
if (-f $target && $operation == $CREATE) {
    print "Overwrite existing packed file $opt (y/N)? :";
    my $key = getc(STDIN);
    exit(0) if (lc($key) ne 'y');
}

$pattern =~ s/([\{\}\$ \.\@])/\\\1/g;       # escape perl pattern chars [] are allowed
$pattern =~ s/([\*\?])/\.\1/g;              # convert * and ? to perl .* .?

$pattern = "(.*\/)?$pattern" if $recurse;

getExisting() unless $operation == $CREATE;
if ($operation != $DELETE) {
    getFiles($makefile) if $operation != $LIST;
} else {
    deleteFiles();
}


if ($operation != $LIST) {
    open($dst, ">", "$target.tmp") || die "$target.tmp:$!";
    for $f (sort keys %fileList) {
        if ($fileStatus{$f} != $DELETE) {
            my $packedName = $f;
            $packedName =~ s/ /`/g;
            print $dst "\cL$packedName\n";
            print $dst $fileList{$f};
        }
        print "$status[$fileStatus{$f}] $f\n" unless $fileStatus{$f} == $COPY;
    }
    close $dst;
    unlink $target if -f $target;
    rename "$target.tmp", $target;
} else {
    for $f (sort keys %fileList) {
        print "$f\n";
    }
}

