# Two Stage Balanced RF Amplifier (4.5 GHz)

This repository contains the design, simulation, and analysis of a **Two-Stage Balanced RF Amplifier** optimized for a center frequency of **4.5 GHz**. The project was developed using **AWR Microwave Office**.

## Project Components

* **`/DATA_SETS/`**: Raw simulation datasets and performance metrics exported from AWR.
* **`Lab_Emma_Alvaro.emp`**: The primary AWR Microwave Office project file containing the schematic, layout, and simulation setup.
* **`Lab_Emma_Alvaro.vin`**: Simulation workspace and variable initialization file.
* **`Report_Lab_TAF.pdf`**: Detailed technical report explaining the amplifier's architecture, including:
    * Matching network design (input/output).
    * Stability analysis and biasing.
    * Gain, Noise Figure, and VSWR performance evaluation.

## Key Specifications
* **Frequency**: 4.5 GHz
* **Architecture**: Balanced configuration using 3dB hybrid couplers to ensure high isolation and input/output matching.
* **Tools**: AWR Microwave Office

## How to View
To interact with the design, open the `.emp` file within **AWR Microwave Office**. The simulation results and optimization graphs are pre-configured in the workspace.
