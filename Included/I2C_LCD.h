
 void i2c_lcd_init_power();
 void i2c_lcd_init(unsigned char func_set,unsigned char display_control, unsigned char mode_set);
 void i2c_lcd_turnoff();
 void i2c_lcd_clear();
 void i2c_lcd_begin(unsigned char position);
 void i2c_lcd_command(unsigned char cmd);
 void i2c_lcd_data(unsigned char cmd);
 void i2c_lcd_string(unsigned char *str);
 
 