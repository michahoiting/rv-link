#ifndef __RVL_ASSERT_H__
#define __RVL_ASSERT_H__

#define rvl_assert(cond) \
  do { \
    if(!(cond)) {for(;;) {} } \
  } while (0)

#endif /* __RVL_ASSERT_H__ */
