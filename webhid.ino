#include "HID-Project.h"
#include <ByteConvert.hpp>

const int pinLed = LED_BUILTIN;
const int pinButton = 9;
const int pinCommand = 10;

#define MAX_BUFFER 255
#define USB_PACKET_SIZE 64

static char hex[] = "0123456789ABCDEF";
uint8_t rawData[MAX_BUFFER];
uint8_t receivedRawData[MAX_BUFFER];
uint8_t buffer[MAX_BUFFER];
uint8_t index = 0;
char input[3];

bool to_hex(char* dest, size_t dest_len, const uint8_t* values, size_t val_len) {
  if (dest_len < (val_len*2+1)) {
    return false;
  }

  *dest = '\0';
  while(val_len--) {
    sprintf(dest, "%02X", *values);
    dest += 2;
    ++values;
  }
  return true;
}

void setup() {
  pinMode(pinLed, OUTPUT);
  pinMode(pinCommand, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);
  digitalWrite(pinCommand, LOW);
  Serial.begin(9600);
  RawHID.begin(rawData, sizeof(rawData));
}

void loop() {
  if (digitalRead(pinButton)) {
    digitalWrite(pinLed, HIGH);
    for (uint8_t i = 0; i < USB_PACKET_SIZE; i++) {
      buffer[i] = ~receivedRawData[i];
    }
    RawHID.write(buffer, USB_PACKET_SIZE);
    delay(300);
    digitalWrite(pinLed, LOW);
    digitalWrite(pinCommand, LOW);
  }

  auto bytesAvailable = RawHID.available();
  if (bytesAvailable) {
    digitalWrite(pinLed, HIGH);
    index = 0;

    while (bytesAvailable--) {
      receivedRawData[index] = RawHID.read();
      to_hex(input, 3, &receivedRawData[index], 1);
      Serial.print(index);
      Serial.print(" > ");
      Serial.println(input);
      index++;
    }
    digitalWrite(pinLed, LOW);
  }
}
