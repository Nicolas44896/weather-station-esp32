#include <stdio.h>
#include "freertos/FreeRTOS.h"     // Core de FreeRTOS: tipos base, macros
#include "freertos/task.h"         // xTaskCreate, vTaskDelay, todo lo de tasks
#include "esp_log.h"               // ESP_LOGI y compañía, para imprimir logs
#include "driver/gpio.h"           // Para configurar y manejar el pin del LED

#define LED_PIN GPIO_NUM_2 // Pin del LED

static const char *TAG = "weather_station";

void config_gpio(void){ //Configurar pin como salida digital para el LED
    gpio_config_t cfgPinLed;

    cfgPinLed.pin_bit_mask = 1ULL << LED_PIN; // Pin del LED
    cfgPinLed.mode = GPIO_MODE_OUTPUT;
    cfgPinLed.pull_up_en = GPIO_PULLUP_DISABLE;
    cfgPinLed.pull_down_en = GPIO_PULLDOWN_DISABLE;
    cfgPinLed.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&cfgPinLed);
}

// Task 1: función que hace parpadear el LED 
void blink_task(void *pvParameter) {
    
    const TickType_t delay = 500/portTICK_PERIOD_MS; // 500 ms

    while (1) {
        // Prender LED, esperar, apagar LED, esperar (buscá gpio_set_level y vTaskDelay)
        gpio_set_level(LED_PIN, 1); // Prender LED
        vTaskDelay(delay); // Esperar
        gpio_set_level(LED_PIN, 0); // Apagar LED
        vTaskDelay(delay); // Esperar
    }
}

// Task 2: función que imprime un contador
void counter_task(void *pvParameter) {
    int contador = 0;

    const TickType_t delay = 1000/portTICK_PERIOD_MS; // 1 segundo
    while (1) {
        // Imprimir contador con ESP_LOGI, incrementar, esperar 1 segundo
        ESP_LOGI(TAG, "Contador: %d", contador); // Imprimir contador
        contador++; // Incrementar contador
        vTaskDelay(delay); // Esperar 1 segundo
    }
}

void app_main(void)
{
    config_gpio(); // Configurar pin como salida digital para el LED

    xTaskCreate(blink_task, "blink_task", 2048, NULL, tskIDLE_PRIORITY, NULL); // Crear task 1  
    xTaskCreate(counter_task, "counter_task", 2048, NULL, tskIDLE_PRIORITY, NULL); // Crear task 2
}
