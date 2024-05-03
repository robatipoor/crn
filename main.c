#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

enum RenameType {
  SNAKE_CASE,
  CAMEL_CASE,
  KEBAB_CASE,
  PASCAL_CASE,
};

int is_file(const char *path);
void rename_filename(char *filename, char find, char replace);
size_t list_files(const char *path, char ***list);

int main() { return 0; }

void rename_filename(char *filename, char find, char replace) {

  char *current_pos = strchr(filename, find);

  while (current_pos) {
    *current_pos = replace;
    current_pos = strchr(current_pos + 1, find);
  }

  return;
}

size_t list_files(const char *path, char ***list) {
  if (is_file(path)) {
    *list = realloc(*list, sizeof(char *));
    char *name = basename(path);
    **list = malloc(sizeof(name));
    strcpy(**list, name);
    return 1;
  }

  struct dirent *dir;
  DIR *d = opendir(path);
  if (d) {
    int i = 0;
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_type == DT_REG) {
        *list = realloc(*list, (i + 1) * sizeof(char *));
        (*list)[i] = malloc(sizeof(dir->d_name));
        strcpy((*list)[i], dir->d_name);
        i++;
      }
    }
    closedir(d);
    return i;
  }

  return 0;
}

int is_file(const char *path) {
  struct stat s;
  stat(path, &s);
  return S_ISREG(s.st_mode);
}
