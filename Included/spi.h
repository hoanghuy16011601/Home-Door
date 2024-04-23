
#include <stdint.h>


void spi_init_master();
void spi_turnoff();
void enable_spi();
void disable_spi();
uint8_t spi_transmit(uint8_t data);