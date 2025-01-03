#include "test.h"
// [116] 支持extern
extern int ext1;
extern int *ext2;
int main() {
  // [116] 支持extern
  ASSERT(5, ext1);
  ASSERT(5, *ext2);

  // [117] 处理块中的extern声明
  extern int ext3;
  ASSERT(7, ext3);
  int ext_fn1(int x);
  ASSERT(5, ext_fn1(5));
  extern int ext_fn2(int x);
  ASSERT(8, ext_fn2(8));
  printf("OK\n");
  return 0;
}
