#ifndef DL_LOAD_H
#define DL_LOAD_H

#ifdef USE_DL
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void load_dl(char* file_path) {
    void *handle = dlopen(file_path, RTLD_NOW);

    if (handle == NULL) {
        printf("dl library not found / opened\n");
        return;
    }

    *(void**) (&init_wc_struct) = dlsym(handle, "init_wc_struct");
    *(void**) (&add_block) = dlsym(handle, "add_block");
    *(void**) (&return_block) = dlsym(handle, "return_block");
    *(void**) (&remove_block) = dlsym(handle, "remove_block");
    *(void**) (&remove_wc_struct) = dlsym(handle, "remove_wc_struct");
}

#else
void load_dl(char* file_path) {}
#endif

#endif