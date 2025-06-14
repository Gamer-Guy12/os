#include <libk/math.h>
double math_sqrtdfast(double input) {
    return (double)math_sqrtffast((float)input)
}