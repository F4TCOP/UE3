#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "array.h"

static inline void error (const char *msg){
	perror(msg);
	exit(-1);
}

void relate(char *out, const char *base, const char *off) {
    // 1: skip the shared prefix
    int shared = 0;
    for (; off[shared] == base[shared] && off[shared]; ++shared);
    // 2: short-circuit for off ǹ== base
    if (off[shared] == base[shared]) {
        memcpy(out, "./", 3); return;
        }
    // 3: back off to the last shared directory
    const char *ptr = off + shared;
    while (*--ptr != '/');
    shared = ptr - off + 1;
    // 4: count ascensions of base path to get to shared root
    int ascend = 0;
    for (const char *ptr = base + shared;
    ptr = strchr(ptr, '/');
    ++ptr, ++ascend);
    ascend *= 3;
    // 5: append remainder from off-path
    memmove(out + ascend, off + shared, strlen(off) - shared + 1);
    // 6: fill the ascensions with ǹ../
    memset(out, '.', ascend);
    for (char *ptr = out; ascend > 0; ptr += 3, ascend -= 3)
    ptr[2] = '/';
}



char** split(char *str, const char *sep, char **arr) {
    arrayPush(arr) = strtok(str, sep);

    while (arrayTop(arr) != NULL)
        arrayPush(arr) = strtok(NULL, sep);

    (void) arrayPop(arr);
    
    return arr;
}


void cd(char *const *args, int rfd) {
    int fd;

    if (arrayLen(args) == 1) {
        if (fchdir(rfd) < 0)
            error("cd failed");
        } else {
            fd = open(args[1], O_RDONLY | O_DIRECTORY);
            if (fd < 0 || fchdir(fd) < 0)
                perror("cd failed");
        }
    }



