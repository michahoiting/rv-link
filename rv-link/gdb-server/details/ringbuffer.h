#ifndef __RV_LINK_GDB_SERVER_DETAILS_RINGBUFFER_H__
#define __RV_LINK_GDB_SERVER_DETAILS_RINGBUFFER_H__
/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/gdb-server/details/ringbuffer.h
 * \brief Private interface of the GDB server response message ringbuffer.
 *
 * RV-LINK is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *     http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v1 for more details.
 */

/* system library header file includes */
#include <stddef.h>

/* other library header file includes */
#include <pt/pt.h>

void gdb_ringbuffer_init(void);
PT_THREAD(gdb_ringbuffer_poll(void));

size_t gdb_resp_buf_putchar(char c);
size_t gdb_resp_buf_puts(const char* s);
size_t gdb_resp_buf_getchar(char* c);

#endif /* __RV_LINK_GDB_SERVER_DETAILS_RINGBUFFER_H__ */
