#pragma once


typedef enum {
    SCR_BOOTLOGO = 0,
    SCR_MAINVIEW,
    SCR_MAIN_MENU,
    SCR_NUMBER
}SCR_e;

typedef enum{
    MAIN_MENU_RETURN = 0,
    MAIN_MENU_SCAN,
    MAIN_MENU_ANTENNA,
    MAIN_MENU_SPECTRUM,
    MAIN_MENU_RSSI,
    MAIN_MENU_CALIB,
    MAIN_MENU_FIRMWARE,
    MAIN_MENU_NULL,
    MAIN_MENU_NUM
}MAIN_MENU_SEL_e;

void vrx_ui();

