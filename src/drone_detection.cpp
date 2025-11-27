/**
 * Drone Detection Module Implementation
 * 
 * Implements drone detection logic with support for multiple modulation types
 * commonly used in the 900MHz frequency range.
 * 
 * The SX1262 radio supports:
 * - LoRa modulation (primary mode for long-range drone control)
 * - FSK/GFSK modulation (telemetry and data links)
 * - OOK modulation (simple control signals)
 */

#include "drone_detection.h"
#include <math.h>

// ============================================================================
// Module State
// ============================================================================

// Current modulation mode for scanning
static ModulationType currentModulation = MOD_LORA;

// Radio initialization status
static bool isInitialized = false;

// Current sweep frequency for FHSS detection
static float currentSweepFrequency = FREQ_900_MIN;
static bool sweepComplete = false;

// ============================================================================
// Known Drone Signatures Database (900MHz Band)
// ============================================================================

/**
 * Database of known drone protocol signatures in the 900MHz band
 * Based on real-world drone systems:
 * - ExpressLRS (ELRS): LoRa (CSS), 100-500 kHz bandwidth
 * - TBS Crossfire (CRSF): Proprietary FSK with FHSS, ~10 MHz hopping
 * - RFD900/SiK Radios: FSK with FHSS, configurable bandwidth
 * - FrSky R9: LoRa-based, 200 kHz bandwidth
 */
static const DroneSignature knownSignatures[] = {
    // ExpressLRS 900MHz - Open-source LoRa system
    // Uses LoRa (Chirp Spread Spectrum), 100-500 kHz channel bandwidth
    // Ultra-fast update rates, frequency hopping across 902-928 MHz
    {
        .name = "ExpressLRS 900",
        .frequencyMin = 902.0f,
        .frequencyMax = 928.0f,
        .modulation = MOD_LORA,
        .bandwidth = 500.0f,       // 100-500 kHz depending on rate
        .minRSSI = -120.0f
    },
    // ExpressLRS 900MHz Narrow Mode
    {
        .name = "ELRS 900 Narrow",
        .frequencyMin = 902.0f,
        .frequencyMax = 928.0f,
        .modulation = MOD_LORA,
        .bandwidth = 100.0f,       // 100 kHz for high rate mode
        .minRSSI = -115.0f
    },
    // TBS Crossfire - Commercial long-range system
    // Proprietary FSK with FHSS, ~10 MHz channel hopping bandwidth
    // Extreme long range, low latency, robust link
    {
        .name = "TBS Crossfire",
        .frequencyMin = 902.0f,
        .frequencyMax = 928.0f,
        .modulation = MOD_FSK,     // Proprietary FSK with FHSS
        .bandwidth = 10000.0f,     // ~10 MHz hopping bandwidth
        .minRSSI = -130.0f
    },
    // RFD900 / SiK Radios - Long-range telemetry
    // Proprietary FSK with FHSS, configurable parameters
    // Used for telemetry and command links on larger UAVs
    {
        .name = "RFD900/SiK",
        .frequencyMin = 902.0f,
        .frequencyMax = 928.0f,
        .modulation = MOD_FSK,     // FSK with FHSS
        .bandwidth = 26000.0f,     // Full band hopping (configurable)
        .minRSSI = -121.0f
    },
    // FrSky R9 System - 900MHz long-range
    // LoRa-based modulation, ~200 kHz bandwidth
    {
        .name = "FrSky R9",
        .frequencyMin = 902.0f,
        .frequencyMax = 928.0f,
        .modulation = MOD_LORA,
        .bandwidth = 200.0f,
        .minRSSI = -120.0f
    },
    // Generic FSK telemetry link (catch-all)
    {
        .name = "FSK Telemetry",
        .frequencyMin = 902.0f,
        .frequencyMax = 928.0f,
        .modulation = MOD_FSK,
        .bandwidth = 156.0f,
        .minRSSI = -110.0f
    },
    // Simple OOK remote control
    {
        .name = "OOK Remote",
        .frequencyMin = 902.0f,
        .frequencyMax = 928.0f,
        .modulation = MOD_OOK,
        .bandwidth = 58.0f,
        .minRSSI = -100.0f
    }
};

static const size_t NUM_SIGNATURES = sizeof(knownSignatures) / sizeof(knownSignatures[0]);

// ============================================================================
// Modulation Name Lookup
// ============================================================================

const char* getModulationName(ModulationType mod) {
    switch (mod) {
        case MOD_LORA:
            return "LoRa";
        case MOD_FSK:
            return "FSK";
        case MOD_OOK:
            return "OOK";
        case MOD_UNKNOWN:
        default:
            return "Unknown";
    }
}

// ============================================================================
// Frequency Validation
// ============================================================================

bool isValid900MHzFrequency(float frequency) {
    return (frequency >= FREQ_900_MIN && frequency <= FREQ_900_MAX);
}

// ============================================================================
// Radio Configuration Functions
// ============================================================================

bool droneDetectionInit(SX1262* radio) {
    if (radio == NULL) {
        return false;
    }
    
    // Start with LoRa mode at 915 MHz
    int state = configureLoRaMode(radio, FREQ_900_CENTER);
    
    if (state == RADIOLIB_ERR_NONE) {
        isInitialized = true;
        currentModulation = MOD_LORA;
        return true;
    }
    
    return false;
}

int configureLoRaMode(SX1262* radio, float frequency) {
    if (radio == NULL) {
        return RADIOLIB_ERR_INVALID_CALL;
    }
    
    // Validate frequency is in 900MHz band
    if (!isValid900MHzFrequency(frequency)) {
        Serial.println(F("[DroneDetect] Warning: Frequency outside 900MHz band"));
    }
    
    // Configure for LoRa mode
    // Parameters: frequency (MHz), bandwidth (kHz), SF, CR
    int state = radio->begin(frequency, LORA_BANDWIDTH, 
                             LORA_SPREADING_FACTOR, LORA_CODING_RATE);
    
    if (state == RADIOLIB_ERR_NONE) {
        currentModulation = MOD_LORA;
        Serial.print(F("[DroneDetect] LoRa mode configured at "));
        Serial.print(frequency);
        Serial.println(F(" MHz"));
    }
    
    return state;
}

int configureFSKMode(SX1262* radio, float frequency) {
    if (radio == NULL) {
        return RADIOLIB_ERR_INVALID_CALL;
    }
    
    // Validate frequency is in 900MHz band
    if (!isValid900MHzFrequency(frequency)) {
        Serial.println(F("[DroneDetect] Warning: Frequency outside 900MHz band"));
    }
    
    // Configure for FSK mode
    // Parameters: frequency (MHz), bit rate (kbps), freq deviation (kHz), 
    //             RX bandwidth (kHz), output power (dBm), preamble length
    int state = radio->beginFSK(frequency, FSK_BITRATE, FSK_FREQUENCY_DEV, 
                                FSK_RX_BANDWIDTH, 14, FSK_PREAMBLE_LEN, 1.6, false);
    
    if (state == RADIOLIB_ERR_NONE) {
        currentModulation = MOD_FSK;
        Serial.print(F("[DroneDetect] FSK mode configured at "));
        Serial.print(frequency);
        Serial.println(F(" MHz"));
    }
    
    return state;
}

int configureOOKMode(SX1262* radio, float frequency) {
    if (radio == NULL) {
        return RADIOLIB_ERR_INVALID_CALL;
    }
    
    // Validate frequency is in 900MHz band
    if (!isValid900MHzFrequency(frequency)) {
        Serial.println(F("[DroneDetect] Warning: Frequency outside 900MHz band"));
    }
    
    // Configure for OOK mode using FSK with zero frequency deviation
    // OOK is essentially FSK with on/off keying
    int state = radio->beginFSK(frequency, OOK_BITRATE, 0.0, 
                                OOK_RX_BANDWIDTH, 14, 16, 1.6, false);
    
    if (state == RADIOLIB_ERR_NONE) {
        currentModulation = MOD_OOK;
        Serial.print(F("[DroneDetect] OOK mode configured at "));
        Serial.print(frequency);
        Serial.println(F(" MHz"));
    }
    
    return state;
}

// ============================================================================
// Modulation Switching
// ============================================================================

ModulationType getCurrentModulation() {
    return currentModulation;
}

ModulationType switchToNextModulation(SX1262* radio, float frequency) {
    if (radio == NULL) {
        return currentModulation;
    }
    
    int state;
    ModulationType nextMod;
    
    // Cycle through modulation types: LoRa -> FSK -> OOK -> LoRa
    switch (currentModulation) {
        case MOD_LORA:
            nextMod = MOD_FSK;
            state = configureFSKMode(radio, frequency);
            break;
        case MOD_FSK:
            nextMod = MOD_OOK;
            state = configureOOKMode(radio, frequency);
            break;
        case MOD_OOK:
        default:
            nextMod = MOD_LORA;
            state = configureLoRaMode(radio, frequency);
            break;
    }
    
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print(F("[DroneDetect] Failed to switch modulation, code: "));
        Serial.println(state);
        // Stay with current modulation on failure
        return currentModulation;
    }
    
    // Update module state with new modulation
    currentModulation = nextMod;
    
    return nextMod;
}

// ============================================================================
// Drone Signal Analysis
// ============================================================================

/**
 * Match signal against known drone signatures
 * Returns the matching signature index or -1 if no match
 */
static int matchSignature(float rssi, float frequency, ModulationType modulation) {
    for (size_t i = 0; i < NUM_SIGNATURES; i++) {
        const DroneSignature* sig = &knownSignatures[i];
        
        // Check modulation type match
        if (sig->modulation != modulation) {
            continue;
        }
        
        // Check frequency is within signature's expected range
        if (frequency < sig->frequencyMin || frequency > sig->frequencyMax) {
            continue;
        }
        
        // Check RSSI is above minimum threshold
        if (rssi < sig->minRSSI) {
            continue;
        }
        
        // Match found
        return i;
    }
    
    return -1;  // No match
}

/**
 * Calculate detection confidence based on signal characteristics
 * Returns confidence percentage (0-100)
 */
static uint8_t calculateConfidence(float rssi, float snr, float freqError) {
    uint8_t confidence = 0;
    
    // RSSI contribution (stronger signal = higher confidence)
    // Scale: -120 dBm = 0%, -30 dBm = 50%
    if (rssi > -120.0f) {
        float rssiScore = (rssi + 120.0f) / 90.0f * 50.0f;
        confidence += (uint8_t)min(rssiScore, 50.0f);
    }
    
    // SNR contribution (higher SNR = higher confidence)
    // Scale: 0 dB = 0%, 20 dB = 30%
    if (snr > 0) {
        float snrScore = snr / 20.0f * 30.0f;
        confidence += (uint8_t)min(snrScore, 30.0f);
    }
    
    // Frequency error contribution (lower error = higher confidence)
    // Scale: <1kHz = 20%, >10kHz = 0%
    float absFreqError = fabsf(freqError);
    if (absFreqError < 10000.0f) {
        float freqScore = (10000.0f - absFreqError) / 10000.0f * 20.0f;
        confidence += (uint8_t)max(freqScore, 0.0f);
    }
    
    return min(confidence, (uint8_t)100);
}

bool analyzeDroneSignal(float rssi, float snr, float freqError, 
                        ModulationType currentMod, DroneSignal* signal) {
    if (signal == NULL) {
        return false;
    }
    
    // Initialize signal structure
    signal->frequency = FREQ_900_CENTER;
    signal->rssi = rssi;
    signal->snr = snr;
    signal->freqError = freqError;
    signal->modulation = currentMod;
    signal->isDroneSignature = false;
    signal->confidence = 0;
    signal->droneType = "Unknown";
    
    // Calculate confidence based on signal quality
    signal->confidence = calculateConfidence(rssi, snr, freqError);
    
    // Try to match against known drone signatures
    // Use the center frequency for matching (signal->frequency)
    int matchIndex = matchSignature(rssi, signal->frequency, currentMod);
    
    if (matchIndex >= 0) {
        signal->isDroneSignature = true;
        signal->droneType = knownSignatures[matchIndex].name;
        
        // Boost confidence for matched signatures
        signal->confidence = min((int)signal->confidence + 20, 100);
        
        Serial.print(F("[DroneDetect] Matched signature: "));
        Serial.println(signal->droneType);
    }
    
    // Log detection details
    Serial.println(F("[DroneDetect] Signal Analysis:"));
    Serial.print(F("  Modulation: "));
    Serial.println(getModulationName(currentMod));
    Serial.print(F("  Confidence: "));
    Serial.print(signal->confidence);
    Serial.println(F("%"));
    Serial.print(F("  Drone Match: "));
    Serial.println(signal->isDroneSignature ? "Yes" : "No");
    
    return signal->isDroneSignature;
}

// ============================================================================
// Sweep Scanning Functions (for FHSS detection)
// ============================================================================

float getCurrentSweepFrequency() {
    return currentSweepFrequency;
}

float sweepToNextFrequency(SX1262* radio) {
    if (radio == NULL) {
        return currentSweepFrequency;
    }
    
    // Step to next frequency
    currentSweepFrequency += (SWEEP_STEP_KHZ / 1000.0f);  // Convert kHz to MHz
    
    // Check if we've reached end of band
    if (currentSweepFrequency > FREQ_900_MAX) {
        currentSweepFrequency = FREQ_900_MIN;
        sweepComplete = true;
        Serial.println(F("[DroneDetect] Sweep scan complete, restarting..."));
    }
    
    // Reconfigure radio at new frequency based on current modulation
    int state;
    switch (currentModulation) {
        case MOD_LORA:
            state = configureLoRaMode(radio, currentSweepFrequency);
            break;
        case MOD_FSK:
            state = configureFSKMode(radio, currentSweepFrequency);
            break;
        case MOD_OOK:
            state = configureOOKMode(radio, currentSweepFrequency);
            break;
        default:
            state = configureLoRaMode(radio, currentSweepFrequency);
            break;
    }
    
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print(F("[DroneDetect] Sweep frequency change failed, code: "));
        Serial.println(state);
    }
    
    return currentSweepFrequency;
}

void resetSweepScan() {
    currentSweepFrequency = FREQ_900_MIN;
    sweepComplete = false;
    Serial.println(F("[DroneDetect] Sweep scan reset to start"));
}

bool isSweepComplete() {
    return sweepComplete;
}
