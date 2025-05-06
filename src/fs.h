#ifndef FS_H
#define FS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int is_directory(const char *path);
int fs_rename(char *old_filename, char *new_filename);
char **list_files(const char *directory_path, int *file_count);

#endif // !FS_H
