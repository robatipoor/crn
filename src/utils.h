#ifndef UTIL_H
#define UTIL_H

#define _GNU_SOURCE
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>

char *join_paths(int count, ...);

void replace_all_chars(char *string, char find, char replace);
#endif // !UTIL_H
