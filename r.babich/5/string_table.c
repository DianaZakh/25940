#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct
{
    long size;
    long skip;
} TableIndex;

typedef struct
{
    TableIndex *arr;
    long len;
    long cap;
} VecTable;

VecTable vec_new(long cap)
{
    VecTable v;

    v.len = 0;
    v.cap = cap;

    v.arr = malloc(sizeof(TableIndex) * v.cap);

    return v;
}

void vec_push(VecTable *v, TableIndex value)
{
    v->arr[v->len++] = value;

    if(v->len == v->cap)
    {
        v->cap *= 2;

        TableIndex *tmp = v->arr;

        v->arr = realloc(v->arr, v->cap * sizeof(TableIndex));

        if(v->arr == NULL)
        {
            printf("Realloc error\n");
            perror("realloc");

            v->cap /= 2;
            v->cap++;

            v->arr = realloc(tmp, v->cap * sizeof(TableIndex));
        }
    }
}

//gcc -Wall -Wextra -Wpedantic -g string_table.c -o res
int main(int argc, char *argv[])
{
    if(argc > 2)
    {
        for(int i = 0; i <= argc; i++)
        {
            printf("%d - %s\n", i, argv[i]);
        }

        printf("Лишние арги\n");
        return 1;
    }

    VecTable v = vec_new(128);

    int desc = open(argv[1], O_RDONLY);

    if(desc == -1)
    {
        printf("Open error\n");
        perror("open");
        return 1;
    }

    long byte_readed = 0;
    long total_skip = 0;

    while(1)
    {
        char mem;

        int eof = read(desc, &mem, 1);

        if(eof == 0)
        {
            break;
        } else if(eof == -1)
        {
            printf("Read error\n");
            perror("read");

            return 1;
        }

        if(mem == '\n')
        {
            TableIndex value = { ++byte_readed, total_skip };

            vec_push(&v, value);

            byte_readed = 0;

            total_skip = lseek(desc, 0L, 1);

            if(total_skip == -1)
            {
                printf("Seek error\n");
                perror("lseek");

                return 1;
            }
        } else {
            byte_readed++;
        }
    }

    if(byte_readed > 0)
    {
        TableIndex value = { byte_readed, total_skip };

        vec_push(&v, value);
    }

    printf("Input line\n");

    while(1)
    {
        long line = 0;
        char tmp[64];

        scanf("%s", &tmp);

        line = atol(tmp);

        if(tmp[0] == '0')
        {
            break;
        } else if(line - 1 > v.len)
        {
            printf("Too long\n");
            continue;
        } else if(line <= 0)
        {
            printf("Wrong long\n");
            continue;            
        }

        TableIndex cur = v.arr[line - 1];

        int err1 = lseek(desc, cur.skip, 0);

        if(err1 == -1)
        {
            printf("Seek error\n");
            perror("lseek");

            return 1;
        }

        char *buf = malloc((cur.size + 1) * sizeof(char));

        int err2 = read(desc, buf, cur.size);

        if(err2 == -1)
        {
            printf("Read2 error\n");
            perror("read");

            return 1;  
        }

        buf[cur.size] = '\0';

        printf("%s", buf);

        free(buf);
    }

    free(v.arr);

    close(desc);

    return 0;
}
