#include "lvgl/lvgl.h"

#include "vrx.h"

typedef enum {
    SCR_BOOTLOGO = 0,
    SCR_MAINVIEW,
    SRC_NUMBER
}SCR_e;

static lv_obj_t* screen[SRC_NUMBER];

static lv_group_t* main_group = NULL;

//bootlogo
static lv_obj_t* bootlogo_label = NULL;

//mainview
static lv_obj_t* mainview_channel_label = NULL;
static lv_obj_t* mainview_frequency_label = NULL;

// 显示动画回调函数：控制透明度从0到255
static void fade_in_anim_cb(void* var, int32_t value) {
    lv_obj_set_style_opa((lv_obj_t*)var, value, LV_PART_MAIN);
}

// 定时器回调：启动显示动画
static void show_timer_cb(lv_timer_t* timer) {
    lv_anim_t a;
    lv_anim_init(&a);

    lv_anim_set_exec_cb(&a, fade_in_anim_cb);
    lv_anim_set_values(&a, LV_OPA_0, LV_OPA_100);    // 从0到100渐显
    lv_anim_set_duration(&a, 500);     // 显示动画持续500ms
    lv_anim_set_var(&a, bootlogo_label);
    lv_anim_set_repeat_count(&a, 0);

    lv_anim_start(&a);
    lv_timer_del(timer);
}
// 切换到主视图的定时器回调
static void switch_to_mainview_cb(lv_timer_t* timer) {
    // 1 秒后加载主视图屏幕
    lv_screen_load(screen[SCR_MAINVIEW]);
    lv_timer_del(timer); // 执行后删除定时器
}

void create_screen_bootlogo() {
    screen[SCR_BOOTLOGO] = lv_obj_create(NULL);

    // 设置屏幕背景为黑色（可选，确保文本可见）
    lv_obj_set_style_bg_color(screen[SCR_BOOTLOGO], lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen[SCR_BOOTLOGO], LV_OPA_100, LV_PART_MAIN);

    // 创建标签并初始隐藏（透明度0）
    bootlogo_label = lv_label_create(screen[SCR_BOOTLOGO]);
    lv_label_set_text(bootlogo_label, "NEWBEEFLY");
    lv_obj_set_align(bootlogo_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(bootlogo_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(bootlogo_label, &lv_font_montserrat_24, LV_PART_MAIN);

    // 初始状态设置为完全透明（隐藏）
    lv_obj_set_style_opa(bootlogo_label, LV_OPA_0, LV_PART_MAIN);

    // 立即启动显示动画定时器（延迟0ms，即马上执行）
    lv_timer_create(show_timer_cb, 0, NULL);

    // 创建 1 秒延迟切换定时器
    lv_timer_create(switch_to_mainview_cb, 1000, NULL); // 1000ms = 1 秒
}

static void mainvide_update_label() {
    char str[64];

    vrx_get_channel_str(str);
    lv_label_set_text(mainview_channel_label, str);
    vrx_get_frequency_str(str);
    lv_label_set_text(mainview_frequency_label, str);
}

static void mainview_key_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        switch (key) {
        case 'W':
        case 'w':
            vrx_set_channel(0, 0);
            mainvide_update_label();
            break;
        case 'S':
        case 's':
            vrx_set_channel(0, 1);
            mainvide_update_label();
            break;
        case 'A':
        case 'a':
            vrx_set_channel(1, 0);
            mainvide_update_label();
            break;
        case 'D':
        case 'd':
            vrx_set_channel(1, 1);
            mainvide_update_label();
            break;
        default:
            break;
        }
    }
}

void create_screen_mainview() {
    char str[64];
    screen[SCR_MAINVIEW] = lv_obj_create(NULL);

    // 设置屏幕背景为黑色（可选，确保文本可见）
    lv_obj_set_style_bg_color(screen[SCR_MAINVIEW], lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen[SCR_MAINVIEW], LV_OPA_100, LV_PART_MAIN);

    mainview_channel_label = lv_label_create(screen[SCR_MAINVIEW]);
    lv_obj_set_style_text_color(mainview_channel_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(mainview_channel_label, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_align(mainview_channel_label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_pad_top(mainview_channel_label, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_left(mainview_channel_label, 10, LV_PART_MAIN);

    vrx_get_channel_str(str);
    lv_label_set_text(mainview_channel_label, str);

    mainview_frequency_label = lv_label_create(screen[SCR_MAINVIEW]);
    lv_obj_set_style_text_color(mainview_frequency_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(mainview_frequency_label, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_align(mainview_frequency_label, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_style_pad_top(mainview_frequency_label, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_right(mainview_frequency_label, 10, LV_PART_MAIN);

    vrx_get_frequency_str(str);
    lv_label_set_text(mainview_frequency_label, str);

    lv_group_add_obj(lv_group_get_default(), screen[SCR_MAINVIEW]);
    lv_obj_add_event_cb(screen[SCR_MAINVIEW], mainview_key_event_handler, LV_EVENT_KEY, NULL);
}


extern lv_indev_t* keypad_indev;
void vrx_ui() {

    lv_group_t* g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_set_group(keypad_indev, g);

    create_screen_bootlogo();
    create_screen_mainview();

    lv_screen_load(screen[SCR_BOOTLOGO]);  // 加载并显示bootlogo屏幕
}
