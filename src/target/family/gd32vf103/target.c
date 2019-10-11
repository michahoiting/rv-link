#include "rvl-target.h"
#include "pt/timer.h"

#define FMC_BASE            0x40022000
#define FMC_WS              (FMC_BASE + 0x00)
#define FMC_KEY             (FMC_BASE + 0x04)
#define FMC_OBKEY           (FMC_BASE + 0x08)
#define FMC_STAT            (FMC_BASE + 0x0C)
#define FMC_CTL             (FMC_BASE + 0x10)
#define FMC_ADDR            (FMC_BASE + 0x14)
#define FMC_OBSTAT          (FMC_BASE + 0x1C)
#define FMC_WP              (FMC_BASE + 0x20)
#define FMC_PID             (FMC_BASE + 0x100)

#define FMC_KEY_VALUE1      0x45670123
#define FMC_KEY_VALUE2      0xCDEF89AB

#define FMC_OBKEY_VALUE1    0x45670123
#define FMC_OBKEY_VALUE2    0xCDEF89AB

#define FMC_STAT_ENDF       (1 << 5) // End of operation flag bit
#define FMC_STAT_WPERR      (1 << 4) // Erase/Program protection error flag bit
#define FMC_STAT_PGERR      (1 << 2) // Program error flag bit
#define FMC_STAT_BUSY       (1 << 0) // The flash busy bit

#define FMC_CTL_ENDIE       (1 << 12) // End of operation interrupt enable bit
#define FMC_CTL_ERRIE       (1 << 10) // Error interrupt enable bit
#define FMC_CTL_OBWEN       (1 << 9) // Option byte erase/program enable bit
#define FMC_CTL_LK          (1 << 7) // FMC_CTL lock bit
#define FMC_CTL_START       (1 << 6) // Send erase command to FMC bit
#define FMC_CTL_OBER        (1 << 5) // Option bytes erase command bit
#define FMC_CTL_OBPG        (1 << 4) // Option bytes program command bit
#define FMC_CTL_MER         (1 << 2) // Main flash mass erase for bank0 command bit
#define FMC_CTL_PER         (1 << 1) // Main flash page erase for bank0 command bit
#define FMC_CTL_PG          (1 << 0) // Main flash program for bank0 command bit

#define MEMORY_DENSITY      0x1FFFF7E0


typedef struct gd32vf103_target_s
{
    struct pt pt;
    struct timer timer;
    uint32_t reg_value;
    rvl_target_addr_t start;
    rvl_target_addr_t end;
    int i;
    rvl_target_memory_t memory_map[3];
}gd32vf103_target_t;

static gd32vf103_target_t gd32vf103_target_i;
#define self gd32vf103_target_i


void riscv_target_init(void);
PT_THREAD(riscv_target_init_post(rvl_target_error_t *err));
PT_THREAD(riscv_target_fini_pre(void));
void riscv_target_fini(void);
uint32_t riscv_target_get_idcode(void);


void rvl_target_init(void)
{
    riscv_target_init();

    PT_INIT(&self.pt);
}


PT_THREAD(rvl_target_init_post(rvl_target_error_t *err))
{
    PT_BEGIN(&self.pt);

    PT_WAIT_THREAD(&self.pt, riscv_target_init_post(err));

    if(*err == rvl_target_error_none) {
        if(riscv_target_get_idcode() != 0x1000563d) {
            *err = rvl_target_error_compat;
            PT_EXIT(&self.pt);
        }
    }

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_init_after_halted(void))
{
    uint32_t flash_density;

    PT_BEGIN(&self.pt);

    // get memory density information
    PT_WAIT_THREAD(&self.pt, rvl_target_read_memory((uint8_t*)&self.reg_value, MEMORY_DENSITY, 4));
    flash_density = self.reg_value & 0xffff;
    flash_density *= 1024;

    self.memory_map[0].type = rvl_target_memory_type_ram;
    self.memory_map[0].start = 0;
    self.memory_map[0].length = 0x08000000;

    self.memory_map[1].type = rvl_target_memory_type_flash;
    self.memory_map[1].start = 0x08000000;
    self.memory_map[1].length = flash_density;
    self.memory_map[1].blocksize = 0x400;

    self.memory_map[2].type = rvl_target_memory_type_ram;
    self.memory_map[2].start = 0x08000000 + flash_density;
    self.memory_map[2].length = ~(0x08000000 + flash_density) + 1;

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_fini_pre(void))
{
    PT_BEGIN(&self.pt);

    PT_WAIT_THREAD(&self.pt, riscv_target_fini_pre());

    PT_END(&self.pt);
}


void rvl_target_fini(void)
{
    riscv_target_fini();
}


PT_THREAD(rvl_target_flash_erase(rvl_target_addr_t addr, size_t len, int* err))
{
    PT_BEGIN(&self.pt);

    // Unlock the FMC_CTL registers
    self.reg_value = FMC_KEY_VALUE1;
    PT_WAIT_THREAD(&self.pt, rvl_target_write_memory((uint8_t*)&self.reg_value, FMC_KEY, 4));
    self.reg_value = FMC_KEY_VALUE2;
    PT_WAIT_THREAD(&self.pt, rvl_target_write_memory((uint8_t*)&self.reg_value, FMC_KEY, 4));

    // Check LK
    for(self.i = 0; self.i < 6; self.i++) {
        PT_WAIT_THREAD(&self.pt, rvl_target_read_memory((uint8_t*)&self.reg_value, FMC_CTL, 4));
        if((self.reg_value & FMC_CTL_LK) == 0) {
            break;
        }
    }
    if(self.reg_value & FMC_CTL_LK) {
        *err = 1;
        PT_EXIT(&self.pt);
    }

    self.start = addr;
    self.end = addr + len;
    for(; self.start < self.end; self.start += 1024) {
        self.reg_value = FMC_CTL_PER;
        PT_WAIT_THREAD(&self.pt, rvl_target_write_memory((uint8_t*)&self.reg_value, FMC_CTL, 4));
        PT_WAIT_THREAD(&self.pt, rvl_target_write_memory((uint8_t*)&self.start, FMC_ADDR, 4));
        self.reg_value |= FMC_CTL_START;
        PT_WAIT_THREAD(&self.pt, rvl_target_write_memory((uint8_t*)&self.reg_value, FMC_CTL, 4));

        // GD32VF103 Page erase time:
        // 数据手册给出的数据是：300 ms
        // 实际情况是：300 ms 内确实擦除成功了，会读取状态寄存器进行判断，多给一些时间也无妨
        timer_set(&self.timer, 600 * 1000);
        do {
            PT_WAIT_THREAD(&self.pt, rvl_target_read_memory((uint8_t*)&self.reg_value, FMC_STAT, 4));
            if(timer_expired(&self.timer)) {
                break;
            }
        } while (self.reg_value & FMC_STAT_BUSY);

        if(self.reg_value & FMC_STAT_BUSY) {
            *err = 2;
            PT_EXIT(&self.pt);
        }
        if(self.reg_value & FMC_STAT_WPERR) {
            *err = 3;
            PT_EXIT(&self.pt);
        }
    }

    *err = 0;

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_flash_write(rvl_target_addr_t addr, size_t len, uint8_t* buffer, int* err))
{
    PT_BEGIN(&self.pt);

    self.reg_value = FMC_CTL_PG;
    PT_WAIT_THREAD(&self.pt, rvl_target_write_memory((uint8_t*)&self.reg_value, FMC_CTL, 4));

    self.start = addr;
    for(self.i = 0; self.i < len; self.i += 4) {
        // GD32VF103 Word programming time:
        // 数据手册给出的数据是：86 us
        // 实际测量的结果是：500 us，多加 100 us 作为裕量
        timer_set(&self.timer, 600);

        self.reg_value = *((uint32_t*)&buffer[self.i]);
        PT_WAIT_THREAD(&self.pt, rvl_target_write_memory((uint8_t*)&self.reg_value, self.start, 4));

        PT_WAIT_UNTIL(&self.pt, timer_expired(&self.timer));

        if((self.start & 0xff) == 0) {
            PT_WAIT_THREAD(&self.pt, rvl_target_read_memory((uint8_t*)&self.reg_value, FMC_STAT, 4));
            if(self.reg_value & FMC_STAT_BUSY) {
                *err = 2;
                PT_EXIT(&self.pt);
            }
            if(self.reg_value & FMC_STAT_PGERR) {
                *err = 3;
                PT_EXIT(&self.pt);
            }
        }

        self.start += 4;
    }

    *err = 0;

    PT_END(&self.pt);
}


PT_THREAD(rvl_target_flash_done(void))
{
    PT_BEGIN(&self.pt);

    self.reg_value = FMC_CTL_LK;
    PT_WAIT_THREAD(&self.pt, rvl_target_write_memory((uint8_t*)&self.reg_value, FMC_CTL, 4));

    PT_END(&self.pt);
}


const char *rvl_target_get_name(void)
{
    return "GD32VF103";
}


const rvl_target_memory_t *rvl_target_get_memory_map(void)
{
    return &self.memory_map[0];
}


size_t rvl_target_get_memory_map_len(void)
{
    return 3;
}

