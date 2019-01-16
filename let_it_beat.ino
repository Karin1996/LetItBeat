/*
Karin Kreeft
*/
#define USE_ARDUINO_INTERRUPTS true // Set-up low-level interrupts for most accurate BPM math.
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <PulseSensorPlayground.h> // Includes the PulseSensorPlayground Library.   
#include "pitches.h"

// Vars
const int ledset1 = 13;
const int ledset2 = 12;
const int startBtn = 2;
const int resetBtn = 4;
const int speaker = 11;
const int sensor = 1; // PulseSensor connected to A1
const int reset = 7; //A wire from pin 7 connected to
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); //0x3F should be replaced with your own LCD address

int btnStateStart = 0;
int btnStateReset = 0;

bool start = false;  // If we can start measuring. Will be true when start button has been pressed
int endTime = 0;  // When to end the measuring in the loop()
int BPM = 0; //Beats Per Minute
const int threshold = 526; // Determine which Signal to "count as a beat" and which to ignore.
PulseSensorPlayground pulseSensor; //Creates an instance of the PulseSensorPlayground object called "pulseSensor"

bool soundAndLed = false; //Make the speaker en Led's do their thing
int soundDelay = 0; // Interval between two beats

int melody[] ={
  NOTE_G4, END
};
int noteDurations[] = {
  4
};

int speed = 0; //place soundDelay value in here when calculated

void setup(){
  digitalWrite(reset, HIGH);
  delay(200);
  lcd.clear();
  lcd.begin(16,2); //Set screen "height"
  lcd.setCursor(0,0); //first lcd line
  lcd.print("Finger/earlobe");
  lcd.setCursor(0,1); //second lcd line
  lcd.print("and press start");
  
  pinMode(ledset1, OUTPUT);
  pinMode(ledset2, OUTPUT);
  pinMode(startBtn, INPUT);
  pinMode(resetBtn, INPUT);
  pinMode(reset, OUTPUT); 

  Serial.begin(9600);

  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(sensor);   
  pulseSensor.setThreshold(threshold);  

  if (pulseSensor.begin()) {
    Serial.println("pulseSensor Object created");  //This prints one time at Arduino power-up
  }
  //Make sure the LED's and speaker are off
  digitalWrite(ledset1, LOW);
  digitalWrite(ledset2, LOW);
  noTone(speaker);
}

void loop() {
  btnStateStart = digitalRead(startBtn); //Start monitoring start btn
  if(btnStateStart == HIGH){ //startbtn pressed
    start = true; //we can now start the code for measuring/searching for a heartbeat
    endTime = millis() + (1000 * 15); //15 seconds 
    printLCD("Measuring...");
  }

  btnStateReset = digitalRead(resetBtn); //Start monitoring start btn
  if(btnStateReset == HIGH){ //resetbtn pressed
    printLCD("Resetting...");
    digitalWrite(reset, LOW);
  }

  if(start == true){
    if (pulseSensor.sawStartOfBeat()) { // Constantly test to see if "a beat happened". 
       Serial.print("BPM: ");                        // Print phrase "BPM: " 
       Serial.println(BPM);                        // Print the value inside of myBPM. 
       BPM = pulseSensor.getBeatsPerMinute();
      }
   }

  if (millis() >= endTime && start == true){ //15 seconds passed
    lcd.clear();
    lcd.print("BPM: ");
    lcd.print(String(BPM));
    soundDelay = 60000/BPM; //Milliseconds between two beats. Depends on the BPM
    speed = soundDelay;
    start = false;
    soundAndLed = BPM == 0 ? false : true; //if BPM is 0 soundAndLed will be false otherwise true
   }
   
  if(soundAndLed == true){ //Loop the LED's and sound on the BPM until reset
    int noteDuration = speed*noteDurations[0];
    tone(speaker, melody[0],noteDuration*.95);  
    digitalWrite(ledset1, HIGH);
    digitalWrite(ledset2, HIGH);  
    delay(soundDelay);
    noTone(speaker);
    digitalWrite(ledset1, LOW);
    digitalWrite(ledset2, LOW);
    delay(soundDelay);
  }
}

void printLCD(String msg){
  lcd.clear();
  lcd.print(msg);
}
