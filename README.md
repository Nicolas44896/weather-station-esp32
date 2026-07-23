# Weather Station - ESP32 + FreeRTOS

Estación meteorológica basada en ESP32, desarrollada con ESP-IDF (no Arduino)
para trabajar directamente con FreeRTOS multi-tarea.

## Objetivo
Proyecto personal para profundizar en sistemas embebidos: arquitectura
multi-tarea con FreeRTOS, comunicación por I2C, y networking (WiFi/HTTP)
hacia un backend propio.

## Hardware
- ESP32 NodeMCU 38 pines (WROOM-32S, CP2102)
- Sensor BME280 (temperatura, humedad, presión) — I2C

## Estado del proyecto
🚧 En desarrollo — **Fase 2 completada** (comunicación entre tasks).

Roadmap:
- ✅ **Fase 1** — Fundamentos de FreeRTOS (tasks concurrentes, GPIO, scheduler)
- ✅ **Fase 2** — Comunicación entre tasks: Queues (`xQueueSend`/`xQueueReceive`) y sincronización con Mutex sobre recurso compartido
- ⏳ **Fase 3** — Sensor real BME280 por I2C
- ⏳ **Fase 4** — Conectividad WiFi + envío HTTP a backend propio (Go)
- ⏳ **Fase 5** — Robustez (watchdog, manejo de fallos, reconexión)

## Setup del entorno
- ESP-IDF (v5.5.5 en Windows, v6.0.2 en Linux)
- Target: esp32

## Compilar y flashear 
### Linux:
```bash
idf.py build                       # compila (no requiere la placa)
idf.py -p /dev/ttyUSB0 flash monitor   # flashea y abre el monitor serie
```
### Windows:
```bash
idf.py build
idf.py -p COM5 flash monitor
```