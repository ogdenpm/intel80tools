extern char *optarg;
extern int optind, opterr;
extern int optopt;

#if defined(__STDC__)
int getopt (int argc, char **argv, const char *options);
#else
int getopt ();
#endif
