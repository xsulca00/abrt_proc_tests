#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <elf.h>
#include <sys/wait.h>

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

#if defined CHANGE

	const char *const username = "nobody";
	struct passwd *pwd = getpwnam(username);

	if (!pwd)
	{
		fprintf(stderr, "Cant get user!");
		return 2;
	}
	
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

#endif

	// now reading as non-root!
	// will it work?

	printf("User ID after: %i\n", geteuid());

	char line[1024];
    unsigned long start, end;

    FILE *mem = fopen("/proc/813/mem", "r");

    if (!mem)
    {
        fprintf(stderr, "Cannot open mem!\n");
        perror("fopen");
        return 1;
    }

    fgets(line, sizeof line, f);
    printf("%s", line);
    fscanf(mem, "%lu-%lu %*s", &start, &end);
    printf("%lu\n%lu\n", start, end);
    fclose(mem);


    int fd_mem = open("/proc/813/mem", O_RDONLY);

	if (fd_mem == -1)
	{
		fprintf(stderr, "Can't open file mem!\n");
        perror("open");
		return 1;
	}


    ptrace(PTRACE_ATTACH, 813, NULL, NULL);
    wait(NULL);
    waitpid(813, NULL, 0);
    lseek(fd_mem, start, SEEK_SET);
    perror("seek");
    errno = 0;

    char buffer[_SC_PAGE_SIZE];
    read(fd_mem, buffer, _SC_PAGE_SIZE);
    perror("read");
    errno = 0;
    printf("%.4096s\n", buffer);

    ptrace(PTRACE_DETACH, 813, NULL, NULL);



 //   while (fgets(line, sizeof line, f))
 //   {
 //         printf("%s", line); 
 //   }


	return 0;

}
