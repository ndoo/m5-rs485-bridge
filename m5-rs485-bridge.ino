
// Board Type
// #define BOARD_M5STICKC
#define BOARD_M5ATOM

#include "config.h"
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>

bool rgb[3];

#ifdef BOARD_M5ATOM
Adafruit_NeoPixel strip(25, 27, NEO_GRB + NEO_KHZ800);
#endif

HardwareSerial rs485(1);
WiFiServer server(RS485_PORT);
WiFiClient serverClients[RS485_CLIENTS];

void setup() {

  strip.begin();
  Serial2.begin(RS485_BAUD, SERIAL_8N1, RS485_RXD, RS485_TXD);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    rgb[0] = !rgb[0];
    statusled();
    delay(200);
  }
  statusled(false, true, false);

  server.begin();
  server.setNoDelay(true);

}

void loop() {

  uint8_t i;

  if (WiFi.status() == !WL_CONNECTED) {
    for (i = 0; i < RS485_CLIENTS; i++) {
      if (serverClients[i]) serverClients[i].stop();
    }
    delay(200);
    statusled(!rgb[0], false, false);
    return;
  } else {
    rgb[1] = LED_ON;
  }


  //check if there are any new clients
  if (server.hasClient()) {
    for (i = 0; i < RS485_CLIENTS; i++) {
      if (!serverClients[i] || !serverClients[i].connected()) {
        if (serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        break;
      }
    }
    if (i >= RS485_CLIENTS) {
      //no free/disconnected spot so reject
      server.available().stop();
    }
  }

  //check clients for data
  for (i = 0; i < RS485_CLIENTS; i++) {
    if (serverClients[i] && serverClients[i].connected()) {
      if (serverClients[i].available()) {
        //get data from the telnet client and push it to the UART
        while (serverClients[i].available()) Serial2.write(serverClients[i].read());
        rgb[0] = !rgb[0];
      }
    }
    else {
      if (serverClients[i]) {
        serverClients[i].stop();
      }
    }
  }

  //check UART for data
  if (Serial2.available()) {
    size_t len = Serial2.available();
    uint8_t sbuf[len];
    Serial2.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    for (i = 0; i < RS485_CLIENTS; i++) {
      if (serverClients[i] && serverClients[i].connected()) {
        serverClients[i].write(sbuf, len);
      }
    }
    rgb[2] = !rgb[2];
  }

  statusled();
}

void statusled() {
#ifdef BOARD_M5ATOM
  strip.fill(strip.Color(rgb[0] ? LED_ON : LED_OFF,
                         rgb[1] ? LED_ON : LED_OFF,
                         rgb[2] ? LED_ON : LED_OFF));
  strip.show();
#endif
}

void statusled(bool r, bool g, bool b) {
  rgb[0] = r;
  rgb[1] = g;
  rgb[2] = b;
  statusled();
}
