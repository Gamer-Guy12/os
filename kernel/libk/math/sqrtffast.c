#include <libk/math.h>
/// github fucked me over
float math_sqrtffast(float x){union{int i;float f;}u={*(int*)&x};u.i=0x1FBD1DF5+(u.i>>1);return u.f;}
