#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#define INPUT_LIMIT 1024
#define INPUT_DELIM " "

#define COMMANDS \
"\
Commands:\n\
  exit\n\
  help\n\
  print [line]\n\
"

#define HELP \
"\
File Editor by Yarik Awesome!!! (FEYA)\n\n\
Usage: feya [path]\n\n\
" COMMANDS

typedef struct file_s
{
  int fd;
  int line_count;
  int *offset_table;
} file_t;

void prompt(char *buffer, int n, const char *msg)
{
  puts(msg);
  fgets(buffer, n, stdin);
  buffer[strcspn(buffer, "\n")] = '\0';
}

// FILE
file_t* file_open(const char *path)
{
  file_t *file = malloc(sizeof(file_t));

  file->fd = open(path, O_RDWR);
  if (!file->fd) return NULL;

  file->line_count = -1;
  file->offset_table = NULL;

  return 0;
}

void file_close(file_t *file)
{
  close(file->fd);
  free(file);
}

void feya_repl(file_t *file)
{
  char input[INPUT_LIMIT];
  while(1)
  {
    prompt(input, INPUT_LIMIT, "Enter command:");
    char *cmd = strtok(input, INPUT_DELIM);
    if (!cmd) continue;

    if (strcmp(cmd, "exit") == 0) break;
    else if (strcmp(cmd, "help") == 0)
    {
      puts(HELP);
    }
    else if (strcmp(cmd, "print") == 0)
    {
      char *arg = strtok(NULL, INPUT_DELIM);
      if (!arg)
      {
        puts(COMMANDS);
        continue;
      }
      int nline = atoi(arg);
      char buf[1024] = {}; 
      lseek(file->fd, nline, SEEK_SET);
      read(file->fd, buf, 1024);
      printf("%s\n", buf);
    }
    else 
    {
      puts(COMMANDS);
    }
  }
}

int main(int argc, char *argv[])
{
  // Incorrect usage
  if (argc != 2)
  {
    puts(HELP);
    return EXIT_SUCCESS;
  }

  // Read file
  file_t *file = file_open(argv[1]);
  if (!file)
  {
    perror("Couldn't open the file");
    return EXIT_FAILURE;
  }

  // REPL
  feya_repl(file);

  // Close file
  file_close(file);
}