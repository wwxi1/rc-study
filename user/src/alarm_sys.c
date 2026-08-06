#include "alarm_sys.h"
#include "led.h"
#include "buzzer.h"
#include "stm32h7xx_hal.h"

#define MODE_HOLD_MS   8000U  /* 演示用：每 8s 自动换模式 */
#define ALARM_SPEED_MS 50U    /* 报警模式流水速度 */
#define ALARM_BEEP_MS  80U    /* 报警模式单次蜂鸣时长 */
#define IDLE_MS        10U    /* 待机空转 */

static sys_mode_t work_mode     = SYS_RUN;  /* 上电默认运行模式 */
static uint8_t    s_current_led = 1U;
static uint16_t   s_blink_times = ALARM_BLINK_TIMES;
static uint32_t   s_delay_ms    = ALARM_DELAY_MS;

static void mode_switch_demo(void);
static void standby_mode(void);
static void run_mode(void);
static void alarm_mode(void);

void alarm_sys_set_mode(sys_mode_t mode) { work_mode = mode; }
sys_mode_t alarm_sys_get_mode(void)      { return work_mode; }

/* 主循环唯一入口：按当前模式执行 */
void alarm_sys_func(void)
{
   
    switch (work_mode)
    {
    case SYS_STANDBY: standby_mode(); break;
    case SYS_RUN:     run_mode();     break;
    case SYS_ALARM:   alarm_mode();   break;
    default:          standby_mode(); break;
    }
}

/* 从 main.c 迁来：第 led_num 颗 LED 闪 times 次 */
void blink_led(uint8_t led_num, uint16_t times, uint32_t delay_ms)
{
    uint16_t i;

    if (led_num > ALARM_LED_COUNT)   /* if 判断：编号只允许 1~4 */
    {
        return;
    }

    for (i = 0U; i < times; i++)     /* for 循环 */
    {
        led_on(led_num);
        HAL_Delay(delay_ms);
        led_off(led_num);
        HAL_Delay(delay_ms);
    }
}

/* 从 main.c 迁来：蜂鸣器响 beep_ms 毫秒 */
void beep(uint32_t beep_ms)
{
    buzzer_on();
    HAL_Delay(beep_ms);
    buzzer_off();
}

/* 演示：每 8s 轮换一次模式 */
static void mode_switch_demo(void)
{
    static uint32_t last_tick = 0U;

    if ((HAL_GetTick() - last_tick) < MODE_HOLD_MS) { return; }
    last_tick = HAL_GetTick();

    switch (work_mode)
    {
    case SYS_STANDBY: work_mode = SYS_RUN;     break;
    case SYS_RUN:     work_mode = SYS_ALARM;   break;
    case SYS_ALARM:   work_mode = SYS_STANDBY; break;
    default:          work_mode = SYS_STANDBY; break;
    }

    s_current_led = 1U;          /* 换模式后从头开始 */
    s_blink_times = ALARM_BLINK_TIMES;
    s_delay_ms    = ALARM_DELAY_MS;
}

/* 待机：全灭、静音 */
static void standby_mode(void)
{
    uint8_t i;
    for (i = 1U; i <= ALARM_LED_COUNT; i++) { led_off(i); }
    buzzer_off();
    HAL_Delay(IDLE_MS);
}

/* 运行：流水灯，次数递增、速度渐快（保留你原 main.c 的逻辑） */
static void run_mode(void)
{
    blink_led(s_current_led, s_blink_times, s_delay_ms);

    s_current_led++;
    s_blink_times++;

    if (s_current_led > ALARM_LED_COUNT)   /* 一轮结束 */
    {
        s_current_led = 1U;
        s_blink_times = ALARM_BLINK_TIMES;

        /* if/else：延时每次减 20，减到 100 就回到初始值 */
        if (s_delay_ms > 100U)
        {
            s_delay_ms -= 20U;
        }
        else
        {
            s_delay_ms = ALARM_DELAY_MS;
        }
    }
}

/* 报警：快速流水，每跑完一轮蜂鸣一声 = 间隔报警 */
static void alarm_mode(void)
{
    blink_led(s_current_led, 1U, ALARM_SPEED_MS);

    s_current_led++;
    if (s_current_led > ALARM_LED_COUNT)
    {
        s_current_led = 1U;
        beep(ALARM_BEEP_MS);   /* 你原来删掉的 beep 调用，放在这里 */
    }
}