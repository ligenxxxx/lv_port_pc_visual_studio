#include <stdio.h>

#include "lvgl/lvgl.h"

#include "vrx.h"
#include "vrx_ui.h"

#include "screen_mainview.h"

static lv_obj_t* label_band;
static lv_obj_t* label_channel;
static lv_obj_t* label_freq[4];
static lv_obj_t* bar_rssi[2];

static void mainview_update_label() {
    char str[8];

    vrx_get_band_str(str);
    lv_label_set_text(label_band, str);

    vrx_get_channel_str(str);
    lv_label_set_text(label_channel, str);

    char str2[8];
    vrx_get_frequency_str(str);
    for (int i = 0; i < 4; i++) {
        sprintf(str2, "%c", str[i]);
        lv_label_set_text(label_freq[i], str2);
    }
}

static void bar_set_value_anim(lv_obj_t* bar, int32_t target_val, uint32_t duration) {
    // 获取当前值作为动画起始值
    int32_t start_val = lv_bar_get_value(bar);
    
    // 创建动画描述符
    lv_anim_t anim;
    lv_anim_init(&anim);
    
    // 设置动画目标和执行函数
    lv_anim_set_var(&anim, bar);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_bar_set_value);
    
    // 设置动画起始值、目标值和时长
    lv_anim_set_values(&anim, start_val, target_val);
    lv_anim_set_duration(&anim, duration); // 动画时长（毫秒）
    
    // 设置动画曲线（可选，让动画更自然）
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
    
    // 启动动画
    lv_anim_start(&anim);
}

void mainview_update_rssi_bar() {
    int rssi;
    for(int i= 0; i < 2; i ++){
        rssi = lv_bar_get_value(bar_rssi[i]);
        rssi += 20;
        if(rssi > 100)
            rssi = 0;
        bar_set_value_anim(bar_rssi[i], rssi, 200);
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
            mainview_update_rssi_bar();
            break;
        case 'S':
        case 's':
            vrx_set_channel_idx(0, 1);
            mainview_update_label();
            mainview_update_rssi_bar();
            break;
        case 'A':
        case 'a':
            vrx_set_channel_idx(1, 0);
            mainview_update_label();
            mainview_update_rssi_bar();
            break;
        case 'D':
        case 'd':
            vrx_set_channel_idx(1, 1);
            mainview_update_label();
            mainview_update_rssi_bar();
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

void create_band_freq() {
    lv_obj_t* scr = screen[SCR_MAINVIEW];
    char str[8];

    label_band = lv_label_create(scr);
    lv_obj_set_style_text_color(label_band, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_band, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_align(label_band, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_pad_top(label_band, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_left(label_band, 10, LV_PART_MAIN);
    
    vrx_get_band_str(str);
    lv_label_set_text(label_band, str);
}

void create_channel_label() {
    lv_obj_t* scr = screen[SCR_MAINVIEW];
    char str[8];

    label_channel = lv_label_create(scr);
    lv_obj_set_style_text_color(label_channel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_channel, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_align(label_channel, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_pad_top(label_channel, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_left(label_channel, 10 + FONT_WIDTH + 6, LV_PART_MAIN);

    vrx_get_channel_str(str);
    lv_label_set_text(label_channel, str);
}

void create_freq_label() {
    lv_obj_t* scr = screen[SCR_MAINVIEW];
    char str1[8];
    char str2[8];

    for (int i = 0; i < 4; i++) {
        label_freq[i] = lv_label_create(scr);
        lv_obj_set_style_text_color(label_freq[i], lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_text_font(label_freq[i], &lv_font_montserrat_48, LV_PART_MAIN);
        lv_obj_set_align(label_freq[i], LV_ALIGN_TOP_LEFT);
        lv_obj_set_style_pad_top(label_freq[i], 10, LV_PART_MAIN);
        lv_obj_set_style_pad_left(label_freq[i], 120 + i * FONT_WIDTH, LV_PART_MAIN);
    }

    vrx_get_frequency_str(str1);
    for (int i = 0; i < 4; i++) {
        sprintf(str2, "%c", str1[i]);
        lv_label_set_text(label_freq[i], str2);
    }
}

void create_rssi_bar() {
    lv_obj_t* scr = screen[SCR_MAINVIEW];
    const int indic_width = 240;
    const int indic_height = 20;

    // 1. 定义样式
    static lv_style_t style_bg;    // 背景（外框）样式
    static lv_style_t style_indic; // 指示器（填充）样式

    // 初始化背景样式（外框）
    lv_style_init(&style_bg);

    // 1. 设置未填充部分颜色（背景色）
    lv_style_set_bg_color(&style_bg, lv_color_hex(333333)); // 深灰色未填充区域
    lv_style_set_bg_opa(&style_bg, LV_OPA_100);              // 完全不透明
    // 2. 边框设置
    lv_style_set_border_color(&style_bg, lv_color_hex(0xFFFFFF)); // 白色边框
    lv_style_set_border_width(&style_bg, 2);
    // 3. 内边距（边框外扩）和圆角
    lv_style_set_pad_all(&style_bg, 6);
    lv_style_set_radius(&style_bg, 3);

    // 初始化填充指示器样式
    lv_style_init(&style_indic);
    lv_style_set_bg_color(&style_indic, lv_color_hex(0x00FF00)); // 绿色填充
    lv_style_set_bg_opa(&style_indic, LV_OPA_100);
    lv_style_set_radius(&style_indic, 1);

    // 2. 创建两个进度条
    for (int i = 0; i < 2; i++) {
        bar_rssi[i] = lv_bar_create(scr);
        lv_obj_remove_style_all(bar_rssi[i]); // 清除默认样式

        // 应用样式
        lv_obj_add_style(bar_rssi[i], &style_bg, 0);              // 应用背景样式
        lv_obj_add_style(bar_rssi[i], &style_indic, LV_PART_INDICATOR); // 应用指示器样式

        // 设置尺寸和位置
        lv_obj_set_size(bar_rssi[i], indic_width, indic_height);  // 宽度，高度
        lv_obj_set_align(bar_rssi[i], LV_ALIGN_BOTTOM_MID);       // 底部居中对齐

        // 垂直偏移（i=0在上，i=1在下）
        int y_offset = -(10 + i * (10 + 15)); // 底部边距10px，间距15px
        lv_obj_set_y(bar_rssi[i], y_offset);

        // 设置初始值（带动画）
        lv_bar_set_value(bar_rssi[i], 50 + i * 20, LV_ANIM_ON);
    }
}

void create_screen_mainview() {
    screen[SCR_MAINVIEW] = lv_obj_create(NULL);
    lv_obj_t* scr = screen[SCR_MAINVIEW];

    // 设置屏幕背景为黑色（可选，确保文本可见）
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_100, LV_PART_MAIN);

    // key event
    lv_group_add_obj(lv_group_get_default(), scr);
    lv_obj_add_event_cb(scr, mainview_key_event_handler, LV_EVENT_KEY, NULL);

    create_band_freq();
    create_channel_label();
    create_freq_label();
    create_rssi_bar();

}
