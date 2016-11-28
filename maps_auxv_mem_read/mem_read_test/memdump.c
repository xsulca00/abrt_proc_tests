#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/ptrace.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void dump_memory_region(FILE* pMemFile, unsigned long start_address, long length)
{
    unsigned long address;
    int pageLength = 4096;
    unsigned char page[pageLength];
    fseek(pMemFile, start_address, SEEK_SET);

    for (address=start_address; address < start_address + length; address += pageLength)
    {
        fread(&page, 1, pageLength, pMemFile);
   
        // write to stdout
        fwrite(&page, 1, pageLength, stdout);
    }
}

int main(int argc, char **argv) {

    if (argc == 2)
    {
        int pid = atoi(argv[1]);
        long ptraceResult = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
        if (ptraceResult < 0)
        {
            printf("Unable to attach to the pid specified\n");
            return 1;
        }
        waitpid(pid, NULL, 0);
        
        char    mapsFilename[1024];
        sprintf(mapsFilename, "/proc/%s/maps", argv[1]);
        FILE*   pMapsFile = fopen(mapsFilename, "r");
        perror("fopen maps");
        errno = 0;

        char memFilename[1024];
        sprintf(memFilename, "/proc/%s/mem", argv[1]);
        FILE* pMemFile = fopen(memFilename, "r");
        perror("fopen mem");
        errno = 0;
  
#if defined CHANGE

        uid_t euid = geteuid();
        gid_t egid = getegid();

        struct passwd *pwd;

        pwd = getpwnam("nobody");
        if (!pwd)
        {
            fprintf(stderr, "Cant get user!");
            return 2;
        }

        fprintf(stderr, "User ID before: %i\n", euid);

        euid = pwd->pw_uid;
        egid = pwd->pw_gid;

        if (setresgid(egid, egid, egid) == -1)
        {
            fprintf(stderr, "Cannot change gid!\n");
            return 3;
        }

        if (setresuid(euid, euid, euid) == -1)
        {
            fprintf(stderr, "Cannot change uid!\n");
            return 3;
        }

        fprintf(stderr, "User ID after: %i\n", geteuid());
#endif

        
        char line[256];
        while (fgets(line, sizeof line, pMapsFile) != NULL)
        {
            unsigned long start_address;
            unsigned long end_address;
            sscanf(line, "%lx-%lx\n", &start_address, &end_address);
            fprintf(stderr, "%lx-%lx\n", start_address, end_address);
            dump_memory_region(pMemFile, start_address, end_address - start_address);
        }
        fclose(pMapsFile);
        fclose(pMemFile);

        ptrace(PTRACE_CONT, pid, NULL, NULL);
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
    }
    else
    {
        printf("%s <pid>\n", argv[0]);
        exit(0);
    }
}
