#define _GNU_SOURCE
#include <ctype.h>
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

typedef int path_state;

const char PATH_SEPARATOR = '/';

static struct option long_options[] = {
    {"case", required_argument, NULL, 'c'},
    {"delimiter", required_argument, NULL, 'd'},
    {"apply", no_argument, NULL, 'a'},
    {"recursive", no_argument, NULL, 'r'},
    {"file", no_argument, NULL, 'f'},
    {"subdirectory", no_argument, NULL, 's'},
    {"path", required_argument, NULL, 'p'},
    {"version", no_argument, NULL, 'v'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

enum CaseType {
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
void rename_appender(char **filename, char find, char replace);
void fs_rename(char *old_filename, char *new_filename);
void join_paths(char *buffer, const char *path1, const char *path2);

int main(int argc, char *argv[]) {
  int c;
  opterr = 0;
  int apply_flag = 0;
  int recursive_flag = 0;
  int file_flag = 0;
  int subdirectory_flag = 0;
  enum CaseType case_type = UNKNOWN;
  char path[PATH_MAX] = {};
  path_state p_state = -1;
  char **files = NULL;
  char delimiter = '\0';
  int d_flag = 0;

  while ((c = getopt_long(argc, argv, "arfsc:d:p:hv", long_options, NULL)) !=
         -1)
    switch (c) {
    case 'c':
      if (strstr(optarg, "snake")) {
        case_type = SNAKE_CASE;
      } else if (strstr(optarg, "kebab")) {
        case_type = KEBAB_CASE;
      } else if (strstr(optarg, "camel")) {
        case_type = CAMEL_CASE;
      } else if (strstr(optarg, "pascal")) {
        case_type = PASCAL_CASE;
      } else {
        fprintf(stderr, "Invalid -c argument \n");
        exit(EXIT_FAILURE);
      }
      break;
    case 'd':
      if (strlen(optarg) == 1 && optarg[0] < 127 && optarg[0] >= 0) {
        delimiter = optarg[0];
        d_flag = 1;
      } else {
        fprintf(stderr, "Invalid -d argument \n");
        exit(EXIT_FAILURE);
      }
      break;
    case 'a':
      apply_flag = 1;
      break;
    case 'r':
      recursive_flag = 1;
      break;
    case 'f':
      file_flag = 1;
      break;
    case 's':
      subdirectory_flag = 1;
      break;
    case 'p':
      if (realpath(optarg, path) == NULL) {
        fprintf(stderr, "Invalid file system path: %s\n", path);
        exit(EXIT_FAILURE);
      }

      path[strcspn(path, "\n")] = 0;
      p_state = check_path(path);
      if (p_state == -1) {
        fprintf(stderr, "Invalid file system path: %s\n", path);
        exit(EXIT_FAILURE);
      }

      break;
    case 'h':
      print_help_message();
      exit(EXIT_SUCCESS);
    case 'v':
      printf("0.0.1v \n");
      exit(EXIT_SUCCESS);
    case '?':
      printf("Unknown option: %c\n", optopt);
      print_help_message();
      exit(EXIT_SUCCESS);
    default:
      print_help_message();
      exit(EXIT_FAILURE);
    }

  if (path[0] == '\0' || case_type == 0) {
    print_help_message();
    exit(EXIT_FAILURE);
  }
  size_t files_size = list_files(path, &files);
  for (int i = 0; i < files_size; i++) {
    rename_appender(&files[i], '\0', '-');
    printf("%s \n", files[i]);
    free(files[i]);
  }
  free(files);

  return 0;
}

void rename_appender(char **file_path, char find, char replace) {
  if (find == '\0') {
    char *dup = strdup(*file_path);
    size_t len = strlen(*file_path);
    int j = 0;
    size_t count = 0;
    for (int i = 0; dup[i] != '\0'; i++) {
      char c = dup[i];
      if (i != 0 && isupper(c) && dup[i - 1] != PATH_SEPARATOR) {
        count++;
        *file_path = (char *)realloc(*file_path, len + count + 1);
        if (*file_path == NULL) {
          perror("Failed to allocate memory");
          return;
        }
        (*file_path)[j++] = replace;
      }
      (*file_path)[j++] = tolower(c);
    }
    (*file_path)[j] = '\0';
    free(dup);
  } else {
    for (int i = 0; (*file_path)[i] != '\0'; i++) {
      char c = (*file_path)[i];
      if (c == find) {
        (*file_path)[i] = replace;
      } else {
        (*file_path)[i] = tolower(c);
      }
    }
  }
}

void fs_rename(char *old_filename, char *new_filename) {

  if (rename(old_filename, new_filename) == 0) {
    printf("Entity %s renamed to %s successfully.\n", old_filename,
           new_filename);
  } else {
    perror("Error renaming file");
  }
}

void rename_filename(char *filename, char find, char replace) {

  char *current_pos = strchr(filename, find);

  while (current_pos) {
    *current_pos = replace;
    current_pos = strchr(current_pos + 1, find);
  }

  return;
}

size_t list_files(const char *path, char ***list) {

  if (check_path(path) == 0) {
    *list = realloc(*list, sizeof(char *));
    (*list)[0] = (char *)malloc(strlen(path) + 1);
    strcpy((*list)[0], path);
    return 1;
  }

  struct dirent *dir;
  DIR *d = opendir(path);
  if (d) {
    int i = 0;
    size_t path_len = strlen(path);
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_type == DT_REG) {
        *list = realloc(*list, (i + 1) * sizeof(char *));
        if (*list == NULL) {
          perror("Failed to allocate memory");
          return 0;
        }
        size_t size = path_len + strlen(dir->d_name) + 2;
        (*list)[i] = (char *)malloc(size * sizeof(char));
        if ((*list)[i] == NULL) {
          perror("Failed to allocate memory");
          return 0;
        }
        snprintf((*list)[i], size, "%s/%s", path, dir->d_name);
        i++;
      }
    }
    closedir(d);
    return i;
  }

  return 0;
}

int check_path(const char *path) {
  struct stat path_stat;
  stat(path, &path_stat);
  if (S_ISREG(path_stat.st_mode)) {
    return 0;
  } else if (S_ISDIR(path_stat.st_mode)) {
    return 1;
  } else {
    return -1;
  }
}

void join_paths(char *buffer, const char *path1, const char *path2) {
  snprintf(buffer, PATH_MAX, "%s/%s", path1, path2);
}

void print_help_message(void) {
  printf("Usage: crm [options]\n");
  printf("Options:\n");
  printf("  -a, --apply        Apply file renaming\n");
  printf("  -c, --case         Choose the case type (snake, camel, kebab, "
         "pascal)\n");
  printf("  -d, --delimiter    Choose the word delimiter\n");
  printf("  -p, --path         Specify the directory or file path\n");
  printf("  -r, --recursive    Recursive \n");
  printf("  -f, --file         Select files\n");
  printf("  -s, --subdirectory Select subdirectories\n");
  printf("  -h, --help         Display this help message\n");
  printf("  -v, --version      Display version information\n");
}
