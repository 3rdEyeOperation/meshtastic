<div align="center" markdown="1">

# 🛸 Drone Detector

An open source drone detection system using RF signal analysis with T-Beam S3-Core and RadioLib.

</div>

## Overview

The Drone Detector identifies drones by matching their RF communication signatures against known frequency ranges and modulation patterns. Built on the LILYGO T-Beam S3-Core hardware with the RadioLib library.

## Features

- **Multi-frequency scanning** - 433 MHz, 868 MHz, or 915 MHz (depending on device variant)
- **Modulation detection** - LoRa, FSK, FHSS, OOK signal identification
- **GPS logging** - Record location of detected signals
- **Button-based interface** - Standalone operation without external devices
- **RadioLib integration** - Comprehensive RF protocol support

## Hardware

**Required:** LILYGO T-Beam S3-Core

| Component | Specification |
|-----------|---------------|
| MCU | ESP32-S3 |
| Radio | Semtech SX1262 |
| GPS | NEO-M10S or Quectel L76K |

## Detection Method

The system detects drones by:

1. Scanning frequency ranges for RF activity
2. Analyzing modulation type of detected signals
3. Matching against known drone signature database
4. Logging detections with GPS coordinates

## Limitations

The SX1262 operates in sub-GHz bands. Many consumer drones use 2.4 GHz / 5.8 GHz which require different hardware. This project focuses on drones using:

- Long-range LoRa control (ExpressLRS, Crossfire)
- Sub-GHz telemetry links
- 433/868/915 MHz control systems

## Documentation

See the full documentation for setup and usage instructions.

## Building the Documentation Site

This repository contains the documentation website built with [Docusaurus](https://docusaurus.io/).

### Prerequisites

- [Node.js](https://nodejs.org/) v23.1.0 or later (see `.nvmrc`)
- [pnpm](https://pnpm.io/) package manager

### Installation

```bash
pnpm install
```

### Development

Start a local development server with hot reloading:

```bash
pnpm start
```

### Production Build

Build the static site for production:

```bash
pnpm run build
```

Serve the production build locally:

```bash
pnpm run serve
```

## Resources

- [RadioLib Library](https://github.com/jgromes/RadioLib)
- [LILYGO T-Beam](https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series)
- [SX1262 Datasheet](https://www.semtech.com/products/wireless-rf/lora-connect/sx1262)

## License

GPL-3.0
