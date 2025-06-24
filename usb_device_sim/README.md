# USB Device Simulator

This subproject simulates a USB CDC ACM device for testing the main eub_monitor project.

## Structure

- src/
  - main.c: Entry point for the simulator
  - cdc_sim.c: Simulates CDC ACM device behavior
  - cdc_sim.h: Header for CDC simulation
- include/
  - cdc_sim.h: Public interface for the simulator
- Makefile: Build file for the simulator

## Features
- Simulates a USB CDC ACM device
- Accepts connections from the main project (eub_monitor)
- Responds to 0x0a and file transfer protocol
- Prints received data for verification

## Usage
Build and run the simulator before running eub_monitor.
