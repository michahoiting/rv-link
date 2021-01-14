#ifndef __RV_LINK_DETAILS_DEBUG_H__
#define __RV_LINK_DETAILS_DEBUG_H__
/**
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/details/debug.h
 * \brief Support for debug logging.
 */

#ifdef RVL_DEBUG_EN
#define RVL_DEBUG_LOG(pt, message)
// TODO Log to usb-serial2 or in GDB in response to 'O XX...'
#else
#define RVL_DEBUG_LOG(pt, message)
#endif

#endif /* __RV_LINK_DETAILS_DEBUG_H__ */
