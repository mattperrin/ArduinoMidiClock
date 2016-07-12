#include <MIDI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include "LiquidCrystal_I2C.h"
#define BACKLIGHT_PIN (3)
#define LED_ADDR (0x27) // might need to be 0x3F, if 0x27 doesn't work
LiquidCrystal_I2C lcd(LED_ADDR, 2, 1, 0, 4, 5, 6, 7, BACKLIGHT_PIN, POSITIVE);

byte midi_start = 0xfa;
byte midi_stop = 0xfc;
byte midi_clock = 0xf8;
byte midi_continue = 0xfb;

unsigned long currentMillis;
unsigned long previousMillis;
unsigned long differenceTiming;

int bpmInterval;
int midiInterval;
int arduinoInterval;
unsigned long beatCounter;
unsigned long subCounter;
int bpmTempo; 

int bpmPotPin = A8;
int ledPin = 13;
int bpmPotValue = 0;

//BPM POT SMOOTHING
const int numReadings = 20;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;  


MIDI_CREATE_DEFAULT_INSTANCE();


void setup() {

  MIDI.begin();
  Serial.begin(115200);
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);
  lcd.begin(20, 4);             // initialize the lcd
  lcd.home();
  lcd.print("MIDI Clock");

  bpmTempo = 60;

  bpmInterval = (60000 / bpmTempo);
  midiInterval = bpmInterval / 24;
  arduinoInterval = bpmInterval / 8;

  pinMode(ledPin, OUTPUT); 

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  
  delay(1000);
}

void loop() {

    //bpmPotValue = (int)analogRead(bpmPotPin);
    //Serial.println(analogRead(bpmPotPin));


    // subtract the last reading:
      total = total - readings[readIndex];
      // read from the sensor:
      readings[readIndex] = analogRead(bpmPotPin);
      // add the reading to the total:
      total = total + readings[readIndex];
      // advance to the next position in the array:
      readIndex = readIndex + 1;
    
      // if we're at the end of the array...
      if (readIndex >= numReadings) {
        // ...wrap around to the beginning:
        readIndex = 0;
      }
    
      // calculate the average:
      average = (total / numReadings);

    //Serial.println((String)average);
    //lcd.setCursor(8,3);
    //lcd.print((String)average);

    int val = map(average, 0, 1023, 30, 300);
    
    Serial.println((String)val);
    lcd.setCursor(8,3);
    lcd.print((String)val);
    

  currentMillis = millis();
  differenceTiming = currentMillis - previousMillis;
  if (differenceTiming >= arduinoInterval)
  {
    
    subCounter += 1;
    lcd.setCursor(1,1);
    lcd.print("TEST  BPM:" + (String)bpmTempo);
    lcd.setCursor(2,2);
    lcd.print((String)subCounter);

    if (subCounter >= 8)
    {
      beatCounter += 1;
      subCounter = 0;
    }
    lcd.setCursor(3,3);
    lcd.print((String)beatCounter);


    
    previousMillis = currentMillis;
  }
}
