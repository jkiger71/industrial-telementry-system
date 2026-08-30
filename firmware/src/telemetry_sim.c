#include "telemetry_sim.h"
#include <stdlib.h>
#include <math.h>

static uint32_t global_sequence_counter = 0;
static uint32_t internal_ticks = 0;
static EquipmentPhase_t current_phase = PHASE_WARMUP;

void telemetry_sim_init(void) {
    global_sequence_counter = 0;
    internal_ticks = 0;
    current_phase = PHASE_WARMUP;
    // Seed standard library random generator
    srand(1337);
}

// Generates a tiny bit of Gaussian-like thermal/sensor white noise
static float get_sensor_noise(float magnitude) {
    return (((float)rand() / (float)RAND_MAX) - 0.5f) * magnitude;
}

void telemetry_sim_generate_next(TelemetryData_t *data_out) {
    internal_ticks++;
    global_sequence_counter++;

    data_out->sequence_id = global_sequence_counter;
    data_out->anomaly_flag = 0;

    // State Machine: Progress through industrial runtime stages every ~60 ticks
    if (internal_ticks < 40) {
        current_phase = PHASE_WARMUP;
        data_out->phase_string = "WARM_UP";
    } else if (internal_ticks < 140) {
        current_phase = PHASE_STEADY_TEST;
        data_out->phase_string = "TESTING";
    } else if (internal_ticks < 170) {
        current_phase = PHASE_ANOMALY_SPIKE;
        data_out->phase_string = "CRITICAL_ANOMALY";
    } else if (internal_ticks < 220) {
        current_phase = PHASE_COOL_DOWN;
        data_out->phase_string = "COOLING";
    } else {
        internal_ticks = 0; // Reset simulation loop indefinitely
    }

    // Mathematical execution modeling per operational phase
    switch (current_phase) {
        case PHASE_WARMUP:
            // Temperature ramps up linearly, motor begins spinning up
            data_out->oil_temperature_c = 25.0f + (internal_ticks * 0.8f) + get_sensor_noise(0.4f);
            data_out->motor_rpm = 1200.0f + (internal_ticks * 10.0f);
            // Viscosity drops naturally as fluid gets hotter
            data_out->fluid_viscosity_cst = 46.0f - (internal_ticks * 0.2f) + get_sensor_noise(0.1f);
            break;

        case PHASE_STEADY_TEST:
            // Settles into optimal industrial operating range with fine sine-wave oscillation
            data_out->oil_temperature_c = 65.0f + (sinf(internal_ticks * 0.1f) * 1.5f) + get_sensor_noise(0.3f);
            data_out->motor_rpm = 1750.0f + get_sensor_noise(5.0f);
            data_out->fluid_viscosity_cst = 36.5f + (cosf(internal_ticks * 0.1f) * 0.5f) + get_sensor_noise(0.1f);
            break;

        case PHASE_ANOMALY_SPIKE:
            // Simulate sudden pump failure/cavitation. Temperature rockets up, viscosity collapses.
            data_out->oil_temperature_c = 65.0f + ((internal_ticks - 140) * 1.8f) + get_sensor_noise(1.0f);
            data_out->motor_rpm = 1750.0f + get_sensor_noise(45.0f); // Erratic RPM
            data_out->fluid_viscosity_cst = 36.5f - ((internal_ticks - 140) * 0.6f) + get_sensor_noise(0.3f);

            // Trigger emergency flags if threshold breaks safety parameters
            if (data_out->oil_temperature_c > 80.0f) {
                data_out->anomaly_flag = 1;
            }
            break;

        case PHASE_COOL_DOWN:
            // System brakes, motor shuts off, temperature returns down toward ambient room level
            data_out->motor_rpm = 0.0f;
            data_out->oil_temperature_c = 85.0f - ((internal_ticks - 170) * 1.1f) + get_sensor_noise(0.5f);
            if (data_out->oil_temperature_c < 40.0f) data_out->oil_temperature_c = 40.0f;
            data_out->fluid_viscosity_cst = 31.0f + ((internal_ticks - 170) * 0.2f);
            break;
    }
}
