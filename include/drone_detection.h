/**
 * Drone Detection Module Header
 * 
 * Implements drone detection logic with support for multiple modulation types
 * commonly used in the 900MHz frequency range.
 * 
 * Supported modulation types:
 * - LoRa (Long Range)
 * - FSK/GFSK (Frequency Shift Keying)
 * - OOK (On-Off Keying)
 */

#ifndef DRONE_DETECTION_H
#define DRONE_DETECTION_H

#include <Arduino.h>
#include <RadioLib.h>

// ============================================================================
// Modulation Type Definitions
// ============================================================================

/**
 * Enum representing supported modulation types for 900MHz drone detection
 */
typedef enum {
    MOD_LORA,       // LoRa modulation (ExpressLRS, Crossfire TBS)
    MOD_FSK,        // FSK/GFSK modulation (common telemetry links)
    MOD_OOK,        // OOK modulation (simple control systems)
    MOD_UNKNOWN     // Unknown/unidentified modulation
} ModulationType;

/**
 * Get string representation of modulation type
 * @param mod Modulation type enum value
 * @return String name of the modulation type
 */
const char* getModulationName(ModulationType mod);

// ============================================================================
// 900MHz Band Configuration
// ============================================================================

// US 900MHz ISM band range (902-928 MHz)
#define FREQ_900_MIN        902.0f
#define FREQ_900_MAX        928.0f
#define FREQ_900_CENTER     915.0f

// Common drone frequencies in 900MHz band
#define FREQ_EXPRESSLRS_900 915.0f    // ExpressLRS 900MHz center
#define FREQ_CROSSFIRE      915.0f    // TBS Crossfire center
#define FREQ_FRSKY_900      915.0f    // FrSky 900MHz systems

// ============================================================================
// LoRa Configuration for 900MHz
// ============================================================================

// LoRa parameters for drone detection
#define LORA_BANDWIDTH      125.0f    // 125 kHz bandwidth
#define LORA_SPREADING_FACTOR 9       // SF9 (common for drone links)
#define LORA_CODING_RATE    7         // 4/7 coding rate

// ============================================================================
// FSK Configuration for 900MHz
// ============================================================================

// FSK/GFSK parameters for 900MHz drone protocols
#define FSK_BITRATE         100.0f    // 100 kbps (common for telemetry)
#define FSK_FREQUENCY_DEV   50.0f     // 50 kHz frequency deviation
#define FSK_RX_BANDWIDTH    156.2f    // Receiver bandwidth in kHz
#define FSK_PREAMBLE_LEN    16        // Preamble length in bits

// ============================================================================
// OOK Configuration for 900MHz
// ============================================================================

// OOK parameters for simple drone control
#define OOK_BITRATE         4.8f      // 4.8 kbps (typical for simple remotes)
#define OOK_RX_BANDWIDTH    58.0f     // Receiver bandwidth in kHz

// ============================================================================
// Drone Signature Detection
// ============================================================================

/**
 * Structure to hold detected drone signal characteristics
 */
typedef struct {
    float frequency;            // Detection frequency (MHz)
    float rssi;                 // Signal strength (dBm)
    float snr;                  // Signal-to-noise ratio (dB)
    float freqError;            // Frequency error (Hz)
    ModulationType modulation;  // Detected modulation type
    bool isDroneSignature;      // True if matches known drone signature
    uint8_t confidence;         // Detection confidence (0-100%)
    const char* droneType;      // Identified drone type/protocol
} DroneSignal;

/**
 * Known drone protocol signatures for 900MHz band
 */
typedef struct {
    const char* name;           // Protocol name
    float frequencyMin;         // Minimum frequency (MHz)
    float frequencyMax;         // Maximum frequency (MHz)
    ModulationType modulation;  // Expected modulation type
    float bandwidth;            // Expected bandwidth (kHz)
    float minRSSI;              // Minimum expected RSSI (dBm)
} DroneSignature;

// ============================================================================
// Detection Functions
// ============================================================================

/**
 * Initialize drone detection module
 * @param radio Pointer to SX1262 radio instance
 * @return true if initialization successful
 */
bool droneDetectionInit(SX1262* radio);

/**
 * Configure radio for LoRa modulation detection
 * @param radio Pointer to SX1262 radio instance
 * @param frequency Center frequency in MHz
 * @return RadioLib status code
 */
int configureLoRaMode(SX1262* radio, float frequency);

/**
 * Configure radio for FSK modulation detection
 * @param radio Pointer to SX1262 radio instance
 * @param frequency Center frequency in MHz
 * @return RadioLib status code
 */
int configureFSKMode(SX1262* radio, float frequency);

/**
 * Configure radio for OOK modulation detection
 * @param radio Pointer to SX1262 radio instance
 * @param frequency Center frequency in MHz
 * @return RadioLib status code
 */
int configureOOKMode(SX1262* radio, float frequency);

/**
 * Analyze received signal for drone signatures
 * @param rssi Signal strength in dBm
 * @param snr Signal-to-noise ratio in dB
 * @param freqError Frequency error in Hz
 * @param currentMod Current modulation mode
 * @param signal Output structure for detection results
 * @return true if signal matches drone signature
 */
bool analyzeDroneSignal(float rssi, float snr, float freqError, 
                        ModulationType currentMod, DroneSignal* signal);

/**
 * Get the currently active modulation type
 * @return Current modulation type enum
 */
ModulationType getCurrentModulation();

/**
 * Switch to next modulation type in scanning sequence
 * @param radio Pointer to SX1262 radio instance
 * @param frequency Center frequency in MHz
 * @return New modulation type after switch
 */
ModulationType switchToNextModulation(SX1262* radio, float frequency);

/**
 * Check if frequency is in valid 900MHz band
 * @param frequency Frequency to check in MHz
 * @return true if frequency is in 900MHz band
 */
bool isValid900MHzFrequency(float frequency);

#endif // DRONE_DETECTION_H
