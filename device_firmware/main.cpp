// libraries
#include <stdio.h>
#include "pico/stdlib.h" 
#include "hardware/adc.h"
#include "tusb.h"
#include <map>

// DEFINES
#define LED_PIN 25     // built-in LED on board
#define BUTTON1_PIN 13 // left click button
#define BUTTON2_PIN 15 // right click button
#define LEFTCLICK_MASK 1 
#define RIGHTCLICK_MASK 2

// Current button mappings (example: map to keyboard keys)
std::map<int, uint8_t> button_mapping = {
    {BUTTON1_PIN, HID_KEY_T},   // default: T key
    {BUTTON2_PIN, HID_KEY_Y}   // default: Y key
};

// HID descriptor
extern "C" uint16_t tud_hid_get_report_cb(uint8_t instance,
                                          uint8_t report_id,
                                          hid_report_type_t report_type,
                                          uint8_t* buffer,
                                          uint16_t reqlen)
{
    return 0;
}

extern "C" void tud_hid_set_report_cb(uint8_t instance,
                                      uint8_t report_id,
                                      hid_report_type_t report_type,
                                      uint8_t const* buffer,
                                      uint16_t bufsize)
{
}

/**
 * main
 */
int main()
{
    // vars 
    static bool clicked = false;
    static absolute_time_t click_time;
    absolute_time_t last_scroll_time = get_absolute_time();
    const uint32_t SCROLL_INTERVAL_US = 90000; // 90ms 
    bool leftclick_pressed;
    bool rightclick_pressed;
    uint8_t buttons;
    uint16_t raw;
    int16_t centered;
    int16_t scaled;
    int8_t scroll;

    // ------ Inits ------
    stdio_init_all(); // init serial comms + board
    tusb_init(); // init tinyusb
    adc_init(); // init adc for joystick readings

    // LED (for testing purposes)
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Mouse Buttons
    gpio_init(BUTTON1_PIN);
    gpio_set_dir(BUTTON1_PIN, GPIO_IN);
    gpio_pull_up(BUTTON1_PIN);

    gpio_init(BUTTON2_PIN); 
    gpio_set_dir(BUTTON2_PIN, GPIO_IN);
    gpio_pull_up(BUTTON2_PIN);

    // Macro Buttons

    // ------ MAIN LOOP ------
    while (true)
    {
        tud_task(); // must be called frequently as this is the USB hid task

        // ---- Button Press ----
        leftclick_pressed = !gpio_get(BUTTON1_PIN);
        rightclick_pressed = !gpio_get(BUTTON2_PIN);

        buttons = 0;

        if (leftclick_pressed)  buttons |= 0x01;
        if (rightclick_pressed) buttons |= 0x02;

        // ---- Scrolling (GP27/ADC1) ----
        adc_select_input(1);
        raw = adc_read();  

        centered = (int16_t)raw - 2048;

        // Deadzone to prevent POTENTIAL stick drift
        if (centered > -10 && centered < 10) 
        {
            scroll = 0;
        } 
        else 
        {
            scaled = centered / 1500;  // linear scrolling w/ no acceleration

            // Clamp
            if (scaled > 1)  scaled = 1;
            if (scaled < -1) scaled = -1;

            scroll = (int8_t)scaled;
        }

        // ---- sending device reports ----
        if (tud_hid_ready())
    {
        absolute_time_t now = get_absolute_time();

        if (scroll != 0)
        {
            if (absolute_time_diff_us(last_scroll_time, now) > SCROLL_INTERVAL_US)
            {
                tud_hid_mouse_report(0, buttons, 0, 0, scroll, 0);
                last_scroll_time = now;
            }
        }
        else
        {
            // Always send zero to stop scrolling immediately
            tud_hid_mouse_report(0, buttons, 0, 0, 0, 0);
        }
    }

        gpio_put(LED_PIN, leftclick_pressed);
    }
}