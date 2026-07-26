#ifndef WIFI_TASK_H
#define WIFI_TASK_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"   

// Bits del event group 
#define WIFI_STA_CONNECTED_BIT      BIT0   // asociado al AP (capa 2), todavia sin IP
#define WIFI_STA_IPV4_OBTAINED_BIT  BIT1   // ya tengo IP -> recien aca puedo hacer HTTP
#define WIFI_STA_IPV6_OBTAINED_BIT  BIT2   // (opcional, no lo necesitas para el POST)

// Inicializa el WiFi en modo STA (cliente) y arranca la conexion.
esp_err_t wifi_sta_init(EventGroupHandle_t event_group);

#endif 
