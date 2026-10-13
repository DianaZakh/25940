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

typedef struct offset_table_s
{

} offset_table_t;

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

int file_open(file_t *file, const char *path)
{
  file->fd = open(path, O_RDWR);
  if (!file->fd) return -1;

  return 0;
}

void file_close(file_t *file)
{
  close(file->fd);
}

int main(int argc, char *argv[])
{
  // Incorrect usage
  if (argc != 2)
  {
    puts(HELP);
    return EXIT_SUCCESS;
  }

  file_t file = {};

  // Read and Analyze file
  if (!file_open(&file, argv[1]))
  {
    perror("Couldn't open the file");
    return EXIT_FAILURE;
  }

  // Read input
  char input[INPUT_LIMIT];

  // REPL
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
      lseek(file.fd, nline, SEEK_SET);
      read(file.fd, buf, 1024);
      printf("%s\n", buf);
    }
    else 
    {
      puts(COMMANDS);
    }
  }

  file_close(&file);
}