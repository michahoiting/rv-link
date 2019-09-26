#include "pt/pt.h"

void gdb_server_init(void);
PT_THREAD(gdb_server_poll(void));
