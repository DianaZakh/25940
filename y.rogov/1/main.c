#if defined(__unix__) || defined(__unix) // Unix

// ISO C
#include <stdlib.h>
#include <stdio.h>
#include <ulimit.h>
#include <string.h>
// POSIX
#include <unistd.h>
#include <sys/resource.h>

#else
#error "Unsupported platform"
#endif

#define OPT_STRING "ispuU:cC:dvV:"
#define OPT_LIMIT 255 
#define OPT_ARG_LIMIT 255 
#define HELP_MESSAGE "Usage: %s options\n\
Options:\n\
    -i: print user and group ids\n\
    -s: make process group leader\n\
    -p: print process id, parent process id, and process group id\n\
    -u: print file size limit\n\
    -U: set file size limit\n\
    -c: print core file size limit\n\
    -C: set core file size limit\n\
    -d: print working directory\n\
    -v: print environment variables\n\
    -V: set environment variable\n\
    -h: print this help\n"

extern char **environ;

int main(int argc, char *argv[]) 
{
    if(argc < 2) {
        printf(HELP_MESSAGE, argv[0]);
        exit(0);
    }

    struct rlimit rl;

    int opts[OPT_LIMIT];
    char opts_arg[OPT_LIMIT][OPT_ARG_LIMIT];
    int opts_count = 0;
    while ((opts[opts_count] = getopt(argc, argv, OPT_STRING)) != -1) 
    {
        if (optarg) strncpy(opts_arg[opts_count], optarg, OPT_ARG_LIMIT);
        opts_count++;
    }
    for (int i = opts_count - 1; i >= 0; --i) {
        switch (opts[i])
        {
            case '?':
                fprintf(stderr, "Unknown option: %c\n", optopt);
                break;
            case 'i':
                printf("real userid = %u\n", getuid());
                printf("effective userid = %u\n", geteuid());
                printf("real groupid = %u\n", getgid());
                printf("effective groupid = %u\n", getegid());
                break;
            case 's':
                if (setpgid(0,0) == -1) 
                    perror("setpgid error");
                break;
            case 'p':
                printf("pid: %d\n", getpid());
                printf("ppid: %d\n", getppid());
                printf("pgid: %d\n", getpgid(0));
                break;
            case 'u':
                printf("ulimit: %lu\n", ulimit(UL_GETFSIZE, 0));
                break;
            case 'U':
                if (ulimit(UL_SETFSIZE, atol(opts_arg[i])) == -1)
                    fprintf(stderr, "ulimit error\n");
                break;
            case 'c':
                getrlimit(RLIMIT_CORE, &rl);
                if (rl.rlim_cur == RLIM_INFINITY)
                    printf("RLIMIT_CORE: unlimited\n");
                else
                    printf("RLIMIT_CORE: %lu\n", rl.rlim_cur);
                break;
            case 'C':
                getrlimit(RLIMIT_CORE, &rl);
                rl.rlim_cur = atoi(opts_arg[i]);
                if (setrlimit(RLIMIT_CORE, &rl) == -1)
                    fprintf(stderr, "setrlimit error\n");
                break;
            case 'd':
                printf("Working directory: %s\n", getcwd(NULL, 0));
                break;
            case 'v':
                puts("Environment variables:");
                for (int i = 0; environ[i]; i++)
                    printf("%s\n", environ[i]);
                break;
            case 'V':
                if (putenv(opts_arg[i]) != 0)
                    perror("putenv error");
                break;
            case 'h':
                printf(HELP_MESSAGE, argv[0]);
            default:
                exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
