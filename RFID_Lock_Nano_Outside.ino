#include <SPI.h>
#include <MFRC522.h>
#include<RTClib.h>

#define SS_PIN_RFID 10
#define RST_PIN_RFID 9
#define GREEN_LED 8
#define RED_LED 7
#define LED_INDICATOR_RED 6
#define LED_INDICATOR_GREEN 5
#define RTC_IN 4

MFRC522 mfrc522;
RTC_DS1307 rtc;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init(SS_PIN_RFID,RST_PIN_RFID);
  Serial.println("Reader Out :");
  mfrc522.PCD_DumpVersionToSerial();

  pinMode(GREEN_LED,OUTPUT);
  pinMode(RED_LED,OUTPUT);
  pinMode(LED_INDICATOR_RED,INPUT);
  pinMode(LED_INDICATOR_GREEN,INPUT);
  pinMode(RTC_IN,INPUT);

  while(!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

}

void loop() {
  if(mfrc522.PICC_IsNewCardPresent())
  {
    String cardUID = readRFID();
    Serial.write(cardUID);
    delay(200);
  }

  if(LED_INDICATOR_GREEN == HIGH && LED_INDICATOR_RED == LOW)
  {
    digitalWrite(GREEN_LED, HIGH);
    while(LED_INDICATOR_GREEN == HIGH && LED_INDICATOR_RED == LOW);
    digitalWrite(GREEN_LED, LOW);
  }
  else if(LED_INDICATOR_GREEN == LOW && LED_INDICATOR_RED == HIGH)
  {
    digitalWrite(RED_LED,HIGH);
    while(LED_INDICATOR_RED == HIGH && LED_INDICATOR_GREEN == LOW);
    digitalWrite(RED_LED,LOW);
  }

  if(LED_INDICATOR_GREEN == HIGH && LED_INDICATOR_RED == HIGH)
  {
    while(LED_INDICATOR_GREEN == HIGH && LED_INDICATOR_RED == HIGH)
    {
      digitalWrite(GREEN_LED, HIGH);
      delay(1000);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED,HIGH);
      delay(1000);
      digitalWrite(RED_LED,LOW);
    }
  }

  if(RTC_IN == HIGH)
  {
    String time;
    DateTime now = rtc.now();
    time = String(now.day(), DEC) + '/' + String(now.month(), DEC) + '/' + String(now.year(), DEC) + " " + String(now.hour(), DEC) + ':' + String(now.minute(), DEC) + ':' + String(now.second(), DEC);
    Serial.write(time);
  }
}

String readRFID() {
  String cardUID ="";
  mfrc522.PICC_ReadCardSerial();
  for(byte i=0; i<mfrc522.uid.size; i++)
  {
    cardUID.concat(String(mfrc522.uid.uidByte[i]<0x10?"0":""));
    cardUID.concat(String(mfrc522.uid.uidByte[i],HEX));
  }
  return cardUID;
}
