#ifndef __GDB_SERIAL_H__
#define __GDB_SERIAL_H__

#include <stdbool.h>
#include <stdint.h>

#define GDB_SERIAL_COMMAND_BUFFER_SIZE          1024
#define GDB_SERIAL_RESPONSE_BUFFER_SIZE         1024

#define GDB_SERIAL_SEND_FLAG_DOLLAR             (1 << 0)
#define GDB_SERIAL_SEND_FLAG_SHARP              (1 << 1)
#define GDB_SERIAL_SEND_FLAG_ALL                (GDB_SERIAL_SEND_FLAG_DOLLAR | GDB_SERIAL_SEND_FLAG_SHARP)

void gdb_serial_init(void);

/*
 ******************************************************************************
 * interface for gdb server
 */

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
void            gdb_serial_no_ack_mode(void);

/*
 ******************************************************************************
 * interface for usb serial
 */
/*
 * USB Serial 接收到数据后调用 gdb_serial_command_recv 传入数据，
 * 如果返回值为 false，那么 command 缓冲满了，稍候再试
 */
bool            gdb_serial_command_recv(const char* buffer, size_t len);

/*
 * USB Serial 任务检查是否由响应要发送，如果返回为 NULL ，说明没数据，非 NULL，说明有数据，
 * 数据长度有 len 返回
 */
const char*     gdb_serial_response_send(size_t* len);

/*
 * USB Serial 发送响应完成后调用 gdb_serial_response_sent
 */
void            gdb_serial_response_sent(void);



#endif /* __GDB_SERIAL_H__ */
