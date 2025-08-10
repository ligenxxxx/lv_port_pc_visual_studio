#pragma once

void vrx_init();
int vrx_get_channel();
void vrx_set_channel_idx(int is_band, int is_next);
void vrx_get_band_str(char* str);
void vrx_get_channel_str(char* str);
void vrx_get_frequency_str(char* str);
