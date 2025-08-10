#include "lvgl/lvgl.h"

#include "vrx_ui.h"

#include "img/img_scan.h"
#include "img/img_return.h"
#include "img/img_null.h"

static int current_selected_idx = 0; // 默认选中第一个图标
static lv_obj_t* mainmenu_imgs[MAIN_MENU_NUM]; // 存储所有图标的指针
static const lv_coord_t icon_original_size = 96; // 原始图标尺寸
static const uint16_t scale_full = 256; // 100% 缩放（LVGL 中 scale 基准值通常为 256）
static const uint16_t scale_half = 128; // 50% 缩放（256/2）

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