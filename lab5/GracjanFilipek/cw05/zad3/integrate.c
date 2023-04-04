#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 64
#define BOUND_L 0.0
#define BOUND_R 1.0
#define PIPE_PATH "/tmp/integrate"


long convert_to_nanoseconds(struct timespec* start, struct timespec* end) {
    return end->tv_nsec + (end->tv_sec - start->tv_sec)*1000000000 - start->tv_nsec;
}


void remove_pipe(const char* path) {
    if (remove(path) == -1) {
        perror("Couldn't delete created pipe");
        exit(1);
    }
}

void close_pipe(int file_descriptor) {

    if (close(file_descriptor) == -1) {
        perror("Couldn't close created pipe");
        exit(1);
    }
}

int open_pipe(const char* path) {
    // we never actually write in the file;
    // "write" flag is necessary so this program doesn't delete named pipe before all other programs write required results
    // (a situation could occur where last executed program didn't open the pipe to write in it but
    // this program would actually start reading the pipe; in this situation the pipe would be closed, because no one
    // would be registered as "writer")
    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("Couldn't open created pipe");
        remove_pipe(path);
        exit(1);
    }

    return fd;
}

void create_pipe(const char* path) {
    mode_t mode = S_ISUID | S_IREAD | S_IWUSR | S_IXUSR;
    if (mkfifo(path, mode) == -1) {
        fprintf(stderr, "Failed to create named pipe in %s\n", PIPE_PATH);
        perror(NULL);
        exit(1);
    }
}


double convert_str_to_double(char* ptr) {
    char* tmp_ptr;
    double result = strtod(ptr, &tmp_ptr);
    if (*tmp_ptr != '\0') {
        fprintf(stderr, "%s is not a double\n", ptr);
        exit(1);
    }

    return result;
}

int convert_str_to_int(char* ptr) {
    char *tmp_ptr;
    int result = strtol(ptr, &tmp_ptr, 10);
    if (*tmp_ptr != '\0') {
        fprintf(stderr, "%s is not an integer\n", ptr);
        exit(1);
    }

    return result;
}

int main(int argc, char **argv)
{
    if (argc != 1 + 2)
    {
        fprintf(stderr, "Expected exactly 2 arguments:\n"
                        "\tprecision (length of rectangles' side)\n"
                        "\tnumber of child processes start\n"
                        "Got %d arguments\n",
                argc - 1);
        return 1;
    }

    double eps = convert_str_to_double(argv[1]);
    int num_of_proc = convert_str_to_int(argv[2]);


    /* ###################
     * START CALCULATIONS
     * ################### */

    // declaration of variables
    double x_step = (BOUND_R - BOUND_L) / num_of_proc;
    char exec_l_bound[BUFFER_SIZE], exec_r_bound[BUFFER_SIZE], exec_eps[BUFFER_SIZE];
    struct timespec time_start, time_end;

    snprintf(exec_eps, BUFFER_SIZE, "%.15f", eps);

    create_pipe(PIPE_PATH);

    clock_gettime(CLOCK_REALTIME, &time_start);

    //calculation loop
    for (int i = 0; i < num_of_proc; ++i) {
        switch (fork()) {
            case -1:
                perror("Failed to fork a process");
                exit(1);
            case 0: // child
                snprintf(exec_l_bound, BUFFER_SIZE, "%.15f", BOUND_L + x_step * i);
                snprintf(exec_r_bound, BUFFER_SIZE, "%.15f", BOUND_L + x_step * (i+1));

                execl("./calculate_integral.exe", "calculate_integral", exec_l_bound, exec_r_bound, exec_eps, PIPE_PATH, NULL);
                /*double result = calc_integral(BOUND_L + x_step * i, BOUND_L + x_step * (i+1), eps);*/
                /*snprintf(buffer, BUFFER_SIZE, "%.15f", result);*/
            default:    // parent
                break;
        }
    }

    char buffer[BUFFER_SIZE];

    int pipe_descriptor = open_pipe(PIPE_PATH);

    for (int i = 0; i < num_of_proc; ++i) {
        read(pipe_descriptor, buffer, BUFFER_SIZE);
        //printf("%s\n", buffer);
    }

    clock_gettime(CLOCK_REALTIME, &time_end);

    printf("%ld\n", convert_to_nanoseconds(&time_start, &time_end));


    close_pipe(pipe_descriptor);
    remove_pipe(PIPE_PATH);

    return 0;
}
