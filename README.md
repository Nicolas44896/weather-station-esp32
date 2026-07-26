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
🚧 En desarrollo — **Fase 3 completada** (pipeline WiFi + HTTP end-to-end con datos simulados).

Roadmap:
- ✅ **Fase 1** — Fundamentos de FreeRTOS (tasks concurrentes, GPIO, scheduler)
- ✅ **Fase 2** — Comunicación entre tasks: Queues (`xQueueSend`/`xQueueReceive`) y sincronización con Mutex sobre recurso compartido
- ✅ **Fase 3** — Conectividad WiFi (modo STA, `esp_wifi` + event group) + envío HTTP POST (JSON) a backend propio en Python/FastAPI, con datos simulados tipados con la forma final del sensor
- ⏳ **Fase 4** — Sensor real BME280 por I2C (drop-in: sólo cambia la fuente del dato, no el pipeline de red)
- ⏳ **Fase 5** — Robustez (watchdog, manejo de fallos, reconexión)

## Arquitectura (firmware)
Modular, con responsabilidad única por archivo y comunicación desacoplada por queue:
- `sensor_task` — genera lecturas (hoy simuladas) y las encola
- `net_task` — consume de la queue, serializa a JSON y hace el HTTP POST
- `wifi_task` — conexión WiFi en modo STA, sincronizada por event group
- `sensor_reading.h` — contrato de datos compartido (espeja el modelo Pydantic del backend)
- `backend/` — servidor FastAPI que valida (Pydantic) y recibe las lecturas

## Setup del entorno
- ESP-IDF (v5.5.5 en Windows, v6.0.2 en Linux)
- Target: esp32

## Configuración
Las credenciales WiFi no se hardcodean: se definen en `main/Kconfig.projbuild` y se
cargan por `menuconfig` (quedan en `sdkconfig`, que está fuera de Git).
```bash
idf.py menuconfig   # → "Weather Station - WiFi" → WiFi SSID / WiFi Password
```
La URL del backend se configura en `main/net_task.c` (`BACKEND_URL`): debe apuntar a la
IP del server en la LAN (no `localhost`). El backend se levanta desde `backend/`:
```bash
uvicorn main:app --host 0.0.0.0 --port 8000 --reload
```

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