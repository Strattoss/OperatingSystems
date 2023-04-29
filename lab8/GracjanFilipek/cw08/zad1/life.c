#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#include "grid.h"

char *foreground;
char *background;

pthread_t *tile_updaters;

void* update_tile_routine(void* coordinates) {
    int i = ((int*)coordinates)[0];
    int j = ((int*)coordinates)[1];
    free(coordinates);

    while (true) {
        update_tile(foreground, background, i, j);
        pause();
    }

    return NULL;
}

void at_exit() {
    endwin(); // End curses mode
    destroy_grid(foreground);
    destroy_grid(background);
    free(tile_updaters);
}

void alarm_signal_handler() {
    return;
}

void init_threads() {
    pthread_t* tmp_thr;
    int *tmp_arg;

    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            tmp_thr = tile_updaters + (i * GRID_WIDTH + j);

            // prepare argument for thread function
            tmp_arg = malloc(sizeof(int) * 2);
            tmp_arg[0] = i;
            tmp_arg[1] = j;

            if (pthread_create(tmp_thr, NULL, update_tile_routine, tmp_arg) == -1) {
                perror("Failed to create a thread");
                free(tmp_arg);
                exit(1);
            }
        }
    }
}

int main() {
	srand(time(NULL));
	setlocale(LC_CTYPE, "");

    on_exit(at_exit, 0);
    signal(SIGALRM, alarm_signal_handler);

	initscr(); // Start curses mode

	foreground = create_grid();
	background = create_grid();
	char *tmp;

	init_grid(foreground);

    tile_updaters = malloc(sizeof(pthread_t*) * GRID_WIDTH * GRID_HEIGHT);
    init_threads();

    usleep(500 * 1000);

	while (true)
	{
		draw_grid(foreground);

		// step simulation
        for (pthread_t* p_thr = tile_updaters; p_thr < tile_updaters + GRID_WIDTH * GRID_HEIGHT ; p_thr++) {
            pthread_kill(*p_thr, SIGALRM);
        }

        usleep(500 * 1000);

        // switch background with foreground
		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	return 0;
}
