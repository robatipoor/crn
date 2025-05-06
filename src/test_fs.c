#include "unity/unity.h"
#include "utils.h"
#include "fs.h"

const char *TEST_TEMP_DIR = "test_tmp";
const char *TEST_TEMP_FILE_PREFIX = "test_";
char **FILES = NULL;

void create_empty_file(char *file_name)
{
  FILE *file = fopen(file_name, "w");
  if (file == NULL)
  {
    perror("Error create file");
  }
  fclose(file);
}

void setUp()
{
  if (mkdir(TEST_TEMP_DIR, 0755) == -1)
  {
    perror("Error creating directory");
  }

  FILES = (char **)malloc(10 * sizeof(char *));
  if (FILES == NULL)
  {
    perror("Error malloc");
    return;
  }

  for (int i = 0; i < 10; i++)
  {
    char file_name[20];
    sprintf(file_name, "%s%d.txt", TEST_TEMP_FILE_PREFIX, i);
    char *file_path = join_paths(2, TEST_TEMP_DIR, file_name);
    create_empty_file(file_path);
    FILES[i] = file_path;
  }
}

void tearDown()
{
  for (int i = 0; i < 10; i++)
  {
    char *file_path = FILES[i];
    remove(file_path);
    free(FILES[i]);
  }
  free(FILES);
  if (rmdir(TEST_TEMP_DIR) == -1)
  {
    perror("Error removing directory");
  }
}

void test_is_directory(void)
{
  TEST_ASSERT_EQUAL_INT32(2, is_directory("test_tmp/test_1.txt"));
  TEST_ASSERT_EQUAL_INT32(1, is_directory("test_tmp/"));
  TEST_ASSERT_EQUAL_INT32(-1, is_directory("great"));
}

int main(void)
{
  UNITY_BEGIN(); 
  RUN_TEST(test_is_directory);
  return UNITY_END();
}
