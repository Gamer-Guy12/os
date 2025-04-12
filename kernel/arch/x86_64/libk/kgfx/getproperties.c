#include <libk/kgfx.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

KGFXProperties kgfx_getproperties(void) {
    KGFXProperties props;
    props.height = VGA_HEIGHT;
    props.width = VGA_WIDTH;
    
    return props;
}
