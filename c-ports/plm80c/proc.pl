for $f (glob("*1*c")) {
        open($in, "<$f") or die "can't open $f\n";
        $. = 0;
        while (<$in>) {
            if (/^[a-z].*\s+(.*\(.*\))/) {
                $proc = $1;
                $page = int($. / 68) + 1;
                if (defined($procs{$proc})) {
                    for ($i = 1; defined($procs{"$proc-$i"}); $i++) {}
                }
                $procs{$proc} = "$f($page): $_";
            }
        }
        close $in;
}

for $p (sort keys %procs) {
    print $procs{$p};
}
