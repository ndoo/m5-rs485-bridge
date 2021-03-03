// Wi-Fi Settings

#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASS "YOUR_PASS"

// M5 Atom LED brightness

#define LED_ON  32
#define LED_OFF 0

// General telnet and RS-485 parameters

#define RS485_BAUD    4800
#define RS485_PORT    8485
#define RS485_CLIENTS 2

// Board-specific defines, do not change unless you need to support new boards

#ifdef BOARD_M5STICKC
#define RS485_RXD     26
#define RS485_TXD     0
#endif

#ifdef BOARD_M5ATOM
#define RS485_RXD     32
#define RS485_TXD     26
#endif
