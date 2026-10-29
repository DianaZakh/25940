#include <stdio.h>
#include <unistd.h>

void print_uid() {
    printf("Real UID: %u\n", getuid());
    printf("Effective UID: %u\n", geteuid());
}

void open_file() {
    FILE *file = fopen("file.txt", "r");
    if (!file) perror("Couldn't open 'file.txt'");
    if (file) fclose(file);
}

int main() 
{
    print_uid();
    open_file();
    if (setuid(getuid()) == -1)
        perror("Couldn't change UID");
    print_uid();
    open_file();
}