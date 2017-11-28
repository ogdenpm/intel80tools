
sub protect
{
	return "($_[0])" if ($_[0] =~ /\w\W+\w/);
	return $_[0];
}



foreach $f (glob("*.plm"), glob("*.ipx")) {
	open $in, "<$f" or die "can't open $f\n";
	$of = $f;
	$of =~ s/\.plm/.c/;
	$of =~ s/\.ipx/.h/;
	open $out, ">$of" or die "can't create $of\n";

	$indeclare = 0;
	$inproc = 0;
	$indata = 0;
	if ($of =~ /\.h/) {
		print $out "#include <stdbool.h>\n";
		print $out "typedef unsigned char byte;\n";
		print $out "typedef unsigned short address;\n";
		print $out "typedef byte *pointer;\n\n";
	}

	while (<$in>) {
		if (/^\s*$/) {
			print $out $_;
			next;
		}
		s/\b0([A-F0-9a-z]+)h/0x\1/g;
		s/\b(\d[A-F0-9a-z]+)h/0x\1/g;
		s/\$([a-z])/\U\1/g;
		s/\$/_/g;
		$indeclare = 1 if (s/^(\s*)declare\s*/\1/);
		$inproc = 1 if /procedure/;
		$inproc = 0 if /^end(\s+\w+)?;/ || /; end;$/;
		if ($indeclare) {
			$indeclare = 0 if /;$/;
			$indeclare = 0 if /;\s*\/\*.*?\*\/\s*\$/;
			if ($indata) {
				if (/[,;](\s\/\*.*?\*\/)?\s*\$/) {
					$indata = 0;
					s/\)[,;]/};/;
				}
			} elsif (s/\b(literally|lit)\b//) {
				s/^\s*(\w+)\s*'(.+)'.*/#define\t\1\t\2/;
			} else {
				s/(\s*)(.*?)\s*(byte|address|pointer|bool)/\1\3 \2/;
				s/\b(initial|data)\((.*?)\)[,;]/= {\2};/;
				$indata = 1 if (s/\b(initial|data)\(/= {/);

				s/\(\*\)/[]/g;
				s/\((.*)\)/[\1]/g;
				if (s/\s+external\b//) {
					s/^(\s*)/\1extern /;
				} elsif (s/\s+public\b//) {
				} elsif (!$inproc) {
					s/^(\s*)/\1static /;
				}
				s/,(\s\/\*.*?\*\/)?\s*\$/;\1/ if (!$indata);
			}
		} else {
			s/\b([A-Z][a-z0-9]\w*)([ ;])/\1()\2/g;
			s/\bshl\((.*?)\s*,\s*(.*)\)/"(" . protect($1) . " << " . protect($2) . ")"/e;
			s/\bshl\((.*?)\s*,\s*(.*)\)/"(" . protect($1) . " >> " . protect($2) . ")"/e;
			s/\brol\((.*?)\s*,\s*(.*)\)/"(" . protect($1) . " << " . protect($2) . ") | (" .
							  protect($1) . " >> (8 - " . protect($2) . "))"/e;
			s/\bror\((.*?)\s*,\s*(.*)\)/"(" . protect($1) . " >> " . protect($2) . ") | (" .
							  protect($1) . " << (8 - " . protect($2) . "))"/e;
			s/\bmove\((.*?)\s*,\s*(.*?)\s*,\s*(.*)\)/Memcpy(\3, \2, \1)/;
			s/\bsize\(/Sizeof(/g;
			s/\blength\(/Length(/g;
			s/\blast\(/Last(/g;
			s/\bhigh\((.*)\)/"(" . protect($1) . " >> 8)"/eg;
			s/\blow\((.*)\)/"(" . protect($1) . " & 0xff)"/eg;
			s/ end;$// if /procedure/;
			s/^(\w+):\s+procedure(\((.*)\))?\s+(byte|address|bool)(\s+reentrant)?\s+public;/\4 \1(\3)\n{/;
			s/^(\w+):\s+procedure(\((.*)\))?(\s+reentrant)?\s+public;/void \1(\3)\n{/;
			s/^(\w+):\s+procedure(\((.*)\))?\s+(byte|address|bool)(\s+reentrant)?;/static \4 \1(\3)\n{/;
			s/^(\w+):\s+procedure(\((.*)\))?(\s+reentrant)?;/static void \1(\3)\n{/;
			s/^(\w+):\s+procedure(\((.*)\))?\s+(byte|address|bool)\s+external;/\4 \1(\3);/;
			s/^(\w+):\s+procedure(\((.*)\))?\s+external;/void \1(\3);/;


			s/\band\s+not\b/& ~/g;
			s/or\s+not/| ~/g;
			if (/\b(if|while)\b/) {
				s/\s=\s/ == /g;
				s/\band\b/&&/g;
				s/\bor\b/||/g;
				s/\bnot\b/!/g;
				s/<>/!=/g;
			}
			s/\b([a-z]\w+)\(([^\)]+)\)/\1\[\2\]/g;
			s/Memcpy/memcpy/g;
			s/Sizeof/sizeof/g;
			s/Length/length/g;
			s/Last/last/g;
			s/do\s+(.*?)\s*=\s*(.*?)\s*to\s*(.*);/for (\1 = \2; \1 <= \3; \1++) {/;
			s/do while\s+(.*);/while (\1) {/;
			s/\bif\b\s+/if (/;
			s/\s+\bthen\b/ )/;
			s/end;/}/;
			s/do;/{/;
			s/call\s*//;
		}
		print $out $_;
	}
	close $in;
	close $out;
}
