#include <time.h>
#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 512
#define BUFFER_LEN 1024

/*
 * reverses first n chars in buffor, for ex.:
 *    abc -> cba
 *    a1b2c3 -> 3c2b1a
 *
 *  n: number of chars to reverse (counting from start of buffor)
 *
 *  returns: NULL
 */
void reverse_buffor(char *buffer, int n)
{
    char *p, *q;
    char tmp;

    p = buffer;
    q = buffer + n - 1;
    while (p < q)
    {
        tmp = *p;
        *p = *q;
        *q = tmp;

        p++;
        q--;
    }
}

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

    // writing reversed text with buffer from in_file to out_file

    fseek(in_file, -BUFFER_LEN, SEEK_END);
    long pos = ftell(in_file);

    char buffer[BUFFER_LEN];

    while (pos >= 0)
    {
        fread(buffer, sizeof(char), BUFFER_LEN, in_file);
        reverse_buffor(buffer, BUFFER_LEN);
        fwrite(buffer, sizeof(char), BUFFER_LEN, out_file);

        pos -= BUFFER_LEN;
        fseek(in_file, pos, SEEK_SET);
    }

    int chars_left = BUFFER_LEN + pos;

    fseek(in_file, 0, SEEK_SET);

    fread(buffer, sizeof(char), chars_left, in_file);
    reverse_buffor(buffer,chars_left);
    fwrite(buffer, sizeof(char), chars_left, out_file);

    fclose(in_file);
    fclose(out_file);

    time_stop = clock();

    printf("Pomiar %s\n", argv[0]);
    printf("Czas = %ld\n", time_stop - time_start);

    return 0;
}
