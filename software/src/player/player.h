#pragma once

#include <Arduino.h>

void player_init();

void player_stop();

void player_play(const char *path);

void player_togglePause();

const bool player_isPlaying();

void player_moveRelative(int seconds);

uint32_t player_getPosition();

void player_loop();

void audio_info(const char *info);
void audio_id3data(const char *info);
void audio_bitrate(const char *info);

void player_setVolume(uint8_t volume);
uint8_t player_get_volume();