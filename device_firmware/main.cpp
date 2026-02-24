// libraries
#include <stdio.h>
#include "pico/stdlib.h" 
#include "tusb.h"
#include <map>

// DEFINES
#define LED_PIN 25     // built-in LED on board
#define BUTTON1_PIN 13 // left click button
#define BUTTON2_PIN 15 // right click button
#define LEFTCLICK_MASK 1

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
 * function allows user to press button to perform mouse left click
 */
void send_left_click() {
    if (tud_hid_ready()) {
        // Use 0 if you don't have a descriptor defining a specific Report ID yet
        tud_hid_mouse_report(
            0, // report id
            LEFTCLICK_MASK, // button mask 
            0,  // x movement
            0,  // y movement
            0,  // vertical scroll
            0   // horizontal scroll
        );
        sleep_ms(10); 
        tud_hid_mouse_report(0, 0, 0, 0, 0, 0); // send left click released flag
    }
}

/**
 * main
 */
int main()
{
    // vars 
    static bool clicked = false;
    static absolute_time_t click_time;
    bool leftclick_pressed;
    bool rightclick_pressed;
    uint8_t buttons;

    // ------ Inits ------
    stdio_init_all(); // init serial comms + board
    tusb_init(); // init tinyusb

    // LED
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

    // main device loop to run
    while (true)
    {
        tud_task(); // must be called frequently as this is the USB hid task

        leftclick_pressed = !gpio_get(BUTTON1_PIN);
        rightclick_pressed = !gpio_get(BUTTON2_PIN);

        buttons = 0;
        if (leftclick_pressed)  buttons |= 0x01;
        if (rightclick_pressed) buttons |= 0x02;

        if (tud_hid_ready())
        {
            tud_hid_mouse_report(0, buttons, 0, 0, 0, 0);
        }

        gpio_put(LED_PIN, leftclick_pressed);

        sleep_ms(1); // VERY SHORT BUFFER
    }
}