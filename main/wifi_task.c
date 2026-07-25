#include "freertos/FreeRTOS.h" 
#include "freertos/task.h" 
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_wifi_netif.h"
#include "esp_log.h" 
#include "wifi_task.h"

static const char *TAG = "wifi";

// static global variables
static esp_netif_t *s_wifi_netif = NULL;
static EventGroupHandle_t s_wifi_event_group = NULL;
static wifi_netif_driver_t s_wifi_driver = NULL;

static void on_wifi_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
    switch(event_id){
        case WIFI_EVENT_STA_START:
            if(s_wifi_netif != NULL){
                wifi_start(s_wifi_netif, event_base, event_id, event_data);
            }
            break;
        case WIFI_EVENT_STA_STOP:
            if(s_wifi_netif != NULL){
                esp_netif_action_stop(s_wifi_netif, event_base, event_id, event_data);
            }
            break;
        case WIFI_EVENT_STA_CONNECTED:
            if(s_wifi_netif == NULL){
                ESP_LOGI(TAG, "Wifi no inicializado");
                return;
            }

            wifi_event_sta_connected_t *event_sta_connected = (wifi_event_sta_connected_t *)event_data;
            ESP_LOGI(TAG, "Conectado al AP");
    }
}

static void on_ip_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

static void wifi_start(void *esp_netif, esp_event_base_t base, int32_t event_id, void *data);




