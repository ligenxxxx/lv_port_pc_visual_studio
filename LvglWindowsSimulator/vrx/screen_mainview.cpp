#include <stdio.h>

#include "lvgl/lvgl.h"

#include "vrx.h"
#include "vrx_ui.h"

#include "screen_mainview.h"

static lv_obj_t* mainview_band_label;
static lv_obj_t* mainview_channel_label;
static lv_obj_t* mainview_frequency_label[4];

static void mainview_update_label() {
    char str[8];

    vrx_get_band_str(str);
    lv_label_set_text(mainview_band_label, str);

    vrx_get_channel_str(str);
    lv_label_set_text(mainview_channel_label, str);

    char str2[8];
    vrx_get_frequency_str(str);
    for (int i = 0; i < 4; i++) {
        sprintf(str2, "%c", str[i]);
        lv_label_set_text(mainview_frequency_label[i], str2);
    }
}

static void mainview_key_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        switch (key) {
        case 'W':
        case 'w':
            vrx_set_channel_idx(0, 0);
            mainview_update_label();
            break;
        case 'S':
        case 's':
            vrx_set_channel_idx(0, 1);
            mainview_update_label();
            break;
        case 'A':
        case 'a':
            vrx_set_channel_idx(1, 0);
            mainview_update_label();
            break;
        case 'D':
        case 'd':
            vrx_set_channel_idx(1, 1);
            mainview_update_label();
            break;

        case 'J':
        case 'j':
            lv_screen_load(screen[SCR_MAIN_MENU]);
            lv_group_focus_obj(screen[SCR_MAIN_MENU]);
/*
            current_selected_idx = MAIN_MENU_SCAN;
            update_mainmenu_selection();
            scroll_to_selected_icon();
*/
            break;

        default:
            break;
        }
    }
}

void create_screen_mainview() {
    char str[8];

    screen[SCR_MAINVIEW] = lv_obj_create(NULL);

    // 设置屏幕背景为黑色（可选，确保文本可见）
    lv_obj_set_style_bg_color(screen[SCR_MAINVIEW], lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen[SCR_MAINVIEW], LV_OPA_100, LV_PART_MAIN);

    // band
    mainview_band_label = lv_label_create(screen[SCR_MAINVIEW]);
    lv_obj_set_style_text_color(mainview_band_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(mainview_band_label, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_align(mainview_band_label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_pad_top(mainview_band_label, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_left(mainview_band_label, 10, LV_PART_MAIN);
    
    vrx_get_band_str(str);
    lv_label_set_text(mainview_band_label, str);

    // channel
    mainview_channel_label = lv_label_create(screen[SCR_MAINVIEW]);
    lv_obj_set_style_text_color(mainview_channel_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(mainview_channel_label, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_align(mainview_channel_label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_pad_top(mainview_channel_label, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_left(mainview_channel_label, 10 + FONT_WIDTH + 6, LV_PART_MAIN);

    vrx_get_channel_str(str);
    lv_label_set_text(mainview_channel_label, str);

    // freq
    for (int i = 0; i < 4; i++) {
        mainview_frequency_label[i] = lv_label_create(screen[SCR_MAINVIEW]);
        lv_obj_set_style_text_color(mainview_frequency_label[i], lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_text_font(mainview_frequency_label[i], &lv_font_montserrat_48, LV_PART_MAIN);
        lv_obj_set_align(mainview_frequency_label[i], LV_ALIGN_TOP_LEFT);
        lv_obj_set_style_pad_top(mainview_frequency_label[i], 10, LV_PART_MAIN);
        lv_obj_set_style_pad_left(mainview_frequency_label[i], 120 + i * FONT_WIDTH, LV_PART_MAIN);
    }

    vrx_get_frequency_str(str);
    char str2[8];
    for (int i = 0; i < 4; i++) {
        sprintf(str2, "%c", str[i]);
        lv_label_set_text(mainview_frequency_label[i], str2);
    }

    // key event
    lv_group_add_obj(lv_group_get_default(), screen[SCR_MAINVIEW]);
    lv_obj_add_event_cb(screen[SCR_MAINVIEW], mainview_key_event_handler, LV_EVENT_KEY, NULL);
}
