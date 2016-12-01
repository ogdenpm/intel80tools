# Simple utility to create a primative ISIS.DIR for all
# files in current dir that match ??????.???
# long name files are excluded and link fields etc. are dummy
# The principle use is to support ixref

open($dir, ">ISIS.DIR") or die "can't create ISIS.DIR\n";
binmode $dir;

foreach $d (glob("*")) {
	($name, $junk, $ext) = ($d =~ /^([^.]{1,6})(\.(.{1,3}))?$/);
	if ($name ne "") {
		$name =~ tr/a-z/A-Z/;
		$ext =~ tr/a-z/A-Z/;
		$entry = pack "ca6a3x6", 0, $name, $ext;
		print $dir $entry;
	}
}
print $dir pack("cx15",0x7f);
close $dir;
