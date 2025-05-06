#include <unity/unity.h>
#include "utils.h"

char *path1 = NULL;
char *path2 = NULL;
char *path3 = NULL;

void setUp() {}

void tearDown()
{

  if (path1)
  {
    free(path1);
    path1 = NULL;
  }
  if (path2)
  {
    free(path2);
    path2 = NULL;
  }
  if (path3)
  {
    free(path3);
    path3 = NULL;
  }
}

void test_join_path(void)
{

  path1 = join_paths(2, "foo", "bar");
  TEST_ASSERT_EQUAL_CHAR_ARRAY("foo/bar", path1, 7);

  path2 = join_paths(2, "scala", "go");
  TEST_ASSERT_EQUAL_CHAR_ARRAY("scala/go", path2, 8);

  path3 = join_paths(2, "scala", "");
  TEST_ASSERT_EQUAL_CHAR_ARRAY("scala", path3, 5);
}

void test_replace_all_chars(void)
{
  char string1[] = "Hell";
  replace_all_chars(string1, 'l', 'k');
  TEST_ASSERT_EQUAL_CHAR_ARRAY("Hekk", string1, 4);

  char string2[] = "Good";
  replace_all_chars(string2, 'G', 'd');
  TEST_ASSERT_EQUAL_CHAR_ARRAY("dood", string2, 4);

  char string3[] = "";
  replace_all_chars(string3, 'g', 'd');
  TEST_ASSERT_EQUAL_CHAR_ARRAY("", string3, 1);

  char string4[] = "great";
  replace_all_chars(string4, 'l', 'd');
  TEST_ASSERT_EQUAL_CHAR_ARRAY("great", string4, 5);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_join_path);
  RUN_TEST(test_replace_all_chars);
  return UNITY_END();
}
