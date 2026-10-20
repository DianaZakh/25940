#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 50000

typedef struct node{
    char line[MAXLEN];
    int length;
    struct node* next;
}node;

int main(void){
    printf("> Вводите строки. Если вначале <.> - завершить\n");
    node* cur_node = malloc(sizeof(node));
    node* start = cur_node;
    while (1){
        scanf("%s", cur_node->line);
        if (cur_node->line[0]=='.') break;
        cur_node->length = strlen(cur_node->line) + 1;
        cur_node->next = malloc(sizeof(node));
        cur_node = cur_node->next;
    }

    printf("> Введённые строки: \n");
    cur_node = start;
    while (cur_node!=NULL) {
        printf("%s", cur_node->line);
	cur_node = cur_node->next;
    }
}
