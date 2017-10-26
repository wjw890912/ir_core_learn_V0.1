#ifndef _IR_H_
#define _IR_H_



void init_IR_INTTRUPUT(void);
void init_PWM(void);
uint16_t Check_Learn_Finsh(void);
void Reset_Finsh(void);
void Triget_Learn_state(uint8_t state);
int  SaveIrCode2Flash(uint16_t *pt,uint16_t lenth);
int GetIrCode2Flash(uint8_t size,uint16_t *p);
uint16_t  Send_IrCodeFlash(uint8_t size,uint16_t *p);

#endif

