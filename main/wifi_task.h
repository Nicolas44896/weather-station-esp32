#ifndef WIFI_TASK_H
#define WIFI_TASK_H

#include "esp_err.h"

#define WIFI_STA_CONNECTED_BIT BIT0
#define WIFI_STA_IPV4_OBTAINED_BIT BIT1
#define WIFI_STA_IPV6_OBTAINED_BIT BIT2

esp_err_t wifi_sta_init(EventGroupHandle_t event_group);

esp_err_t wifi_sta_stop(void);

esp_err_t wifi_sta_reconnect(void);

#endif // WIFI_TASK_H