#ifndef TELEMETRY_SIM_H
#define TELEMETRY_SIM_H

#include <stdint.h>

// Distinct operational phases of an industrial hydraulic oil tester
typedef enum {
    PHASE_WARMUP = 0,
    PHASE_STEADY_TEST,
    PHASE_ANOMALY_SPIKE,
    PHASE_COOL_DOWN
} EquipmentPhase_t;

// Standard enterprise telemetry payload
typedef struct {
    uint32_t sequence_id;       // Increments per reading to catch packet loss
    float oil_temperature_c;    // Target: 40.0C - 85.0C
    float fluid_viscosity_cst;  // Target: 32.0 cSt - 46.0 cSt
    float motor_rpm;            // Target: 0.0 - 1800.0 RPM
    uint8_t anomaly_flag;       // 0 = Normal, 1 = Warning Critical
    const char* phase_string;   // Current phase representation for the UI
} TelemetryData_t;

// Function declarations
void telemetry_sim_init(void);
void telemetry_sim_generate_next(TelemetryData_t *data_out);

#endif // TELEMETRY_SIM_H
