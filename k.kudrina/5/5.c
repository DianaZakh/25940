#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE_SIZE 1024
#define MAX_LINE_COUNT 1024

int main(int argc, char* argv[])
{
    if (argc < 2){
        printf("> Point input file name and number of line.\n");
        return 0;
    } 
    char* input_file = argv[1];
    unsigned int R = atoi(argv[2]);
    if (R == 0) return 0;

    unsigned int all_line_sizes[MAX_LINE_COUNT] = {0};
    unsigned int all_line_offsets[MAX_LINE_COUNT];
    unsigned int line_counter = 0;
    char c = '\0';
    int in = open("input.txt", 0); // 0 - O_RDONLY
    char* line = calloc(MAX_LINE_SIZE, sizeof(char));

    
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
    
    int line_R = lseek(in, all_line_offsets[R - 1], SEEK_SET);
    read(in, line, all_line_sizes[R - 1]);
    printf("Line %d: %s\n", R, line);
}
