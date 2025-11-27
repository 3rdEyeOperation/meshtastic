/**
 * Display Module Implementation
 * 
 * TFT display support for the Drone Detector using TFT_eSPI library.
 * Provides visual feedback for RF signal detections.
 */

#include "display.h"

// Global TFT display instance
TFT_eSPI tft = TFT_eSPI();

// Detection counter
static uint32_t detectionCount = 0;

void displayInit() {
    tft.init();
    tft.setRotation(1);  // Landscape mode
    tft.fillScreen(COLOR_BG);
    
    // Enable backlight if pin is defined
    #ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    #endif
}

void displaySplash() {
    tft.fillScreen(COLOR_BG);
    
    // Title
    tft.setTextColor(COLOR_TITLE, COLOR_BG);
    tft.setTextSize(2);
    tft.setCursor(20, 30);
    tft.println("Drone Detector");
    
    // Subtitle
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextSize(1);
    tft.setCursor(20, 60);
    tft.println("T-Beam Supreme");
    tft.setCursor(20, 75);
    tft.println("RF Signal Analysis");
    
    // Version/info
    tft.setTextColor(COLOR_SUCCESS, COLOR_BG);
    tft.setCursor(20, 100);
    tft.println("Initializing...");
}

void displayScanning(float frequency) {
    tft.fillScreen(COLOR_BG);
    
    // Header
    tft.setTextColor(COLOR_TITLE, COLOR_BG);
    tft.setTextSize(2);
    tft.setCursor(10, 5);
    tft.println("SCANNING");
    
    // Frequency display
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextSize(1);
    tft.setCursor(10, 35);
    tft.print("Frequency: ");
    tft.print(frequency, 1);
    tft.println(" MHz");
    
    // Detection count
    tft.setCursor(10, 55);
    tft.print("Detections: ");
    tft.println(detectionCount);
    
    // Status
    tft.setTextColor(COLOR_SUCCESS, COLOR_BG);
    tft.setCursor(10, 80);
    tft.println("Listening for RF signals...");
    
    // Visual indicator
    tft.drawRect(10, 100, 220, 20, COLOR_SUCCESS);
}

void displayDetection(float rssi, float snr, float freqError) {
    detectionCount++;
    
    tft.fillScreen(COLOR_BG);
    
    // Alert header
    tft.setTextColor(COLOR_ALERT, COLOR_BG);
    tft.setTextSize(2);
    tft.setCursor(10, 5);
    tft.println("RF DETECTED!");
    
    // Signal details
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextSize(1);
    
    tft.setCursor(10, 35);
    tft.print("RSSI: ");
    tft.setTextColor(rssi > -70 ? COLOR_SUCCESS : COLOR_WARNING, COLOR_BG);
    tft.print(rssi, 1);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.println(" dBm");
    
    tft.setCursor(10, 50);
    tft.print("SNR:  ");
    tft.setTextColor(snr > 0 ? COLOR_SUCCESS : COLOR_WARNING, COLOR_BG);
    tft.print(snr, 1);
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.println(" dB");
    
    tft.setCursor(10, 65);
    tft.print("Freq Error: ");
    tft.print(freqError, 0);
    tft.println(" Hz");
    
    // Detection count
    tft.setTextColor(COLOR_TITLE, COLOR_BG);
    tft.setCursor(10, 85);
    tft.print("Total: ");
    tft.println(detectionCount);
    
    // Visual alert bar
    tft.fillRect(10, 105, 220, 15, COLOR_ALERT);
    tft.setTextColor(COLOR_BG, COLOR_ALERT);
    tft.setCursor(80, 108);
    tft.println("SIGNAL");
}

void displayError(const char* message) {
    tft.fillScreen(COLOR_BG);
    
    // Error header
    tft.setTextColor(COLOR_ALERT, COLOR_BG);
    tft.setTextSize(2);
    tft.setCursor(10, 30);
    tft.println("ERROR");
    
    // Error message
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextSize(1);
    tft.setCursor(10, 60);
    tft.println(message);
}

void displayStatus(const char* status) {
    // Clear status area at bottom
    tft.fillRect(0, DISPLAY_HEIGHT - 15, DISPLAY_WIDTH, 15, COLOR_BG);
    
    tft.setTextColor(COLOR_TEXT, COLOR_BG);
    tft.setTextSize(1);
    tft.setCursor(5, DISPLAY_HEIGHT - 12);
    tft.println(status);
}
