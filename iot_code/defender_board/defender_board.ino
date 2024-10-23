#include <SoftwareSerial.h>

SoftwareSerial uart(0, 1);  // RX, TX

void setup() {
  Serial.begin(9600);
  uart.begin(9600);  // 소프트웨어 시리얼 초기화
  Serial.println("Ready to receive data");
}

void loop() {
  if (uart.available()) {
    String receivedData = uart.readStringUntil('\n');
    Serial.print("Received UID: ");
    Serial.println(receivedData);
  }
}