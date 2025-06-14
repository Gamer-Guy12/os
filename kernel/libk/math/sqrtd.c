#include <libk/math.h>
double math_sqrtd(double input) {
    double x = input;
    while (math_absf(x * x - input) > 0.00001){
        x = 0.5 * (x + input/x);
    }
    return x;
}