#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILENAME_LEN 512

int main(int argc, char **argv)
{
    clock_t time_start, time_stop;

    time_start = clock();

    char in_filename[MAX_FILENAME_LEN];
    char out_filename[MAX_FILENAME_LEN];

    // arguments validation
    if (argc != 2 + 1)
    {
        fprintf(stderr, "Expected exactly 2 arguments:\n"
                        "\tinput file name\n"
                        "\toutput file name\n"
                        "Got %d arguments\n",
                argc - 1);
        return -1;
    }

    // renaming arguments for better readability
    strcpy(in_filename, argv[1]);
    strcpy(out_filename, argv[2]);

    FILE *in_file = fopen(in_filename, "r");

    if (!in_file)
    {
        perror(in_filename);
        return -1;
    }

    FILE *out_file = fopen(out_filename, "w");

    if (!out_file)
    {
        perror(out_filename);
        return -1;
    }

    // writing reversed text byte by byte from in_file to out_file

    // set pointer at the end of the input file
    fseek(in_file, -1, SEEK_END);

    // current pointer position (from the start of the file)
    int pos = ftell(in_file);;

    char read_char;

    while (pos >= 0)
    {
        fread(&read_char, sizeof(char), 1, in_file);

        fwrite(&read_char, sizeof(char), 1, out_file);

        pos--;
        fseek(in_file, pos, SEEK_SET);
    }

    fclose(in_file);
    fclose(out_file);

    time_stop = clock();

    printf("Pomiar %s\n", argv[0]);
    printf("Czas = %ld\n", time_stop - time_start);

    return 0;
}
