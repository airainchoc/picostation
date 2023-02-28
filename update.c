#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "ff.h"

#define FILENAME "picostation.uf2"

void update() {
    FATFS fs;
    FRESULT res;
    DIR dir;
    FILINFO fno;
    UINT br;
    uint8_t buf[256];
    uint32_t addr = 0;
    uint32_t size = 0;

    res = f_mount(&fs, "", 1);
    if (res != FR_OK) {
        return;
    }

    res = f_findfirst(&dir, &fno, "", FILENAME);
    if (res == FR_OK && fno.fname[0] != '.') {
        FIL file;
        res = f_open(&file, fno.fname, FA_READ);
        if (res == FR_OK) {
            size = f_size(&file);

            // Erase flash
            flash_range_erase(0, size);

            // Write to flash
            while (f_read(&file, buf, sizeof(buf), &br) == FR_OK && br > 0) {
                flash_range_program(addr, buf, br);
                addr += br;
            }
            f_close(&file);

            // Delete file
            f_unlink(fno.fname);
        }
    } else {
        f_mount(NULL, "", 1);
        return;
    }

    f_mount(NULL, "", 1);
}
