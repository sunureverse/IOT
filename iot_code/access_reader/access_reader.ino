#include <SPI.h>
#include <Wire.h> 
#include "LiquidCrystal_I2C.h"
#include <MFRC522.h>
                    //부저 핀 선언
#define REDLED 5                                //3색 LED 핀 선언
#define YELLOWLED 6
#define BLUELED 7
#define RST_PIN 9                  //SPI 리셋 핀 설정
#define SS_PIN 10                  //SPI 선택 핀 설정

MFRC522 mfrc522(SS_PIN, RST_PIN);               //MFRC522 인스턴스 생성
LiquidCrystal_I2C lcd(0x27,16,2);
byte buff[18];                                  //바이트 저장 변수 
byte buff_size = sizeof(buff);                  //사이즈 크기 변수


void setup() {
  pinMode(REDLED,OUTPUT);                       //3색 LED 출력핀 설정
  pinMode(YELLOWLED,OUTPUT);
  pinMode(BLUELED,OUTPUT);                      //부져 출력핀 설정
  Serial.begin(9600);                           //시리얼 통신 시작
  SPI.begin();                                  //SPI 버스 시작
  mfrc522.PCD_Init();                           //MFRC522 시작
  mfrc522.PCD_DumpVersionToSerial();            //PCD의 세부정보 표시 - MFRC522 카드 세부정보 표시
  Serial.println(F("RFID Hacking Ready"));
  lcd.init();                                   //LCD 초기화 
  lcd.backlight();                              //LCD 백라이트 출력
  lcd.print("RFID Hacking DEF");               //LCD 글자 표시
}

void print_buffer(byte buffer[])                //버퍼에 있는 16바이트를 시리얼 모니터에 표시
{
  Serial.print(String("buffer[] = {"));
  for(int i = 0; i < 16; i++){
    if(i != 0)
    Serial.print(',');
    Serial.print(buffer[i]);
  }
  Serial.print("}");
}

void loop(){

  String content= "";

  if ( !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial() ) {
    delay(500);
    return;
  } 
  Serial.print("Card UID:"); // 태그의 UID출력
  for (byte i = 0; i < 4; i++) { // 태그의 ID출력하는 반복문.태그의 ID사이즈(4)까지
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" "); 
  }
  Serial.println(); 

  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();

   if(content.substring(1) == "F4 E2 50 89"){  //RFID Key 확인
     lcd.clear();                               //LCD 표기된 글자 지우기
     digitalWrite(REDLED,LOW);                  //빨간색 LED 끄기
     digitalWrite(BLUELED,HIGH);                //파란색 LED 켜기
     lcd.print("Access");                       //LCD 글자 표기
   }
   else                                         //아닌 경우
   {
     lcd.clear();                               //LCD 표기된 글자 지우기
     digitalWrite(REDLED,HIGH);                 //빨간색 LED 켜기
     digitalWrite(BLUELED,LOW);                 //파란색 LED 끄기
     lcd.print("Non Access");                   //LCD 글자 표기
   }

   delay(2000);
}