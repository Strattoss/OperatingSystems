#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_SIZE 64
#define BOUND_L 0.0
#define BOUND_R 1.0


double fun_to_integr(double x) {
    return 4/(x*x + 1);
}


// integrate from *a* to *b* with step of *eps*
double calc_integral(double a, double b, double eps) {
    double sum = 0;
    double x = a;

    while (x + eps <= b) {
        sum += fun_to_integr(x + eps/2) * eps;

        x += eps;
    }

    return sum;
}


int main(int argc, char **argv)
{
    /* ####################
     * ARGUMENTS VALIDATION
     * #################### */

    if (argc != 1 + 2)
    {
        fprintf(stderr, "Expected exactly 2 arguments:\n"
                        "\tprecision (length of rectangles' side\n"
                        "\tnumber of child processes start\n"
                        "Got %d arguments\n",
                argc - 1);
        return 1;
    }

    char* tmp_ptr;
    double eps = strtod(argv[1], &tmp_ptr);
    if (*tmp_ptr != '\0') {
        fprintf(stderr, "Requested precision not a number: %s\n", argv[1]);
        exit(1);
    }

    int num_of_proc = strtol(argv[2], &tmp_ptr, 10);
    if (*tmp_ptr != '\0') {
        fprintf(stderr, "Requested number of processes nor a number: %s\n", argv[2]);
        exit(1);
    }


    /* ###################
     * START CALCULATIONS
     * ################### */

    // declaration of variables
    int child_to_parent[2];
    int read_file_descriptors[num_of_proc];
    int pid;
    double x_step = (BOUND_R - BOUND_L) / num_of_proc;
    double integral_sum = 0;
    char buffer[BUFFER_SIZE];

    // start measuring time
    struct timespec time_start, time_end;
    clock_gettime(CLOCK_REALTIME, &time_start);

    //calculation loop
    for (int i = 0; i < num_of_proc; ++i) {
        // open pipe
        pipe(child_to_parent);

        pid = fork();
        if (pid == -1) {
            perror("Failed to fork a process");
            exit(1);
        }
        if (pid == 0) { //child
            // close unnecessary file descriptor
            close(child_to_parent[0]);

            double result = calc_integral(BOUND_L + x_step * i, BOUND_L + x_step * (i+1), eps);
            snprintf(buffer, BUFFER_SIZE, "%.15f", result);

            write(child_to_parent[1], buffer, BUFFER_SIZE);
            close(child_to_parent[1]);
            exit(0);
        }
        else {  //parent
            // close unnecessary file descriptor
            close(child_to_parent[1]);

            // copy file descriptor so we can use it later
            read_file_descriptors[i] = child_to_parent[0];
        }
    }

    // parent gathers results from children and adds them up
    for (int i = 0; i < num_of_proc; ++i) {
        if (read(read_file_descriptors[i], buffer, BUFFER_SIZE) == -1) {
            perror("Failed to read a pipe");
            exit(1);
        }
        close(read_file_descriptors[i]);
        integral_sum += atof(buffer);
    }

    clock_gettime(CLOCK_REALTIME, &time_end);

    printf("%ld\n", time_end.tv_nsec + (time_end.tv_sec - time_start.tv_sec)*1000000000 - time_start.tv_nsec);
    //printf("%.15f\n", integral_sum);

    return 0;
}
