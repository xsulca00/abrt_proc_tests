#define _GNU_SOURCE
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pwd.h>
#include <elf.h>

#define align(adr) (adr - (adr + (sysconf(_SC_PAGE_SIZE) - 1)) / (sysconf(_SC_PAGE_SIZE) - 1))

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "%s <pid>\n", argv[0]);
        return 1;
    }
	// 813 pid meho abrtd
    char fname[512];
    sprintf(fname, "/proc/%s/auxv", argv[1]);
	FILE* f = fopen(fname, "r");
	if (!f)
	{
        perror("fopen auxv");
		return 1;
	}

#if defined CHANGE
	const char *const username = "colord";
	struct passwd *pwd = getpwnam(username);

	if (!pwd)
	{
		fprintf(stderr, "Cant get user!");
		return 2;
	}
	
	// still running under root
	// now switching to non-root user!
	
	printf("User ID before: %i\n", geteuid());
	if (setresgid(pwd->pw_gid, pwd->pw_gid, pwd->pw_gid) == -1)
	{
		fprintf(stderr, "Cannot change gid!\n");
		return 3;
	}

	if (setresuid(pwd->pw_uid, pwd->pw_uid, pwd->pw_uid) == -1)
	{
		fprintf(stderr, "Cannot change uid!\n");
		return 3;
	}

	// now reading as non-root!
	// will it work?

	printf("User ID after: %i\n", geteuid());
#endif

#define PAIR 2
#define TYPE 0
#define VALUE   1 

    unsigned long entry[PAIR];

    while (fread(entry, sizeof entry[0], 2, f) == 2)
    {
       if (entry[TYPE] == 0 && entry[VALUE] == 0)
           break;
        
       if (entry[TYPE] == AT_SYSINFO_EHDR)
       {
          printf("%lu : %lu\n", entry[TYPE], entry[VALUE]);
          break;
       }
    }

    perror("fread");

	return 0;

}
