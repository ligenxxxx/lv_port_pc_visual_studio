#include <stdio.h>

#include "vrx.h"

static int frequency_list[48] = {
    5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725,
    5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866,
    5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945,
    5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880,
    5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917,
    5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621
};

static int vrx_channel;

void vrx_init() {
    vrx_channel = 32; // R1
}

int vrx_get_channel() {
    return vrx_channel;
}

void vrx_set_channel_idx(int is_band, int is_next) {
    int band = vrx_channel >> 3;
    int channel = vrx_channel & 7;
    if (is_band) {
        if (is_next) {
            band += 1;
            if (band > 5)
                band = 0;
        }
        else {
            band -= 1;
            if (band < 0)
                band = 5;
        }
    }
    else {
        if (is_next) {
            channel += 1;
            if (channel > 7)
                channel = 0;
        }
        else {
            channel -= 1;
            if (channel < 0)
                channel = 7;
        }
    }
    vrx_channel = (band << 3) | channel;
}

void vrx_get_band_str(char *str) {
    const char band_str[6] = {'A', 'B', 'E', 'F', 'R', 'L'};
    int band = vrx_channel >> 3;

    sprintf(str, "%c", band_str[band]);
}
void vrx_get_next_band_str(char *str) {
    const char band_str[6] = {'A', 'B', 'E', 'F', 'R', 'L'};
    int band = vrx_channel >> 3;

    band += 1;
    if (band > 5)
        band = 0;

    sprintf(str, "%c", band_str[band]);
}
void vrx_get_last_band_str(char *str) {
    const char band_str[6] = {'A', 'B', 'E', 'F', 'R', 'L'};
    int band = vrx_channel >> 3;

    band -= 1;
    if (band < 0)
        band = 5;

    sprintf(str, "%c", band_str[band]);
}

void vrx_get_channel_str(char *str) {
    int channel = vrx_channel & 7;
    sprintf(str, "%d", channel + 1);
}
void vrx_get_next_channel_str(char *str) {
    int channel = vrx_channel & 7;

    channel += 1;
    if (channel > 7)
        channel = 0;

    sprintf(str, "%d", channel + 1);
}
void vrx_get_last_channel_str(char *str) {
    int channel = vrx_channel & 7;

    channel -= 1;
    if (channel < 0)
        channel = 7;

    sprintf(str, "%d", channel + 1);
}

void vrx_get_frequency_str(char *str) {

    sprintf(str, "%d", frequency_list[vrx_channel]);
}
