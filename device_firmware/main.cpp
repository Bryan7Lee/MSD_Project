// ---- Libraries ----
#include <stdio.h>
#include "pico/stdlib.h" 
#include "hardware/adc.h"
#include "tusb.h"
#include <map>

// ---- DEFINES ----
#define LED_PIN 25     // built-in LED on board
#define SS_BUTTON_PIN 14 // scrolling speed cycle button
#define LC_BUTTON_PIN 13 // left click button
#define RC_BUTTON_PIN 15 // right click button
#define LEFTCLICK_MASK 1 
#define RIGHTCLICK_MASK 2

// ---- Structs and whatnot ----
// Current button mappings (example: map to keyboard keys)
std::map<int, uint8_t> button_mapping = {
    {LC_BUTTON_PIN, HID_KEY_T},   // default: T key
    {RC_BUTTON_PIN, HID_KEY_Y}   // default: Y key
};

struct ScrollSpeed
{
    uint32_t interval_us;          // how often scroll ticks send
    uint16_t sensitivity_divider;  // joystick scaling 
};

// 3 default profiles
ScrollSpeed scrollSpeeds[3] = {
    {8000, 2000},   // Profile 0 (fast)
    {15000, 2500},  // Profile 1 (medium)
    {30000, 3000}   // Profile 2 (slow)
};

uint8_t activeScrollSpeed = 0;

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
    const uint32_t SCROLL_INTERVAL_X = 100000; // 100ms
    bool leftclick_pressed;
    bool rightclick_pressed;
    uint8_t buttons;
    uint16_t rawX;
    uint16_t rawY;
    int16_t centeredX;
    int16_t centeredY;
    int16_t scaledX;
    int16_t scaledY;
    int8_t scrollX;
    int8_t scrollY;

    // ------ Inits ------
    stdio_init_all(); // init serial comms + board
    tusb_init(); // init tinyusb
    adc_init(); // init adc for joystick readings

    // LED (for testing purposes)
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // On board buttons
    gpio_init(SS_BUTTON_PIN);
    gpio_set_dir(SS_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(SS_BUTTON_PIN);

    gpio_init(LC_BUTTON_PIN);
    gpio_set_dir(LC_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(LC_BUTTON_PIN);

    gpio_init(RC_BUTTON_PIN); 
    gpio_set_dir(RC_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(RC_BUTTON_PIN);

    // Macro Buttons

    // ------ MAIN LOOP ------
    while (true)
    {
        tud_task(); // must be called frequently as this is the USB hid task

        // ---- Button Press ----
        leftclick_pressed = !gpio_get(LC_BUTTON_PIN);
        rightclick_pressed = !gpio_get(RC_BUTTON_PIN);

        buttons = 0;

        if (leftclick_pressed)  buttons |= 0x01;
        if (rightclick_pressed) buttons |= 0x02;

        // ---- Read Scrolling ----
        // read horizontal (X axis) scrolling (pin GP26/ADC0)
        adc_select_input(0);
        rawX = adc_read();  
        centeredX = (int16_t)rawX - 2048;

        // read vertical (Y axis) scrolling (pin GP27/ADC1)
        adc_select_input(1);
        rawY = adc_read();  
        centeredY = (int16_t)rawY - 2048;

        // Deadzone to prevent POTENTIAL stick drift
        if (centeredY > -10 && centeredY < 10) 
        {
            scrollY = 0;
        } 
        else 
        {
            scaledY = centeredY / 1500;  // linear scrolling w/ no acceleration

            // Clamp
            if (scaledY > 1)  scaledY = 1;
            if (scaledY < -1) scaledY = -1;

            scrollY = (int8_t)scaledY;
        }

        // Deadzone to prevent POTENTIAL stick drift
        if (centeredX > -10 && centeredX < 10) 
        {
            scrollX = 0;
        } 
        else 
        {
            scaledX = centeredX / 1500;  // linear scrolling w/ no acceleration

            // Clamp
            if (scaledX > 1)  scaledX = 1;
            if (scaledX < -1) scaledX = -1;

            scrollX = -(int8_t)scaledX;
        }

        // ---- sending device reports ----
        if (tud_hid_ready())
        {
        absolute_time_t now = get_absolute_time();

            if (scrollX != 0 || scrollY != 0)
            {
                if (absolute_time_diff_us(last_scroll_time, now) > SCROLL_INTERVAL_US)
                {
                    tud_hid_mouse_report(0, buttons, 0, 0, scrollY, scrollX);
                    last_scroll_time = now;
                }
            }
            else
            {
                // Always send zero to stop scrolling immediately
                tud_hid_mouse_report(0, buttons, 0, 0, 0, 0);
            }
        }
    }
}