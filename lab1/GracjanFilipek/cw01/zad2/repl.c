#ifdef USE_DL
#include "wclib_dl.h"
#else
#include "wclib.h"
#endif

#include "dl_load.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define COMMAND_SIZE 1024

int main()
{
    load_dl("libwclib.so");
    WcStruct *wc_struct = NULL;
    int init_size, index;
    char *file_path, *p_end;

    char command[COMMAND_SIZE];
    char delims[] = " \n";

    char *curr_p = command;

    while (1)
    {
        printf("repl> ");
        fgets(command, COMMAND_SIZE, stdin);

        curr_p = strtok(command, delims);
        if (curr_p == NULL)
        {
            continue;
        }

        else if (strcmp(curr_p, "init") == 0)
        {
            curr_p = strtok(NULL, delims);
            if (curr_p == NULL)
            {
                printf("you have to specify initial size of the array\n");
                continue;
            }
            init_size = (int)strtol(curr_p, &p_end, 10);
            if (curr_p == p_end)
            {
                printf("no number found after \"init\", correct your command\n");
                continue;
            }
            if (p_end[0] != '\0' && p_end[0] != '\n')
            {
                printf("unexpected string of characters \"%s\" after the number, corret your command\n", p_end);
                continue;
            }
            if (init_size <= 0)
            {
                printf("\"%s\" is not a correct size. It has to be a positive integer\n", curr_p);
                continue;
            }
            curr_p = strtok(NULL, delims);
            if (curr_p != NULL)
            {
                printf("unexpected string \"%s\" after the positive integer, fix your command\n", curr_p);
                continue;
            }
            if (wc_struct != NULL)
            {
                printf("there already exist an array, remove it first before initializing\n");
                continue;
            }

            wc_struct = init_wc_struct(init_size);
        }

        else if (strcmp(curr_p, "count") == 0)
        {
            curr_p = strtok(NULL, delims);
            if (curr_p == NULL)
            {
                printf("you have to specify path to the file\n");
                continue;
            }
            file_path = curr_p;
            curr_p = strtok(NULL, delims);
            if (curr_p != NULL)
            {
                printf("unexpected string \"%s\" after file path, fix your command\n", curr_p);
                continue;
            }
            if (wc_struct == NULL)
            {
                printf("structure hasn't been initialized yet\n");
                continue;
            }
            if (add_block(wc_struct, file_path) == -1)
            {
                printf("couldn't open the file with path \"%s\" or read permission not granted\n", file_path);
                continue;
            }
        }

        else if (strcmp(curr_p, "show") == 0)
        {
            curr_p = strtok(NULL, delims);
            if (curr_p == NULL)
            {
                printf("you have to specify index of a block to show from the array\n");
                continue;
            }
            index = (int)strtol(curr_p, &p_end, 10);
            if (curr_p == p_end)
            {
                printf("no number found after \"show\", correct your command\n");
                continue;
            }
            if (p_end[0] != '\0' && p_end[0] != '\n')
            {
                printf("unexpected string of characters \"%s\" after the number, corret your command\n", p_end);
                continue;
            }

            if (index < 0)
            {
                printf("\"%s\" is not a correct index. It has to be a nonnegative integer\n", curr_p);
                continue;
            }
            if (wc_struct == NULL)
            {
                printf("the array doesn't exist, initialize it first\n");
                continue;
            }
            curr_p = strtok(NULL, delims);
            if (curr_p != NULL)
            {
                printf("unexpected string \"%s\" after the positive integer, fix your command\n", curr_p);
                continue;
            }
            if (index > wc_struct->curr_arr_size - 1)
            {
                printf("block with this index doesn't exist, because your index is %d and there is %d currently allocated block in the array\n", index, wc_struct->curr_arr_size);
                continue;
            }
            printf("Block: %s", return_block(wc_struct, index));
        }

        else if (strcmp(curr_p, "delete") == 0)
        {
            curr_p = strtok(NULL, delims);
            if (curr_p == NULL)
            {
                printf("you have to write \"index\" after \"delete\"\n");
                continue;
            }
            if (strcmp(curr_p, "index") != 0)
            {
                printf("after \"delete\" you have write \"index\" and then give an index\n");
                continue;
            }

            curr_p = strtok(NULL, delims);
            if (curr_p == NULL)
            {
                printf("no index to delete given\n");
                continue;
            }

            index = (int)strtol(curr_p, &p_end, 10);
            if (curr_p == p_end)
            {
                printf("no number found after \"index\", correct your command\n");
                continue;
            }
            if (p_end[0] != '\0' && p_end[0] != '\n')
            {
                printf("unexpected string of characters \"%s\" after the number, corret your command\n", p_end);
                continue;
            }

            if (index < 0)
            {
                printf("%s is not a correct index, it has to be a nonnegative integer\n", curr_p);
                continue;
            }
            if (wc_struct == NULL)
            {
                printf("the array hasn't been allocated yet, initialize it first\n");
                continue;
            }
            if (index > wc_struct->curr_arr_size - 1)
            {
                printf("block with this index doesn't exist, because your index is %d and there is %d currently allocated block in the array\n", index, wc_struct->curr_arr_size);
                continue;
            }
            curr_p = strtok(NULL, delims);
            if (curr_p != NULL)
            {
                printf("unexpected string \"%s\" after the positive integer, fix your command", curr_p);
                continue;
            }

            remove_block(wc_struct, index);
        }

        else if (strcmp(curr_p, "destroy") == 0)
        {
            curr_p = strtok(NULL, delims);
            if (curr_p != NULL)
            {
                printf("unexpected string \"%s\" after the \"destroy\", fix your command\n", curr_p);
                continue;
            }
            // remove all allocated blocks in wc_struct's array
            if (wc_struct == NULL)
            {
                printf("the array doesn't exist, cannot remove it\n");
                continue;
            }
            remove_wc_struct(wc_struct);
            wc_struct = NULL;
        }

        else if (strcmp(curr_p, "quit") == 0)
        {
            curr_p = strtok(NULL, delims);
            if (curr_p != NULL)
            {
                printf("unexpected string \"%s\" after the \"quit\", fix your command\n", curr_p);
                continue;
            }
            // remove all allocated blocks in wc_struct's array
            if (wc_struct != NULL)
            {
                remove_wc_struct(wc_struct);
            }

            wc_struct = NULL;
            break;
        }

        else
        {
            printf("command \"%s\" not recognised\n", curr_p);
        }
    }
    return 0;
}