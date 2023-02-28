#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/bootrom_metadata.h"
#include "hardware/watchdog.h"
#include "ff.h"

// Set the path to the update file
#define UPDATE_FILE_PATH "update.uf2"

int main() {
    stdio_init_all();

    // Mount the SD card
    FATFS fs;
    FRESULT res = f_mount(&fs, "0:/", 1);
    if (res != FR_OK) {
        printf("Failed to mount SD card: %d\n", res);
        return 1;
    }

    // Check for the presence of the update file
    FIL file;
    res = f_open(&file, UPDATE_FILE_PATH, FA_READ);
    if (res != FR_OK) {
        printf("Update file not found\n");
        return 0;
    }

    // Get the size of the update file
    uint32_t update_size = f_size(&file);

    // Allocate memory for the update buffer
    uint8_t* update_buffer = (uint8_t*)malloc(update_size);
    if (!update_buffer) {
        printf("Failed to allocate memory for update buffer\n");
        f_close(&file);
        return 1;
    }

    // Read the update file into the update buffer
    UINT bytes_read;
    res = f_read(&file, update_buffer, update_size, &bytes_read);
    if (res != FR_OK || bytes_read != update_size) {
        printf("Failed to read update file\n");
        f_close(&file);
        free(update_buffer);
        return 1;
    }

    // Close the update file
    f_close(&file);
    
    // Delete the file
    f_unlink(UPDATE_FILE_PATH);

    // Disable the watchdog
    watchdog_disable();

    // Disable interrupts
    __disable_irq();

    // Switch to the bootrom
    uint32_t metadata[16];
    memset(metadata, 0, sizeof(metadata));
    metadata[0] = BOOTROM_MAGIC | 1;
    metadata[1] = update_size;
    metadata[2] = BOOTROM_METADATA_FLAG_VALIDATE | BOOTROM_METADATA_FLAG_ERASE;
    metadata[3] = BOOTROM_METADATA_TYPE_APP;
    metadata[4] = BOOTROM_METADATA_ADDR_APP;
    metadata[5] = (uint32_t)update_buffer;
    metadata[6] = 0;
    metadata[7] = 0;
    metadata[8] = 0;
    metadata[9] = 0;
    metadata[10] = 0;
    metadata[11] = 0;
    metadata[12] = 0;
    metadata[13] = 0;
    metadata[14] = 0;
    metadata[15] = 0;

    // Jump to the bootrom
    bootloader_reset(metadata);

    // The code should not reach here, as the bootloader_reset() function will cause a reset
    // However, we include a return statement to avoid a compiler warning
    return 0;
}
