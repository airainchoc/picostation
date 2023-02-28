#ifndef UPDATE_H
#define UPDATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#define UPDATE_FILE_PATH "/sdcard/update.uf2"
#define UPDATE_MAGIC_LENGTH 4
#define UPDATE_MAGIC_NUMBER 0x57f2c384

void update();

#endif /* UPDATE_H */
