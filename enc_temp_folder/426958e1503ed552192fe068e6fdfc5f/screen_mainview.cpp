#include <stdio.h>

#include "lvgl/lvgl.h"

#include "vrx.h"
#include "vrx_ui.h"

#include "screen_mainview.h"

static lv_obj_t* label_band;
static lv_obj_t* label_channel;
static lv_obj_t* label_freq[4];
static lv_obj_t* bar_rssi[2];//0 下面的，1上面的

// 新增：动画相关函数声明
static void label_anim_in(lv_obj_t* label, int y);
static void bar_anim_in(lv_obj_t* bar, int y);
static void start_mainview_animations(lv_timer_t* timer); // 定时器回调启动动画
static void stop_mainview_animations(void (*on_complete)(void));


// 新增：动画结束回调（用于执行后续操作，如切换界面）
static void (*anim_complete_cb)(void) = NULL; // 存储用户传入的完成回调
static void switch_to_main_menu_after_anim();

// 新增：存储标签和进度条的目标位置（用于动画结束后定位）
static int label_band_target_y;
static int label_channel_target_y;
static int label_freq_target_y[4];
static int bar_rssi_target_y[2];

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
            //lv_screen_load(screen[SCR_MAIN_MENU]);
            stop_mainview_animations(switch_to_main_menu_after_anim);
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
void create_band_label() {
    lv_obj_t* scr = screen[SCR_MAINVIEW];
    char str[8];

    label_band = lv_label_create(scr);
    lv_obj_set_style_text_color(label_band, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(label_band, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_align(label_band, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_pad_top(label_band, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_left(label_band, 10, LV_PART_MAIN);
    
    // 存储目标Y位置（原本的正常位置）
    label_band_target_y = lv_obj_get_y(label_band); 
    lv_obj_set_y(label_band, -100); // 固定在屏幕外，与stop动画保持统一

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

    // 存储目标Y位置
    label_channel_target_y = lv_obj_get_y(label_channel);
    // 初始位置：屏幕顶部外部
    lv_obj_set_y(label_channel, -100);

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

        // 存储目标Y位置
        label_freq_target_y[i] = lv_obj_get_y(label_freq[i]);
        // 初始位置：屏幕顶部外部
        lv_obj_set_y(label_freq[i], -100);
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

    static lv_style_t style_bg;
    static lv_style_t style_indic;

    lv_style_init(&style_bg);
    lv_style_set_bg_color(&style_bg, lv_color_hex(0x333333)); // 修正：添加0x前缀
    lv_style_set_bg_opa(&style_bg, LV_OPA_100);
    lv_style_set_border_color(&style_bg, lv_color_hex(0xFFFFFF));
    lv_style_set_border_width(&style_bg, 2);
    lv_style_set_pad_all(&style_bg, 6);
    lv_style_set_radius(&style_bg, 3);

    lv_style_init(&style_indic);
    lv_style_set_bg_color(&style_indic, lv_color_hex(0x00FF00));
    lv_style_set_bg_opa(&style_indic, LV_OPA_100);
    lv_style_set_radius(&style_indic, 1);

    for (int i = 0; i < 2; i++) {
        bar_rssi[i] = lv_bar_create(scr);
        lv_obj_remove_style_all(bar_rssi[i]);
        lv_obj_add_style(bar_rssi[i], &style_bg, LV_PART_MAIN); // 明确指定PART_MAIN
        lv_obj_add_style(bar_rssi[i], &style_indic, LV_PART_INDICATOR);

        lv_obj_set_size(bar_rssi[i], indic_width, indic_height);
        lv_obj_set_align(bar_rssi[i], LV_ALIGN_BOTTOM_MID);

        // 强制初始位置在屏幕顶部外（使用固定高度计算，避免动态获取失败）
        lv_obj_set_y(bar_rssi[i], -40);

        // 明确目标位置为屏幕底部内（负值表示从底部向上偏移）
        int y_offset = -(10 + i * 26);
        bar_rssi_target_y[i] = y_offset;

        // 初始值无动画，后续通过动画函数设置
        lv_bar_set_value(bar_rssi[i], 0, LV_ANIM_OFF);
    }
}

// 标签动画：修改Y坐标从初始位置滑到目标位置
static void label_anim_in(lv_obj_t* label, int32_t y) {
    lv_obj_set_y(label, y);
}

// 进度条动画：修改Y坐标从初始位置滑到目标位置
static void bar_anim_in(lv_obj_t* bar, int32_t y) {
    lv_obj_set_y(bar, y);
}

// 定时器回调：启动所有元素的动画
static void start_mainview_animations(lv_timer_t* timer) {
    lv_obj_t* scr = screen[SCR_MAINVIEW];
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_out); // 动画曲线：先快后慢
    lv_anim_set_duration(&anim, 300); // 动画总时长300ms

    // 1. 标签动画（从上方滑入）
    // band标签动画（延迟0ms启动）
    lv_anim_set_var(&anim, label_band);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)label_anim_in);
    lv_anim_set_values(&anim, lv_obj_get_y(label_band), label_band_target_y);
    lv_anim_set_delay(&anim, 0);
    lv_anim_start(&anim);

    // channel标签动画（延迟100ms启动，形成层次感）
    lv_anim_set_var(&anim, label_channel);
    lv_anim_set_values(&anim, lv_obj_get_y(label_channel), label_channel_target_y);
    lv_anim_set_delay(&anim, 0);
    lv_anim_start(&anim);

    // freq标签动画（每个延迟50ms启动）
    for (int i = 0; i < 4; i++) {
        lv_anim_set_var(&anim, label_freq[i]);
        lv_anim_set_values(&anim, lv_obj_get_y(label_freq[i]), label_freq_target_y[i]);
        lv_anim_set_delay(&anim, 150 + i * 50); // 依次延迟启动
        lv_anim_start(&anim);
    }

    // 2. 进度条动画

    // 下面的
    lv_anim_set_var(&anim, bar_rssi[0]);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)bar_anim_in);
    lv_anim_set_values(&anim, -200, bar_rssi_target_y[0]);
    lv_anim_set_delay(&anim, 100);
    lv_anim_start(&anim);

    //上面的
    lv_anim_set_var(&anim, bar_rssi[1]);
    lv_anim_set_values(&anim, -200, bar_rssi_target_y[1]);
    lv_anim_set_delay(&anim, 300);
    lv_anim_start(&anim);

    // 启动进度条数值动画（从0到目标值）
    bar_set_value_anim(bar_rssi[0], 50, 800);  // 第一个进度条到50
    bar_set_value_anim(bar_rssi[1], 70, 800);  // 第二个进度条到70

    lv_timer_del(timer); // 动画启动后删除定时器
}

void switch_in_mainview() {
    lv_screen_load(screen[SCR_MAINVIEW]);
    // 延迟50ms启动动画（确保界面已渲染）
    lv_timer_create(start_mainview_animations, 20, NULL);
}

// 标签滑出动画：从当前位置到屏幕顶部外
static void label_anim_out(lv_obj_t* label, int32_t y) {
    lv_obj_set_y(label, y);

    // 获取标签存储的回调函数（通过 lv_obj_set_user_data 提前设置）
    void (*on_complete)(void) = (void (*)())lv_obj_get_user_data(label);

    // 判断是否达到目标位置且是最后一个滑出的标签（label_band）
    if (on_complete && label == label_band && y == -lv_obj_get_height(label) * 2) {
        on_complete(); // 触发回调
    }
}

// 进度条滑出动画：从当前位置到屏幕底部外
static void bar_anim_out(lv_obj_t* bar, int32_t y) {
    lv_obj_set_y(bar, y);
}

static void stop_mainview_animations(void (*on_complete)(void)) {
    lv_obj_t* scr = screen[SCR_MAINVIEW];
    if (scr == NULL) return;

    // 给最后滑出的标签（label_band）设置用户数据（存储回调函数）
    lv_obj_set_user_data(label_band, on_complete);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in);
    lv_anim_set_duration(&anim, 300);

    // 1. 标签滑出
    int32_t hide_y = -lv_obj_get_height(label_band) * 2;

    // freq标签滑出
    for (int i = 3; i >= 0; i--) {
        lv_anim_set_var(&anim, label_freq[i]);
        lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)label_anim_out);
        lv_anim_set_values(&anim, lv_obj_get_y(label_freq[i]), hide_y);
        lv_anim_set_delay(&anim, 150 - i * 50);
        lv_anim_start(&anim);
    }

    // channel标签滑出
    lv_anim_set_var(&anim, label_channel);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)label_anim_out);
    lv_anim_set_values(&anim, lv_obj_get_y(label_channel), hide_y);
    lv_anim_set_delay(&anim, 200);
    lv_anim_start(&anim);

    // band标签滑出（最后滑出，触发回调）
    lv_anim_set_var(&anim, label_band);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)label_anim_out);
    lv_anim_set_values(&anim, lv_obj_get_y(label_band), hide_y);
    lv_anim_set_delay(&anim, 300);
    lv_anim_start(&anim);

    // 进度条滑出
    // 上面的
    lv_anim_set_var(&anim, bar_rssi[1]);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)bar_anim_out);
    lv_anim_set_values(&anim, bar_rssi_target_y[1], -200);
    lv_anim_set_delay(&anim, 100);
    lv_anim_start(&anim);

    // 下面的
    lv_anim_set_var(&anim, bar_rssi[0]);
    lv_anim_set_values(&anim, bar_rssi_target_y[0], -200);
    lv_anim_set_delay(&anim, 300);
    lv_anim_start(&anim);
}

static void switch_to_main_menu_after_anim() {
    lv_screen_load(screen[SCR_MAIN_MENU]);
    lv_group_focus_obj(screen[SCR_MAIN_MENU]);
    // 重置主界面元素位置（可选，下次进入时重新动画）
    // reset_mainview_element_positions(); 
}

void create_screen_mainview() {
    screen[SCR_MAINVIEW] = lv_obj_create(NULL);
    lv_obj_t* scr = screen[SCR_MAINVIEW];

    // 设置屏幕背景为黑色（可选，确保文本可见）
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_100, LV_PART_MAIN);
    
    // 关键：禁用滚动条（水平和垂直方向均不显示）
    lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);

    // key event
    lv_group_add_obj(lv_group_get_default(), scr);
    lv_obj_add_event_cb(scr, mainview_key_event_handler, LV_EVENT_KEY, NULL);

    create_band_label();
    create_channel_label();
    create_freq_label();
    create_rssi_bar();
}
