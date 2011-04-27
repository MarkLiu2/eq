/*
  http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/dft/
  */

#include "math.h"

#define FFT_FORWARD 0
#define FFT_REVERSE 1

int fft(int dir,long m,float *x,float *y);
