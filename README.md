# Zephyr Sensor Simulation – Producer-Consumer System

## Overview

This project implements a producer-consumer system using Zephyr RTOS and a lock-free ring buffer.

Since the application is executed on the `native_sim` platform (which has no physical sensors), sensor data is simulated rather than obtained from hardware.

---

## Features

- Simulated accelerometer data generation (producer)
- Consumer thread processing data from a ring buffer
- Single-producer single-consumer (SPSC) ring buffer implementation
- Periodic sampling using Zephyr timer and work queue
- Logging output in JSON format
- Unit testing using Zephyr `ztest` framework

---

## Architecture

### Producer
- Runs periodically using a timer + work handler
- Generates simulated sensor data using system uptime
- Pushes samples into the ring buffer

### Consumer
- Runs in a dedicated thread
- Drains samples from the ring buffer
- Prints formatted JSON output

### Ring Buffer
- Lock-free SPSC design
- Prevents overwrite when full
- Logs warning on overflow

---