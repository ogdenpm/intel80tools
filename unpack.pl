use File::Path qw(make_path);
use Cwd;

sub usage {
    print <<"END";
unpack an editable packed file

usage: unpack.pl [-r] [-f] [file]

where   -r      unpacks recursively
        -f      forces overwrite even if existing file is newer and changed
        file    packed file - defaults to {curdif}_all.src

The packed file is a concatenation of multiple files each one stored as

    ^Lfilename 
    contents of file

File names beginning with ? are treated as nested file and spaces are replaced by `
END

}


my @packed;
my $recurse = 0;
my $force = 0;
my $sfile;

sub unpackFile {
    my $sfile = $_[0];
    open (my $src, "<$sfile") || die "can't open $sfile\n";
    my @files = split /\cL([^\?]\S*)\n/, do { local $/; <$src> };
    close $src;
    my $dir = do { $sfile =~ /(.*(\/|\\))/; $1 };

    for (my $i = 1; $i < $#files; $i += 2) {
        $files[$i] =~ s/`/ /g;          # spaces stored as `
        $userfile = "$dir$files[$i]";   # file name
        $ref = $files[$i + 1];          # and its content
        push @packed, "$userfile" if $ref =~ s/\cL\?/\cL/mg;     # check for a nested file
        if (open(my $file, "<$userfile")) {	# open if it exists
            my $content = do { local $/; <$file> };
            close $file;
            next if ($content eq $ref); # skip if no change
            if (!$force && (stat $sfile)[9] < (stat $userfile)[9]) {
                print "skipping $userfile use -f option to force extract\n";
                next;
            }
        }
        make_path($1) if $userfile =~ /(.*(\/|\\))/ && ! -d $1;  # make sure containing directory exists

        print "saving $userfile\n";
        open(my $file, ">$userfile") or die "can't create $userfile\n";
        print $file $ref;
        close $file;
    }
}

my $opt;
while ($opt = shift @ARGV) {
    last unless $opt =~ /^-/;   
    if ($opt eq '-r') {
        $recurse++;
    } elsif ($opt eq '-f') {
        $force++;
    }
}


$sfile = $opt || (split /\/|\\/, getcwd)[-1] . "_all.src";

unpackFile($sfile);
if ($recurse) {
    while ($sfile = shift @packed) {
        print "unpacking $sfile\n";
        unpackFile($sfile);
    }
}
