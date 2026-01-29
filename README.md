# MCUXpresso SDK : Motor Control

## Overview
This repository is for MCUXpresso SDK motor control middleware delivery and it contains the components officially provided in NXP MCUXpresso SDK. The repositary contains motor control software for Permanent Magnet Synchronous Motor (PMSM) and Brushlees DC Motor (BLDC). The applications can support low-voltage hardware platforms and various MCU types.

## Documentation
Overall [SDK Documentation](https://mcuxpresso.nxp.com/mcuxsdk/latest/html/introduction/README.html) and [Motor Control Website](https://www.nxp.com/sdkmotorcontrol) is provided to introduce more details of the motor control middleware.

## Setup
Instructions on how to install the MCUXpresso SDK provided from GitHub via west manifest [Getting Started with SDK](https://mcuxpresso.nxp.com/mcuxsdk/latest/html/gsd/installation.html#installation)

## Contribution
Contributions are not currently accepted. Guidelines to contribute will be posted in the future.

---------------------------------
## Available examples
Some motor control examples are available just for selected platforms. The following application types are available:

- **mc_pmsm_snsless** - Sensorless FOC example utilizing fractional and/or floating-point arithmetics. The Motor Identification (MID) software module in combination with the Motor Control Application Tool (MCAT) allow for rapid application development.
  
- **mc_pmsm_enc** - Sensor and sensorless FOC example utilizing fractional and/or floating-point arithmetics. The MID software module in combination with the MCAT allow for rapid application development.
  
- **mc_pmsm_enc_iopamp** - Sensor and sensorless FOC example utilizing floating-point arithmetics. The MID software module in combination with the MCAT allow for rapid application development. In this example, the internal operational amplifiers are used instead of the external operational amplifiers.
  
- **mc_pmsm_enc_dual** - Sensor and sensorless FOC example utilizing floating-point arithmetic for dual motor control application. The MID software is available. The MCAT is not available for this example.

- **mc_pmsm_servo** - Sensor FOC example utilizing floating-point arithmetic for single motor control application. The MID software is not available yet. The MCAT is available.
  
- **mc_pmsm_servo_dual** - Sensor FOC example utilizing floating-point arithmetic for dual motor control application. The MID software is not available yet. The MCAT is available only for Motor Controller 1.
  
- **mc_bldc** - Sensorless 6-step speed control example utilizing fractional and/or floating-point arithmetics. The example with MCAT allows for rapid application development.
  
All examples support the FreeMASTER interface for quick and simple application debugging, tuning, control,and monitoring. See [www.nxp.com/freemaster](www.nxp.com/freemaster) and the application user's guide for more information.
  
## Feedback
Your feedback is very important to us. Please feel free to leave a comment [Feedback](https://forms.office.com/Pages/ResponsePage.aspx?id=06FuaCu8b0ypLNmcXDAWNRtX7GIvW5NIhyLFq8A5Qw9UMUJNTVdPVFJVWVRZT1gzWVdLS0lUVTRVVSQlQCN0PWcu).