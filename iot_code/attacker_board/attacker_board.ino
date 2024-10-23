#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

SoftwareSerial uart(0, 1);  // RX, TX

void setup() {
  Serial.begin(9600);
  uart.begin(9600);  // 소프트웨어 시리얼 초기화
  Serial.println("Enabling RFID");

  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("Enabled RFID");
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("UID Tag: ");
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    Serial.println(content);

    // UID를 UART로 전송
    uart.println(content);

    // RFID 카드 인식 상태 초기화
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(1000);  // 1초 대기
  }
}