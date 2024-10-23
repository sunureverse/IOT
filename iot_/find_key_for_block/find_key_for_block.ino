#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9    // Configurable, see typical pin layout above
#define SS_PIN 10    // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

byte uidByte_prev[10];
int keyCounter = 0;

/*
 * Initialize.
 */
void setup() {
    Serial.begin(9600);           // Initialize serial communications with the PC
    while (!Serial)
        ;                        // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();                  // Init SPI bus
    mfrc522.PCD_Init();           // Init MFRC522 card
    mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

    Serial.println(F("Try different keys to print block 0-63 of a MIFARE Classic 1k."));
}

/*
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/*
 * Try using the PICC (the tag/card) with the given key to access the specified block.
 * On success, it will show the key details, and dump the block data on Serial.
 *
 * @return true when the given key worked, false otherwise.
 */
boolean try_key(MFRC522::MIFARE_Key *key, byte block) {
    boolean result = false;
    byte buffer[18];
    MFRC522::StatusCode status;

    Serial.print(F("  -> Authenticating using key A for block "));
    Serial.print(block);
    Serial.print(F("...  "));
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return false;
    }

    // Read block
    byte byteCount = sizeof(buffer);
    status = mfrc522.MIFARE_Read(block, buffer, &byteCount);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    } else {
        // Successful read
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

    mfrc522.PICC_HaltA();       // Halt PICC
    mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
    return result;
}

/*
 * Main loop.
 */
void loop() {
    // Look for new cards
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

    // key값 대입하기
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
