#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>

#define INPUT_LIMIT 1024
#define INPUT_DELIM " "

#define BLOCK_LIMIT (2 << 12)

#define LINES_DEFAULT_CAP 256

#define COMMANDS \
"\
Commands:\
  exit\
  help\
  length [line]\
"

#define HELP \
"\
File Editor by Yarik Awesome!!! (FEYA)\n\
Usage: feya [path]\n\
" COMMANDS

#define HELP_LENGTH(n) printf("Enter a line number in [1; %d]\n", (n));

typedef struct file_s
{
  int fd;
  struct {
    int count;
    int capacity;
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
void file_add_line(file_t *file, int offset, int length)
{
  file->lines.count++;
  if (file->lines.count > file->lines.capacity)
  {
    file->lines.offsets = realloc(
      file->lines.offsets, 
      sizeof(int) * file->lines.capacity * 2
    );
    file->lines.lenghts = realloc(
      file->lines.lenghts, 
      sizeof(int) * file->lines.capacity * 2
    );
    file->lines.capacity *= 2;
  }

  int idx = file->lines.count - 1;
  file->lines.offsets[idx] = offset;
  file->lines.lenghts[idx] = length;
}

void file_block_analyze(file_t *file, const char *block, int file_offset)
{
  const char *line_start = block;
  const char *line_end = block;
  while((line_end = strchr(line_start, '\n')))
  {
    size_t offset = file_offset + (size_t)(line_start - block);
    size_t length = line_end - line_start;
    file_add_line(file, offset, length);

    line_start = line_end + 1;
  }

  line_end = line_start + strlen(line_start);
  size_t offset = file_offset + (size_t)(line_start - block);
  size_t length = line_end - line_start + 1;
  file_add_line(file, offset, length);
}

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
  file->lines.capacity = LINES_DEFAULT_CAP;
  file->lines.offsets = malloc(sizeof(int) * LINES_DEFAULT_CAP);
  file->lines.lenghts = malloc(sizeof(int) * LINES_DEFAULT_CAP);

  // Starting pos
  lseek(file->fd, 0, SEEK_SET);

  // Read file by blocks
  puts("Reading file...");
  size_t offset = 0;
  char block[BLOCK_LIMIT + 1]; 
  while (1)
  {
    int n = read(file->fd, block, BLOCK_LIMIT);

    if (n == 0) 
    {
      puts("Reading file done!");
      break;
    }
    if (n == -1)
    {
      perror("Couldn't read a block");
      free(file);
      return NULL;
    }
    block[n] = '\0';

    file_block_analyze(file, block, offset);
    offset += n;
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
void feya_length(file_t *file)
{
  char *arg = strtok(NULL, INPUT_DELIM);
  if (!arg)
  {
    HELP_LENGTH(file->lines.count);
    return;
  }

  int nline = atoi(arg);
  if (nline > file->lines.count || nline < 1)
  {
    HELP_LENGTH(file->lines.count);
    return;
  }

  printf("%d\n", file->lines.lenghts[nline - 1]);
}

void feya_print(file_t *file)
{
  // char *arg = strtok(NULL, INPUT_DELIM);
  // if (!arg)
  // {
  //   HELP_PRINT(file->lines.count);
  //   return;
  // }

  // int nline = atoi(arg);
  // if (nline > file->lines.count || nline < 1)
  // {
  //   HELP_PRINT(file->lines.count);
  //   return;
  // }

  // printf("%d\n", file->lines.lenghts[nline - 1]);
}

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
    else if (strcmp(cmd, "length") == 0)
    {
      feya_length(file);
    }
    else if (strcmp(cmd, "print") == 0)
    {
      feya_print(file);
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