#include "utils.h"

char *join_paths(int count, ...)
{
  if (count <= 0)
  {
    return NULL;
  }

  char *new_path = NULL;
  size_t total_length = 0;
  va_list args;
  va_start(args, count);

  for (int i = 0; i < count; i++)
  {
    const char *path = va_arg(args, char *);
    if (path == NULL)
    {
      continue;
    }
    size_t path_length = strlen(path) + (i == 0 ? 0 : 1);

    char *temp_path = realloc(new_path, total_length + path_length + 1);
    if (temp_path == NULL)
    {
      free(new_path);
      perror("Unable to allocate memory");
      return NULL;
    }
    new_path = temp_path;
    if (i == 0)
    {
      new_path[0] = '\0';
    }
    else
    {
      strcat(new_path, "/");
    }
    strcat(new_path, path);
    total_length += path_length;
  }
  va_end(args);

  return new_path;
}
void replace_all_chars(char *string, char find, char replace)
{
  char *current_pos = strchr(string, find);
  while (current_pos)
  {
    *current_pos = replace;
    current_pos = strchr(current_pos + 1, find);
  }
}
