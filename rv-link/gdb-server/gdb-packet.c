/**
 * Copyright (c) 2019 zoomdy@163.com
 * Copyright (c) 2020, Micha Hoiting <micha.hoiting@gmail.com>
 *
 * \file  rv-link/gdb-server/gdb-packet.c
 * \brief Implementation of the GDB server RSP packet handler.
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

/* own header file include */
#include <rv-link/gdb-server/gdb-packet.h>

/* system library header file includes */
#include <stdio.h>
#include <string.h>

/* own component header file includes */
#include <rv-link/gdb-server/details/gdb-packet.h>

typedef struct gdb_serial_command_s
{
    size_t total_len;
    size_t cmd_len;
    int index_dollar;
    int index_sharp;
    int index_scan;
    bool busy;

    /* There is a '$' in the front of the command buffer, and
     * a '#' checksum in the back, and possibly a '+' so add 8 more bytes.
     */
    char buffer[GDB_PACKET_COMMAND_BUFFER_SIZE + 8];
} gdb_serial_command_t;

typedef struct gdb_serial_response_s
{
    size_t total_len;
    int index_start;
    bool no_ack_mode;
    bool busy;

    /* The response buffer should also be filled with '$''#' checksum,
     * and there may be '+', so add 8 more bytes.
     */
    char buffer[GDB_PACKET_RESPONSE_BUFFER_SIZE + 8];
} gdb_serial_response_t;

static gdb_serial_command_t gdb_serial_command_i;
#define command gdb_serial_command_i

static gdb_serial_response_t gdb_serial_response_i;
#define response gdb_serial_response_i

static uint8_t gdb_packet_checksum(const uint8_t* p, size_t len);

/*---------------------------------------------------------------------------*/
void gdb_packet_init(void)
{
    /* initialize command pipe */
    command.total_len = 0;
    command.index_dollar = -1;
    command.index_sharp = -1;
    command.index_scan = 0;
    command.busy = false;

    /* initialize response pipe */
    response.busy = true;
    response.no_ack_mode = false;
    response.total_len = 0;
}
/*---------------------------------------------------------------------------*/
const char* gdb_packet_command_buffer(void)
{
    /* find index of '$' RSP packet start character in buffer */
    for (; command.index_scan < command.total_len; command.index_scan++) {
        if (command.index_dollar == -1 && command.buffer[command.index_scan] == '$') {
            command.index_dollar = command.index_scan;
        }

        /* find index of '#' RSP packet character in buffer */
        if (command.index_sharp == -1 && command.buffer[command.index_scan] == '#') {
            command.index_sharp = command.index_scan;
        }
    }

    if (command.index_dollar >= 0 && command.index_sharp > 0) {
        command.busy = true;
        /* found a complete RSP packet */
        command.cmd_len = command.index_sharp - command.index_dollar - 1;
        return &command.buffer[command.index_dollar + 1];
    } else if (command.total_len == 1 && command.buffer[0] == '\x03') {
        /* received a ctrl-C BREAK signal */
        command.busy = true;
        command.cmd_len = 1;
        return &command.buffer[0];
    } else {
        return NULL;
    }
}
/*---------------------------------------------------------------------------*/
size_t gdb_packet_command_length(void)
{
    return command.cmd_len;
}
/*---------------------------------------------------------------------------*/
void gdb_packet_command_done(void)
{
    command.busy = false;
    command.total_len = 0;
    command.cmd_len = 0;
    command.index_dollar = -1;
    command.index_sharp = -1;
    command.index_scan = 0;
}
/*---------------------------------------------------------------------------*/
bool gdb_packet_process_command(const uint8_t* buffer, size_t len)
{
    if (command.busy) {
        return false;
    } else {
        if (sizeof(command.buffer) - command.total_len < len) {
            return false;
        } else {
            memcpy(&command.buffer[command.total_len], buffer, len);
            command.total_len += len;
            return true;
        }
    }
}
/*---------------------------------------------------------------------------*/
char* gdb_packet_response_buffer(void)
{
    if (response.busy) {
        /* The first two bytes are used to fill in '+' and '$' */
        return &response.buffer[2];
    } else {
        return NULL;
    }
}
/*---------------------------------------------------------------------------*/
void gdb_packet_response_done(size_t len, gdb_packet_send_flags_t send_flags)
{
    uint8_t checksum;

    response.busy = false;
    response.total_len = len;

    if (send_flags & GDB_PACKET_SEND_FLAG_DOLLAR) {
        response.buffer[1] = '$';
        if (!response.no_ack_mode) {
            response.buffer[0] = '+';
            response.index_start = 0;
            response.total_len += 2;
        } else {
            response.index_start = 1;
            response.total_len += 1;
        }
    }

    if (send_flags & GDB_PACKET_SEND_FLAG_SHARP) {
        response.buffer[response.total_len + response.index_start] = '#';
        response.total_len++;

        checksum = gdb_packet_checksum((const uint8_t*)&response.buffer[2], len);
        snprintf(&response.buffer[response.total_len + response.index_start], 3, "%02x", checksum);
        response.total_len += 2;
    }
}
/*---------------------------------------------------------------------------*/
const uint8_t* gdb_packet_get_response(size_t* len)
{
    if (response.busy) {
        return NULL;
    } else {
        *len = response.total_len;
        return (uint8_t*)(&response.buffer[response.index_start]);
    }
}
/*---------------------------------------------------------------------------*/
void gdb_packet_release_response(void)
{
    response.total_len = 0;
    response.busy = true;
}
/*---------------------------------------------------------------------------*/
void
gdb_packet_no_ack_mode(bool no_ack_mode)
{
    response.no_ack_mode = no_ack_mode;
}
/*---------------------------------------------------------------------------*/
static uint8_t gdb_packet_checksum(const uint8_t* p, size_t len)
{
    uint32_t checksum = 0;
    size_t i;

    for (i = 0; i < len; i++) {
        checksum += p[i];
    }
    checksum &= 0xff;

    return (uint8_t)checksum;
}
