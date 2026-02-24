#include "tusb.h"

/* Device Descriptor */
tusb_desc_device_t const desc_device =
{
  .bLength            = sizeof(tusb_desc_device_t),
  .bDescriptorType    = TUSB_DESC_DEVICE,
  .bcdUSB             = 0x0200,

  .bDeviceClass       = 0x00,
  .bDeviceSubClass    = 0x00,
  .bDeviceProtocol    = 0x00,

  .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

  .idVendor           = 0xCafe,
  .idProduct          = 0x4000,
  .bcdDevice          = 0x0100,

  .iManufacturer      = 0x01,
  .iProduct           = 0x02,
  .iSerialNumber      = 0x03,

  .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &desc_device;
}

/* HID Report Descriptor */
uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_MOUSE()
};

/* Configuration Descriptor */
#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

uint8_t const desc_configuration[] =
{
  TUD_CONFIG_DESCRIPTOR(1, 1, 0, CONFIG_TOTAL_LEN, 0x00, 100),

  TUD_HID_DESCRIPTOR(
    0, // report ID
    0, 
    HID_ITF_PROTOCOL_MOUSE,
    sizeof(desc_hid_report),
    0x81, 
    16, 
    10
  )
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  return desc_configuration;
}

/* HID Descriptor Callback */
uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
{
  return desc_hid_report;
}

// String descriptors

static const char* string_desc_arr[] = {
    (const char[]){0x09, 0x04}, // 0: English (0x0409)
    "Bryan",                    // 1: Manufacturer
    "Pico Mouse",               // 2: Product
    "123456",                   // 3: Serial
};

static uint16_t _desc_str[32];

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void) langid;

    uint8_t chr_count;

    if (index == 0)
    {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    }
    else
    {
        if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) )
            return NULL;

        const char* str = string_desc_arr[index];
        chr_count = strlen(str);

        for(uint8_t i=0; i<chr_count; i++)
        {
            _desc_str[1+i] = str[i];
        }
    }

    _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

    return _desc_str;
}