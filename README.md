## NXP motor control middleware

This repositary contains motor-control software for Permanent Magnet Synchronous Motor (PMSM)
and Brushlees DC Motor (BLDC). The applications can support low-voltage hardware
platforms and various MCU types.

### Documentation

The user guides are provided on [MCUXpresso SDK for Motor Control](https://www.nxp.com/sdkmotorcontrol) web.

### License

This repository is under **LA_OPT_Online Code Hosting NXP_Software_License**.

### Examples

Check repository [mcux-sdk-examples](https://github.com/nxp-mcuxpresso/mcuxsdk-examples) for specific board and available motor-control examples. 
Some examples are available just for selected platforms. The following application types are available:

- **mc_pmsm_snsless** - Sensorless FOC example utilizing fractional and/or floating-point arithmetics. The Motor 
  Identification (MID) software module in combination with the Motor Control Application Tool (MCAT) allow for 
  rapid application development.
  
- **mc_pmsm_enc** - Sensor and sensorless FOC example utilizing fractional and/or floating-point arithmetics. The 
  MID software module in combination with the MCAT allow for rapid application development.
  
- **mc_pmsm_enc_iopamp** - Sensor and sensorless FOC example utilizing floating-point arithmetics. The MID software module 
  in combination with the MCAT allow for rapid application development. In this example, the internal operational 
  amplifiers are used instead of the external operational amplifiers.
  
- **mc_pmsm_enc_dual** - Sensor and sensorless FOC example utilizing floating-point arithmetic for dual motor 
  control application. The MID software is available. The MCAT is not available for this example.
  
- **mc_bldc** - Sensorless 6-step speed control example utilizing fractional and/or floating-point arithmetics. The example 
  with MCAT allows for rapid application development.
  
All examples support the FreeMASTER interface for quick and simple application debugging, tuning, control,and monitoring. 
See www.nxp.com/freemaster and the application user's guide for more information.
  
### Feedback

Your feedback is very important to us. Please feel free to leave a comment [here](https://forms.office.com/Pages/ResponsePage.aspx?id=06FuaCu8b0ypLNmcXDAWNRtX7GIvW5NIhyLFq8A5Qw9UMUJNTVdPVFJVWVRZT1gzWVdLS0lUVTRVVSQlQCN0PWcu).