#ifndef WCLIB_H
#define WCLIB_H

typedef struct WcStruct {
	char** array;
	int max_arr_size;
	int curr_arr_size;
} WcStruct;

// initialize new wc_struct and return it
WcStruct* init_wc_struct(unsigned int initial_size);

// add new block to wc_struct's array
int add_block(WcStruct* wc_struct, char* file_name);

// return pointer at given index in wc_struct's aray
char* return_block(WcStruct* wc_struct, unsigned int index);

// remove block in wc_struct at given index
int remove_block(WcStruct* wc_struct, unsigned int index);

// remove (free memmory) array of wc_struct (NOT whole wc_struct)
int remove_wc_struct(WcStruct* wc_struct);

#endif