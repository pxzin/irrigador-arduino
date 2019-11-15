#include <Wire.h>
#include "RTClib.h"

//PINS:
const int pumpPin = 9;
const int rpin = 12;
const int gpin = 11;
const int bpin = 10;
const int buttonPin = 2; 
const int wetPin = 0;

const bool canPump = false;
const int pumpUntil = 10000;
const int loopDelay = 100;

const int dryTrashold = 700;
const int wetTrashold = 500;


bool isPumping = false;
bool isWet = false;
bool isInitialized = false;
bool isTimeToPump = false;

int hourToPump = -1;
int minutesToPump = -1;
int pumpRemaning = 0;
int buttonState = 0;
int wetState = 0;

RTC_DS1307 rtc;


void ledRed(){
  digitalWrite(rpin, 1);
  digitalWrite(gpin, 0);
  digitalWrite(bpin, 0);
}
void ledBlue(){
  digitalWrite(rpin, 0);
  digitalWrite(gpin, 0);
  digitalWrite(bpin, 1);
}
void ledGreen(){
  digitalWrite(rpin, 0);
  digitalWrite(gpin, 1);
  digitalWrite(bpin, 0);
}
void ledYellow(){
  digitalWrite(rpin, 0);
  digitalWrite(gpin, 1);
  digitalWrite(bpin, 1);
}
void ledOff(){
  digitalWrite(rpin, 0);
  digitalWrite(gpin, 0);
  digitalWrite(bpin, 0);
}
void ledAll(){
  digitalWrite(rpin, 1);
  digitalWrite(gpin, 1);
  digitalWrite(bpin, 1);
}

void startPump(){
    if(!isPumping){
      ledBlue();
      digitalWrite(pumpPin, 0);
      isPumping = true;
    }
    
}

void stopPump(){
    if(isPumping){
      ledOff();
      digitalWrite(pumpPin, 1);
      isPumping = false;
    }
}
void checkPump(int vdelay){
  
  if ((buttonState == LOW || isTimeToPump) && pumpRemaning == 0 && !isWet) {
    ledAll();
    pumpRemaning = pumpUntil;
    isTimeToPump = false;
    startPump();
    delay(300);
  }else{if (buttonState == LOW && pumpRemaning > 0){
    pumpRemaning = 0;
    stopPump();
    delay(300);
  }}

  if(pumpRemaning > 0){
    startPump();
    pumpRemaning = pumpRemaning - vdelay;
  }else{
    stopPump();
    pumpRemaning = 0;
  }
}

void checkWet(){
  if(!isPumping){
    if(wetState <= wetTrashold){
      ledRed();
      isWet = true;
    }else if(wetState >= dryTrashold){
      ledGreen();
      isWet = false;
    }else{
      ledYellow();
      isWet = false;
    }
  }
  
}

void checkIsTimeToPump(int hour, int minutes, int seconds){
  if(hour == hourToPump && minutes == minutesToPump && seconds < 2 && !isTimeToPump && !isPumping){
    isTimeToPump = true;
  }else if(hour != hourToPump || minutes != minutesToPump){
    isTimeToPump = false;
  }
}

/////////////////////////////////
void setup() {
  // prepare pins:
  pinMode(rpin, OUTPUT);
  pinMode(gpin, OUTPUT);
  pinMode(bpin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  digitalWrite(pumpPin, HIGH);
  digitalWrite(buttonPin, HIGH);
  
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));    
  }
}

void loop() {
   
DateTime now = rtc.now();
   
   //initialize delay
   if(isInitialized){

    

    
    buttonState = digitalRead(buttonPin);
    wetState = analogRead(wetPin);
    checkWet();
    checkIsTimeToPump(now.hour(),now.minute(),now.second());
    checkPump(loopDelay);
    
    // Serial.print("Minute now ");
    // Serial.print(now.minute());
    // Serial.print(" - Minute to fire ");
    // Serial.println(minutesToPump);
    // Serial.print("Pumping ");
    // Serial.println(isPumping);
    // Serial.print("Time to Pump ");
    // Serial.println(isTimeToPump);
    delay(loopDelay);
   }else{
     hourToPump = now.hour();
     minutesToPump = now.minute()+1;
    delay(5000);
    isInitialized = true;
   }
   
}
