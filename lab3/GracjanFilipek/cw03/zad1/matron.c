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
                        "\tnumber of processes to make\n"
                        "Got %d arguments\n",
                argc - 1);
        return -1;
    }

    char *last_char = NULL;
    int processes = strtol(argv[1], &last_char, 10);

    if (*last_char != '\0')
    {
        fprintf(stderr, "unexpected character %d in %s\n", *last_char, argv[1]);
        return -1;
    }

    // forking processes
    pid_t child_pid;

    for (int i = 0; i < processes; i++)
    {
        child_pid = fork();

        if (child_pid == -1)
        {
            perror(NULL);
            return -1;
        }

        if (child_pid == 0)
        {
            printf("%d %d\n", (int)getpid(), (int)getppid());
            return 0;
        }
    }

    waitpid(child_pid, NULL, 0);
    printf("%d %d\n", (int)getpid(), (int)getppid());

    return 0;
}
