#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    // arguments validation
    if (argc != 1 + 1)
    {
        fprintf(stderr, "Expected exactly 1 argument:\n"
                        "\tpath to a directory\n"
                        "Got %d arguments\n",
                argc - 1);
        return -1;
    }

    printf("%s ", argv[0]);
    
    fflush(stdout);

    int ret = execl("/bin/ls", "ls", argv[1], NULL);
    if (ret == -1) {
        perror("error ");
    }
    

    return 0;
}
