#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>

#define MAX_STR_LENGTH 255

/*
 * Create new process of "rec_ls.exe" to look through given directory.
 * Arguments:
 * direntry: directory in which new rec_ls.exe will be executed
 * new_dir_path: path of directory for which the function create fork new process
 * searched_string: new process of rec_ls.exe will look for this string at the start of files in the new directory
 */
int handle_directory(const char *new_dir_path, const char *searched_string)
{
    //printf("    dirent: %s\n", new_dir_path);
    fflush(stdout);

    if (fork() == 0)
    {
        /*printf("\033[0;31m");
        printf("exec: %s, %s\n", new_dir_path, searched_string);
        printf("\033[0m");*/
        fflush(stdout);
        if (execl("./rec_search.exe", "rec_search.exe", new_dir_path, searched_string, NULL) == -1)
        {
            perror(NULL);
            return -1;
        }
    }

    return 0;
}

int handle_not_directory(const char* file_path, const char* searched_string)
{
    //printf("    file: %s\n", file_path);

    FILE* my_file = fopen(file_path, "r");
    if (my_file == NULL) {
        fprintf(stderr, "%s ", file_path);
        perror(NULL);
        return -1;
    }

    char buffer[MAX_STR_LENGTH + 1] = "\0";

    errno = 0;
    int chars_read = (fread(buffer, sizeof(char), MAX_STR_LENGTH + 1, my_file) == 0);
    if (chars_read == 0 && errno != 0) {
        perror(NULL);
        return -1;
    }

    if (strncmp(buffer, searched_string, strlen(searched_string)) == 0) {
        printf("%s, %d\n", file_path, getpid());
    }

    fclose(my_file);

    return 0;
}

/*
 * Returns next direntry in a directory.
 * If an error occured, print it with perror and return NULL
 */
struct dirent *next_dirent(DIR *directory)
{
    errno = 0;
    struct dirent *directory_entry = readdir(directory);
    if (directory_entry == NULL && errno != 0)
    {
        perror(NULL);
        return NULL;
    }
    return directory_entry;
}

/*
* This function looks at each file in given directory.
* If file - check if it starts with given string; if yes, print path to it
* If directory - create separate process to search through files of the directory 
*/
int rec_ls(const char *curr_dir_path, const char *searched_string)
{
    // open a directory
    DIR *directory = opendir(curr_dir_path);
    if (directory == NULL)
    {
        fprintf(stderr, "Couldn't open directory %s\n", curr_dir_path);
        perror(NULL);
        return -1;
    }

    // looking through files in the current directory
    struct dirent *my_dirent = next_dirent(directory);
    struct stat entry_stat;
    int return_code;
    char my_direntry_path_buffor[PATH_MAX];

    while (my_dirent != NULL)
    {
        // skip "." and ".." folders
        if (strcmp(my_dirent->d_name, ".") == 0 || strcmp(my_dirent->d_name, "..") == 0)
        {
            my_dirent = next_dirent(directory);
            continue;
        }

        // prepare this dirent's path
        sprintf(my_direntry_path_buffor, "%s/%s", curr_dir_path, my_dirent->d_name);

        if (stat(my_direntry_path_buffor, &entry_stat) == -1)
        {
            perror(NULL);
            my_dirent = next_dirent(directory);
        }

        return_code = (S_ISDIR(entry_stat.st_mode) == 1) ?
            handle_directory(my_direntry_path_buffor, searched_string) :
            handle_not_directory(my_direntry_path_buffor, searched_string);

        if (return_code != 0)
        {
            // error occured, perror already called in handle_directory ot handle_not_directory
        }

        my_dirent = next_dirent(directory);
    }

    closedir(directory);
    return 0;
}

int main(int argc, char **argv)
{
    // arguments validation
    if (argc != 2 + 1)
    {
        fprintf(stderr, "Expected exactly 2 argument:\n"
                        "\tpath to a directory\n"
                        "\tstring of characters shorter or equal to 255 characters\n"
                        "Got %d arguments\n",
                argc - 1);
        return -1;
    }

    if (strlen(argv[2]) > MAX_STR_LENGTH)
    {
        fprintf(stderr, "length of string of characters exceeds %d, its length: %ld", MAX_STR_LENGTH, strlen(argv[2]));
        return -1;
    }

    // run recursive function
    rec_ls(argv[1], argv[2]);

    return 0;
}
