#include <string.h>
#include <stdio.h>

#include "rvl-usb-serial.h"
#include "gdb-serial.h"

typedef struct gdb_serial_command_s
{
    size_t total_len;
    size_t cmd_len;
    int index_dollar;
    int index_sharp;
    int index_scan;
    bool gdb_server_own;

    // 命令缓冲的前部有 '$' ，后部由 '#' checksum，还可能有 '+' 因此多添加 8 个字节
    char buffer[GDB_SERIAL_COMMAND_BUFFER_SIZE + 8];
}gdb_serial_command_t;

typedef struct gdb_serial_response_s
{
    bool gdb_server_own;
    bool no_ack_mode;
    size_t total_len;
    int index_start;

    // 响应缓冲还要填入 '$' '#' checksum，还可能有 '+' 因此多添加 8 个字节
    char buffer[GDB_SERIAL_RESPONSE_BUFFER_SIZE + 8];
}gdb_serial_response_t;

static gdb_serial_command_t gdb_serial_command_i;
#define command gdb_serial_command_i

static gdb_serial_response_t gdb_serial_response_i;
#define response gdb_serial_response_i

static uint8_t gdb_serial_checksum(const uint8_t* p, size_t len);

void gdb_serial_init(void)
{
    command.total_len = 0;
    command.index_dollar = -1;
    command.index_sharp = -1;
    command.index_scan = 0;
    command.gdb_server_own = false;

    response.gdb_server_own = true;
    response.no_ack_mode = false;
    response.total_len = 0;
}


const char* gdb_serial_command_buffer(void)
{
    for(; command.index_scan < command.total_len; command.index_scan++) {
        if(command.index_dollar == -1 && command.buffer[command.index_scan] == '$') {
            command.index_dollar = command.index_scan;
        }

        if(command.index_sharp == -1 && command.buffer[command.index_scan] == '#') {
            command.index_sharp = command.index_scan;
        }
    }

    if(command.index_dollar >= 0 && command.index_sharp > 0) {
        command.gdb_server_own = true;
        command.cmd_len = command.index_sharp - command.index_dollar - 1;
        return &command.buffer[command.index_dollar + 1];
    } else if(command.total_len == 1 && command.buffer[0] == '\x03') {
        command.gdb_server_own = true;
        command.cmd_len = 1;
        return &command.buffer[0];
    }else {
        return NULL;
    }
}


size_t gdb_serial_command_length(void)
{
    return command.cmd_len;
}


void gdb_serial_command_done(void)
{
    command.gdb_server_own = false;
    command.total_len = 0;
    command.cmd_len = 0;
    command.index_dollar = -1;
    command.index_sharp = -1;
    command.index_scan = 0;
}


bool usb_serial_put_recv_data(const uint8_t* buffer, size_t len)
{
    if(command.gdb_server_own) {
        return false;
    } else {
        if(sizeof(command.buffer) - command.total_len < len) {
            return false;
        } else {
            memcpy(&command.buffer[command.total_len], buffer, len);
            command.total_len += len;
            return true;
        }
    }
}


char * gdb_serial_response_buffer(void)
{
    if(response.gdb_server_own) {
        // 前两个字节用作填入 '+' 和 '$'
        return &response.buffer[2];
    } else {
        return NULL;
    }
}

void gdb_serial_response_done(size_t len, uint32_t send_flags)
{
    uint8_t checksum;

    response.gdb_server_own = false;
    response.total_len = len;

    if(send_flags & GDB_SERIAL_SEND_FLAG_DOLLAR) {
        response.buffer[1] = '$';
        if(!response.no_ack_mode) {
            response.buffer[0] = '+';
            response.index_start = 0;
            response.total_len += 2;
        } else {
            response.index_start = 1;
            response.total_len += 1;
        }
    }

    if(send_flags & GDB_SERIAL_SEND_FLAG_SHARP) {
        response.buffer[response.total_len + response.index_start] = '#';
        response.total_len++;

        checksum = gdb_serial_checksum((const uint8_t*)&response.buffer[2], len);
        snprintf(&response.buffer[response.total_len + response.index_start], 3, "%02x", checksum);
        response.total_len += 2;
    }
}


const uint8_t* usb_serial_get_send_data(size_t* len)
{
    if(response.gdb_server_own) {
        return NULL;
    } else {
        *len = response.total_len;
        return (uint8_t*)(&response.buffer[response.index_start]);
    }
}

void usb_serial_data_sent(void)
{
    response.total_len = 0;
    response.gdb_server_own = true;
}

void gdb_serial_no_ack_mode(bool no_ack_mode)
{
    response.no_ack_mode = no_ack_mode;
}


static uint8_t gdb_serial_checksum(const uint8_t* p, size_t len)
{
    uint32_t checksum = 0;
    size_t i;

    for(i = 0; i < len; i++) {
        checksum += p[i];
    }
    checksum &= 0xff;

    return (uint8_t)checksum;
}
