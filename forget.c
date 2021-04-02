#include <dirent.h>
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <unistd.h>


void
usage(char *argv0)
{
	errx(EINVAL, "usage: %s TODO", argv0);
}

//  days
//    maxd |\
//         | \
//         |  \
//         |   \
//         |    \
//         |     \
//         |      ..
//         |        \
//        -|- - - - - . - - - - - - - - - - - - - - - - - - - - -
//         |           ..
//         |             \
//         |              ..
//         |                ...
//         |                   ..
//         |                     ...
//         |                        ....
//         |                            ......
//    mind |                                  ...................
//         0                                                 maxs
//
static inline double
maxage(double size, double days, double mind, double maxd, double maxs)
{
	return mind + (-maxd + mind) * pow((size / maxs - 1), 3.0);
}


void
get_cachedir(char *cachedir) {
	char *buf;
	if ((buf = getenv("REMEMBER_DIR")))
		strcpy(cachedir, buf);
	else if ((buf = getenv("XDG_CACHE_HOME")))
		sprintf(cachedir, "%s/remember", buf);
	else if ((buf = getenv("HOME")))
		sprintf(cachedir, "%s/.cache/remember", buf);
	else
		errx(1, "couldn't assign cache directory\n");
}


int
main(int argc, char *argv[])
{
	DIR *dir;
	struct dirent *ent;
	struct stat ent_stat;
	char cachedir[BUFSIZ] = {0};
	char fpath[BUFSIZ];
	long long fsize;
	long fmtime;
	struct timeval now;
	long maxfsize = 1000000;
	double mage;
	/* int xtime; */
	/* char xtime_s[3]; */

	gettimeofday(&now, NULL);
	get_cachedir(cachedir);
	printf("%s\n", cachedir);

	bool dryrunflag = 0;
	bool verboseflag = 0;

	int opt;
	while ((opt = getopt(argc, argv, "C:hnS:v")) != -1) {
		switch (opt) {
			case 'C': strcpy(cachedir, optarg); break;
			case 'h': usage(argv[0]); break;
			case 'n': dryrunflag = verboseflag = true; break;
			case 'S': maxfsize = atoi(optarg); break;
			case 'v': verboseflag = true; break;
			default: usage(argv[0]); break;
		}
	}
	argc -= optind; argv += optind;

	if ((dir = opendir(cachedir)) == NULL) {
		errx(ENOENT, "can't open %s\n", cachedir);
	}

	while ((ent = readdir(dir)) != NULL) {
		if (ent->d_type != DT_REG) {
			continue;
		}

		// filepath
		sprintf(fpath, "%s/%s", cachedir, ent->d_name);

		// mtime
		stat(fpath, &ent_stat);
		fsize = ent_stat.st_size;
		fmtime = ent_stat.st_mtimespec.tv_sec;

		/* // exectime */
		/* strncpy(xtime_s, ent->d_name, 2); */
		/* xtime = atoi(xtime_s); */

		// maxage scaling
		mage = maxage(fsize, now.tv_sec - fmtime, 1.0,  30.0, maxfsize);
		double age = (now.tv_sec - fmtime) / 86000.0;

		if (verboseflag) {
			printf("%lld\t%f\t%f\t%s\n", fsize, age, mage, fpath);
		}

		if (age > mage) {
			/* if (unlink(fpath) != 0) { */
			/* 	errx(errno, "failed to remove file %s\n", fpath); */
			/* } */
			printf("%s\n", fpath);
		}
	}
	closedir(dir);

	return 0;
}

