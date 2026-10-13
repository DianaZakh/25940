#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#define INPUT_LIMIT 1024
#define INPUT_DELIM " "

#define BLOCK_LIMIT (2 << 12)

#define COMMANDS \
"\
Commands:\
  exit\
  help\
  print [line]\
"

#define HELP \
"\
File Editor by Yarik Awesome!!! (FEYA)\n\
Usage: feya [path]\n\
" COMMANDS

typedef struct file_s
{
  int fd;
  struct {
    int count;
    int *offsets;
    int *lenghts;
  } lines;
} file_t;

// HELPER
void prompt(char *buffer, int n, const char *msg)
{
  printf("%s", msg);
  fgets(buffer, n, stdin);
  buffer[strcspn(buffer, "\n")] = '\0';
}
// !HELPER

// FILE
file_t* file_open(const char *path)
{
  file_t *file = malloc(sizeof(file_t));

  // Get file descriptor
  file->fd = open(path, O_RDWR);
  if (!file->fd)
  {
    perror("Couldn't open the file");
    return NULL;
  }

  // Default
  file->lines.count = 0;
  file->lines.offsets = NULL;
  file->lines.lenghts = NULL;

  // Starting pos
  lseek(file->fd, 0, SEEK_SET);

  // Read file by blocks
  puts("Reading file...");
  int code = 0;
  while (1)
  {
    char block[BLOCK_LIMIT] = { 0 }; 
    code = read(file->fd, block, BLOCK_LIMIT);
    if (code == 0) 
    {
      puts("Reading file done!");
      break;
    }
    if (code == -1)
    {
      perror("Couldn't read a block");
      free(file);
      return NULL;
    }
  }

  return file;
}

void file_close(file_t *file)
{
  close(file->fd);
  free(file);
}
// !FILE

// FEYA
void feya_repl(file_t *file)
{
  puts(COMMANDS);
  char input[INPUT_LIMIT];
  while(1)
  {
    prompt(input, INPUT_LIMIT, "> ");
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
    }
    else 
    {
      puts(COMMANDS);
    }
  }
}
// !FEYA

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
  if (!file) return EXIT_FAILURE;

  // REPL
  feya_repl(file);

  // Close file
  file_close(file);
}