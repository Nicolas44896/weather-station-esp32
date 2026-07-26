#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"     
#include "freertos/task.h"         
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"               
#include "esp_random.h"
#include "driver/gpio.h"           
#include "net_task.h"
#include "sensor_reading.h"
#include "sensor_task.h"
#include "wifi_task.h"

void app_main(void)
{
    EventGroupHandle_t wifi_eg = xEventGroupCreate();
    wifi_sta_init(wifi_eg);
    xEventGroupWaitBits(wifi_eg, WIFI_STA_IPV4_OBTAINED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    QueueHandle_t q = xQueueCreate(5, sizeof(sensor_reading_t));
    sensor_task_start(q);
    net_task_start(q);
}
