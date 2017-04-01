# simple program to scan asm80, plm80 listing files
# and determine whether there is an error
# invoked as
# perl errtest.pl lstFile [objFile]
# if there is an error detected the optional objFile is deleted
# exits with none 0 for error and 0 for no error
#
die "usage: errtest.pl lstfile [objfile]\n" if $#ARGV < 0 || $#ARGV > 1;
$lstFile = shift @ARGV;
$objFile = shift @ARGV;

open($in, "<$lstFile") or die "can't open $lstFile\n";

@listing = <$in>;
close $in;

if ($listing[-1] =~ /^ASSEMBLY COMPLETE/) {	# asm80 file
	exit 0 if $listing[-1] =~ /NO ERRORS/;
} elsif ($listing[-1] =~ /^END OF PL\/M-80 COMPILATION/) {
	exit 0 if $listing[-3] =~ / 0 PROGRAM ERROR/;	# normal location
	exit 0 if $listing[-5] =~ / 0 PROGRAM ERROR/;	# paging location
}
unlink $objFile if ($objFile ne "" && -e $objFile);
exit 1;
