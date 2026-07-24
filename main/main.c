#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"     // Core de FreeRTOS: tipos base, macros
#include "freertos/task.h"         // xTaskCreate, vTaskDelay, todo lo de tasks
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"               // ESP_LOGI y compañía, para imprimir logs
#include "esp_random.h"
#include "driver/gpio.h"           // Para configurar y manejar el pin del LED
#include "net_task.h"
#include "sensor_reading.h"
#include "sensor_task.h"

void app_main(void)
{
    QueueHandle_t q = xQueueCreate(5, sizeof(sensor_reading_t));
    sensor_task_start(q);
    net_task_start(q);
}
