#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <sys/resource.h>
#include <ulimit.h>

extern char *optarg; //Чтобы мне ерорки глаза не мозолили 
extern int optind, opterr, optopt;

extern char **environ; //Окружение 

/*
1) Аргумент флага
2) Сл флаг
3) 
*/

//gcc -Wall -Wextra -Wpedantic -g getopt.c -o res

//https://www.opennet.ru/man.shtml

int main(int argc, char *argv[])
{
    const char *optstring = "ispuU:cC:dvV:"; //Допустиые аргументы, : после для продления

    int flag;

    while((flag = getopt(argc, argv, optstring)) != -1)
    {
        switch(flag)
        {
            case 'i':
                // system("id"); //А так мне нравиться больше
                /*
                getuid(); //Реальный id пользователя
                geteuid(); //eффективный id пользователя
                getgit(); //Аналогично группы
                getegit();
                */
                printf("UID - %d\teUID - %d\tGID - %d\teGID - %d\n", getuid(), geteuid(), getgid(), getegid()); //Ладно, по ТЗ
                break;
            case 's':
                // printf("Pre: PID - %d PGID - %d\n", getpid(), getpgrp());

                setpgrp(); // Он же - setpgid(0, 0); //Первый - не менять группу | Второй - поставить ID равный группе | Иначе там хз сложно не осилил буквы

                // printf("Post: PID - %d PGID - %d\n", getpid(), getpgrp());
                break;
            case 'p':
                printf("PID - %d\tPGID - %d\tPPID - %d\n", getpid(), getpgrp(), getppid());
                break;
            case 'u':
                //system("ulimit"); //Вызов для печати
                printf("ULIMIT - %ld\n", ulimit(UL_GETFSIZE));
                break;
            case 'U':
                {
                    long new_limit = atol(optarg);

                    ulimit(UL_SETFSIZE, new_limit);

                    // printf("NEW_LIMIT - %ld\n", new_limit);

                    // if(!(new_limit <= 0 || new_limit >= 4096)) //Мне так хочется
                    // {
                    //     // char *command = "ulimit -f";
                    //     // char *buf;
                    //     // strcat(command, buf);
                    //     // sprintf(buf, "%d", new_limit);

                    //     // system(command);

                    //     ulimit(UL_SETFSIZE, new_limit);
                    // } else if(new_limit == -1)
                    // {
                    //     ulimit(UL_SETFSIZE, -1);
                    // }
                }
                break;
            case 'c':
                {
                    // system("ulimit -c");
                    struct rlimit tmp;
                    getrlimit(RLIMIT_CORE, &tmp);
                    printf("CORE - %lu\n", tmp.rlim_cur);
                }
                break;
            case 'C':
                {
                    int new_size = atoi(optarg);
                    struct rlimit tmp;
                    getrlimit(RLIMIT_CORE, &tmp);

                    tmp.rlim_cur = new_size;

                    setrlimit(RLIMIT_CORE, &tmp);
                    // if(new_size <= 0 || new_size >= 1024) //Мне так хочется
                    // {
                    //     // char *command = "ulimit -c";
                    //     // char *buf;
                    //     // strcat(command, buf);
                    //     // sprintf(buf, "%d", new_size);

                    //     // system(command);

                    //     tmp.rlim_cur = new_size;

                    //     setrlimit(RLIMIT_CORE, &tmp);
                    // } else if(new_size == -1)
                    // {
                    //     // system("ulimit -c unlimit");
                    //     tmp.rlim_cur = -1;
                    //     setrlimit(RLIMIT_CORE, &tmp);
                    // }   
                } 
                break;
            case 'd':
                // system("ls");
                printf("DIR - %s\n", getcwd(NULL, 0));
                break;
            case 'v':
                // system("env");
                {
                    int cur = 0;

                    while(environ[cur] != NULL)
                    {
                        printf("%s\n", environ[cur++]);
                    }
                }
                break;
            case 'V':
                putenv(optarg);
                break;
            case '?':
                printf("Неизвестный аргумент - -%c\n", optopt);
                break;
        }
    }

    for(int i = optind; i < argc; i++)
    {
        printf("Неподдерживаевый аргумент - %s\n", argv[i]);
    }

    return 0;
}