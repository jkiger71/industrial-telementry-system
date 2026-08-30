#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "telemetry_sim.h"

// 📶 Network Configuration & Security Tokens
#include "secrets.h"

// Global thread-safe queue handle
static QueueHandle_t xTelemetryQueue = NULL;

/**
 * 📊 Task 1: Telemetry Data Producer
 * Runs deterministically at 1Hz to calculate engine physics and queue data
 */
void vTelemetryTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1-second interval
    TelemetryData_t standard_payload;

    printf("[SYS] Telemetry Engine task successfully spawned.\n");

    while (true) {
        // Execute math simulation state machine
        telemetry_sim_generate_next(&standard_payload);

        // Attempt to push packet onto queue (timeout immediately if full to protect memory)
        if (xQueueSend(xTelemetryQueue, &standard_payload, (TickType_t)0) != pdPASS) {
            printf("[WARN] Telemetry queue full. Dropping packet ID: %lu\n", standard_payload.sequence_id);
        } else {
            // Blink onboard LED quickly to signify successful sensor logging
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(40));
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        }

        // Wait deterministically for exactly 1 second from last execution check
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/**
 * 📡 Task 2: Network Packet Consumer
 * Blocks until data arrives, formats JSON, and ships via lwIP network layer
 */
void vNetworkTask(void *pvParameters) {
    TelemetryData_t outbound_payload;
    char json_buffer[256];

    printf("[SYS] Network operational task successfully spawned.\n");

    while (true) {
        // Block indefinitely until a data structure drops into the queue
        if (xQueueReceive(xTelemetryQueue, &outbound_payload, portMAX_DELAY) == pdPASS) {

            // Format data into standard enterprise JSON format
            snprintf(json_buffer, sizeof(json_buffer),
                     "{\"seq\":%lu,\"temp\":%.2f,\"visc\":%.2f,\"rpm\":%.2f,\"anomaly\":%d,\"phase\":\"%s\"}",
                     outbound_payload.sequence_id,
                     outbound_payload.oil_temperature_c,
                     outbound_payload.fluid_viscosity_cst,
                     outbound_payload.motor_rpm,
                     outbound_payload.anomaly_flag,
                     outbound_payload.phase_string);

            printf("[NET] Transmitting payload: %s\n", json_buffer);

            /*
             * TODO: Integrate standard lwIP / HTTP client calls here
             * This will perform a non-blocking socket send straight to DJANGO_API_URL
             * injecting "Authorization: Bearer " BACKEND_TOKEN into the request headers.
             */
        }
    }
}

int main() {
    // 1. Initialize Standard I/O (Maps printf statements over USB back to CLion console tool)
    stdio_init_all();
    vTaskDelay(pdMS_TO_TICKS(2000)); // Short pause to let terminal connect smoothly
    printf("[BOOT] Initializing Industrial IoT Gateway Gateway...\n");

    // 2. Initialize Telemetry Simulator Core
    telemetry_sim_init();

    // 3. Initialize Wireless CYW43 Architecture Hardware
    if (cyw43_arch_init()) {
        printf("[CRITICAL] Failed to execute CYW43 wireless initialization.\n");
        return -1;
    }
    cyw43_arch_enable_sta_mode();

    // 4. Connect to Local Access Point Network Router
    printf("[BOOT] Authenticating with network Access Point SSID: %s...\n", WIFI_SSID);
    if (cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("[CRITICAL] Wi-Fi connection timed out or rejected credentials.\n");
        return -1;
    }
    printf("[BOOT] Connected successfully! IP Address assigned: %s\n",
           ip4addr_ntoa(netif_ip4_addr(netif_default)));

    // 5. Create Thread-Safe Communication Queue (Capacity for 10 sequential payloads)
    xTelemetryQueue = xQueueCreate(10, sizeof(TelemetryData_t));
    if (xTelemetryQueue == NULL) {
        printf("[CRITICAL] Internal memory failure creating FreeRTOS queue.\n");
        return -1;
    }

    // 6. Spawn Multi-Threaded Real-Time Tasks
    xTaskCreate(vTelemetryTask, "TelemetryEngine", 256, NULL, 2, NULL);
    xTaskCreate(vNetworkTask,   "NetworkEngine",   512, NULL, 1, NULL);

    // 7. Relinquish Control to FreeRTOS System Kernal Scheduler
    printf("[BOOT] Launching multi-threaded scheduler...\n");
    vTaskStartScheduler();

    // Code execution path is systematically captured by kernel scheduler; this row should never trigger
    while (true) {}
}
