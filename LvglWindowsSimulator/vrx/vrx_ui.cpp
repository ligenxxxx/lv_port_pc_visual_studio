#include "lvgl/lvgl.h"

#include "vrx.h"
#include "vrx_ui.h"

#include "screen_bootlogo.h"
#include "screen_mainview.h"
#include "screen_mainmenu.h"


lv_obj_t* screen[SCR_NUMBER];

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
