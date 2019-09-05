#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "interface/led.h"
#include "gdb-server.h"
#include "gdb-serial.h"

typedef int16_t gdb_server_tid_t;

typedef struct gdb_server_s
{
    struct pt pt_server;
    struct pt pt_cmd;
    const char *cmd;
    size_t cmd_len;
    char *res;
    size_t res_len;

    gdb_server_tid_t tid_g;
    gdb_server_tid_t tid_G;
    gdb_server_tid_t tid_m;
    gdb_server_tid_t tid_M;
    gdb_server_tid_t tid_c;
}gdb_server_t;

static gdb_server_t gdb_server_i;
#define self gdb_server_i


PT_THREAD(gdb_server_cmd_q(void));
PT_THREAD(gdb_server_cmd_Q(void));
PT_THREAD(gdb_server_cmd_H(void));
PT_THREAD(gdb_server_cmd_question_mark(void));
PT_THREAD(gdb_server_cmd_g(void));
PT_THREAD(gdb_server_cmd_k(void));

static void gdb_server_connected(void);
static void gdb_server_disconnected(void);

static void gdb_server_reply_ok(void);
static void gdb_server_reply_empty(void);


void gdb_server_init(void)
{
    PT_INIT(&self.pt_server);
    PT_INIT(&self.pt_cmd);
}


PT_THREAD(gdb_server_poll(void))
{
    char c;

    PT_BEGIN(&self.pt_server);

    while (1) {
        PT_WAIT_UNTIL(&self.pt_server, (self.cmd = gdb_serial_command_buffer()) != NULL);
        self.cmd_len = gdb_serial_command_length();

        PT_WAIT_UNTIL(&self.pt_server, (self.res = gdb_serial_response_buffer()) != NULL);
        self.res_len = 0;

        c = *self.cmd;
        if(c == 'q') {
            PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_q());
        } else if(c == 'Q') {
            PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_Q());
        } else if(c == 'H') {
            PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_H());
        } else if(c == '?') {
            PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_question_mark());
        } else if(c == 'g') {
            PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_g());
        } else if(c == 'k') {
            PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_k());
        } else {
            gdb_server_reply_empty();
        }

        gdb_serial_command_done();
    }

    PT_END(&self.pt_server);
}


const char qSupported_res[] = "PacketSize=400;QStartNoAckMode+";


/*
 * ‘q name params...’
 * General query (‘q’) and set (‘Q’).
 */
PT_THREAD(gdb_server_cmd_q(void))
{
    PT_BEGIN(&self.pt_cmd);

    if(strncmp(self.cmd, "qSupported:", 11) == 0){
        gdb_server_connected();
        strncpy(self.res, qSupported_res, GDB_SERIAL_RESPONSE_BUFFER_SIZE);
        gdb_serial_response_done(strlen(qSupported_res), GDB_SERIAL_SEND_FLAG_ALL);
    } else {
        gdb_server_reply_empty();
    }

    PT_END(&self.pt_cmd);
}


/*
 * ‘Q name params...’
 * General query (‘q’) and set (‘Q’).
 */
PT_THREAD(gdb_server_cmd_Q(void))
{
    PT_BEGIN(&self.pt_cmd);

    if(strncmp(self.cmd, "QStartNoAckMode", 15) == 0){
        gdb_server_reply_ok();
        gdb_serial_no_ack_mode(true);
    } else {
        gdb_server_reply_empty();
    }

    PT_END(&self.pt_cmd);
}


/*
 * ‘H op thread-id’
 * Set thread for subsequent operations (‘m’, ‘M’, ‘g’, ‘G’, et.al.).
 */
PT_THREAD(gdb_server_cmd_H(void))
{
    unsigned int n;
    char cmd;
    gdb_server_tid_t tid;

    PT_BEGIN(&self.pt_cmd);

    cmd = self.cmd[1];
    if(cmd == 'g' || cmd == 'G' || cmd == 'm' || cmd == 'M' || cmd == 'c'){
        sscanf(&self.cmd[2], "%x", &n);
        gdb_server_reply_ok();

        tid = (gdb_server_tid_t)n;
        if(cmd == 'g') {
            self.tid_g = tid;
        } else if(cmd == 'G') {
            self.tid_G = tid;
        } else if(cmd == 'm') {
            self.tid_m = tid;
        } else if(cmd == 'M'){
            self.tid_M = tid;
        } else {
            self.tid_c = tid;
        }
    } else {
        gdb_server_reply_empty();
    }

    PT_END(&self.pt_cmd);
}


/*
 * ‘?’
 * Indicate the reason the target halted. The reply is the same as for step and continue.
 */
PT_THREAD(gdb_server_cmd_question_mark(void))
{
    PT_BEGIN(&self.pt_cmd);

    strncpy(self.res, "S02", GDB_SERIAL_RESPONSE_BUFFER_SIZE);
    gdb_serial_response_done(3, GDB_SERIAL_SEND_FLAG_ALL);

    PT_END(&self.pt_cmd);
}


/*
 * ‘g’
 * Read general registers.
 */
PT_THREAD(gdb_server_cmd_g(void))
{
    int i;

    PT_BEGIN(&self.pt_cmd);

    for(i = 0; i < 33; i++) {
        snprintf(&self.res[i * 8], GDB_SERIAL_RESPONSE_BUFFER_SIZE - i * 8, "%08x", i);
    }
    gdb_serial_response_done(8 * 33, GDB_SERIAL_SEND_FLAG_ALL);

    PT_END(&self.pt_cmd);
}


/*
 * ‘k’
 * Kill request.
 */
PT_THREAD(gdb_server_cmd_k(void))
{
    PT_BEGIN(&self.pt_cmd);

    gdb_server_reply_ok();
    gdb_server_disconnected();

    PT_END(&self.pt_cmd);
}


static void gdb_server_reply_ok(void)
{
    strncpy(self.res, "OK", GDB_SERIAL_RESPONSE_BUFFER_SIZE);
    gdb_serial_response_done(2, GDB_SERIAL_SEND_FLAG_ALL);
}


static void gdb_server_reply_empty(void)
{
    gdb_serial_response_done(0, GDB_SERIAL_SEND_FLAG_ALL);
}


static void gdb_server_connected(void)
{
    gdb_serial_no_ack_mode(false);
    rvl_led_gdb_connect(1);
}


static void gdb_server_disconnected(void)
{
    rvl_led_gdb_connect(0);
}

