#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "rvl-app-config.h"
#include "rvl-link-config.h"
#include "rvl-target-config.h"
#include "riscv_encoding.h"
#include "rvl-led.h"
#include "rvl-serial.h"
#include "rvl-link.h"
#include "gdb-server.h"

#include "rvl-target.h"
#include "gdb-serial.h"

typedef int16_t gdb_server_tid_t;

typedef struct gdb_server_s
{
    struct pt pt_server;
    struct pt pt_cmd;
    struct pt pt_cmd_sub;
    struct pt pt_sub_routine;
    const char *cmd;
    size_t cmd_len;
    char *res;
    bool target_running;
    bool gdb_connected;
    rvl_target_halt_info_t halt_info;
    rvl_target_error_t target_error;
    rvl_target_addr_t mem_addr;
    size_t mem_len;
    uint8_t mem_buffer[GDB_SERIAL_RESPONSE_BUFFER_SIZE];
    int flash_err;
    int i;
    rvl_target_reg_t regs[RVL_TARGET_CONFIG_REG_NUM];
    rvl_target_reg_t reg_tmp;
    int reg_tmp_num;

    gdb_server_tid_t tid_g;
    gdb_server_tid_t tid_G;
    gdb_server_tid_t tid_m;
    gdb_server_tid_t tid_M;
    gdb_server_tid_t tid_c;

    rvl_target_breakpoint_type_t breakpoint_type;
    rvl_target_addr_t breakpoint_addr;
    int breakpoint_kind;
    int breakpoint_err;
}gdb_server_t;

static gdb_server_t gdb_server_i;
#define self gdb_server_i


PT_THREAD(gdb_server_cmd_q(void));
PT_THREAD(gdb_server_cmd_qSupported(void));
void gdb_server_cmd_qxfer_features_read_target_xml(void);
void gdb_server_cmd_qxfer_memory_map_read(void);
PT_THREAD(gdb_server_cmd_qRcmd(void));
PT_THREAD(gdb_server_cmd_Q(void));
PT_THREAD(gdb_server_cmd_H(void));
PT_THREAD(gdb_server_cmd_g(void));
PT_THREAD(gdb_server_cmd_G(void));
PT_THREAD(gdb_server_cmd_k(void));
PT_THREAD(gdb_server_cmd_c(void));
PT_THREAD(gdb_server_cmd_s(void));
PT_THREAD(gdb_server_cmd_m(void));
PT_THREAD(gdb_server_cmd_M(void));
PT_THREAD(gdb_server_cmd_X(void));
PT_THREAD(gdb_server_cmd_p(void));
PT_THREAD(gdb_server_cmd_P(void));
PT_THREAD(gdb_server_cmd_z(void));
PT_THREAD(gdb_server_cmd_Z(void));
PT_THREAD(gdb_server_cmd_v(void));
PT_THREAD(gdb_server_cmd_vFlashErase(void));
PT_THREAD(gdb_server_cmd_vFlashWrite(void));
PT_THREAD(gdb_server_cmd_vFlashDone(void));
PT_THREAD(gdb_server_cmd_vMustReplyEmpty(void));
PT_THREAD(gdb_server_cmd_question_mark(void));
PT_THREAD(gdb_server_cmd_ctrl_c(void));

PT_THREAD(gdb_server_connected(void));
PT_THREAD(gdb_server_disconnected(void));

static void gdb_server_target_run(bool run);
static void gdb_server_reply_ok(void);
static void gdb_server_reply_empty(void);
static void gdb_server_reply_err(int err);

static void bin_to_hex(const uint8_t *bin, char *hex, size_t nbyte);
static void word_to_hex_le(uint32_t word, char *hex);
static void hex_to_bin(const char *hex, uint8_t *bin, size_t nbyte);
static void hex_to_word_le(const char *hex, uint32_t *word);
static size_t bin_decode(const uint8_t* xbin, uint8_t* bin, size_t xbin_len);


void gdb_server_init(void)
{
    PT_INIT(&self.pt_server);
    PT_INIT(&self.pt_cmd);
    PT_INIT(&self.pt_cmd_sub);
    PT_INIT(&self.pt_sub_routine);

    gdb_server_target_run(false);
    self.gdb_connected = false;
}


PT_THREAD(gdb_server_poll(void))
{
    char c;
    size_t ret, len;

    PT_BEGIN(&self.pt_server);

    while (1) {
        PT_YIELD(&self.pt_server);

        if(self.gdb_connected && self.target_running) {
            ret = rvl_serial_getchar(&c);
            if(ret > 0) {
                self.res[0] = 'O';
                len = 0;
                do {
                    bin_to_hex((uint8_t*)&c, &self.res[1 + len * 2], 1);
                    len++;
                    ret = rvl_serial_getchar(&c);
                } while (ret > 0);

                gdb_serial_response_done(len * 2 + 1, GDB_SERIAL_SEND_FLAG_ALL);

                PT_WAIT_UNTIL(&self.pt_server, (self.res = gdb_serial_response_buffer()) != NULL);
            }

            self.cmd = gdb_serial_command_buffer();
            if(self.cmd != NULL) {
                self.cmd_len = gdb_serial_command_length();
                if(*self.cmd == '\x03' && self.cmd_len == 1) {
                    PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_ctrl_c());
                    gdb_server_target_run(false);
                    strncpy(self.res, "T02", GDB_SERIAL_RESPONSE_BUFFER_SIZE);
                    gdb_serial_response_done(3, GDB_SERIAL_SEND_FLAG_ALL);
                }
                gdb_serial_command_done();
            }

            if(self.target_running) {
                PT_WAIT_THREAD(&self.pt_server, rvl_target_halt_check(&self.halt_info));
                if(self.halt_info.reason != rvl_target_halt_reason_running) {
                    gdb_server_target_run(false);

                    if(self.halt_info.reason == rvl_target_halt_reason_other) {
                        strncpy(self.res, "T05", GDB_SERIAL_RESPONSE_BUFFER_SIZE);
                    } else if(self.halt_info.reason == rvl_target_halt_reason_write_watchpoint) {
                        snprintf(self.res, GDB_SERIAL_RESPONSE_BUFFER_SIZE, "T05watch:%x;", (unsigned int)self.halt_info.addr);
                    } else if(self.halt_info.reason == rvl_target_halt_reason_read_watchpoint) {
                        snprintf(self.res, GDB_SERIAL_RESPONSE_BUFFER_SIZE, "T05rwatch:%x;", (unsigned int)self.halt_info.addr);
                    } else if(self.halt_info.reason == rvl_target_halt_reason_access_watchpoint) {
                        snprintf(self.res, GDB_SERIAL_RESPONSE_BUFFER_SIZE, "T05awatch:%x;", (unsigned int)self.halt_info.addr);
                    } else if(self.halt_info.reason == rvl_target_halt_reason_hardware_breakpoint) {
                        strncpy(self.res, "T05hwbreak:;", GDB_SERIAL_RESPONSE_BUFFER_SIZE);
                    } else if(self.halt_info.reason == rvl_target_halt_reason_software_breakpoint) {
                        strncpy(self.res, "T05swbreak:;", GDB_SERIAL_RESPONSE_BUFFER_SIZE);
                    } else {

                    }

                    gdb_serial_response_done(strlen(self.res), GDB_SERIAL_SEND_FLAG_ALL);
                }
            }
        } else {
            PT_WAIT_UNTIL(&self.pt_server, (self.cmd = gdb_serial_command_buffer()) != NULL);
            self.cmd_len = gdb_serial_command_length();

            PT_WAIT_UNTIL(&self.pt_server, (self.res = gdb_serial_response_buffer()) != NULL);

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
            } else if(c == 'G') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_G());
            } else if(c == 'k') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_k());
            } else if(c == 'c') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_c());
            } else if(c == 'm') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_m());
            } else if(c == 'M') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_M());
            } else if(c == 'X') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_X());
            } else if(c == 'p') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_p());
            } else if(c == 'P') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_P());
            } else if(c == 's') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_s());
            } else if(c == 'z') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_z());
            } else if(c == 'Z') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_Z());
            } else if(c == 'v') {
                PT_WAIT_THREAD(&self.pt_server, gdb_server_cmd_v());
            } else {
                gdb_server_reply_empty();
            }

            gdb_serial_command_done();
        }
    }

    PT_END(&self.pt_server);
}


/*
 * ‘q name params...’
 * General query (‘q’) and set (‘Q’).
 */
PT_THREAD(gdb_server_cmd_q(void))
{
    PT_BEGIN(&self.pt_cmd);

    if(strncmp(self.cmd, "qSupported:", 11) == 0){
        PT_WAIT_THREAD(&self.pt_cmd, gdb_server_cmd_qSupported());
    } else if(strncmp(self.cmd, "qXfer:features:read:target.xml:", 31) == 0){
        gdb_server_cmd_qxfer_features_read_target_xml();
    } else if(strncmp(self.cmd, "qXfer:memory-map:read::", 23) == 0){
        gdb_server_cmd_qxfer_memory_map_read();
    } else if(strncmp(self.cmd, "qRcmd,", 6) == 0){
        PT_WAIT_THREAD(&self.pt_cmd, gdb_server_cmd_qRcmd());
    } else {
        gdb_server_reply_empty();
    }

    PT_END(&self.pt_cmd);
}


/*
 * ‘qSupported [:gdbfeature [;gdbfeature]... ]’
 * Tell the remote stub about features supported by gdb, and query the stub for
 * features it supports.
 */
PT_THREAD(gdb_server_cmd_qSupported(void))
{
    const char qSupported_res[] =
            "PacketSize=405"
            ";QStartNoAckMode+"
            ";qXfer:features:read+"
            ";qXfer:memory-map:read+"
            ";swbreak+"
            ";hwbreak+"
            ;

    PT_BEGIN(&self.pt_cmd_sub);

    gdb_serial_no_ack_mode(false);
    strncpy(self.res, qSupported_res, GDB_SERIAL_RESPONSE_BUFFER_SIZE);
    gdb_serial_response_done(strlen(qSupported_res), GDB_SERIAL_SEND_FLAG_ALL);
    PT_WAIT_THREAD(&self.pt_cmd_sub, gdb_server_connected());

    PT_END(&self.pt_cmd_sub);
}

/*
 * qXfer:features:read:target.xml
 */
void gdb_server_cmd_qxfer_features_read_target_xml(void)
{
    size_t target_xml_len;
    const char *target_xml_str;
    unsigned int read_addr;
    unsigned int read_len;

    sscanf(&self.cmd[31], "%x,%x", &read_addr, &read_len);

    if(read_len > GDB_SERIAL_RESPONSE_BUFFER_SIZE) {
        read_len = GDB_SERIAL_RESPONSE_BUFFER_SIZE;
    }

    target_xml_len = rvl_target_get_target_xml_len();

    if(read_len >= target_xml_len - read_addr) {
        read_len = target_xml_len - read_addr;
        self.res[0] = 'l';
    } else {
        self.res[0] = 'm';
    }

    target_xml_str = rvl_target_get_target_xml();
    strncpy(&self.res[1], &target_xml_str[read_addr], read_len);
    gdb_serial_response_done(read_len + 1, GDB_SERIAL_SEND_FLAG_ALL);
}


/*
 * qXfer:memory-map:read::
 */
void gdb_server_cmd_qxfer_memory_map_read(void)
{
    size_t memory_map_len;
    const rvl_target_memory_t* memory_map;
    size_t res_len;

    /*
     * 假设一包数据可以全部发完！
     */

    memory_map_len = rvl_target_get_memory_map_len();
    memory_map = rvl_target_get_memory_map();

    res_len = 0;
    res_len += snprintf(&self.res[0], GDB_SERIAL_RESPONSE_BUFFER_SIZE, "l<memory-map>");
    for(self.i = 0; self.i < memory_map_len; self.i++) {
#if RVL_TARGET_CONFIG_ADDR_WIDTH == 32
        res_len += snprintf(&self.res[res_len], GDB_SERIAL_RESPONSE_BUFFER_SIZE - res_len,
                "<memory type=\"%s\" start=\"0x%x\" length=\"0x%x\"",
                memory_map[self.i].type == rvl_target_memory_type_flash ? "flash" : "ram",
                (unsigned int)memory_map[self.i].start, (unsigned int)memory_map[self.i].length);

        if(memory_map[self.i].type == rvl_target_memory_type_flash) {
            res_len += snprintf(&self.res[res_len], GDB_SERIAL_RESPONSE_BUFFER_SIZE - res_len,
                    "><property name=\"blocksize\">0x%x</property></memory>",
                    (unsigned int)memory_map[self.i].blocksize);
        } else {
            res_len += snprintf(&self.res[res_len], GDB_SERIAL_RESPONSE_BUFFER_SIZE - res_len,
                    "/>");
        }
#else
#error FIXME
#endif
    }
    res_len += snprintf(&self.res[res_len], GDB_SERIAL_RESPONSE_BUFFER_SIZE - res_len,
            "</memory-map>");

    gdb_serial_response_done(res_len, GDB_SERIAL_SEND_FLAG_ALL);
}


/*
 * ‘qRcmd,command’
 * command (hex encoded) is passed to the local interpreter for execution.
 */
PT_THREAD(gdb_server_cmd_qRcmd(void))
{
    char c;
    size_t ret, len;
    const char * err_str;
    uint32_t err_pc;

    const char unspported_monitor_command[] = ":( unsupported monitor command!\n";

    PT_BEGIN(&self.pt_cmd_sub);

    ret = rvl_serial_getchar(&c);
    if(ret > 0) {
        self.res[0] = 'O';
        len = 0;
        do {
            bin_to_hex((uint8_t*)&c, &self.res[1 + len * 2], 1);
            len++;
            ret = rvl_serial_getchar(&c);
        } while (ret > 0);

        gdb_serial_response_done(len * 2 + 1, GDB_SERIAL_SEND_FLAG_ALL);

        PT_WAIT_UNTIL(&self.pt_cmd_sub, (self.res = gdb_serial_response_buffer()) != NULL);
    }

    self.mem_len = (self.cmd_len - 6) / 2;
    hex_to_bin(&self.cmd[6], self.mem_buffer, self.mem_len);
    self.mem_buffer[self.mem_len] = 0;

    if(strncmp((char*)self.mem_buffer, "reset", 5) == 0) {
        PT_WAIT_THREAD(&self.pt_cmd_sub, rvl_target_reset());
        gdb_server_reply_ok();
    } else if(strncmp((char*)self.mem_buffer, "halt", 4) == 0) {
        gdb_server_reply_ok();
    } else if(strncmp((char*)self.mem_buffer, "show error", 10) == 0) {
        rvl_target_get_error(&err_str, &err_pc);
        snprintf((char*)self.mem_buffer, GDB_SERIAL_RESPONSE_BUFFER_SIZE,
                "RV-LINK ERROR: %s, @0x%08x\r\n", err_str, (unsigned int)err_pc);
        len = strlen((char*)self.mem_buffer);
        self.res[0] = 'O';
        bin_to_hex(self.mem_buffer, &self.res[1], len);
        gdb_serial_response_done(len * 2 + 1, GDB_SERIAL_SEND_FLAG_ALL);

        PT_WAIT_UNTIL(&self.pt_cmd_sub, (self.res = gdb_serial_response_buffer()) != NULL);
        gdb_server_reply_ok();
    } else {
        bin_to_hex((uint8_t*)unspported_monitor_command, self.res, sizeof(unspported_monitor_command) - 1);
        gdb_serial_response_done((sizeof(unspported_monitor_command) - 1) * 2, GDB_SERIAL_SEND_FLAG_ALL);
    }

    PT_END(&self.pt_cmd_sub);
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

    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_read_core_registers(&self.regs[0]));

    for(i = 0; i < RVL_TARGET_CONFIG_REG_NUM; i++) {
        word_to_hex_le(self.regs[i], &self.res[i * (RVL_TARGET_CONFIG_REG_WIDTH / 8 * 2)]);
    }

    gdb_serial_response_done(RVL_TARGET_CONFIG_REG_WIDTH / 8 * 2 * RVL_TARGET_CONFIG_REG_NUM, GDB_SERIAL_SEND_FLAG_ALL);

    PT_END(&self.pt_cmd);
}


/*
 * ‘G XX...’
 * Write general registers.
 */
PT_THREAD(gdb_server_cmd_G(void))
{
    int i;

    PT_BEGIN(&self.pt_cmd);

    for(i = 0; i < RVL_TARGET_CONFIG_REG_NUM; i++) {
        hex_to_word_le(&self.cmd[i * (RVL_TARGET_CONFIG_REG_WIDTH / 8 * 2) + 1], &self.regs[i]);
    }

    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_write_core_registers(&self.regs[0]));

    gdb_server_reply_ok();

    PT_END(&self.pt_cmd);
}


/*
 * ‘p n’
 * Read the value of register n; n is in hex.
 */
PT_THREAD(gdb_server_cmd_p(void))
{
    PT_BEGIN(&self.pt_cmd);

    sscanf(&self.cmd[1], "%x", &self.reg_tmp_num);

    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_read_register(&self.reg_tmp, self.reg_tmp_num));

    word_to_hex_le(self.reg_tmp, &self.res[0]);

    gdb_serial_response_done(RVL_TARGET_CONFIG_REG_WIDTH / 8 * 2, GDB_SERIAL_SEND_FLAG_ALL);

    PT_END(&self.pt_cmd);
}


/*
 * ‘P n...=r...’
 * Write register n... with value r... The register number n is in hexadecimal,
 * and r... contains two hex digits for each byte in the register (target byte order).
 */
PT_THREAD(gdb_server_cmd_P(void))
{
    const char *p;

    PT_BEGIN(&self.pt_cmd);

    sscanf(&self.cmd[1], "%x", &self.reg_tmp_num);
    p = strchr(&self.cmd[1], '=');
    p++;

    hex_to_word_le(p, &self.reg_tmp);
    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_write_register(self.reg_tmp, self.reg_tmp_num));

    gdb_server_reply_ok();

    PT_END(&self.pt_cmd);
}


/*
 * ‘m addr,length’
 * Read length addressable memory units starting at address addr.
 * Note that addr may not be aligned to any particular boundary.
 */
PT_THREAD(gdb_server_cmd_m(void))
{
    char *p;

    PT_BEGIN(&self.pt_cmd);

    p = strchr(&self.cmd[1], ',');
    p++;
#if RVL_TARGET_CONFIG_ADDR_WIDTH == 32
    sscanf(&self.cmd[1], "%x", (unsigned int*)(&self.mem_addr));
#else
#error
#endif
    sscanf(p, "%x", (unsigned int*)(&self.mem_len));
    if(self.mem_len > sizeof(self.mem_buffer)) {
        self.mem_len = sizeof(self.mem_buffer);
    }

    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_read_memory(self.mem_buffer, self.mem_addr, self.mem_len));

    bin_to_hex(self.mem_buffer, self.res, self.mem_len);
    gdb_serial_response_done(self.mem_len * 2, GDB_SERIAL_SEND_FLAG_ALL);

    PT_END(&self.pt_cmd);
}


/*
 * ‘M addr,length:XX...’
 * Write length addressable memory units starting at address addr.
 */
PT_THREAD(gdb_server_cmd_M(void))
{
    const char *p;

    PT_BEGIN(&self.pt_cmd);

    sscanf(&self.cmd[1], "%x,%x", (unsigned int*)(&self.mem_addr), (unsigned int*)(&self.mem_len));
    p = strchr(&self.cmd[1], ':');
    p++;

    if(self.mem_len > sizeof(self.mem_buffer)) {
        self.mem_len = sizeof(self.mem_buffer);
    }

    hex_to_bin(p, self.mem_buffer, self.mem_len);
    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_write_memory(self.mem_buffer, self.mem_addr, self.mem_len));

    gdb_server_reply_ok();

    PT_END(&self.pt_cmd);
}


/*
 * ‘X addr,length:XX...’
 * Write data to memory, where the data is transmitted in binary.
 */
PT_THREAD(gdb_server_cmd_X(void))
{
    const char *p;
    size_t length;

    PT_BEGIN(&self.pt_cmd);

    sscanf(&self.cmd[1], "%x,%x", (unsigned int*)(&self.mem_addr), (unsigned int*)(&self.mem_len));
    if(self.mem_len == 0) {
        gdb_server_reply_ok();
        PT_EXIT(&self.pt_cmd);
    }

    p = strchr(&self.cmd[1], ':');
    p++;

    if(self.mem_len > sizeof(self.mem_buffer)) {
        self.mem_len = sizeof(self.mem_buffer);
    }

    length = self.cmd_len - ((size_t)p - (size_t)self.cmd);
    bin_decode((uint8_t*)p, self.mem_buffer, length);

    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_write_memory(self.mem_buffer, self.mem_addr, self.mem_len));

    gdb_server_reply_ok();
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

    PT_WAIT_THREAD(&self.pt_cmd, gdb_server_disconnected());

    PT_END(&self.pt_cmd);
}


/*
 * ‘c [addr]’
 * Continue at addr, which is the address to resume. If addr is omitted, resume
 * at current address.
 */
PT_THREAD(gdb_server_cmd_c(void))
{
    PT_BEGIN(&self.pt_cmd);

    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_resume());
    gdb_server_target_run(true);

    PT_END(&self.pt_cmd);
}


/*
 * ‘s [addr]’
 * Single step, resuming at addr. If addr is omitted, resume at same address.
 */
PT_THREAD(gdb_server_cmd_s(void))
{
    PT_BEGIN(&self.pt_cmd);

    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_step());
    gdb_server_target_run(true);

    PT_END(&self.pt_cmd);
}


/*
 * ‘z type,addr,kind’
 * Insert (‘Z’) or remove (‘z’) a type breakpoint or watchpoint starting at address
 * address of kind kind.
 */
PT_THREAD(gdb_server_cmd_z(void))
{
    int type, addr, kind;

    PT_BEGIN(&self.pt_cmd);

    sscanf(self.cmd, "z%x,%x,%x", &type, &addr, &kind);
    self.breakpoint_type = type;
    self.breakpoint_addr = addr;
    self.breakpoint_kind = kind;

    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_remove_breakpoint(
            self.breakpoint_type, self.breakpoint_addr, self.breakpoint_kind, &self.breakpoint_err));
    if(self.breakpoint_err == 0) {
        gdb_server_reply_ok();
    } else {
        gdb_server_reply_err(self.breakpoint_err);
    }

    PT_END(&self.pt_cmd);
}


/*
 * ‘Z type,addr,kind’
 * Insert (‘Z’) or remove (‘z’) a type breakpoint or watchpoint starting at address
 * address of kind kind.
 */
PT_THREAD(gdb_server_cmd_Z(void))
{
    int type, addr, kind;

    PT_BEGIN(&self.pt_cmd);

    sscanf(self.cmd, "Z%x,%x,%x", &type, &addr, &kind);
    self.breakpoint_type = type;
    self.breakpoint_addr = addr;
    self.breakpoint_kind = kind;

    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_insert_breakpoint(
            self.breakpoint_type, self.breakpoint_addr, self.breakpoint_kind, &self.breakpoint_err));
    if(self.breakpoint_err == 0) {
        gdb_server_reply_ok();
    } else {
        gdb_server_reply_err(self.breakpoint_err);
    }

    PT_END(&self.pt_cmd);
}


/*
 * Ctrl+C
 */
PT_THREAD(gdb_server_cmd_ctrl_c(void))
{
    PT_BEGIN(&self.pt_cmd);

    PT_WAIT_THREAD(&self.pt_cmd, rvl_target_halt());

    PT_END(&self.pt_cmd);
}


/*
 * ‘v’
 * Packets starting with ‘v’ are identified by a multi-letter name.
 */
PT_THREAD(gdb_server_cmd_v(void))
{
    PT_BEGIN(&self.pt_cmd);

    if(strncmp(self.cmd, "vFlashErase:", 12) == 0) {
        PT_WAIT_THREAD(&self.pt_cmd, gdb_server_cmd_vFlashErase());
    } else if(strncmp(self.cmd, "vFlashWrite:", 12) == 0) {
        PT_WAIT_THREAD(&self.pt_cmd, gdb_server_cmd_vFlashWrite());
    } else if(strncmp(self.cmd, "vFlashDone", 10) == 0) {
        PT_WAIT_THREAD(&self.pt_cmd, gdb_server_cmd_vFlashDone());
    } else if(strncmp(self.cmd, "vMustReplyEmpty", 15) == 0) {
        PT_WAIT_THREAD(&self.pt_cmd, gdb_server_cmd_vMustReplyEmpty());
    } else {
        gdb_server_reply_empty();
    }

    PT_END(&self.pt_cmd);
}


/*
 * ‘vFlashErase:addr,length’
 * Direct the stub to erase length bytes of flash starting at addr.
 */
PT_THREAD(gdb_server_cmd_vFlashErase(void))
{
    int addr, length;

    PT_BEGIN(&self.pt_cmd_sub);

    sscanf(&self.cmd[12], "%x,%x", &addr, &length);
    self.mem_addr = addr;
    self.mem_len = length;

    PT_WAIT_THREAD(&self.pt_cmd_sub, rvl_target_flash_erase(self.mem_addr, self.mem_len, &self.flash_err));
    if(self.flash_err == 0) {
        gdb_server_reply_ok();
    } else {
        gdb_server_reply_err(self.flash_err);
        PT_WAIT_THREAD(&self.pt_cmd_sub, gdb_server_disconnected());
    }

    PT_END(&self.pt_cmd_sub);
}


/*
 * ‘vFlashWrite:addr:XX...’
 * Direct the stub to write data to flash address addr.
 * The data is passed in binary form using the same encoding as for the ‘X’ packet.
 */
PT_THREAD(gdb_server_cmd_vFlashWrite(void))
{
    int addr;
    const char *p;
    size_t length;

    PT_BEGIN(&self.pt_cmd_sub);

    sscanf(&self.cmd[12], "%x", &addr);
    self.mem_addr = addr;

    p = strchr(&self.cmd[12], ':');
    p++;

    length = self.cmd_len - ((size_t)p - (size_t)self.cmd);
    self.mem_len = bin_decode((uint8_t*)p, self.mem_buffer, length);

    PT_WAIT_THREAD(&self.pt_cmd_sub, rvl_target_flash_write(self.mem_addr, self.mem_len, self.mem_buffer, &self.flash_err));
    if(self.flash_err == 0) {
        gdb_server_reply_ok();
    } else {
        gdb_server_reply_err(self.flash_err);
        PT_WAIT_THREAD(&self.pt_cmd_sub, gdb_server_disconnected());
    }

    PT_END(&self.pt_cmd_sub);
}


/*
 * ‘vFlashDone’
 * Indicate to the stub that flash programming operation is finished.
 */
PT_THREAD(gdb_server_cmd_vFlashDone(void))
{
    PT_BEGIN(&self.pt_cmd_sub);

    PT_WAIT_THREAD(&self.pt_cmd_sub, rvl_target_flash_done());
    gdb_server_reply_ok();

    PT_END(&self.pt_cmd_sub);
}


/*
 * ‘vMustReplyEmpty’
 * RV-LINK 利用产生非预期的 vMustReplyEmpty 响应告知错误
 */
PT_THREAD(gdb_server_cmd_vMustReplyEmpty(void))
{
    size_t len;
    char c;

    PT_BEGIN(&self.pt_cmd_sub);

    if(self.target_error == rvl_target_error_none) {
        gdb_server_reply_empty();
    } else {
        len = 0;
        while(rvl_serial_getchar(&c)) {
            self.res[len] = c;
            len++;
        }
        gdb_serial_response_done(len, GDB_SERIAL_SEND_FLAG_ALL);

        PT_WAIT_THREAD(&self.pt_cmd_sub, gdb_server_disconnected());
    }

    PT_END(&self.pt_cmd_sub);
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


static void gdb_server_reply_err(int err)
{
    snprintf(self.res, GDB_SERIAL_RESPONSE_BUFFER_SIZE, "E%02x", err);
    gdb_serial_response_done(strlen(self.res), GDB_SERIAL_SEND_FLAG_ALL);
}


PT_THREAD(gdb_server_connected(void))
{
    char c;

    PT_BEGIN(&self.pt_sub_routine);

    while(rvl_serial_getchar(&c)) {};

    rvl_led_gdb_connect(1);
    self.gdb_connected = true;
    gdb_server_target_run(false);

    rvl_target_init();
    PT_WAIT_THREAD(&self.pt_sub_routine, rvl_target_init_post(&self.target_error));

    if(self.target_error == rvl_target_error_none) {
        PT_WAIT_THREAD(&self.pt_sub_routine, rvl_target_halt());
        PT_WAIT_THREAD(&self.pt_sub_routine, rvl_target_init_after_halted(&self.target_error));
    }

    if(self.target_error != rvl_target_error_none) {
        switch(self.target_error) {
        case rvl_target_error_line:
            rvl_serial_puts("\nRV-LINK ERROR: the target is not connected!\n");
            break;
        case rvl_target_error_compat:
            rvl_serial_puts("\nRV-LINK ERROR: the target is not supported, upgrade RV-LINK firmware!\n");
            break;
        case rvl_target_error_debug_module:
            rvl_serial_puts("\nRV-LINK ERROR: something wrong with debug module!\n");
            break;
        case rvl_target_error_protected:
            rvl_serial_puts("\nRV-LINK ERROR: the target under protected! disable protection then try again.\n");
            break;
        default:
            rvl_serial_puts("\nRV-LINK ERROR: unknown error!\n");
            break;
        }
    }

    rvl_serial_puts("RV-LINK " RVL_APP_CONFIG_GDB_SERVER_VERSION ": ");
    rvl_serial_puts(RVL_LINK_CONFIG_NAME);
    rvl_serial_puts(", configed for ");
    rvl_serial_puts(rvl_target_get_name());
    rvl_serial_puts(" family.\n");

    PT_END(&self.pt_sub_routine);
}


PT_THREAD(gdb_server_disconnected(void))
{
    PT_BEGIN(&self.pt_sub_routine);

    if(self.target_running == false) {
        if(self.target_error != rvl_target_error_line) {
            PT_WAIT_THREAD(&self.pt_sub_routine, rvl_target_resume());
            gdb_server_target_run(true);
        }
    }

    if(self.target_error != rvl_target_error_line) {
        PT_WAIT_THREAD(&self.pt_sub_routine, rvl_target_fini_pre());
    }
    rvl_target_fini();

    self.gdb_connected = false;
    rvl_led_gdb_connect(0);

    PT_END(&self.pt_sub_routine);
}


static void gdb_server_target_run(bool run)
{
    self.target_running = run;
    rvl_led_target_run(run);
}


static void bin_to_hex(const uint8_t *bin, char *hex, size_t nbyte)
{
    size_t i;
    uint8_t hi;
    uint8_t lo;

    for(i = 0; i < nbyte; i++) {
        hi = (*bin >> 4) & 0xf;
        lo = *bin & 0xf;

        if(hi < 10) {
            *hex = '0' + hi;
        } else {
            *hex = 'a' + hi - 10;
        }

        hex++;

        if(lo < 10) {
            *hex = '0' + lo;
        } else {
            *hex = 'a' + lo - 10;
        }

        hex++;
        bin++;
    }
}


static void word_to_hex_le(uint32_t word, char *hex)
{
    uint8_t bytes[4];

    bytes[0] = word & 0xff;
    bytes[1] = (word >> 8) & 0xff;
    bytes[2] = (word >> 16) & 0xff;
    bytes[3] = (word >> 24) & 0xff;

    bin_to_hex(bytes, hex, 4);
}


static void hex_to_bin(const char *hex, uint8_t *bin, size_t nbyte)
{
    size_t i;
    uint8_t hi, lo;
    for(i = 0; i < nbyte; i++) {
        if(hex[i * 2] <= '9') {
            hi = hex[i * 2] - '0';
        } else if(hex[i * 2] <= 'F') {
            hi = hex[i * 2] - 'A' + 10;
        } else {
            hi = hex[i * 2] - 'a' + 10;
        }

        if(hex[i * 2 + 1] <= '9') {
            lo = hex[i * 2 + 1] - '0';
        } else if(hex[i * 2 + 1] <= 'F') {
            lo = hex[i * 2 + 1] - 'A' + 10;
        } else {
            lo = hex[i * 2 + 1] - 'a' + 10;
        }

        bin[i] = (hi << 4) | lo;
    }
}


static void hex_to_word_le(const char *hex, uint32_t *word)
{
    uint8_t bytes[4];

    hex_to_bin(hex, bytes, 4);

    *word = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
}


static size_t bin_decode(const uint8_t* xbin, uint8_t* bin, size_t xbin_len)
{
    size_t bin_len = 0;
    size_t i;
    int escape_found = 0;

    for(i = 0; i < xbin_len; i++){
        if(xbin[i] == 0x7d) {
            escape_found = 1;
        } else {
            if(escape_found) {
                bin[bin_len] = xbin[i] ^ 0x20;
                escape_found = 0;
            } else {
                bin[bin_len] = xbin[i];
            }
            bin_len++;
        }
    }

    return bin_len;
}
