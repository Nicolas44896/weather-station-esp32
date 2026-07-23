#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"     // Core de FreeRTOS: tipos base, macros
#include "freertos/task.h"         // xTaskCreate, vTaskDelay, todo lo de tasks
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"               // ESP_LOGI y compañía, para imprimir logs
#include "esp_random.h"
#include "driver/gpio.h"           // Para configurar y manejar el pin del LED


#define LED_PIN GPIO_NUM_2 // Pin del LED

static const char *TAG = "weather_station";

typedef struct {
    SemaphoreHandle_t   mutex;
    uint32_t            *valor;
    QueueHandle_t       queue;
} shared_ctx_t;

void config_gpio(void){ //Configurar pin como salida digital para el LED
    gpio_config_t cfgPinLed;

    cfgPinLed.pin_bit_mask = 1ULL << LED_PIN; // Pin del LED
    cfgPinLed.mode = GPIO_MODE_OUTPUT;
    cfgPinLed.pull_up_en = GPIO_PULLUP_DISABLE;
    cfgPinLed.pull_down_en = GPIO_PULLDOWN_DISABLE;
    cfgPinLed.intr_type = GPIO_INTR_DISABLE;

    gpio_config(&cfgPinLed);
}

void incremental_task_a(void *pvParameter){
    shared_ctx_t *ctx = (shared_ctx_t *) pvParameter;
    const TickType_t delay = 500/portTICK_PERIOD_MS;

    while(1){
        if(xSemaphoreTake(ctx->mutex, portMAX_DELAY) == pdTRUE){
            (*ctx->valor)++;
            ESP_LOGI(TAG, "Valor por task a = %"PRIu32"", *ctx->valor);
            xSemaphoreGive(ctx->mutex);
        }
        vTaskDelay(delay);
    }
}

void incremental_task_b(void *pvParameter){
    shared_ctx_t *ctx = (shared_ctx_t *) pvParameter;
    const TickType_t delay = 500/portTICK_PERIOD_MS;

    while(1){
        if(xSemaphoreTake(ctx->mutex, portMAX_DELAY) == pdTRUE){
            (*ctx->valor)++;
            ESP_LOGI(TAG, "Valor por task b = %"PRIu32"", *ctx->valor);
            xSemaphoreGive(ctx->mutex);
        }
        vTaskDelay(delay);
    }
}


void producer_task(void *pvParameter){
    shared_ctx_t *ctx = (shared_ctx_t *) pvParameter; 

    while(1){
        uint32_t random = esp_random();
        xQueueSend(ctx->queue, (void *) &random, portMAX_DELAY);
        ESP_LOGI(TAG, "Numero random generado: %" PRIu32"", random);
    }
}

void consumer_task(void *pvParameter){
    shared_ctx_t *ctx = (shared_ctx_t *) pvParameter;
    const TickType_t delay = 1000/portTICK_PERIOD_MS;
    uint32_t number;

    while(1){
        xQueueReceive(ctx->queue, (void *) &number, portMAX_DELAY);
        ESP_LOGI(TAG, "Numero recibido: %" PRIu32"", number);
        vTaskDelay(delay);
    }
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
    static shared_ctx_t ctx;
    static uint32_t valor_compartido = 0;
    ctx.mutex = xSemaphoreCreateMutex();
    ctx.valor = &valor_compartido; 
    ctx.queue = xQueueCreate(3, sizeof(uint32_t));

    configASSERT(ctx.mutex != NULL);

    xTaskCreate(incremental_task_a, "incremental_task_a", 2048, &ctx, tskIDLE_PRIORITY, NULL);   
    xTaskCreate(incremental_task_b, "incremental_task_b", 2048, &ctx, tskIDLE_PRIORITY, NULL); 
    //xTaskCreate(producer_task, "producer_task", 2048, &ctx, tskIDLE_PRIORITY, NULL); 
    //xTaskCreate(consumer_task, "consumer_task", 2048, &ctx, tskIDLE_PRIORITY, NULL);   
    xTaskCreate(blink_task, "blink_task", 2048, NULL, tskIDLE_PRIORITY, NULL);
    // xTaskCreate(counter_task, "counter_task", 2048, NULL, tskIDLE_PRIORITY, NULL);
}
