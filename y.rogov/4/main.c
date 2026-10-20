#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_LIMIT 1024

typedef struct node_s
{
  struct node_s *next;
  char *s;
} node_t;

typedef struct list_s
{
  node_t *head;
  node_t *tail;
} list_t;

void list_push(list_t *list, char *s)
{
  node_t *new = malloc(sizeof(node_t));
  new->next = NULL;
  new->s = s;

  if (!list->head || !list->tail) 
  {
    list->head = new;
    list->tail = new;
  }
  else 
  {
    list->tail->next = new;
    list->tail = new;
  }
}

const char* prompt(char *buffer, int n)
{
  puts("Enter your string ('.' to print all strings and exit):");
  const char *r = fgets(buffer, n, stdin);
  buffer[strcspn(buffer, "\n")] = '\0';
  return r;
}

int main() {
  list_t list = { NULL, NULL };

  char buffer[BUFFER_LIMIT];
  while(1)
  {
    if (prompt(buffer, BUFFER_LIMIT) == NULL)
    {
      if (feof(stdin)) puts("End of file reached.");
      if (ferror(stdin))
      {
        perror("Error while reading user input");
        return 1;
      }
      break;
    }

    if (strncmp(buffer, ".", 1) == 0) break;

    char *s = strdup(buffer);

    list_push(&list, s);
  }

  node_t *it = list.head;
  while (it)
  {
    printf("%s\n", it->s);
    it = it->next;
  }
}