#include "lvgl/lvgl.h"

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
    lv_anim_set_duration(&a, 500);     // 显示动画持续500ms
    lv_anim_set_var(&a, bootlogo_label);
    lv_anim_set_repeat_count(&a, 0);

    lv_anim_start(&a);
    lv_timer_del(timer);
}

void create_bootlogo() {
    lv_obj_t* scr = lv_screen_active();

    // 设置屏幕背景为黑色（可选，确保文本可见）
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_100, LV_PART_MAIN);

    // 创建标签并初始隐藏（透明度0）
    bootlogo_label = lv_label_create(scr);
    lv_label_set_text(bootlogo_label, "NEWBEEFLY");
    lv_obj_set_align(bootlogo_label, LV_ALIGN_CENTER);
    lv_obj_set_style_text_color(bootlogo_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(bootlogo_label, &lv_font_montserrat_24, LV_PART_MAIN);

    // 初始状态设置为完全透明（隐藏）
    lv_obj_set_style_opa(bootlogo_label, LV_OPA_0, LV_PART_MAIN);

    // 立即启动显示动画定时器（延迟0ms，即马上执行）
    lv_timer_create(show_timer_cb, 0, NULL);
}

void vrx_ui() {
    create_bootlogo();
}
