#ifndef __GDB_SERIAL_H__
#define __GDB_SERIAL_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define GDB_SERIAL_COMMAND_BUFFER_SIZE          1024
#define GDB_SERIAL_RESPONSE_BUFFER_SIZE         1024

#define GDB_SERIAL_SEND_FLAG_DOLLAR             (1 << 0)
#define GDB_SERIAL_SEND_FLAG_SHARP              (1 << 1)
#define GDB_SERIAL_SEND_FLAG_ALL                (GDB_SERIAL_SEND_FLAG_DOLLAR | GDB_SERIAL_SEND_FLAG_SHARP)

void gdb_serial_init(void);

/*
 * GDB Server 调用 gdb_serial_command_buffer 获得命令的首地址，不包括 '$'，
 * 如果返回 NULL 说明 USB Serial 真正接收命令，暂时不能被 GDB Server 使用
 */
const char*     gdb_serial_command_buffer(void);

/*
 * GDB Server 调用 gdb_serial_command_length 获得命令的长度，不包括 '$' '#' checksum
 * 仅当 gdb_serial_command_buffer 返回有效值时，gdb_serial_command_length 才有效
 */
size_t          gdb_serial_command_length(void);

/*
 * GDB Server 处理完命令后调用 gdb_serial_command_done 通知 GDB Serial 可以接收下一条命令了
 */
void            gdb_serial_command_done(void);

/*
 * GDB Server 调用 gdb_serial_response_buffer 获得响应数据缓冲的首地址，
 * GDB Server 不需要写入 '$' '#' checksum，
 * 如果返回 NULL，说明响应缓冲真正被 USB Serial 使用，GDB Server 不能使用
 */
char *          gdb_serial_response_buffer(void);
/*
 * GDB Server 调用 gdb_serial_response_done 通知 GDB Serial 发送响应，
 * 发送前 GDB Serial 根据 send_flags 填入 '$' '#' checksum
 */
void            gdb_serial_response_done(size_t len, uint32_t send_flags);

/*
 * 进入 NoAckMode
 */
void            gdb_serial_no_ack_mode(bool no_ack_mode);

#endif /* __GDB_SERIAL_H__ */
