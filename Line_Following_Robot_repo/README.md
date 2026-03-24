# Line Following Robot (STM32)

This repository documents the development of a Line Following Robot using the **STM32L152RCTX Discovery Board**. The project follows a modular 4-phase progression, from basic GPIO control to advanced motor logic.

## Project Structure

* **`/Common/Drivers/`**: Centralized STM32 HAL and Core driver dependencies to reduce redundancy across phases.
* **`/Phase_1/`**: Initial exploration of the ST development environment (CubeIDE) and basic GPIO configuration.
* **`/Phase_2/`**: Implementation of sensor data acquisition using ADC (Analog-to-Digital Converter).
* **`/Phase_3/`**: Refinement of control logic and initial motor interface testing.
* **`/Phase_4/`**: Finalized autonomous movement logic. The robot executes a sequence of maneuvers: Forward, Stop, Right Turn, Left Turn, and Reverse.
* **`REPORT_SDBM.pdf`**: Comprehensive technical report detailing the hardware architecture and software design.

## Hardware Assembly
The robot is built on an STM32 foundation, utilizing infrared sensors for line detection and DC motors driven by peripheral timers and GPIO signals.

## Demonstration
Check out the demonstration videos at the root:
* `SDBM_bluetooh_R11.mp4`: Bluetooth connectivity and remote control test.
* `SDBM_circtuito_R11.mp4`: Autonomous circuit navigation.

## Development Environment
* **IDE**: STM32CubeIDE
* **Language**: C (STM32 HAL)
* **Microcontroller**: STM32L152RCTX (Cortex-M3)
