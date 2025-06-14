#include <libk/math.h>
float math_sqrtf(float input) {
    float x = input;
    while (math_absf(x * x - input) > 0.00001){
        x = 0.5 * (x + input/x);
    }
    return x;
}