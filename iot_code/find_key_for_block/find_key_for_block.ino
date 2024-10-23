#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9    
#define SS_PIN 10    

MFRC522 mfrc522(SS_PIN, RST_PIN);  

byte uidByte_prev[10];
int keyCounter = 0;

void setup() {
    Serial.begin(9600);           
    while (!Serial)
        ;                        
    SPI.begin();                
    mfrc522.PCD_Init();     
    mfrc522.PCD_DumpVersionToSerial(); 

    Serial.println(F("mifare classic 1k에 시도합니다."));
}

void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

boolean try_key(MFRC522::MIFARE_Key *key, byte block) {
    boolean result = false;
    byte buffer[18];
    MFRC522::StatusCode status;

    Serial.print(F("  -> key A에 진행합니다. "));
    Serial.print(block);
    Serial.print(F("...  "));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return false;
    }

    byte byteCount = sizeof(buffer);
    status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    } else {
        // 성공 시
        result = true;
        Serial.print(F("Success with key:"));
        dump_byte_array((*key).keyByte, MFRC522::MF_KEY_SIZE);
        Serial.println();

        // Dump block data
        Serial.print(F("Block ")); Serial.print(block); Serial.print(F(":"));
        dump_byte_array(buffer, 16);
        Serial.println();
    }
    Serial.println();
    //초기화
    mfrc522.PICC_HaltA();       
    mfrc522.PCD_StopCrypto1();  
    return result;
}


void loop() {
    //카드 인식 할 때만 작동
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }

    if (uidByte_prev[0] != mfrc522.uid.uidByte[0]) {
        keyCounter = 0;

        uidByte_prev[0] = mfrc522.uid.uidByte[0];

        Serial.print(F("\n\nCard UID:"));
        dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
        Serial.println();

        Serial.print(F("PICC type: "));
        MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
        Serial.println(mfrc522.PICC_GetTypeName(piccType));
        Serial.println("\n\n");
    }

    // key값 대입하기(00 00 00 00 00 00 부터 FF FF FF FF FF FF)
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) {
        key.keyByte[i] = (keyCounter >> (i * 8)) & 0xFF;
    }

    Serial.print("Trying key: ");
    dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);

    // 29번 블록에 대해서만 시도
    try_key(&key, 29);

    delay(0000);  // 1초 대기
    keyCounter++;
}
