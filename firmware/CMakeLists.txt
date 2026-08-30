cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(industrial_telemetry C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# 1. Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# 2. DEFINE THE FREERTOS CONFIG TARGET (Fixes your current error)
# This points FreeRTOS to the root directory where your FreeRTOSConfig.h lives
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}
)

# SET THE PROCESSOR TARGET PORT (Fixes your current warning/error)
set(FREERTOS_PORT GCC_ARM_CM0 CACHE STRING "")

# Pull in the FreeRTOS kernel folder
add_subdirectory(FreeRTOS-Kernel)

# 4. Define your main executable target
add_executable(pico_iot_app
		src/main.c
		src/telemetry_sim.c
		src/telemetry_sim.h
		src/main.h
		src/secrets.h
)

# 5. Bind the hardware libraries, Wi-Fi driver, and RTOS core together
target_link_libraries(pico_iot_app
		pico_stdlib
		pico_cyw43_arch_lwip_threadsafe_background
		FreeRTOSKernel
)

# Route standard output over the USB cable
pico_enable_stdio_usb(pico_iot_app 1)
pico_enable_stdio_uart(pico_iot_app 0)

# Generate the .uf2 file for flash loading
pico_add_extra_outputs(pico_iot_app)
