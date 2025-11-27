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
 * Display RF signal detection information
 * @param rssi Signal strength in dBm
 * @param snr Signal-to-noise ratio in dB
 * @param freqError Frequency error in Hz
 */
void displayDetection(float rssi, float snr, float freqError);

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
