

void OLED_RST_1(void);
void OLED_RST_0(void);
void Driver_Delay_ms(int delay);
void iic_init(void);
void iic_start(void);
void iic_write_byte(char byte);
int iic_wait_for_ack(void);
void iic_stop(void);
