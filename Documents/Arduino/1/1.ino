#include <Wire.h>
#include <RTClib.h>
#include <SD.h>

RTC_DS3231 rtc;
const int chipSelect = 4;
const int pin = 7;

void setup() {
  Serial.begin(9600);
  // Inicializace RTC a SD karty
  Wire.begin();
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(pin, OUTPUT);
//  if (!SD.begin(chipSelect)) {
//    Serial.println("Chyba při inicializaci SD karty");
//    return;
//  }
}

void loop() {
  // Získání aktuálního času
  DateTime now = rtc.now();
  Serial.print(now.hour());
  Serial.print(":");
  Serial.println(now.minute());
  // Pokud je 13:00, začneme vypínat pin 7
  if (now.hour() == 13 && now.minute() == 0) {
    int repeat = 0;
    switch (now.dayOfTheWeek()) {
      case 1:
        repeat = 1000;
        break;
      case 2:
        repeat = 2000;
        break;
      case 3:
        repeat = 4000;
        break;
      case 4:
        repeat = 0;
        break;
      case 5:
        repeat = 8000;
        break;
      case 6:
        repeat = 10000;
        break;
      case 7:
        repeat = 5000;
        break;
    }

    // Vypínání pinu s 100ms zpožděním
    for (int i = 0; i < repeat; i++) {
      digitalWrite(pin, HIGH);
      delay(100);
      digitalWrite(pin, LOW);
      delay(100);
    }
  }
}
