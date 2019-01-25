//---------------------------------------------------------------
/*
  Windowed Sinc FIR Generator
  Bob Maling (BobM.DSP@gmail.com)
  Contributed to musicdsp.org Source Code Archive
  Last Updated: April 12, 2005

  http://www.musicdsp.org/showone.php?id=194

  Usage:
    Lowpass:  wsfirLP(h, N, WINDOW, fc)
    Highpass: wsfirHP(h, N, WINDOW, fc)
    Bandpass: wsfirBP(h, N, WINDOW, fc1, fc2)
    Bandstop: wsfirBS(h, N, WINDOW, fc1, fc2)

  where:
    h (float32_t[N]):  filter coefficients will be written to this array
    N (int):    number of taps
    WINDOW (int): Window (W_BLACKMAN, W_HANNING, or W_HAMMING)
    fc (float32_t):  cutoff (0 < fc < 0.5, fc = f/fs)
            --> for fs=48kHz and cutoff f=12kHz, fc = 12k/48k => 0.25

    fc1 (float32_t): low cutoff (0 < fc < 0.5, fc = f/fs)
    fc2 (float32_t): high cutoff (0 < fc < 0.5, fc = f/fs)


  Windows included here are Blackman, Hanning, and Hamming. Other windows can be
  added by doing the following:
    1. "Window type constants" section: Define a global constant for the new window.
    2. wsfirLP() function: Add the new window as a case in the switch() statement.
    3. Create the function for the window

       For windows with a design parameter, such as Kaiser, some modification
       will be needed for each function in order to pass the parameter.
*/
#ifndef WSFIR_H
#define WSFIR_H



#ifdef __cplusplus
 extern "C" {
#endif

// Filter selection pin (Toggle low to select next)
#define SELECT_PIN    0


// ID's for the different types of filters
#define ID_LOWPASS   1
#define ID_HIGHPASS  2
#define ID_BANDPASS  3
#define ID_BANDSTOP  4


// ID's for window type contstants
static const int W_BLACKMAN =  1;
static const int W_HANNING =   2;
static const int W_HAMMING =   3;


// Function prototypes
void audioFilter(float32_t h[], const int N, const int TYPE, const int WINDOW,  float32_t fc1,  float32_t fc2);
void bandpass(float32_t h[], const int N, const int WINDOW, float32_t fc1,  float32_t fc2);
void wsfirLP(float32_t h[], const int N, const int WINDOW, float32_t fc);
void wsfirHP(float32_t h[], const int N, const int WINDOW, float32_t fc);
void wsfirBS(float32_t h[], const int N, const int WINDOW, float32_t fc1,  float32_t fc2);
void wsfirBP(float32_t h[], const int N, const int WINDOW,  float32_t fc1, float32_t fc2);
void genSinc(float32_t sinc[], const int N, float32_t fc);
void wBlackman(float32_t w[], const int N);
void wHanning(float32_t w[], const int N);
void wHamming(float32_t w[], const int N);


#ifdef __cplusplus
}
#endif

#endif
