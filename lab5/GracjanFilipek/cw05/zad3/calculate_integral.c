#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 64

double fun_to_integrate(double x) {
    return 4/(x*x + 1);
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


int main(int argc, char* argv[]) {

    if (argc != 1 + 4)
    {
        fprintf(stderr, "Expected exactly 4 arguments:\n"
                        "\tleft bound\n"
                        "\tright bound\n"
                        "\tprecision (length of rectangles' side)\n"
                        "\tname of named pipe\n"
                        "Got %d arguments\n",
                argc - 1);
        return 1;
    }


    double eps = convert_str_to_double(argv[3]);
    double left_bound = convert_str_to_double(argv[1]);
    double right_bound = convert_str_to_double(argv[2]);

    int pipe_descriptor = open(argv[4], O_WRONLY);
    if (pipe_descriptor == -1) {
        perror("Couldn't open the named pipe");
        exit(1);
    }

    double result = calc_integral(left_bound, right_bound, eps);

    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "%.15f", result);

    if (write(pipe_descriptor, buffer, BUFFER_SIZE) == -1) {
        perror("Failed to write into the named pipe");
        exit(1);
    }

    if (close(pipe_descriptor) == -1) {
        perror("Couldn't close the named pipe");
        exit(1);
    }

    return 0;
}
