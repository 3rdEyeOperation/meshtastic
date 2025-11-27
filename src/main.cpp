/**
 * Drone Detector - Main Entry Point
 * 
 * An open source drone detection system using RF signal analysis
 * with LILYGO T-Beam Supreme and RadioLib.
 * 
 * Hardware: LILYGO T-Beam Supreme (ESP32-S3 + SX1262)
 */

#include <Arduino.h>
#include <RadioLib.h>

// SX1262 radio module configuration
// Pin definitions from platformio.ini build flags
SX1262 radio = new Module(RADIO_CS, RADIO_DIO1, RADIO_RST, RADIO_BUSY);

// Detection state
volatile bool receivedFlag = false;

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
    
    // Initialize SX1262 radio
    Serial.print(F("[SX1262] Initializing ... "));
    
    // Default frequency: 915 MHz (adjust for your region)
    // Bandwidth: 125 kHz
    // Spreading Factor: 9
    // Coding Rate: 7
    int state = radio.begin(915.0, 125.0, 9, 7);
    
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
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
    } else {
        Serial.print(F("[SX1262] Receive failed, code "));
        Serial.println(state);
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
            // Signal detected - log information
            Serial.println(F("--- RF Signal Detected ---"));
            Serial.print(F("RSSI: "));
            Serial.print(radio.getRSSI());
            Serial.println(F(" dBm"));
            Serial.print(F("SNR: "));
            Serial.print(radio.getSNR());
            Serial.println(F(" dB"));
            Serial.print(F("Frequency error: "));
            Serial.print(radio.getFrequencyError());
            Serial.println(F(" Hz"));
            Serial.println(F("--------------------------"));
        }
        
        // Restart receive mode
        radio.startReceive();
    }
    
    delay(10);
}
