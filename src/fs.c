#define _GNU_SOURCE
#include <ctype.h>
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fs.h"

int is_directory(const char *path)
{
  struct stat path_stat;
  if (stat(path, &path_stat) != 0)
  {
    return -1;
  }

  if (S_ISDIR(path_stat.st_mode))
  {
    return 1;
  }
  else if (S_ISREG(path_stat.st_mode))
  {
    return 2;
  }
  else
  {
    return 0;
  }
}

int fs_rename(char *old_filename, char *new_filename)
{
  int result = rename(old_filename, new_filename);

  if (result == 0)
  {
    printf("Entity '%s' renamed to '%s' successfully.\n", old_filename, new_filename);
  }
  else
  {
    perror("Error renaming file");
  }

  return result;
}

char **list_files(const char *directory_path, int *file_count)
{
  struct dirent *entry;
  DIR *dp = opendir(directory_path);
  char **file_list = NULL;
  int count = 0;

  if (dp == NULL)
  {
    perror("opendir");
    return NULL;
  }

  // First, count the number of files
  while ((entry = readdir(dp)) != NULL)
  {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
      count++;
    }
  }

  // Allocate memory for the file list
  file_list = malloc(count * sizeof(char *));
  if (file_list == NULL)
  {
    perror("malloc");
    closedir(dp);
    return NULL;
  }

  // Reset the directory stream to read the entries again
  rewinddir(dp);
  count = 0;

  // Read the entries again and store them in the file list
  while ((entry = readdir(dp)) != NULL)
  {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
      file_list[count] = strdup(entry->d_name); // Duplicate the string
      if (file_list[count] == NULL)
      {
        perror("strdup");
        // Free previously allocated memory in case of error
        for (int i = 0; i < count; i++)
        {
          free(file_list[i]);
        }
        free(file_list);
        closedir(dp);
        return NULL;
      }
      count++;
    }
  }

  closedir(dp);
  *file_count = count; // Set the count of files found
  return file_list;    // Return the list of file names
}