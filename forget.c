#include <dirent.h>
#include <err.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>


void
usage(char *argv0)
{
	errx(EINVAL, "usage: %s TODO", argv0);
}


static inline double
maxage(double size, double days, double mind, double maxd, double maxs)
{
	return mind + (-maxd + mind) * pow((size / maxs - 1), 3.0);
}


void
get_cachedir(char *remember_dir)
{
	char *buf;
	if ((buf = getenv("REMEMBER_DIR")))
		strcpy(remember_dir, buf);
	else if ((buf = getenv("XDG_CACHE_HOME")))
		sprintf(remember_dir, "%s/remember", buf);
	else if ((buf = getenv("HOME")))
		sprintf(remember_dir, "%s/.cache/remember", buf);
	else
		errx(1, "couldn't assign cache directory\n");
}


int
main(int argc, char *argv[])
{
	DIR *d;
	struct dirent *e;
	struct stat e_st;
	char remember_dir[BUFSIZ] = {0};
	char fpath[BUFSIZ];
	long long fsize;
	long fmtime;
	struct timeval now;
	long maxfsize = 1000000;
	double mage;
	/* int xtime; */
	/* char xtime_s[3]; */
	bool dryrunflag = false;
	bool verboseflag = false;

	gettimeofday(&now, NULL);
	get_cachedir(remember_dir);

	int opt;
	while ((opt = getopt(argc, argv, "d:hnS:v")) != -1) {
		switch (opt) {
			case 'd': strcpy(remember_dir, optarg); break;
			case 'h': usage(argv[0]); break;
			case 'n': dryrunflag = verboseflag = true; break;
			case 'S': maxfsize = atoi(optarg); break;
			case 'v': verboseflag = true; break;
			default: usage(argv[0]); break;
		}
	}
	/* argc -= optind; argv += optind; */

	if ((d = opendir(remember_dir)) == NULL) {
		errx(ENOENT, "can't open %s\n", remember_dir);
	}

	while ((e = readdir(d)) != NULL) {
		if (e->d_type != DT_REG) {
			continue;
		}

		// filepath
		sprintf(fpath, "%s/%s", remember_dir, e->d_name);

		// mtime
		stat(fpath, &e_st);
		fsize = e_st.st_size;
		fmtime = e_st.st_mtimespec.tv_sec;

		/* // exectime */
		/* strncpy(xtime_s, e->d_name, 2); */
		/* xtime = atoi(xtime_s); */

		// maxage scaling
		mage = maxage(fsize, now.tv_sec - fmtime, 1.0,  30.0, maxfsize);
		double age = (now.tv_sec - fmtime) / 86000.0;

		if (verboseflag) {
			printf("%lld\t%f\t%f\t%s\n", fsize, age, mage, fpath);
		}

		if (!dryrunflag && age > mage) {
			if (unlink(fpath) != 0) {
				errx(errno, "failed to remove file %s\n", fpath);
			}
		}
	}
	closedir(d);

	return 0;
}

