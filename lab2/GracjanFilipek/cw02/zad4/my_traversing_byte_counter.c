#include <stdio.h>
#include <sys/stat.h>
#include <ftw.h>

#define MAX_FILENAME_LEN 512

long long sum_sizes = 0;

int fun(const char *fpath, const struct stat *sb, int typeflag)
{
    // stat failed
    if (typeflag == FTW_NS) {
        fprintf(stderr, "The stat (2) call failed on %s\n", fpath);
        return -1;
    }

    // not directory
    if (typeflag == FTW_D) {
        return 0;
    }

    printf("%ld B\t", sb->st_size);
    printf("%s\n", fpath);

    sum_sizes += (long long)sb->st_size;
    
    return 0;
}

int main(int argc, char **argv)
{
    // arguments validation
    if (argc != 1 + 1)
    {
        fprintf(stderr, "Expected exactly 1 arguments:\n"
                        "\tdirectory name\n"
                        "Got %d arguments\n",
                argc - 1);
        return -1;
    }

    
    ftw(argv[1],&fun, 0);
    
    printf("Summarized size: %lld B\n", sum_sizes);

    return 0;
}
