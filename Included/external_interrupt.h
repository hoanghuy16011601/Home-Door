
#define LOW 0x00
#define DIGITAL  1
#define FALLING  2
#define RAISING  3
#define  PORT_B   0
#define  PORT_C	 1
#define  PORT_D	 2


void external_interrupt(char mask, char type);
void external_interrupt_stop(char mask);
void pinchange_interrupt(char PORT, char PIN);
void pinchange_interrupt_stop(char PORT, char PIN);

