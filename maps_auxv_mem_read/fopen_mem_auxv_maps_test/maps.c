#define _GNU_SOURCE
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pwd.h>
#include <elf.h>

int main()
{
	// 813 pid meho abrtd
	FILE* f = fopen("/proc/813/maps", "r");

	if (!f)
	{
		fprintf(stderr, "Can't open file!\n");
        perror("fopen");
		return 1;
	}

	const char *const username = "nobody";
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

	char line[1024];

    while (fgets(line, sizeof line, f))
    {
          printf("%s", line); 
    }

    perror("fgets");

	return 0;

}
