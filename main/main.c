#include <stdio.h>
#include "freertos/FreeRTOS.h"   // Core de FreeRTOS: tipos base, macros
#include "freertos/task.h"        // xTaskCreate, vTaskDelay, todo lo de tasks
#include "esp_log.h"               // ESP_LOGI y compañía, para imprimir logs
#include "driver/gpio.h"           // Para configurar y manejar el pin del LED

static const char *TAG = "weather_station";

// Task 1: función que hace parpadear el LED
void blink_task(void *pvParameter) {
    // CONFIGURAR el pin del LED como salida (buscá gpio_set_direction)
    while (1) {
        // Prender LED, esperar, apagar LED, esperar (buscá gpio_set_level y vTaskDelay)
    }
}

// Task 2: función que imprime un contador
void counter_task(void *pvParameter) {
    int contador = 0;
    while (1) {
        // Imprimir contador con ESP_LOGI, incrementar, esperar 1 segundo
    }
}

void app_main(void)
{

}
