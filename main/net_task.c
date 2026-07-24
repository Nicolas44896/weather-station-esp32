#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sensor_reading.h"
#include "sensor_task.h"
#include "net_task.h"           

static const char *TAG = "net";

static void net_receiving_task(void *pvParameter){
    QueueHandle_t queue = (QueueHandle_t) pvParameter;
    sensor_reading_t lecturas;

    while(1){
        xQueueReceive(queue, &lecturas, portMAX_DELAY);
        ESP_LOGI(TAG, "Recibido -> %.2f C, %.2f %%, %.2f hPa",
             lecturas.temperatura, lecturas.humedad, lecturas.presion);
    }
}

void net_task_start(QueueHandle_t out_queue){
    xTaskCreate(net_receiving_task, "net_receiving_task", 2048,(void *) out_queue, tskIDLE_PRIORITY, NULL);
}
