#ifndef ALARM_SYS_H
#define ALARM_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* 三种工作模式 */
typedef enum
{
    SYS_STANDBY = 0,   /* 待机：全灭、静音 */
    SYS_RUN,           /* 运行：只跑流水灯 */
    SYS_ALARM          /* 报警：流水灯加速 + 每轮蜂鸣一声 */
} sys_mode_t;

#define ALARM_LED_COUNT   4U     /* LED 数量 */
#define ALARM_BLINK_TIMES 1U     /* 运行模式每颗 LED 初始闪烁次数 */
#define ALARM_DELAY_MS    300U   /* 运行模式初始延时 */

void       blink_led(uint8_t led_num, uint16_t times, uint32_t delay_ms);
void       beep(uint32_t beep_ms);
void       alarm_sys_func(void);                 /* 主循环唯一入口 */
void       alarm_sys_set_mode(sys_mode_t mode);  /* 以后接按键时用 */
sys_mode_t alarm_sys_get_mode(void);

#ifdef __cplusplus
}
#endif

#endif /* ALARM_SYS_H */