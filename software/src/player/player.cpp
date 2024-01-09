
#include "./player.h"

#include "./define.h"
#include "./file/file.h"
#include "Audio.h"

Audio audio;

void player_init() {
  // enable amplifier
  pinMode(AMP_EN, OUTPUT);
  digitalWrite(AMP_EN, HIGH);
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolumeSteps(100);
  audio.setVolume(50);
}

void player_stop() { audio.stopSong(); }

void player_play(const char *path, int32_t position) {
  player_stop();
  Serial.print("Playing with seek  :");
  audio.connecttoFS(SD_MMC, path);
}

void player_togglePause() {
  Serial.println("Toogle pause");
  audio.pauseResume();
}

const bool player_isPlaying() { return audio.isRunning(); }

void player_moveRelative(int seconds) {
  uint32_t newPosition = audio.getAudioCurrentTime() + seconds;
  audio.setAudioPlayPosition(newPosition);
}

uint32_t player_get_position() { return audio.getAudioCurrentTime(); }
void player_set_position(uint32_t position) {
  audio.setAudioPlayPosition(position);
}

void player_loop() { audio.loop(); }

void audio_info(const char *info) {
  Serial.print("info        ");
  Serial.println(info);
}
void audio_id3data(const char *info) {  // id3 metadata
  Serial.print("id3data     ");
  Serial.println(info);
}
void audio_bitrate(const char *info) {
  Serial.print("bitrate     ");
  Serial.println(info);
}

void player_setVolume(uint8_t volume) {
  // int drop = 0;
  // if (volume > 50) {
  //   drop = map(volume, 50, 100, 0, -10);
  // }
  // audio.setTone(drop, drop, 0);

  audio.setVolume(volume);
}
uint8_t player_get_volume() { return audio.getVolume(); }