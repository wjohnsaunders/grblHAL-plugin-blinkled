/*
  blinkled.c - plugin that blinks the LED for the STM32F1xx driver
  Used by grblHAL
  Refer to LICENSE
*/

#include "driver.h"

#ifdef BLINKLED_ENABLE
#if defined(BLINK_LED_PORT) && defined(BLINK_LED_PIN)

static on_report_options_ptr on_report_options;
static on_execute_realtime_ptr on_execute_realtime;

static void report_blinkled_options(bool newopt)
{
    on_report_options(newopt);

    if (!newopt)
    {
        hal.stream.write("[PLUGIN:BlinkLED]" ASCII_EOL);
    }
}

static void execute_blinkled_realtime(sys_state_t state)
{
    static uint32_t blink_period = 2000; // flashing rate in milliseconds
    static uint8_t led_on = 0;
    static uint32_t ms = 0;

    if (hal.get_elapsed_ticks() >= ms)
    {
        ms = hal.get_elapsed_ticks() + (blink_period / 2);
        led_on = 1 - led_on;
        BITBAND_PERI(BLINK_LED_PORT->ODR, BLINK_LED_PIN) = led_on;
    }

    on_execute_realtime(state);
}

void blinkled_init(void)
{
    // Add plugin report to the $I report.
    on_report_options = grbl.on_report_options;
    grbl.on_report_options = report_blinkled_options;

    // Add realtime execution to grblHAL foreground process
    on_execute_realtime = grbl.on_execute_realtime;
    grbl.on_execute_realtime = execute_blinkled_realtime;

    GPIO_InitTypeDef GPIO_Init = {
        .Mode  = GPIO_MODE_OUTPUT_PP,
        .Speed = GPIO_SPEED_FREQ_LOW,
        .Pin   = (1 << BLINK_LED_PIN),
    };
    HAL_GPIO_Init(BLINK_LED_PORT, &GPIO_Init);
}

#endif
#endif
