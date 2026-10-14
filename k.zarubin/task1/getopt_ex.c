#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <errno.h>
#include <unistd.h>

#define MAX_OPTS 100
#define MAX_ARGS 1024

typedef struct {
  int opt_name;
  char *arg;
} Option;

/* обработка опций при слитном написании (например -i-p) */
int is_option_char(char c) { return strchr("ispuUcCdvV", c) != NULL; }
int takes_argument(char c) { return c == 'U' || c == 'C' || c == 'V'; }

int add_argument(char **args, int *count, const char *value, size_t len) {
  if (*count >= MAX_ARGS - 1) {
    fprintf(stderr, "too many arguments\n");
    return -1;
  }

  args[*count] = malloc(len + 1);
  if (args[*count] == NULL) {
    perror("malloc");
    return -1;
  }

  memcpy(args[*count], value, len);
  args[*count][len] = '\0';
  (*count)++;

  return 0;
}

int expand_arguments(int argc, char **argv, char **result) {
  int count = 1;
  result[0] = argv[0];

  for (int i = 1; i < argc; i++) {
    const char *arg = argv[i];

    if (arg[0] != '-' || arg[1] == '\0' ||
        arg[1] == '-' || strchr(arg + 1, '-') == NULL) {
      if (add_argument(result, &count, arg, strlen(arg)) == -1)
        goto error;

      continue;
    }

    int j = 1;

    while (arg[j] != '\0') {
      if (arg[j] == '-') {
        j++;
        continue;
      }

      char option = arg[j++];

      if (!is_option_char(option)) {
        char token[] = {'-', option, '\0'};

        if (add_argument(result, &count, token, 2) == -1)
          goto error;

        continue;
      }

      if (takes_argument(option)) {
        int start = j;
        int end = j;

        while (arg[end] != '\0' &&
               !(arg[end] == '-' && is_option_char(arg[end + 1]))) {
          end++;
        }

        char *token = malloc((size_t)(end - start) + 3);
        if (token == NULL) {
          perror("malloc");
          goto error;
        }

        token[0] = '-';
        token[1] = option;
        memcpy(token + 2, arg + start, (size_t)(end - start));
        token[end - start + 2] = '\0';

        int status = add_argument(result, &count, token, strlen(token));
        free(token);

        if (status == -1)
          goto error;

        j = end;
      } else {
        char token[] = {'-', option, '\0'};

        if (add_argument(result, &count, token, 2) == -1)
          goto error;
      }
    }
  }

  result[count] = NULL;
  return count;

error:
  for (int i = 1; i < count; i++)
    free(result[i]);

  return -1;
}

void free_arguments(char **args, int count) {
  for (int i = 1; i < count; i++)
    free(args[i]);
}
/* ---------------------- */

int main(int argc, char *argv[]) {
  Option options[MAX_OPTS];
  char *expanded_argv[MAX_ARGS];

  int expanded_argc = expand_arguments(argc, argv, expanded_argv);
  if (expanded_argc == -1)
    return 1;

  int cnt = 0; int c;
  opterr = 0;

  while ((c = getopt(expanded_argc, expanded_argv, "ispuU:cC:dvV:")) != -1) {
    if (cnt >= MAX_OPTS) {
      fprintf(stderr, "too many arguments\n");
      free_arguments(expanded_argv, expanded_argc);
      return 1;
    }

    if (c == '?') options[cnt].opt_name = optopt;
    else options[cnt].opt_name = c;

    options[cnt].arg = optarg;
    cnt++;
  }

  if (cnt == 0) {
    fprintf(stderr, "No options provided\n");
    free_arguments(expanded_argv, expanded_argc);
    return 1;
  }

  if (optind < expanded_argc) {
    fprintf(stderr, "Unexpected argument: %s\n", expanded_argv[optind]);
    free_arguments(expanded_argv, expanded_argc);
    return 1;
  }

  int opt = 1;
  for (int i = cnt-1; i >= 0; i--) {
    if (cnt > 1) printf("%d.\n", opt);

    switch (options[i].opt_name) {
      case 'i': {
        printf("UID Real: %ld\n", (long)getuid());
        printf("UID Effective: %ld\n", (long)geteuid());
        printf("GID Real: %ld\n", (long)getgid());
        printf("GID Effective: %ld\n\n", (long)getegid());
        break;
      }

      case 's': {
        if (getpid() == getpgrp()) {
          printf("Process is already a group leader: PID = PGID (%ld, %ld)\n\n", (long)getpid(), (long)getpgrp());
          break;
        }

        if (setpgid(0,0) == -1) {
          perror("setpgid");
          break;
        }

        if (getpid() == getpgrp()) printf("Process successfully became a group leader! PID = PGID (%ld, %ld)\n\n", (long)getpid(), (long)getpgrp());
        else printf("Error: process is not a group leader:\nPID != PGID (%ld, %ld)\n\n", (long)getpid(), (long)getpgrp());
        break;
      }

      case 'p': {
        printf("PID: %ld\n", (long)getpid());
        printf("PPID: %ld\n", (long)getppid());
        printf("PGID: %ld\n\n", (long)getpgrp());
        break;
      }

      case 'u': {
        struct rlimit limit;

        if (getrlimit(RLIMIT_NPROC, &limit) == -1) {
          perror("getrlimit");
          break;
        }

        if (limit.rlim_cur == RLIM_INFINITY) printf("Current limit is unlimited\n\n");
        else printf("Current limit: %llu\n\n", (unsigned long long)limit.rlim_cur);
        break;
      }

      case 'U': {
        if (options[i].arg == NULL) {
          fprintf(stderr, "Option -U requires an argument\n\n");
          break;
        }

        unsigned long long value;
        char *end;

        errno = 0;
        value = strtoull(options[i].arg, &end, 10);

        if (errno != 0 || end == options[i].arg || *end != '\0' || options[i].arg[0] == '-') {
          fprintf(stderr, "Invalid ulimit value\n\n");
          break;
        }

        struct rlimit limit;

        if (getrlimit(RLIMIT_NPROC, &limit) == -1) {
          perror("getrlimit");
          break;
        }

        if ((unsigned long long)(rlim_t)value != value) {
          fprintf(stderr, "Ulimit value is too large\n\n");
          break;
        }

        limit.rlim_cur = (rlim_t)value;

        if (setrlimit(RLIMIT_NPROC, &limit) == -1) {
          perror("setrlimit");
          break;
        }

        printf("Ulimit changed to %llu\n\n", value);
        break;
      }

      case 'c': {
        struct rlimit limit;

        if (getrlimit(RLIMIT_CORE, &limit) == -1) {
          perror("getrlimit");
          break;
        }

        if (limit.rlim_cur == RLIM_INFINITY) printf("Core file limit is unlimited\n\n");
        else printf("Core file limit: %llu\n\n", (unsigned long long)limit.rlim_cur);
        break;
      }

      case 'C': {
        if (options[i].arg == NULL) {
          fprintf(stderr, "Option -C requires an argument\n\n");
          break;
        }

        unsigned long long value;
        char *end;

        errno = 0;
        value = strtoull(options[i].arg, &end, 10);

        if (errno != 0 || end == options[i].arg || *end != '\0' || options[i].arg[0] == '-') {
          fprintf(stderr, "Invalid core file limit\n\n");
          break;
        }

        struct rlimit limit;

        if (getrlimit(RLIMIT_CORE, &limit) == -1) {
          perror("getrlimit");
          break;
        }

        if ((unsigned long long)(rlim_t)value != value) {
          fprintf(stderr, "Core file limit is too large\n\n");
          break;
        }

        limit.rlim_cur = (rlim_t)value;

        if (setrlimit(RLIMIT_CORE, &limit) == -1) {
          perror("setrlimit");
          break;
        }

        printf("Core file limit changed to %llu\n\n", value);
        break;
      }

      case 'd': {
        char *cwd = getcwd(NULL, 0);
        if (cwd == NULL) {
          perror("getcwd");
          break;
        }

        printf("Current directory: %s\n\n", cwd);
        free(cwd);
        break;
      }

      case 'v': {
        extern char **environ;
        for (char **env = environ; *env != NULL; env++){ printf("%s\n", *env); }
        printf("\n");
        break;
      }

      case 'V': {
        if (options[i].arg == NULL) {
          fprintf(stderr, "Option -V requires an argument\n\n");
          break;
        }

        char *equal = strchr(options[i].arg, '=');

        if (equal == NULL || equal == options[i].arg) {
          fprintf(stderr, "Invalid environment variable format. Use name=value\n\n");
          break;
        }

        *equal = '\0';

        if (setenv(options[i].arg, equal + 1, 1) == -1) {
          perror("setenv");
          *equal = '=';
          break;
        }

        *equal = '=';

        printf("Environment variable set: %s\n\n", options[i].arg);
        break;
      }

      default: {
        fprintf(stderr, "Unknown option - %c\n\n", options[i].opt_name);
        break;
      }
    }

    opt++;
  }

  free_arguments(expanded_argv, expanded_argc);
  return 0;
}
