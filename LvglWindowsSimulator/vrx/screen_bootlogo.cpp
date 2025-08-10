#include "lvgl/lvgl.h"

#include "vrx_ui.h"

#include "screen_bootlogo.h"

//bootlogo
static lv_obj_t* bootlogo_label = NULL;

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
    lv_anim_set_duration(&a, 1500);     // 显示动画持续1500ms
    lv_anim_set_var(&a, bootlogo_label);
    lv_anim_set_repeat_count(&a, 0);

    lv_anim_start(&a);
    lv_timer_del(timer);
}

// 切换到主视图的定时器回调
static void switch_to_mainview_cb(lv_timer_t* timer) {
    // 1 秒后加载主视图屏幕
    lv_screen_load(screen[SCR_MAINVIEW]);
    lv_group_focus_obj(screen[SCR_MAINVIEW]);
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
    lv_obj_set_style_text_font(bootlogo_label, &lv_font_montserrat_28, LV_PART_MAIN);

    // 初始状态设置为完全透明（隐藏）
    lv_obj_set_style_opa(bootlogo_label, LV_OPA_0, LV_PART_MAIN);

    // 立即启动显示动画定时器（延迟0ms，即马上执行）
    lv_timer_create(show_timer_cb, 0, NULL);

    // 创建 1 秒延迟切换定时器
    lv_timer_create(switch_to_mainview_cb, 2000, NULL); // 2000ms = 2 秒
}
