// ---- Libraries ----
#include <stdio.h>
#include "pico/stdlib.h" 
#include "pico/time.h"
#include "hardware/adc.h"
#include "tusb.h"
#include <map>

// ---- DEFINES ---- (FOR THE NEWEST BOARD)
#define SS_BUTTON_PIN 12 // scrolling speed cycle button (GP12)
#define LC_BUTTON_PIN 11 // left click button (GP11)
#define RC_BUTTON_PIN 10 // right click button (GP10)
#define X_AXIS_SCROLL_ADC 0 // gp26(adc0)
#define Y_AXIS_SCROLL_ADC 1 // gp27(adc1)
#define MACRO1_BUTTON_PIN 1 // left most button 
#define MACRO2_BUTTON_PIN 2 
#define MACRO3_BUTTON_PIN 3
#define MACRO4_BUTTON_PIN 4 // right most button
#define LEFTCLICK_MASK 1 
#define RIGHTCLICK_MASK 2

enum {
  REPORT_ID_MOUSE = 1,
  REPORT_ID_KEYBOARD
};

// ---- Structs and whatnot ----
struct Macro
{
    uint8_t modifier; // modifier keys include ctrl, alt, shift, etc.
    uint8_t keys[6];  // HID supports up to 6 simultaneous key presses alongside modifier
};

Macro macros[4] = {
    {KEYBOARD_MODIFIER_LEFTCTRL, {HID_KEY_T, 0}},                 // Ctrl+T
    {KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTSHIFT, {HID_KEY_T, 0}}, // Ctrl+Shift+T
    {0, {HID_KEY_T, 0}},                                          // T
    {KEYBOARD_MODIFIER_LEFTSHIFT, {HID_KEY_A, HID_KEY_B, 0}}       // Shift+A+B
};

struct ScrollSpeed
{
    uint32_t interval_us;          // how often scroll ticks send
    uint16_t sensitivity_divider;  // joystick scaling 
};

// 3 default profiles
ScrollSpeed scrollSpeeds[3] = {
    {200000, 1500}, // Profile 0 (slow)
    {90000, 1500},  // Profile 1 (medium)
    {30000, 1000}   // Profile 2 (fast)
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
 * send_macro function sends the selected macro to HID to perform
 */
void send_macro(const Macro& m)
{
    uint8_t report[8] = {0};

    report[0] = m.modifier;

    for (int i = 0; i < 6; i++)
    {
        report[2 + i] = m.keys[i];
    }

    // press
    tud_hid_report(REPORT_ID_KEYBOARD, report, sizeof(report));
    tud_task();

    // release
    memset(report, 0, sizeof(report));
    tud_hid_report(REPORT_ID_KEYBOARD, report, sizeof(report));
    tud_task();
}

/**
 * main
 */
int main()
{
    // vars 
    static bool clicked = false;
    static absolute_time_t click_time;
    absolute_time_t last_scroll_time_y = get_absolute_time(); // used for debouncing
    absolute_time_t last_scroll_time_x = get_absolute_time(); // used for debouncing
    uint64_t last_profile_press_time = 0; // used for debouncing
    const uint32_t PROFILE_DEBOUNCE_US = 200000; // how long to wait before registering the next button press
    const uint32_t SCROLL_INTERVAL_X_US = 90000; // 90ms (controls how often to scroll horizontally when held)
    bool leftclick_pressed;
    bool rightclick_pressed;
    bool profile_pressed;
    bool profileButtonHandled = false; // flag to prevent 
    uint8_t buttons;
    uint8_t activeScrollSpeed = 0;
    uint16_t rawX;
    uint16_t rawY;
    int16_t centeredX;
    int16_t centeredY;
    int16_t scaledX;
    int16_t scaledY;
    int8_t scrollX;
    int8_t scrollY;
    bool macro_pressed_flag[4] = {false, false, false, false}; // used for edge detecting to prevent spam button 
                                                               // presses when buttons are held past the debouncing time

    // ------ Inits ------
    stdio_init_all(); // init serial comms + board
    tusb_init(); // init tinyusb
    adc_init(); // init adc for joystick readings

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
    gpio_init(MACRO1_BUTTON_PIN); 
    gpio_set_dir(MACRO1_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(MACRO1_BUTTON_PIN);

    gpio_init(MACRO2_BUTTON_PIN); 
    gpio_set_dir(MACRO2_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(MACRO2_BUTTON_PIN);

    gpio_init(MACRO3_BUTTON_PIN); 
    gpio_set_dir(MACRO3_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(MACRO3_BUTTON_PIN);

    gpio_init(MACRO4_BUTTON_PIN); 
    gpio_set_dir(MACRO4_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(MACRO4_BUTTON_PIN);

    // ------ MAIN LOOP ------
    while (true)
    {
        tud_task(); // must be called frequently as this is the USB hid task

        // ---- Profile Button Press ----
        profile_pressed = !gpio_get(SS_BUTTON_PIN);
        uint64_t  now = time_us_64();

        if (profile_pressed && !profileButtonHandled)
        {
            if (now - last_profile_press_time > PROFILE_DEBOUNCE_US)
            {
                printf("PROFILE BUTTON PRESSED \n");
                activeScrollSpeed = (activeScrollSpeed + 1) % 3;
                last_profile_press_time = now;

                profileButtonHandled = true; // set flag
            }
        }

        // reset flag when profile button released
        if (!profile_pressed)
        {
            profileButtonHandled = false;
        }

        // ---- Left/Right Click Button Press ----
        leftclick_pressed = !gpio_get(LC_BUTTON_PIN);
        rightclick_pressed = !gpio_get(RC_BUTTON_PIN);

        buttons = 0;

        if (leftclick_pressed) 
        {
            printf("left click pressed \n");
            buttons |= LEFTCLICK_MASK;
        } 
        if (rightclick_pressed)
        {
            printf("right click pressed \n");
            buttons |= RIGHTCLICK_MASK;
        } 

        // ---- Macro Button Press ----
        // detect if any macro buttons were pressed
        bool macro_pressed[4] = {
            !gpio_get(MACRO1_BUTTON_PIN),
            !gpio_get(MACRO2_BUTTON_PIN),
            !gpio_get(MACRO3_BUTTON_PIN),
            !gpio_get(MACRO4_BUTTON_PIN)
        };

        for (int i=0; i<4; i++) {
            printf("GPIO %d = %d\n", 1+i, !gpio_get(1+i));
        }
        
        for (int i = 0; i < 4; i++)
        {
            if (macro_pressed[i] && !macro_pressed_flag[i])
            {
                printf("MACRO %d PRESSED\n");
                if (tud_hid_ready())
                {
                    send_macro(macros[i]);
                }
            }

            macro_pressed_flag[i] = macro_pressed[i];
        }

        // ---- Read Scrolling ----
        // read horizontal (X axis) scrolling (pin GP26/ADC0)
        adc_select_input(X_AXIS_SCROLL_ADC);
        rawX = adc_read();  
        centeredX = (int16_t)rawX - 2048;

        // read vertical (Y axis) scrolling (pin GP27/ADC1)
        adc_select_input(Y_AXIS_SCROLL_ADC);
        rawY = adc_read();  
        centeredY = (int16_t)rawY - 2048;

        // Deadzone to prevent POTENTIAL stick drift
        if (centeredY > -10 && centeredY < 10) 
        {
            scrollY = 0;
        } 
        else 
        {
            scaledY = centeredY / scrollSpeeds[activeScrollSpeed].sensitivity_divider;  // linear scrolling w/ no acceleration

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
            int8_t linesY = 0;
            int8_t linesX = 0;

            // if vertically scrolling...
            if (scrollY != 0)
            {
                if (absolute_time_diff_us(last_scroll_time_y, now) > scrollSpeeds[activeScrollSpeed].interval_us)
                {
                    linesY = scrollY;
                    last_scroll_time_y = now;
                }
            }
            // if horizontally scrolling...
            if (scrollX != 0)
            {
                if (absolute_time_diff_us(last_scroll_time_x, now) > SCROLL_INTERVAL_X_US)
                {
                    linesX = scrollX;
                    last_scroll_time_x = now;
                }
            }

            static hid_mouse_report_t prev_report = {0};

            hid_mouse_report_t report = {0};
            report.buttons = buttons;
            report.x = 0;
            report.y = 0;
            report.wheel = linesY;
            report.pan = linesX;

            // send only if the report has changed
            if (memcmp(&report, &prev_report, sizeof(report)) != 0)
            {
                tud_hid_report(REPORT_ID_MOUSE, &report, sizeof(report));
                prev_report = report;
            }
        }
    }
}