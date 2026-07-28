#include "sensor_reading.h"
#include "sensor_task.h"
#include "freertos/FreeRTOS.h" 
#include "freertos/task.h" 
#include "freertos/queue.h"
#include "esp_log.h"               
#include "esp_random.h"

static const char *TAG = "sensor";

static void sensor_reading_task(void *pvParameter){
    QueueHandle_t queue = (QueueHandle_t) pvParameter; 
    const TickType_t periodo = 2000 / portTICK_PERIOD_MS; // 2 seg
    TickType_t last_wake = xTaskGetTickCount();

    while(1){
        sensor_reading_t lecturas;

        lecturas.temperatura = (15.0f + (esp_random() % 1500) / 100.0f);
        lecturas.presion = (980.0 + (esp_random() % 4000) / 100.0f);

        xQueueSend(queue, &lecturas, portMAX_DELAY);
        ESP_LOGI(TAG, "Lectura enviada: %.2f C, %.2f hPa", lecturas.temperatura, lecturas.presion);
        vTaskDelayUntil(&last_wake, periodo); 
    }
}

void sensor_task_start(QueueHandle_t out_queue){
    xTaskCreate(sensor_reading_task, "sensor_reading_task", 2048,(void *) out_queue, tskIDLE_PRIORITY, NULL);
}