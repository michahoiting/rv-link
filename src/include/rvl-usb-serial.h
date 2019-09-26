#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "pt/pt.h"

/*
 * 以下三个函数由 app 的 main 函数调用，由 link 的 USB Serial 组件实现。
 */
void usb_serial_init(void);
PT_THREAD(usb_serial_recv_poll(void));
PT_THREAD(usb_serial_send_poll(void));

/*
 * USB Serial 接收到数据后调用 usb_serial_put_recv_data 向使用虚拟串口的组件传递数据，
 * 如果返回值为 false，那么缓冲满了，稍候再试；如果返回值为 true，说明传递成功，可以接收下一包数据了。
 * 该函数由使用虚拟串口的组件实现。
 */
bool            usb_serial_put_recv_data(const uint8_t* buffer, size_t len);

/*
 * USB Serial 调用 usb_serial_get_send_data 检查是否有数据要发送，
 * 如果返回值有效，那么说明有数据发送，待发送的数据长度由 len 参数返回；
 * 如果返回为 NULL ，说明没数据要发送。
 * 该函数由使用虚拟串口的组件实现。
 */
const uint8_t*  usb_serial_get_send_data(size_t* len);

/*
 * USB Serial 发送数据完成后调用 gdb_serial_data_sent 通知使用虚拟串口的组件，
 * 使用方收到该通知后，可以重新开始使用发送缓冲。
 */
void            usb_serial_data_sent(void);

