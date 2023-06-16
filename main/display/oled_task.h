#pragma once
#include "../main.h"
#include "ssd1306.h"


#ifdef __cplusplus
extern "C" {
#endif


void oled_init();
void oled_task(void *pv);

extern SSD1306_t dev;


#ifdef __cplusplus
}
#endif