#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

String tags[9] = {
  " b3 b6 c5 13", //mormon
  " 63 f9 cc 13", //condom
  " 37 b0 3d b3", //kaoliang
  " b7 4a 26 b3", //ballet
  " 03 1d c9 13", //needle
  " dc 07 57 23", //shot glass
  " 17 8a 34 b3", //softie
  " 6a 1f 9e 7f", //traveling w/ mum
  " 53 81 c2 13", //uncle earl
};
uint8_t currentTag = 0;
long long cooldownSave = 0;
long long cooldownDur = 1300;

void setup() {
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}

void loop() {
  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent()){
    if (mfrc522.PICC_ReadCardSerial()){
      //Show UID on serial monitor
      String content= "";
      for (byte i = 0; i < mfrc522.uid.size; i++){
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      // Serial.print("UID tag :");
      // Serial.println(content);

      if (millis() - cooldownSave > cooldownDur || content.compareTo(tags[currentTag]) != 0) {
        for (int i=0; i<9; i++){
          if (content.compareTo(tags[i]) == 0){
            currentTag = i;
            // Serial.print("New tag!! UID = ");
            // Serial.println(tags[currentTag]);
            Serial.write(currentTag);
            break;
          }
        }
      }
      cooldownSave = millis();
    }
    // else Serial.println("Can't read card");
  }
  // else Serial.println("No card present");
} 
