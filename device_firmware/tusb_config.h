#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

// The Pico SDK defines some of these via command line, 
// so we wrap them in #ifndef to avoid redefinition warnings.
#ifndef CFG_TUSB_MCU
#define CFG_TUSB_MCU                OPT_MCU_RP2040
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS                 OPT_OS_PICO
#endif

// This tells TinyUSB to use the Pico's native USB port in Device mode
#define CFG_TUSB_RHPORT0_MODE       OPT_MODE_DEVICE

#define CFG_TUD_ENABLED             1
#define CFG_TUD_CDC  1   // enables serial (printf)
#define CFG_TUD_CDC_RX_BUFSIZE  64
#define CFG_TUD_CDC_TX_BUFSIZE  64
#define CFG_TUD_HID                 1
#define CFG_TUD_HID_EP_BUFSIZE      64

#endif