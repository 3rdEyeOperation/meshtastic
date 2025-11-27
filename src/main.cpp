/**
 * Drone Detector - Main Entry Point
 * 
 * An open source drone detection system using RF signal analysis
 * with LILYGO T-Beam Supreme and RadioLib.
 * 
 * Hardware: LILYGO T-Beam Supreme (ESP32-S3 + SX1262)
 * Display: TFT via TFT_eSPI library
 * 
 * Features:
 * - Multi-modulation detection (LoRa, FSK, OOK)
 * - 900MHz band drone signature matching
 * - Real-time signal analysis and display
 */

#include <Arduino.h>
#include <RadioLib.h>
#include "display.h"
#include "drone_detection.h"

// SX1262 radio module configuration
// Pin definitions from platformio.ini build flags
SX1262 radio = new Module(RADIO_CS, RADIO_DIO1, RADIO_RST, RADIO_BUSY);

// Default scanning frequency (MHz) - 900MHz band center
const float SCAN_FREQUENCY = FREQ_900_CENTER;

// Detection state
volatile bool receivedFlag = false;

// Timing for display updates
unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_UPDATE_INTERVAL = 3000; // 3 seconds

// Timing for modulation switching
unsigned long lastModulationSwitch = 0;
const unsigned long MODULATION_SWITCH_INTERVAL = 10000; // 10 seconds per modulation

// ISR callback for radio receive
#if defined(ESP32) || defined(ESP8266)
ICACHE_RAM_ATTR
#endif
void receiveCallback() {
    receivedFlag = true;
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    Serial.println(F("=============================="));
    Serial.println(F("Drone Detector - T-Beam Supreme"));
    Serial.println(F("900MHz Multi-Modulation Scanner"));
    Serial.println(F("=============================="));
    
    // Initialize TFT display
    Serial.print(F("[Display] Initializing TFT ... "));
    displayInit();
    displaySplash();
    Serial.println(F("success!"));
    delay(2000);  // Show splash screen
    
    // Initialize drone detection module with SX1262 radio
    Serial.print(F("[DroneDetect] Initializing 900MHz detection ... "));
    displayStatus("Initializing radio...");
    
    // Initialize drone detection (starts in LoRa mode at 915 MHz)
    if (droneDetectionInit(&radio)) {
        Serial.println(F("success!"));
        Serial.print(F("[DroneDetect] Scanning frequency: "));
        Serial.print(SCAN_FREQUENCY);
        Serial.println(F(" MHz"));
    } else {
        Serial.println(F("failed!"));
        displayError("Radio init failed!");
        while (true) {
            delay(1000);
        }
    }
    
    // Set receive callback
    radio.setDio1Action(receiveCallback);
    
    // Start receiving
    Serial.println(F("[DroneDetect] Starting continuous receive mode..."));
    int state = radio.startReceive();
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("[DroneDetect] Listening for RF signals..."));
        Serial.print(F("[DroneDetect] Modulation: "));
        Serial.println(getModulationName(getCurrentModulation()));
        displayScanningWithModulation(SCAN_FREQUENCY, getModulationName(getCurrentModulation()));
        lastDisplayUpdate = millis();
        lastModulationSwitch = millis();
    } else {
        Serial.print(F("[DroneDetect] Receive failed, code "));
        Serial.println(state);
        displayError("Receive mode failed!");
    }
}

void loop() {
    // Check if a signal was received
    if (receivedFlag) {
        receivedFlag = false;
        
        // Read received data
        String str;
        int state = radio.readData(str);
        
        if (state == RADIOLIB_ERR_NONE) {
            // Get signal parameters
            float rssi = radio.getRSSI();
            float snr = radio.getSNR();
            float freqError = radio.getFrequencyError();
            
            // Analyze signal for drone signatures
            DroneSignal droneSignal;
            bool isDrone = analyzeDroneSignal(rssi, snr, freqError, 
                                              getCurrentModulation(), &droneSignal);
            
            // Signal detected - log to Serial
            Serial.println(F("--- RF Signal Detected ---"));
            Serial.print(F("Modulation: "));
            Serial.println(getModulationName(getCurrentModulation()));
            Serial.print(F("RSSI: "));
            Serial.print(rssi);
            Serial.println(F(" dBm"));
            Serial.print(F("SNR: "));
            Serial.print(snr);
            Serial.println(F(" dB"));
            Serial.print(F("Frequency error: "));
            Serial.print(freqError);
            Serial.println(F(" Hz"));
            Serial.print(F("Drone detected: "));
            Serial.println(isDrone ? "YES" : "No");
            if (isDrone) {
                Serial.print(F("Drone type: "));
                Serial.println(droneSignal.droneType);
                Serial.print(F("Confidence: "));
                Serial.print(droneSignal.confidence);
                Serial.println(F("%"));
            }
            Serial.println(F("--------------------------"));
            
            // Update TFT display with detection info including modulation
            displayDroneDetection(rssi, snr, freqError,
                                  getModulationName(getCurrentModulation()),
                                  isDrone ? droneSignal.droneType : NULL,
                                  droneSignal.confidence);
            lastDisplayUpdate = millis();
        }
        
        // Restart receive mode
        radio.startReceive();
    }
    
    // Periodically switch modulation type to scan for different drone protocols
    if (millis() - lastModulationSwitch > MODULATION_SWITCH_INTERVAL) {
        Serial.println(F("[DroneDetect] Switching modulation mode..."));
        
        // Switch to next modulation type
        ModulationType newMod = switchToNextModulation(&radio, SCAN_FREQUENCY);
        
        Serial.print(F("[DroneDetect] Now scanning with: "));
        Serial.println(getModulationName(newMod));
        
        // Restart receive mode with new modulation
        int state = radio.startReceive();
        if (state != RADIOLIB_ERR_NONE) {
            Serial.print(F("[DroneDetect] Failed to restart receive, code: "));
            Serial.println(state);
        }
        
        // Update display with new modulation
        displayScanningWithModulation(SCAN_FREQUENCY, getModulationName(newMod));
        
        lastModulationSwitch = millis();
        lastDisplayUpdate = millis();
    }
    
    // Return to scanning display after detection timeout
    if (millis() - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL) {
        displayScanningWithModulation(SCAN_FREQUENCY, getModulationName(getCurrentModulation()));
        lastDisplayUpdate = millis();
    }
    
    delay(10);
}
