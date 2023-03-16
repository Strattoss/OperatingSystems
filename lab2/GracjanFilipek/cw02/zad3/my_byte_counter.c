#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_FILENAME_LEN 512

int main()
{
    DIR* curr_dir = opendir("./");
    if (curr_dir == NULL) {
        perror(NULL);
        return -1;
    }

    long long sum_sizes = 0;

    struct dirent* curr_dirent = readdir(curr_dir);

    struct stat statistict;

    while (curr_dirent != NULL)
    {
        stat(curr_dirent->d_name, &statistict);

        // if file (not directory)
        if (!S_ISDIR(statistict.st_mode)) {
            sum_sizes += statistict.st_size;
            printf("%ld B\t", statistict.st_size);

            printf("%s\n", curr_dirent->d_name);
            
        }

        curr_dirent = readdir(curr_dir);
    }

    printf("Summarized size: %lld B\n", sum_sizes);

    return 0;
}
