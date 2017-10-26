
#ifndef _MAIN_H
#define _MAIN_H


void initsys(void);
void initTMR0(void);
void init_PWM(void);
void init_INT0(void);
void time1_callback(uint32_t da);//定时器0专门做采样工作和刷新屏幕显示
#endif


