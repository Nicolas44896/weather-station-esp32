#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void net_task_start(QueueHandle_t out_queue);
