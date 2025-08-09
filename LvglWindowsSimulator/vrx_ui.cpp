#include "lvgl/lvgl.h"

#include "vrx.h"
#include "vrx_ui.h"

#include "img_scan.h"
#include "img_return.h"
#include "img_null.h"

static lv_obj_t* screen[SCR_NUMBER];

static lv_group_t* main_group = NULL;

//bootlogo
static lv_obj_t* bootlogo_label = NULL;

//mainview
static lv_obj_t* mainview_channel_label = NULL;
static lv_obj_t* mainview_frequency_label = NULL;

//mainmenu
static int current_selected_idx = 0; // 默认选中第一个图标
static lv_obj_t* mainmenu_imgs[MAIN_MENU_NUM]; // 存储所有图标的指针
const lv_coord_t icon_original_size = 96; // 原始图标尺寸
const uint16_t scale_full = 256; // 100% 缩放（LVGL 中 scale 基准值通常为 256）
const uint16_t scale_half = 128; // 50% 缩放（256/2）
void update_mainmenu_selection(void);
void scroll_to_selected_icon(void);



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

        case 'J':
        case 'j':
            lv_screen_load(screen[SCR_MAIN_MENU]);
            lv_group_focus_obj(screen[SCR_MAIN_MENU]);

            current_selected_idx = MAIN_MENU_SCAN;
            update_mainmenu_selection();
            scroll_to_selected_icon();
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
    lv_obj_set_align(mainview_frequency_label, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_pad_top(mainview_frequency_label, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_left(mainview_frequency_label, 130, LV_PART_MAIN);

    vrx_get_frequency_str(str);
    lv_label_set_text(mainview_frequency_label, str);

    lv_group_add_obj(lv_group_get_default(), screen[SCR_MAINVIEW]);
    lv_obj_add_event_cb(screen[SCR_MAINVIEW], mainview_key_event_handler, LV_EVENT_KEY, NULL);
}

// 更新图标选中状态和缩放
static void update_mainmenu_selection(void) {
    for (int i = 0; i < MAIN_MENU_NUM; i++) {
        if (i == current_selected_idx) {
            // 选中状态：100% 原始大小
            lv_image_set_scale(mainmenu_imgs[i], scale_full);
        }
        else {
            // 未选中状态：50% 缩小
            lv_image_set_scale(mainmenu_imgs[i], scale_half);
        }
    }
}
// 平滑滚动到选中图标并居中
static void scroll_to_selected_icon(void) {
    lv_obj_t* scr = screen[SCR_MAIN_MENU];
    lv_obj_t* selected_img = mainmenu_imgs[current_selected_idx];

    // 计算图标实际宽度（原始宽度 × 缩放比例）
    lv_coord_t icon_original_width = 96; // 原始图标宽度
    lv_coord_t icon_scaled_width = (icon_original_width * scale_full) / 256;

    // 计算选中图标的中心坐标（相对于屏幕）
    lv_coord_t icon_center_x = lv_obj_get_x(selected_img) + (icon_scaled_width / 2);
    lv_coord_t screen_center_x = lv_obj_get_width(scr) / 2;

    // 计算目标滚动位置
    lv_coord_t target_scroll = icon_center_x - screen_center_x;

    lv_obj_scroll_to_x(scr, target_scroll, 1);
}

static void mainmenu_key_event_handler(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        switch (key) {
        case 'W':
        case 'w':
            break;
        case 'S':
        case 's':
            break;
        case 'A':
        case 'a':
            current_selected_idx--;
            if (current_selected_idx < 0)
                current_selected_idx = 0;
            update_mainmenu_selection();
            scroll_to_selected_icon();
            break;
        case 'D':
        case 'd':
            current_selected_idx++;
            if (current_selected_idx == MAIN_MENU_NULL)
                current_selected_idx = MAIN_MENU_NULL - 1;
            update_mainmenu_selection();
            scroll_to_selected_icon();
            break;

        case 'J':
        case 'j':
            switch (current_selected_idx) {
            case MAIN_MENU_RETURN:
                lv_screen_load(screen[SCR_MAINVIEW]); // 返回主视图
                lv_group_focus_obj(screen[SCR_MAINVIEW]);
                current_selected_idx = MAIN_MENU_SCAN;
                update_mainmenu_selection();
                scroll_to_selected_icon();
                break;
            }
            break;

        default:
            break;
        }
    }
}

void create_screen_mainmenu() {
    screen[SCR_MAIN_MENU] = lv_obj_create(NULL);
    // 设置屏幕背景为黑色（可选，确保文本可见）
    lv_obj_set_style_bg_color(screen[SCR_MAIN_MENU], lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen[SCR_MAIN_MENU], LV_OPA_100, LV_PART_MAIN);

    // 配置主屏幕为可横向滚动
    lv_obj_set_scroll_dir(screen[SCR_MAIN_MENU], LV_DIR_HOR);
    lv_obj_set_scrollbar_mode(screen[SCR_MAIN_MENU], LV_SCROLLBAR_MODE_OFF);  // 隐藏滚动条
    lv_obj_set_size(screen[SCR_MAIN_MENU], LV_PCT(100), LV_PCT(100));

    // 键盘事件
    lv_group_add_obj(lv_group_get_default(), screen[SCR_MAIN_MENU]);
    lv_obj_add_event_cb(screen[SCR_MAIN_MENU], mainmenu_key_event_handler, LV_EVENT_KEY, NULL);

    // 图标布局参数（适配缩放后的间距）
    const int spacing = 10; // 图标间距（考虑缩小后的尺寸）
    const int top_margin = 6; // 顶部边距（居中靠上）

    // 计算第一个图标的起始位置（基于原始尺寸）
    int start_x = (256 - icon_original_size) / 2;

    // 创建图标并初始化
    for (int i = 0; i < MAIN_MENU_NUM; i++) {
        mainmenu_imgs[i] = lv_image_create(screen[SCR_MAIN_MENU]);
        lv_image_set_src(mainmenu_imgs[i], &img_scan);
        // 计算位置（基于原始尺寸排列，确保缩放后布局协调）
        int x_pos = start_x + i * (icon_original_size + spacing);
        int y_pos = top_margin;
        lv_obj_set_pos(mainmenu_imgs[i], x_pos, y_pos);
    }
    lv_image_set_src(mainmenu_imgs[MAIN_MENU_RETURN], &img_return);
    lv_image_set_src(mainmenu_imgs[MAIN_MENU_SCAN], &img_scan);
    lv_image_set_src(mainmenu_imgs[MAIN_MENU_NULL], &img_null);

    // 初始化选中状态（默认选中第二个）
    current_selected_idx = MAIN_MENU_SCAN;
    update_mainmenu_selection();
    scroll_to_selected_icon(); // 确保初始选中项居中
}

extern lv_indev_t* keypad_indev;
void vrx_ui() {

    lv_group_t* g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_set_group(keypad_indev, g);

    create_screen_bootlogo();
    create_screen_mainview();
    create_screen_mainmenu();

    lv_screen_load(screen[SCR_BOOTLOGO]);  // 加载并显示bootlogo屏幕
    lv_group_focus_obj(screen[SCR_BOOTLOGO]);
}
