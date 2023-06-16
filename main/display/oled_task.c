/*
    ESP-IDF SSD1306 Display driver: https://github.com/nopnop2002/esp-idf-ssd1306
*/
#include "oled_task.h"
#include <stdlib.h>

#include "font8x8_basic.h"

SSD1306_t dev;


/* Measured values */
void oled_init()
{

    static const char *tag = "OLED_INIT";

    ESP_LOGI(tag, "INTERFACE is i2c");
    ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d", CONFIG_SDA_GPIO);
    ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d", CONFIG_SCL_GPIO);
    ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d", CONFIG_RESET_GPIO);

    (void)i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);

    ESP_LOGI(tag, "Panel is 128x64");
    ssd1306_init(&dev, 128, 64);

}

void oled_task(void *pv)
{
    vTaskSuspend(NULL);
    static const char *tag = "OLED_TASK";
    static _current_values_t rcvValues;
    char xsprintf[32];

    (void)oled_init();

    ssd1306_clear_screen(&dev, false);


    for (;;) {      /* @non-terminating@ */
        ssd1306_clear_screen(&dev, false);
        ssd1306_contrast(&dev, 0xFF);       // full contrast,...I guess
        ssd1306_display_text(&dev, 0, "Energy Monitor", 15, true);


        if (xQueueReceive(oled_queue, &(rcvValues), 0) == pdTRUE) {

            /*  PAGE I */
            memset(xsprintf, 0, sizeof(xsprintf));
            snprintf(xsprintf, sizeof(xsprintf), "Vrms: %.1f V", rcvValues.voltage );
            ssd1306_display_text(&dev, 2, xsprintf, sizeof(xsprintf), false);

            memset(xsprintf, 0, sizeof(xsprintf));
            snprintf(xsprintf, sizeof(xsprintf), "Irms: %.3f A", rcvValues.current );
            ssd1306_display_text(&dev, 3, xsprintf, sizeof(xsprintf), false);

            memset(xsprintf, 0, sizeof(xsprintf));
            snprintf(xsprintf, sizeof(xsprintf), "Pact: %.1f W", rcvValues.power );
            ssd1306_display_text(&dev, 4, xsprintf, sizeof(xsprintf), false);

            memset(xsprintf, 0, sizeof(xsprintf));
            snprintf(xsprintf, sizeof(xsprintf), "Freq: %.1f Hz", rcvValues.frequency );
            ssd1306_display_text(&dev, 5, xsprintf, sizeof(xsprintf), false);

            vTaskDelay(3000 / portTICK_PERIOD_MS);

            /*  PAGE II */
            ssd1306_clear_screen(&dev, false);
            ssd1306_contrast(&dev, 0xFF);       // full contrast,...I guess
            ssd1306_display_text(&dev, 0, "Energy Monitor", 15, true);

            memset(xsprintf, 0, sizeof(xsprintf));
            snprintf(xsprintf, sizeof(xsprintf), "S:\t\t%.1f VA", rcvValues.apparent_power);
            ssd1306_display_text(&dev, 2, xsprintf, sizeof(xsprintf), false);

            memset(xsprintf, 0, sizeof(xsprintf));
            snprintf(xsprintf, sizeof(xsprintf), "Q:\t\t%.1f VAr", rcvValues.reactive_power );
            ssd1306_display_text(&dev, 3, xsprintf, sizeof(xsprintf), false);

            memset(xsprintf, 0, sizeof(xsprintf));
            snprintf(xsprintf, sizeof(xsprintf), "cos(fi): %.2f", rcvValues.pf );
            ssd1306_display_text(&dev, 4, xsprintf, sizeof(xsprintf), false);

            memset(xsprintf, 0, sizeof(xsprintf));
            snprintf(xsprintf, sizeof(xsprintf), "fi:\t\t%.1f ", rcvValues.fi );
            ssd1306_display_text(&dev, 5, xsprintf, sizeof(xsprintf), false);

            vTaskDelay(3000 / portTICK_PERIOD_MS);
            //     ssd1306_fadeout(&dev);

            ESP_LOGI( tag, "Stack High Water Mark: %ld Bytes free, running Core: %d", ( unsigned long int ) uxTaskGetStackHighWaterMark( NULL ), xPortGetCoreID() );
        } else {
            ESP_LOGE(tag, "Failed to receive from Queue");

            memset(xsprintf, 0, sizeof(xsprintf));
            snprintf(xsprintf, sizeof(xsprintf), "Not data received from sensor");
            ssd1306_display_text(&dev, 3, xsprintf, sizeof(xsprintf), false);
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }       // endles for-loop

    vTaskDelete(NULL);
}