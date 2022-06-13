#ifndef SHELL_H_INCLUDED
#define SHELL_H_INCLUDED

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

void relate(char *out, const char *base, const char *off);

char** split(char *str, const char *sep, char **arr);

void cd(char *const *args, int rfd);

#endif 