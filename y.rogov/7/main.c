#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

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
  print [line]\
"

#define HELP \
"\
File Editor by Yarik Awesome!!! (FEYA)\n\
Usage: feya [path]\n\
" COMMANDS

#define HELP_LENGTH(n) printf("Enter a line number in range [1; %d]\n", (n));
#define HELP_PRINT(n) printf("Enter a line number in range [1; %d]\n", (n));

typedef struct file_s
{
  const char *data;
  struct stat stat;
  struct {
    int count;
    int capacity;
    int *offsets;
    int *lenghts;
  } lines;
} file_t;

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

file_t* file_open(const char *path)
{
  file_t *file = malloc(sizeof(file_t));

  // Get file descriptor
  int fd = open(path, O_RDWR);
  if (!fd)
  {
    perror("Couldn't open the file");
    free(file);
    return NULL;
  }

  // Get file stats
  fstat(fd, &file->stat);

  // Map the file
  file->data = mmap(NULL, file->stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (file->data == MAP_FAILED)
  {
    perror("Mapping the file failed");
    free(file);
    return NULL;
  }

  // Close file
  close(fd);

  // Default
  file->lines.count = 0;
  file->lines.capacity = LINES_DEFAULT_CAP;
  file->lines.offsets = malloc(sizeof(int) * LINES_DEFAULT_CAP);
  file->lines.lenghts = malloc(sizeof(int) * LINES_DEFAULT_CAP);

  // Read file
  const char* end = file->data;
  const char* start = file->data;
  const char* limit = file->data + file->stat.st_size;
  while(start <= limit)
  {
    end = memchr(start, '\n', limit - start);
    if (!end) 
    {
      int len = limit - start;
      file_add_line(file, start - file->data, len);
      break;
    }

    int len = end - start;
    file_add_line(file, start - file->data, len);

    start = end + 1;
  }

  return file;
}

void file_close(file_t *file)
{
  munmap((void*)file->data, file->stat.st_size);
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
  char *arg = strtok(NULL, INPUT_DELIM);
  if (!arg)
  {
    write(STDOUT_FILENO, file->data, file->stat.st_size);
    return;
  }

  int nline = atoi(arg);
  if (nline > file->lines.count || nline < 1)
  {
    HELP_PRINT(file->lines.count);
    return;
  }

  int offset = file->lines.offsets[nline - 1];
  int length = file->lines.lenghts[nline - 1];

  char *line = malloc(length + 1);
  memcpy(line, &file->data[offset], length);
  line[length] = '\0';
  
  printf("%s\n", line);
}

void feya_repl(file_t *file)
{
  puts(COMMANDS);
  char input[INPUT_LIMIT];
  while(1)
  {
    printf("%s", "> ");
    if (!fgets(input, INPUT_LIMIT, stdin)) break;
    input[strcspn(input, "\n\r")] = '\0';

    // Dismiss the alarm
    alarm(0);

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

file_t *file = NULL;

void alarm_func(int _handler)
{
  if (!file) exit(EXIT_SUCCESS);

  write(STDOUT_FILENO, file->data, file->stat.st_size);

  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
  // Set alarm function
  signal(SIGALRM, alarm_func);

  // Incorrect usage
  if (argc != 2)
  {
    puts(HELP);
    return EXIT_SUCCESS;
  }

  // Read file
  file = file_open(argv[1]);
  if (!file) return EXIT_FAILURE;

  // Set alarm
  alarm(5);

  // REPL
  feya_repl(file);

  // Close file
  file_close(file);
}