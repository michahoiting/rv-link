#ifndef __RV_LINK_DETAILS_DEBUG_H__
#define __RV_LINK_DETAILS_DEBUG_H__
/**
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/details/debug.h
 * \brief Support for debug logging.
 */

#ifdef RVL_DEBUG_EN

void rvl_debug_log(const char *fmt, ...);

// TODO remove the `pt` parameter
#define RVL_DEBUG_LOG(pt, message)                                      \
do {                                                                    \
    rvl_debug_log message ;                                             \
} while (0)

#else

#define RVL_DEBUG_LOG(pt, message)

#endif

#endif /* __RV_LINK_DETAILS_DEBUG_H__ */
