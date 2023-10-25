#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>

#define SS_PIN_RFID 10
#define RST_PIN_RFID 9 
#define GREEN_LED 8
#define RED_LED 7
#define LED_INDICATOR_RED 6
#define LED_INDICATOR_GREEN 5
#define RTC_OUT 4
#define SS_PIN_SD 3
#define SOLENOID_LOCK 2
#define ACCESS_LIST "Access.csv"
#define ACCESS_LOG "Log.csv"

MFRC522 mfrc522;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init(SS_PIN_RFID,RST_PIN_RFID);
  Serial.println("Reader In :");
  mfrc522.PCD_DumpVersionToSerial();
  Serial.setTimeout(50);

  pinMode(GREEN_LED,OUTPUT);
  pinMode(RED_LED,OUTPUT);
  pinMode(LED_INDICATOR_RED,OUTPUT);
  pinMode(LED_INDICATOR_GREEN,OUTPUT);
  pinMode(RTC_OUT,OUTPUT);
  pinMode(SOLENOID_LOCK,OUTPUT);

  while(!SD.begin(SS_PIN_SD)) {
    Serial.println("SD card initialization failed...");
    digitalWrite(LED_INDICATOR_GREEN, HIGH);
    digitalWrite(LED_INDICATOR_RED, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    delay(1000);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED,HIGH);
    delay(1000);
    digitalWrite(RED_LED,LOW);
  }
  digitalWrite(LED_INDICATOR_GREEN, LOW);
  digitalWrite(LED_INDICATOR_RED, LOW);
}

void loop() {
  while(!SD.begin(SS_PIN_SD)) {
    Serial.println("SD card initialization failed...");
    digitalWrite(LED_INDICATOR_GREEN, HIGH);
    digitalWrite(LED_INDICATOR_RED, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    delay(1000);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED,HIGH);
    delay(1000);
    digitalWrite(RED_LED,LOW);
  }
  digitalWrite(LED_INDICATOR_GREEN, LOW);
  digitalWrite(LED_INDICATOR_RED, LOW);

  if(mfrc522.PICC_IsNewCardPresent()) {
    String cardUID = readRFID();
    if(validity(cardUID)) {
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(SOLENOID_LOCK, HIGH);
      delay(3000);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(SOLENOID_LOCK, LOW);

      String time;
      digitalWrite(RTC_OUT, HIGH);
      while(Serial.available() == 0);
      time = Serial.readString();
      time.trim();
      log(cardUID,time,"Exit");
    }
    else {
      digitalWrite(RED_LED,HIGH);
      delay(1000);
      digitalWrite(RED_LED,LOW);
    }
  }
  else if(Serial.available()) {
    String cardUID;
    cardUID = Serial.readString();
    cardUID.trim();
    if(validity(cardUID)) {
      digitalWrite(LED_INDICATOR_GREEN, HIGH);
      digitalWrite(SOLENOID_LOCK, HIGH);
      delay(3000);
      digitalWrite(LED_INDICATOR_GREEN, LOW);
      digitalWrite(SOLENOID_LOCK, LOW);
      

      String time;
      digitalWrite(RTC_OUT, HIGH);
      while(Serial.available() == 0);
      time = Serial.readString();
      time.trim();
      log(cardUID,time,"Entry");
    }
    else {
      digitalWrite(LED_INDICATOR_RED, HIGH);
      delay(1000);
      digitalWrite(LED_INDICATOR_RED, LOW);
    }
  }
}

String readRFID() {
  String cardUID ="";
  mfrc522.PICC_ReadCardSerial();
  for(byte i=0; i<mfrc522.uid.size; i++) {
    cardUID.concat(String(mfrc522.uid.uidByte[i]<0x10?"0":""));
    cardUID.concat(String(mfrc522.uid.uidByte[i],HEX));
  }
  return cardUID;
}

bool validity(const String& cardUID) {
  File file = SD.open(ACCESS_LIST,FILE_READ);
  if(file) {
    while(file.available()) {
      String line = file.readStringUntil('\n');
      String validCardID = line.substring(0,line.indexOf(','));
      if(cardUID.equals(validCardID)) { 
        file.close();
        return true;
      }
    }
    file.close();
  }
  else {
    Serial.println("Error opening valid cards file...");
  }
  return false;
}

void log(const String& cardUID,const String& time,const String& action) {
  File file = SD.open(ACCESS_LIST, FILE_READ);
  String finalName = "";
  if (file)
  {
    while (file.available()) {
      String line = file.readStringUntil('\n');
      String validCardID = line.substring(0, line.indexOf(','));
      String name = line.substring(line.indexOf(',') + 1);
      if (validCardID.equals(cardUID)) {
        file.close();
        finalName = name;
        break;
      }
    }
  }

  if(!SD.exists(ACCESS_LOG)) {
    File file = SD.open(ACCESS_LOG, FILE_WRITE);
    if (file) {
      file.println("RFID Tag,Name,Date & Time,Action");
    }
    else {
      Serial.println("Error creating file");
    }
  }
  else {
    File file = SD.open(ACCESS_LOG, FILE_WRITE);
  }
  String record = cardUID + "," + finalName + "," + time + "," + action;
  file.print(record);
  file.println();
  file.close();
}