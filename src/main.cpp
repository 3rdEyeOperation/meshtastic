/**
 * Drone Detector - Main Entry Point
 * 
 * An open source drone detection system using RF signal analysis
 * with LILYGO T-Beam Supreme and RadioLib.
 * 
 * Hardware: LILYGO T-Beam Supreme (ESP32-S3 + SX1262)
 * Display: TFT via TFT_eSPI library
 */

#include <Arduino.h>
#include <RadioLib.h>
#include "display.h"

// SX1262 radio module configuration
// Pin definitions from platformio.ini build flags
SX1262 radio = new Module(RADIO_CS, RADIO_DIO1, RADIO_RST, RADIO_BUSY);

// Default scanning frequency (MHz)
const float SCAN_FREQUENCY = 915.0;

// Detection state
volatile bool receivedFlag = false;

// Timing for display updates
unsigned long lastDisplayUpdate = 0;
const unsigned long DISPLAY_UPDATE_INTERVAL = 3000; // 3 seconds

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
    Serial.println(F("=============================="));
    
    // Initialize TFT display
    Serial.print(F("[Display] Initializing TFT ... "));
    displayInit();
    displaySplash();
    Serial.println(F("success!"));
    delay(2000);  // Show splash screen
    
    // Initialize SX1262 radio
    Serial.print(F("[SX1262] Initializing ... "));
    displayStatus("Initializing radio...");
    
    // Default frequency: 915 MHz (adjust for your region)
    // Bandwidth: 125 kHz
    // Spreading Factor: 9
    // Coding Rate: 7
    int state = radio.begin(SCAN_FREQUENCY, 125.0, 9, 7);
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        displayError("Radio init failed!");
        while (true) {
            delay(1000);
        }
    }
    
    // Set receive callback
    radio.setDio1Action(receiveCallback);
    
    // Start receiving
    Serial.println(F("[SX1262] Starting continuous receive mode..."));
    state = radio.startReceive();
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("[SX1262] Listening for RF signals..."));
        displayScanning(SCAN_FREQUENCY);
    } else {
        Serial.print(F("[SX1262] Receive failed, code "));
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
            
            // Signal detected - log to Serial
            Serial.println(F("--- RF Signal Detected ---"));
            Serial.print(F("RSSI: "));
            Serial.print(rssi);
            Serial.println(F(" dBm"));
            Serial.print(F("SNR: "));
            Serial.print(snr);
            Serial.println(F(" dB"));
            Serial.print(F("Frequency error: "));
            Serial.print(freqError);
            Serial.println(F(" Hz"));
            Serial.println(F("--------------------------"));
            
            // Update TFT display with detection info
            displayDetection(rssi, snr, freqError);
            lastDisplayUpdate = millis();
        }
        
        // Restart receive mode
        radio.startReceive();
    }
    
    // Return to scanning display after detection timeout
    if (millis() - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL) {
        displayScanning(SCAN_FREQUENCY);
        lastDisplayUpdate = millis();
    }
    
    delay(10);
}
