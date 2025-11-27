/**
 * Display Module Header
 * 
 * TFT display support for the Drone Detector using TFT_eSPI library.
 * Provides visual feedback for RF signal detections.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>

// Display colors
#define COLOR_BG        TFT_BLACK
#define COLOR_TITLE     TFT_CYAN
#define COLOR_TEXT      TFT_WHITE
#define COLOR_ALERT     TFT_RED
#define COLOR_SUCCESS   TFT_GREEN
#define COLOR_WARNING   TFT_YELLOW

// Display dimensions (from platformio.ini config)
#define DISPLAY_WIDTH   TFT_WIDTH
#define DISPLAY_HEIGHT  TFT_HEIGHT

/**
 * Initialize the TFT display
 */
void displayInit();

/**
 * Show the startup splash screen
 */
void displaySplash();

/**
 * Display the main scanning status screen
 * @param frequency Current scanning frequency in MHz
 */
void displayScanning(float frequency);

/**
 * Display the main scanning status screen with modulation info
 * @param frequency Current scanning frequency in MHz
 * @param modulation Current modulation type name
 */
void displayScanningWithModulation(float frequency, const char* modulation);

/**
 * Display RF signal detection information
 * @param rssi Signal strength in dBm
 * @param snr Signal-to-noise ratio in dB
 * @param freqError Frequency error in Hz
 */
void displayDetection(float rssi, float snr, float freqError);

/**
 * Display RF signal detection with modulation and drone type information
 * @param rssi Signal strength in dBm
 * @param snr Signal-to-noise ratio in dB
 * @param freqError Frequency error in Hz
 * @param modulation Detected modulation type name
 * @param droneType Identified drone type/protocol (NULL if unknown)
 * @param confidence Detection confidence percentage (0-100)
 */
void displayDroneDetection(float rssi, float snr, float freqError, 
                           const char* modulation, const char* droneType, 
                           uint8_t confidence);

/**
 * Display an error message
 * @param message Error message to display
 */
void displayError(const char* message);

/**
 * Update status line at bottom of screen
 * @param status Status text to display
 */
void displayStatus(const char* status);

#endif // DISPLAY_H
