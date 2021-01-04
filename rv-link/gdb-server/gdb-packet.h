#ifndef __RV_LINK_GDB_SERVER_GDB_PACKET_H__
#define __RV_LINK_GDB_SERVER_GDB_PACKET_H__
/*
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/gdb-server/gdb-packet.h
 * \brief Interface of the GDB server RSP packet handler.
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
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* TODO correct comments
 * After USB serial receives the data, it calls gdb_packet_process_command to
 * transfer data to the component using the virtual serial port.
 * If the return value is false, then the buffer is full and try again later;
 * if the return value is true, the transmission is successful and the next
 * packet of data can be received.
 */
bool gdb_packet_process_command(const uint8_t* buffer, size_t len);

/* TODO correct comments
 * USB serial calls gdb_packet_get_response to check if there is data to
 * be sent. If the return value is valid, then there is data to be sent, and the
 * length of the data to be sent is returned by the len parameter; if it returns
 * NULL, it means that there is no data to send.
 */
const uint8_t* gdb_packet_get_response(size_t* len);

/* TODO correct comments
 * After USB serial sends data, call gdb_packet_release_response to notify the
 * components using the virtual serial port. After receiving the notification,
 * the consumer can start using the sending buffer again.
 */
void gdb_packet_release_response(void);

#endif /* __RV_LINK_GDB_SERVER_GDB_PACKET_H__ */
