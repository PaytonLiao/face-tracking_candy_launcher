#include "mbed.h"
#include "SPI.h"

SPI spi(D11, D12, D13);  // MOSI, MISO, SCLK
DigitalOut cs(D9);      // Chip select

int main() {
    spi.format(8, 0);   // 8 bits per frame, mode 0
    spi.frequency(1000000); // 1 MHz

    cs = 1; // Deselect slave
    int counter = 0;
    int hexval = 0;
    while (1) {
        // Select slave
        cs = 0;

        // Send dummy data to trigger response from slave
        spi.write(0x00);

        // Receive response data (assuming a single byte)
        uint8_t received_byte = spi.write(0x00);
        
        // Deselect slave
        cs = 1;

        // Convert received_byte from hexadecimal to decimal
        int received_decimal = (int)received_byte;

        // Process received data
        // printf("Received data from slave (hex): 0x%X\n", received_byte);
        // printf("Received data from slave (decimal): %d\n", received_decimal);

        if(counter%2 == 0){
            hexval|= received_byte << 8;
            // printf("1Received data from slave (hex): 0x%X\n", hexval);
            counter++;
        }
        else{
            hexval|= received_byte;
            // printf("2Received data from slave (hex): 0x%X\n", hexval);
            int decval = (int)hexval;
            printf("!!!Received data from slave (decimal): %d\n", decval);
            counter--;
            hexval = 0;
        }
        // Wait for a while before sending again
        thread_sleep_for(500);
    }
}
