#pragma once

#pragma once

#include "FS.h"
#include "SD_MMC.h"

void fs_init();
int fs_read_file(const char* fileName, String& fileContent);
int fs_write_file(const char* fileName, char* fileContent);
uint64_t fs_sd_card_total_space();
uint64_t fs_sd_card_used_space();