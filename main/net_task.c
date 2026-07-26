#include <string.h>              
#include <stdio.h>               
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_http_client.h"     
#include "sensor_reading.h"
#include "net_task.h"

static const char *TAG = "net";

// URL del backend FastAPI.
#define BACKEND_URL "http://192.168.1.xx:8000/readings" // Cambiar la IP a la de tu PC en la LAN

//  Serializa una lectura a JSON en el buffer dado.
static int reading_to_json(const sensor_reading_t *r, char *buf, size_t buf_len)
{
    int ret = snprintf(buf, buf_len, "{\"temperatura\":%.2f,\"humedad\":%.2f,\"presion\":%.2f}", r->temperatura, r->humedad, r->presion); // 
    return ret; 
}

//  Hace un POST del JSON al backend. Devuelve ESP_OK si el request se completo
static esp_err_t post_reading(const char *json)
{
    esp_http_client_config_t config = {0}; // Inicializar la configuración del cliente con la URL y el método POST.
    config.url = BACKEND_URL;
    config.method = HTTP_METHOD_POST;

    esp_http_client_handle_t client = esp_http_client_init(&config); // Crear el cliente HTTP con la configuración dada. Devuelve un handle que usamos para las siguientes llamadas.

    esp_http_client_set_header(client, "Content-Type", "application/json"); // Setea el header Content-Type a application/json para indicar que el cuerpo del POST es JSON.

    esp_http_client_set_post_field(client, json, strlen(json)); // Setea el cuerpo del POST con el JSON y su longitud.

    esp_err_t err = esp_http_client_perform(client); // Ejecutar el request.

    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client); // Obtener el código de estado HTTP devuelto por el servidor.
        ESP_LOGI(TAG, "POST ok, status=%d", status);
    } else {
        ESP_LOGE(TAG, "POST fallo: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client); // Liberar el cliente HTTP.
    
    return err; 
}

static void net_receiving_task(void *pvParameter)
{
    QueueHandle_t queue = (QueueHandle_t) pvParameter;
    sensor_reading_t lecturas;
    char json[128];   

    while (1) {
        xQueueReceive(queue, &lecturas, portMAX_DELAY);
        ESP_LOGI(TAG, "Recibido -> %.2f C, %.2f %%, %.2f hPa", lecturas.temperatura, lecturas.humedad, lecturas.presion);

        int ret = reading_to_json(&lecturas, json, sizeof(json));
        if (ret > 0) {
            ESP_LOGD(TAG, "JSON: %s", json);
            post_reading(json);
        }    
    }
}

void net_task_start(QueueHandle_t out_queue)
{
    xTaskCreate(net_receiving_task, "net_receiving_task", 4096, (void *) out_queue, tskIDLE_PRIORITY, NULL);
}
