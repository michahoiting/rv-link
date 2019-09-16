#ifndef __RVL_ASSERT_H__
#define __RVL_ASSERT_H__

#ifdef RVL_ASSERT_EN

#define rvl_assert(cond) \
  do { \
    if(!(cond)) {for(;;) {} } \
  } while (0)

#else

#define rvl_assert(cond)

#endif

#endif /* __RVL_ASSERT_H__ */
