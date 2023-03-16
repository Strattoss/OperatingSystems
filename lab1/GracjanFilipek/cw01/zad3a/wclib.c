#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "wclib.h"

WcStruct* init_wc_struct(size_t initial_size)
{
	WcStruct* res = malloc(sizeof(WcStruct));
	res->array = calloc(initial_size, sizeof(char *));
	res->max_arr_size = initial_size;
	res->curr_arr_size = 0;
	return res;
}

int add_block(WcStruct *wc_struct, char *file_name)
{
	// if file exists and if we can read it
	if (access(file_name, R_OK) == -1) {
		return -1;
	}
	char command[255];
	char tmp_file_name[] = "/tmp/.wclibXXXXXX";
	int file_descriptor = mkstemp(tmp_file_name);
	if (file_descriptor == -1)
	{
		return -1;
	}
	// printf("%s\n", tmp_file_name);

	snprintf(command, 255, "wc '%s' 1>'%s' 2>/dev/null", file_name, tmp_file_name);
	// printf("sklejona komenda: %s\n", command);

	if(system(command) == -1) {
		return -1;
	}

	FILE *tmp_file = fdopen(file_descriptor, "r");
	if (tmp_file == NULL)
	{
		return -1;
	}

	char buffor[255];
	if (fgets(buffor, 255, tmp_file) == NULL) {
		return -1;
	}

	unlink(tmp_file_name);
	fclose(tmp_file);

	snprintf(command, 255, "rm -f '%s' 2>/dev/null", tmp_file_name);
	if(system(command) == -1) {
		return -1;
	}


	// printf("Wczytane z pliku: %s\n", buffor);

	char *block = malloc(sizeof(char) * (strlen(buffor)+1));
	strcpy(block, buffor);
	// printf("Zapisane w bloku: %s\n", block);

	// if wc_struct is full

	if (wc_struct->curr_arr_size == wc_struct->max_arr_size)
	{
		wc_struct->array = realloc(wc_struct->array, sizeof(char *) * wc_struct->max_arr_size * 2);
		wc_struct->max_arr_size = wc_struct->max_arr_size * 2;
	}

	wc_struct->array[wc_struct->curr_arr_size] = block;
	wc_struct->curr_arr_size += 1;

	return 0;
}

char *return_block(WcStruct *wc_struct, size_t index)
{
	// if block on given index even exists
	if (index >= wc_struct->curr_arr_size)
	{
		return NULL;
	}
	return wc_struct->array[index];
}

int remove_block(WcStruct *wc_struct, size_t index)
{
	// if index is in currently occupied wc_struct's array indexes
	if (wc_struct->curr_arr_size - 1 < index)
	{
		return -1;
	}
	// at the end of wc_struct's array
	if (wc_struct->curr_arr_size - 1 == index)
	{
		wc_struct->array[index] = NULL;
		wc_struct->curr_arr_size--;
	}
	// in the middle (or begining) of the array
	else
	{
		// remove the block which is being pointed to by wc_struct'->array[index]
		free(wc_struct->array[index]);
		// shift indexes, so we don't heve hole in the middle
		memmove(wc_struct->array + index, wc_struct->array + index + 1, sizeof(char *) * (wc_struct->curr_arr_size - index - 1));
		// set last pointer to null (so it looks pretty)
		wc_struct->array[wc_struct->curr_arr_size - 1] = NULL;
		wc_struct->curr_arr_size--;
	}
	return 0;
}

int remove_wc_struct(WcStruct *wc_struct)
{
	// remove all allocated blocks in wc_struct's array
	while (wc_struct->curr_arr_size > 0)
	{
		if (remove_block(wc_struct, 0) != 0)
		printf("usun\n");
		{
			break;
		}
	}
	free(wc_struct->array);
	free(wc_struct);
	return 0;
}
