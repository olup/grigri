#pragma once

#include <FS.h>
#include <SD_MMC.h>
#include <TFT_eSPI.h>

void display_fill_black();
void display_init();
void display_set_bl(int power);
int display_get_bl();
void display_draw_image(const char *filename);