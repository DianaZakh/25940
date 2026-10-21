#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAX_LINE_COUNT 1024

static unsigned int all_line_sizes[MAX_LINE_COUNT] = {0};
static unsigned int all_line_offsets[MAX_LINE_COUNT];
static unsigned int line_counter = 0;

void handle_timeout(int sig, int file) {
    for (int i = 0; i < line_counter; ++i){
        char* line = calloc(all_line_sizes[i], sizeof(char));
        read(file, line, all_line_sizes[i]);
        printf("Line %d: %s\n", i + 1, line);
    }   
}

int main()
{
    char c;
    int in = open("input.txt", 0); // O_RDONLY=0
    signal(SIGALRM, handle_timeout);
    
    while (read(in, &c, 1)!=0) {
	printf("%c",c);
        all_line_offsets[line_counter] = line_counter == 0 ? SEEK_SET : all_line_offsets[line_counter - 1] + all_line_sizes[line_counter - 1];
        while (c != '\n' && c != '\0' && c != EOF) {
            all_line_sizes[line_counter]++;
            read(in, &c, 1);
	    printf("%c",c);
        }
	all_line_sizes[line_counter]++;
        line_counter++;
    }
    
    int R;
    printf("> Point number of line (you have 5 sec):\n");
    alarm(5);
    if (scanf("%d", &R)!=1) perror("Wrong line number");
    if (R < 0) perror("Wrong line number");
    if (R == 0) return 0;
    alarm(0);

    int line_R = lseek(in, all_line_offsets[R - 1], SEEK_SET);
    char* line = calloc(all_line_sizes[R - 1], sizeof(char));
    read(in, line, all_line_sizes[R - 1]);
    printf("Line %d: %s\n", R, line);
    close(in);
    return 0;
}
