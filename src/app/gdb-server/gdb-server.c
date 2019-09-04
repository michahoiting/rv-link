#include <stddef.h>
#include <string.h>

#include "gdb-server.h"
#include "gdb-serial.h"

typedef struct gdb_server_s
{
    struct pt pt_server;
    struct pt pt_cmd;
    const char *cmd;
    size_t cmd_len;
    char *res;
    size_t res_len;
}gdb_server_t;

static gdb_server_t gdb_server_i;
#define self gdb_server_i


void gdb_server_init(void)
{
    PT_INIT(&self.pt_server);
}


PT_THREAD(gdb_server_cmd_q(void));


int gdb_server_strncmp(const char *p1, const char *p2, size_t size);
size_t gdb_server_strncpy(char *dest, const char *src, size_t size);


PT_THREAD(gdb_server_poll(void))
{
    char c;

    PT_BEGIN(&self.pt_server);

    while (1) {
        PT_WAIT_UNTIL(&self.pt_server, (self.cmd = gdb_serial_command_buffer()) != NULL);
        self.cmd_len = gdb_serial_command_length();

        PT_WAIT_UNTIL(&self.pt_cmd, (self.res = gdb_serial_response_buffer()) != NULL);
        self.res_len = 0;

        c = *self.cmd;
        if(c == 'q') {
            PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_q());
        } else if(c == 'Q') {

        } else {

        }

        gdb_serial_command_done();
    }

    PT_END(&self.pt_server);
}


PT_THREAD(gdb_server_cmd_q(void))
{
    size_t len;

    PT_BEGIN(&self.pt_cmd);

    if(strncmp(self.cmd, "qSupported:", self.cmd_len)){
        len = gdb_server_strncpy(self.res, "PacketSize=400;QStartNoAckMode+", GDB_SERIAL_RESPONSE_BUFFER_SIZE);
        gdb_serial_response_done(len, GDB_SERIAL_SEND_FLAG_ALL);
    }

    PT_END(&self.pt_cmd);
}

size_t gdb_server_strncpy(char *dest, const char *src, size_t size)
{
    size_t len = 0;

    while(*src) {
        *dest = *src;
        dest++;
        src++;
        len++;
        size--;
        if(size <= 1) {
            break;
        }
    }
    *dest = '\0';

    return len;
}

int gdb_server_strncmp(const char *p1, const char *p2, size_t size)
{
    while(*p1 && *p2 && size > 0) {
        if(*p1 != *p2) {
            return *p1 - *p2;
        } else {
            p1++;
            p2++;
            size--;
        }
    }

    return *p1 - *p2;
}
