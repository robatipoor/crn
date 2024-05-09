#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

static struct option long_options[] =
    {
        {"case", required_argument, NULL, 'c'},
        {"delimiter", required_argument, NULL, 'd'},
        {"apply", no_argument, NULL, 'a'},
        {"path", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}};

enum CaseType
{
  SNAKE_CASE,
  CAMEL_CASE,
  KEBAB_CASE,
  PASCAL_CASE,
};

int is_file(const char *path);
void rename_filename(char *filename, char find, char replace);
size_t list_files(const char *path, char ***list);

int main(int argc, char *argv[])
{
  int c;
  opterr = 0;
  int apply_flag = 0;
  enum CaseType case_type;
  char path[100] = {};
  char delimiter = 0;
  int d_flag = 0;

  while ((c = getopt_long(argc, argv, "ac:d:p:", long_options, NULL)) != -1)
    switch (c)
    {
    case 'c':
      if (strstr(optarg, "snake"))
      {
        case_type = SNAKE_CASE;
      }
      else if (strstr(optarg, "kebab"))
      {
        case_type = KEBAB_CASE;
      }
      else if (strstr(optarg, "camel"))
      {
        case_type = CAMEL_CASE;
      }
      else if (strstr(optarg, "pascal"))
      {
        case_type = PASCAL_CASE;
      }
      else
      {
        fprintf(stderr, "Invalid -c argument \n");
        exit(EXIT_FAILURE);
      }
      break;
    case 'd':
      if (strlen(optarg) == 1 && optarg[0] < 127 && optarg[0] >= 0)
      {
        delimiter = optarg[0];
        d_flag = 1;
      }
      else
      {
        fprintf(stderr, "Invalid -d argument \n");
        exit(EXIT_FAILURE);
      }

      break;
    case 'a':
      apply_flag = 1;
      break;
    case 'p':
      // memset(path, 0, sizeof(path));
      strlcpy(path, optarg, sizeof(path));
      break;
    case 'h':
      printf("crn help message \n");
      exit(EXIT_SUCCESS);
    case 'v':
      printf("0.0.1v \n");
      exit(EXIT_SUCCESS);
    case '?':
      printf("help \n");
      exit(EXIT_SUCCESS);
    default:
      printf("crn help message... %c \n", c);
      exit(EXIT_FAILURE);
    }

  return 0;
}

void rename_filename(char *filename, char find, char replace)
{

  char *current_pos = strchr(filename, find);

  while (current_pos)
  {
    *current_pos = replace;
    current_pos = strchr(current_pos + 1, find);
  }

  return;
}

size_t list_files(const char *path, char ***list)
{
  if (is_file(path))
  {
    *list = realloc(*list, sizeof(char *));
    char *name = basename(path);
    **list = malloc(sizeof(name));
    strcpy(**list, name);
    return 1;
  }

  struct dirent *dir;
  DIR *d = opendir(path);
  if (d)
  {
    int i = 0;
    while ((dir = readdir(d)) != NULL)
    {
      if (dir->d_type == DT_REG)
      {
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

int is_file(const char *path)
{
  struct stat s;
  stat(path, &s);
  return S_ISREG(s.st_mode);
}
