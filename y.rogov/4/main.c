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

void prompt(char *buffer, int n)
{
  puts("Enter your string ('.' to print all strings and exit):");
  fgets(buffer, n, stdin);
  buffer[strcspn(buffer, "\n")] = '\0';
}

int main() {
  list_t list = { NULL, NULL };

  char buffer[BUFFER_LIMIT];
  prompt(buffer, BUFFER_LIMIT);

  while (strcmp(buffer, ".") != 0)
  {
    char *s = malloc(strlen(buffer) + 1);
    strncpy(s, buffer, strlen(buffer) + 1);

    list_push(&list, s);

    prompt(buffer, BUFFER_LIMIT);
  }

  node_t *it = list.head;
  while (it)
  {
    printf("%s\n", it->s);
    it = it->next;
  }
}