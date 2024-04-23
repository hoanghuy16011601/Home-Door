
void i2c_initialization_master();
void i2c_initialization_slaver(unsigned char address);
void i2c_start();
void i2c_stop();
unsigned char i2c_det();
void i2c_write_master(unsigned char data);
void i2c_write_slaver(unsigned char data);
unsigned char i2c_read_master();
unsigned char i2c_read_slaver();
void i2c_nak();