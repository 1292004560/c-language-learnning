#include <stdio.h>
#include <string.h>

int main() {
  const char str[] = "https://www.runoob.com";
  const char ch = 'o';
  char *ptr;

  ptr = strchr(str, ch);

  if (ptr != NULL) {
    printf("字符 'o' 出现的位置为 %ld。\n", ptr - str + 1);
    printf("|%c| 之后的字符串是 - |%s|\n", ch, ptr);
  } else {
    printf("没有找到字符 'o' 。\n");
  }
  return (0);
}
