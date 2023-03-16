#ifndef WCLIB_DL_H
#define WCLIB_DL_H

#include <stdio.h>

typedef struct WcStruct {
	char** array;
	int max_arr_size;
	int curr_arr_size;
} WcStruct;


WcStruct* (*init_wc_struct)(size_t);

int (*add_block)(WcStruct*, char*);

char* (*return_block)(WcStruct*, size_t);

int (*remove_block)(WcStruct*, size_t);

int (*remove_wc_struct)(WcStruct*);

#endif