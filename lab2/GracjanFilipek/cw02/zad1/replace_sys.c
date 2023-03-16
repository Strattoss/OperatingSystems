#include <time.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_FILENAME_LEN 512
#define BUFFER_SIZE 1024

int main(int argc, char **argv)
{
    clock_t time_start, time_stop;

    time_start = clock();

    char char_to_find;
    char char_to_replace_with;

    char in_filename[MAX_FILENAME_LEN];
    char out_filename[MAX_FILENAME_LEN];

    char buffer[1024];


    // arguments validation
    if (argc != 4 + 1)
    {
        fprintf(stderr, "Expected exactly 4 arguments:\n"
                        "\tcharacter to replace\n"
                        "\tcharacter to replace with\n"
                        "\tinput file name\n"
                        "\toutput file name\n"
                        "Got %d arguments\n",
                argc - 1);
        return -1;
    }

    if (strlen(argv[1]) != 1)
    {
        fprintf(stderr, "First argument should be a single character\n");
        return -1;
    }

    if (strlen(argv[2]) != 1)
    {
        fprintf(stderr, "Second argument should be a single character\n");
        return -1;
    }

    // renaming arguments for better readability
    char_to_find = argv[1][0];
    char_to_replace_with = argv[2][0];
    strcpy(in_filename, argv[3]);
    strcpy(out_filename, argv[4]);

    if (char_to_find == char_to_replace_with) {
        fprintf(stderr, "Cannot replace a character with the same character\n");
        return -1;
    }

    int in_file_desc = open(in_filename, O_RDONLY);

    if (in_file_desc < 0)
    {
        perror(in_filename);
        return -1;
    }

    int out_file_desc = open(out_filename, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);

    if (out_file_desc < 0)
    {
        perror(out_filename);
        return -1;
    }


    // replacing char_to_find with char_to_replace_with from in_file to out_file
    
    int chars_read = read(in_file_desc, buffer, sizeof(char) * BUFFER_SIZE);
    char *curr_char;

    while (chars_read > 0)
    {
        curr_char = strchr(curr_char, char_to_find);
        while (curr_char != NULL)
        {
            *curr_char = char_to_replace_with;
            curr_char = strchr(curr_char, char_to_find);
        }

        chars_read = read(in_file_desc, buffer, sizeof(char) * BUFFER_SIZE);
        curr_char = buffer;
    }

    close(in_file_desc);
    close(out_file_desc);

    time_stop = clock();

    printf("Pomiar %s\n", argv[0]);
    printf("Czas = %ld\n", time_stop - time_start);

    return 0;
}
