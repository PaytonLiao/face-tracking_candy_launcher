#include "mbed.h"
DigitalIn bIn1(D15); // initialise pin for digital read
int main()
{
    bIn1.mode(PullUp); // initialise the pin with pull-down resistor
    while (1)
    {
        printf("The value is %d \n", bIn1.read());
        thread_sleep_for(100);
    }
}
