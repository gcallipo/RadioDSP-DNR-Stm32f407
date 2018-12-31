# RadioDSP-DNR-Stm32f407

 THE RADIO DSP - DNR - PROJECT - Giuseppe Callipo - IK8YFW

This is an open source project that define a simple platform to make an
Audio DSP Processor for shortwave radio receiver with Digital Noise Reduction capabilities based on Spectral Subtraction. 

The system uses the good performances offered by the stm32f4 processor based on the CortexM4 specification, thanks to DSP extensions it is possible to process 256-element FFT real time (bin) ensuring a good response on the DNR functionalities, filtering and visualization of the audio signal.
Four filter elements have been implemented, currently fixed and implemented with Fir, with the following characteristics:
Filter 0: passes through (> 16kHz)
Filter 1: low pass for AM (<6kHz)
Filter 2: low pass for SSB (<3kHz)
Filter 3: Switch bandwidth for CW (500 Hz)

The display of the spectrum of the signal received in 8kHz and one is also implemented
secondary display to Waterfall with 3kHz zoom.

The project is work in progress and may be subject to change without notice.

 The RadioDSP openSource software is released under the license:
            Common Creative - Attribution 3.0
            
Vy 73 de Giuseppe Callipo - IK8YFW
