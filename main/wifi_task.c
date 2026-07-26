#include <string.h>           
#include "sdkconfig.h"        
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"        
#include "esp_wifi.h"         
#include "esp_netif.h"        
#include "esp_log.h"
#include "nvs_flash.h"        
#include "wifi_task.h"

static const char *TAG = "wifi_p";

static EventGroupHandle_t s_wifi_event_group = NULL;

//  HANDLERS DE EVENTOS

// Maneja los eventos de la base WIFI_EVENT (capa WiFi / asociacion al AP).
static void on_wifi_event(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    switch (event_id) {

        case WIFI_EVENT_STA_START:
            esp_wifi_connect(); // pedir la asociacion al AP.
            ESP_LOGI(TAG, "STA start -> conectando al AP...");
            break;

        case WIFI_EVENT_STA_CONNECTED:
            xEventGroupSetBits(s_wifi_event_group, WIFI_STA_CONNECTED_BIT); // Asociado en capa 2, todavia no tenemos IP (falta el DHCP).
            ESP_LOGI(TAG, "Asociado al AP (sin IP todavia)");
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            xEventGroupClearBits(s_wifi_event_group, WIFI_STA_CONNECTED_BIT); // Se cayo la conexion o nunca asocio, limpiar el bit de "conectado" y reintentar.
            ESP_LOGW(TAG, "Desconectado -> reintentando...");
            esp_wifi_connect();
            break;

        default:
            break;
    }
}

// Maneja los eventos de la base IP_EVENT (capa de red / DHCP).
static void on_ip_event(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    if (event_id == IP_EVENT_STA_GOT_IP) {
        // Momento clave: ya tenemos IP -> recien ahora se puede hacer HTTP.
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "IP obtenida: " IPSTR, IP2STR(&event->ip_info.ip));

        xEventGroupSetBits(s_wifi_event_group, WIFI_STA_IPV4_OBTAINED_BIT);
    }
}

//  INICIALIZACION
//  Los 7 pasos del flujo STA estandar de ESP-IDF

esp_err_t wifi_sta_init(EventGroupHandle_t event_group)
{
    s_wifi_event_group = event_group;

    // Paso 1: NVS 
    esp_err_t ret = nvs_flash_init(); // Inicializa la particion NVS (para calibracion PHY)
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Paso 2: base de red + event loop + netif STA por defecto 
    esp_netif_init(); // Inicializa la capa de red (netif, DHCP, TCP/IP, etc.)
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // Crea el event loop por defecto (para que los eventos de WiFi y DHCP lleguen a nuestros handlers)
    esp_netif_create_default_wifi_sta(); // Crea el netif por defecto para la interfaz WiFi en modo STA (cliente)

    // Paso 3: init del driver WiFi con config por defecto 
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); // Estructura con la configuracion default del driver WiFi.
    ESP_ERROR_CHECK(esp_wifi_init(&cfg)); // Inicializa el driver WiFi con la config default. 

    // Paso 4: registrar NUESTROS handlers 
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                        &on_wifi_event, NULL, NULL); // Registra el handler para eventos de WiFi
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, 
                                        &on_ip_event, NULL, NULL); // Registra el handler para eventos de IP (DHCP)

    // Paso 5: modo STA + credenciales 
    esp_wifi_set_mode(WIFI_MODE_STA); // Modo STA (cliente)
    wifi_config_t wifi_config = {0}; // Inicializa la estructura de config WiFi en 0
    strncpy((char *)wifi_config.sta.ssid,     CONFIG_WIFI_SSID,     sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, CONFIG_WIFI_PASSWORD, sizeof(wifi_config.sta.password));
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config); // Setea la config de la interfaz STA (SSID y password)

    // Paso 6: arrancar 
    esp_wifi_start(); // Arranca el driver WiFi disparando el evento WIFI_EVENT_STA_START que maneja on_wifi_event.
    ESP_LOGI(TAG, "wifi_sta_init completado, esperando conexion...");

    // Paso 7: NO bloqueamos 
    return ESP_OK;
}
