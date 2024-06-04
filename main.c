#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <getopt.h>

typedef int path_state;

static struct option long_options[] =
    {
        {"case", required_argument, NULL, 'c'},
        {"delimiter", required_argument, NULL, 'd'},
        {"apply", no_argument, NULL, 'a'},
        {"path", required_argument, NULL, 'p'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}};

enum CaseType
{
  UNKNOWN,
  SNAKE_CASE,
  CAMEL_CASE,
  KEBAB_CASE,
  PASCAL_CASE,
};

path_state check_path(const char *path);
void rename_filename(char *filename, char find, char replace);
size_t list_files(const char *path, char ***list);
void print_help_message(void);
void rename_to_snake_case(char **filename, char find);
void fs_rename(char *old_filename, char *new_filename);
void join_paths(char *buffer, const char *path1, const char *path2);

int main(int argc, char *argv[])
{
  // int c;
  // opterr = 0;
  // int apply_flag = 0;
  // enum CaseType case_type = UNKNOWN;
  // char path[MAX_PATH] = {};
  // path_state p_state = -1;
  // char ***files;
  // char delimiter = '\0';
  // int d_flag = 0;

  // while ((c = getopt_long(argc, argv, "ac:d:p:hv", long_options, NULL)) != -1)
  //   switch (c)
  //   {
  //   case 'c':
  //     if (strstr(optarg, "snake"))
  //     {
  //       case_type = SNAKE_CASE;
  //     }
  //     else if (strstr(optarg, "kebab"))
  //     {
  //       case_type = KEBAB_CASE;
  //     }
  //     else if (strstr(optarg, "camel"))
  //     {
  //       case_type = CAMEL_CASE;
  //     }
  //     else if (strstr(optarg, "pascal"))
  //     {
  //       case_type = PASCAL_CASE;
  //     }
  //     else
  //     {
  //       fprintf(stderr, "Invalid -c argument \n");
  //       exit(EXIT_FAILURE);
  //     }
  //     break;
  //   case 'd':
  //     if (strlen(optarg) == 1 && optarg[0] < 127 && optarg[0] >= 0)
  //     {
  //       delimiter = optarg[0];
  //       d_flag = 1;
  //     }
  //     else
  //     {
  //       fprintf(stderr, "Invalid -d argument \n");
  //       exit(EXIT_FAILURE);
  //     }
  //     break;
  //   case 'a':
  //     apply_flag = 1;
  //     break;
  //   case 'p':
  //     strlcpy(path, optarg, sizeof(path));
  //     path[strcspn(path, "\n")] = 0;
  //     p_state = check_path(path);
  //     if (p_state == -1)
  //     {
  //       fprintf(stderr, "Invalid file system path: %s\n", path);
  //       exit(EXIT_FAILURE);
  //     }
  //     break;
  //   case 'h':
  //     print_help_message();
  //     exit(EXIT_SUCCESS);
  //   case 'v':
  //     printf("0.0.1v \n");
  //     exit(EXIT_SUCCESS);
  //   case '?':
  //     printf("Unknown option: %c\n", optopt);
  //     print_help_message();
  //     exit(EXIT_SUCCESS);
  //   default:
  //     print_help_message();
  //     exit(EXIT_FAILURE);
  //   }

  // if (path[0] == '\0' || case_type == 0)
  // {
  //   print_help_message();
  //   exit(EXIT_FAILURE);
  // }

  char **files = NULL;
  char path[PATH_MAX];
  if (realpath("~/w/crn/test", path) == NULL)
  {
    perror("realpath");
    return 1;
  }

  size_t files_size = list_files(path, &files);
  for (int i = 0; i < files_size; i++)
  {
    rename_to_snake_case(&files[i], '\0');
    printf("%s \n", files[i]);
    free(files[i]);
  }
  free(files);

  return 0;
}

void join_paths(char *buffer, const char *path1, const char *path2)
{
  snprintf(buffer, PATH_MAX, "%s/%s", path1, path2);
}

void rename_to_snake_case(char **filename, char find)
{
  if (find != '\0')
  {
    for (int i = 0; i < strlen(*filename); i++)
    {
      if ((*filename)[i] == find)
      {
        (*filename)[i] = '_';
      }
    }
  }
  else
  {
    int length = strlen(*filename);
    int new_length = length;
    char *out = (char *)calloc(length + 1, sizeof(char));
    if (out == NULL)
    {
      perror("Failed to allocate memory");
      return;
    }
    int j = 0;
    for (int i = 0; i < length; i++)
    {

      if (isupper((*filename)[i]) && i != 0)
      {
        new_length++;
        out = realloc(out, new_length);
        if (out == NULL)
        {
          perror("Failed to allocate memory");
          return;
        }
        out[j++] = '_';
      }
      out[j++] = tolower((*filename)[i]);
    }
    out[new_length] = '\0';
    free(*filename);
    *filename = out;
  }
}

void fs_rename(char *old_filename, char *new_filename)
{

  if (rename(old_filename, new_filename) == 0)
  {
    printf("Entity %s renamed to %s successfully.\n", old_filename, new_filename);
  }
  else
  {
    perror("Error renaming file");
  }
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

  if (check_path(path) == 0)
  {
    *list = realloc(*list, sizeof(char *));
    char *name = basename(path);
    **list = malloc(strlen(name));
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
        (*list)[i] = malloc(strlen(dir->d_name));
        strcpy((*list)[i], dir->d_name);
        i++;
      }
    }
    closedir(d);
    return i;
  }

  return 0;
}

int check_path(const char *path)
{
  struct stat path_stat;
  stat(path, &path_stat);
  if (S_ISREG(path_stat.st_mode))
  {
    return 0;
  }
  else if (S_ISDIR(path_stat.st_mode))
  {
    return 1;
  }
  else
  {
    return -1;
  }
}

void print_help_message(void)
{
  printf("Usage: crm [options]\n");
  printf("Options:\n");
  printf("  -a, --apply     Apply file renaming\n");
  printf("  -c, --case      Choose the case type (snake, camel, kebab, pascal)\n");
  printf("  -d, --delimiter Choose the word delimiter\n");
  printf("  -p, --path      Specify the directory or file path\n");
  printf("  -h, --help      Display this help message\n");
  printf("  -v, --version   Display version information\n");
}