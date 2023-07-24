#include <cstdint>

// define used registers
static volatile unsigned &RCC_AHB1 = *reinterpret_cast<unsigned *>(0x40021014);
static volatile unsigned &GPIOE_MODER = *reinterpret_cast<unsigned *>(0x48001000);
static volatile unsigned &GPIOE_BSRR = *reinterpret_cast<unsigned *>(0x48001018);

#define bit(x) (1 << x)

void delay (void) {
    for (volatile uint32_t i = 0; i < 100000; i++);
}

// application
void main_app() {

    RCC_AHB1 |= bit(21);  // enable clock for GPIOE
    GPIOE_MODER |= 0x40000000;  // set output on GPIOE.P15
    GPIOE_BSRR = 0x00008000;  // set output on GPIOE.P15

    while (true) {
        delay();
        GPIOE_BSRR = 0x80000000;  // clear output on GPIOE.P15
        delay();
        GPIOE_BSRR = 0x00008000;  // set output on GPIOE.P15
    }
}
