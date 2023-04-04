#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 64
#define BOUND_L 0.0
#define BOUND_R 1.0


double fun_to_integrate(double x) {
    return 4/(x*x + 1);
}


// integrate from *a* to *b* with step of *eps*
double calc_integral(double a, double b, double eps) {
    double sum = 0;
    double x = a;

    while (x + eps < b) {
        sum += fun_to_integrate(x + eps/2) * eps;

        x += eps;
    }

    // add one last rectangle (it stretches from current x to b)
    sum += fun_to_integrate((x + b) / 2) * (b - x);

    return sum;
}


double read_pipes_and_sum_results(int num_of_processes, int* descriptors_to_read) {
    double integral_sum = 0;
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < num_of_processes; ++i) {
        if (read(descriptors_to_read[i], buffer, BUFFER_SIZE) == -1) {
            perror("Failed to read a pipe");
            exit(1);
        }
        close(descriptors_to_read[i]);
        integral_sum += atof(buffer);
    }
    return integral_sum;
}


long convert_to_nanoseconds(struct timespec* start, struct timespec* end) {
    return end->tv_nsec + (end->tv_sec - start->tv_sec)*1000000000 - start->tv_nsec;
}


int main(int argc, char **argv)
{
    /* ####################
     * ARGUMENTS VALIDATION
     * #################### */

    if (argc != 1 + 2)
    {
        fprintf(stderr, "Expected exactly 2 arguments:\n"
                        "\tprecision (length of rectangles' side)\n"
                        "\tnumber of child processes start\n"
                        "Got %d arguments\n",
                argc - 1);
        return 1;
    }

    char* tmp_ptr;
    double eps = strtod(argv[1], &tmp_ptr);
    if (*tmp_ptr != '\0') {
        fprintf(stderr, "Requested precision is not a number: %s\n", argv[1]);
        exit(1);
    }

    int num_of_proc = strtol(argv[2], &tmp_ptr, 10);
    if (*tmp_ptr != '\0') {
        fprintf(stderr, "Requested number of processes is not an integer: %s\n", argv[2]);
        exit(1);
    }


    /* ###################
     * START CALCULATIONS
     * ################### */

    // declaration of variables
    int child_to_parent[2];
    int descriptors_to_read[num_of_proc];
    double x_step = (BOUND_R - BOUND_L) / num_of_proc;
    char buffer[BUFFER_SIZE];

    // start measuring time
    struct timespec time_start, time_end;
    clock_gettime(CLOCK_REALTIME, &time_start);

    //calculation loop
    for (int i = 0; i < num_of_proc; ++i) {
        // open pipe
        pipe(child_to_parent);

        switch (fork()) {
            case -1:
                perror("Failed to fork a process");
                exit(1);
            case 0: // child
                // close unnecessary file descriptor
                close(child_to_parent[0]);

                double result = calc_integral(BOUND_L + x_step * i, BOUND_L + x_step * (i+1), eps);
                snprintf(buffer, BUFFER_SIZE, "%.15f", result);

                write(child_to_parent[1], buffer, BUFFER_SIZE);
                close(child_to_parent[1]);
                exit(0);
            default:    // parent
                // close unnecessary file descriptor
                close(child_to_parent[1]);

                // copy file descriptor so we can use it later
                descriptors_to_read[i] = child_to_parent[0];
                break;
        }
    }

    // parent gathers results from children and adds them up
    read_pipes_and_sum_results(num_of_proc, descriptors_to_read);

    clock_gettime(CLOCK_REALTIME, &time_end);

    printf("%ld\n", convert_to_nanoseconds(&time_start, &time_end));

    return 0;
}
