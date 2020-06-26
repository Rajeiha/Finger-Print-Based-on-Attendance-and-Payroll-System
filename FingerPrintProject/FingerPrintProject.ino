#include <Adafruit_Fingerprint.h>    // Finger Print Sensor Module Library
#include <virtuabotixRTC.h>          //RTC Library
#include <Wire.h>                    //LCD_i2c Library
#include <LCD.h>                     //LCD Library
#include <LiquidCrystal_I2C.h>       //LCD_i2c Library

#define I2C_ADDR 0x27      //LCD i2c stuff
#define BACKLIGHT_PIN 3
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7

LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;         // for Finger print sensor module

virtuabotixRTC myRTC(6, 7, 8); //If you change the wiring change the pins here also     

// CLK -> 6 , DAT -> 7, Reset -> 8 For RTC Module

const int enrollButtonPin = 4;  // the number of the pushbutton pin
const int okButtonPin = 5;       // the number of the pushbutton pin
const int addButtonPin = 10;     // the number of the pushbutton pin
const int subButtonPin = 11;     // the number of the pushbutton pin

void setup() {
  
  pinMode(enrollButtonPin, INPUT);
  pinMode(okButtonPin, INPUT);
  pinMode(addButtonPin, INPUT);
  pinMode(subButtonPin, INPUT);

  Serial.begin(9600);
  
  lcd.begin (16,2);
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home ();

//Enroll SetUp
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

//RTC Setup
myRTC.setDS1302Time(15, 22, 21, 7, 14, 1, 2018);

}

void loop() {

showDateTime(); //Created function for show the date and time

int  enrollButtonState = 0;
enrollButtonState = digitalRead(enrollButtonPin);
if (enrollButtonState == HIGH) {
     
 getFingerprintEnroll();
  
  }
  
//GetId
getFingerprintIDez();
  
delay(1000);
}



//Enroll function 
uint8_t getFingerprintEnroll() {

    lcd.clear();
    lcd.print("Enroll On");
    delay(2000);

    int  okButtonState = 0;
    int  addButtonState = 0;
    int  subButtonState = 0; 
    int idCount = 1;
    while(okButtonState == 0)
    {
      lcd.clear();
      lcd.print("Type ID:");lcd.print(idCount);
      addButtonState = digitalRead(addButtonPin);
      subButtonState = digitalRead(subButtonPin);
      if((addButtonState == 1)&&(idCount<30))
      {
        idCount++;
      }
      if((subButtonState == 1)&&(idCount>1))
      {
        idCount--;
      }
      okButtonState = digitalRead(okButtonPin);
      delay(1000);
    }
    
    id = idCount;
    if (id == 0) {// ID #0 not allowed, try again!
     return;
    }
  
  int p = -1;
  lcd.clear();
  lcd.print("finger for"); lcd.print(id);lcd.print(":");
  delay(1500);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
    lcd.clear();
      lcd.print("Taken");
      delay(600);
      break;
    case FINGERPRINT_NOFINGER:
      lcd.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      lcd.print("Imaging error");
      break;
    default:
      lcd.print("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
    lcd.clear();
      lcd.print("Img converted");
      delay(100);
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.print("Image too messy");
      delay(100);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      delay(100);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.print("Could not find fingerprint features");
      delay(100);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.print("Could not find fingerprint features");
      delay(100);
      return p;
    default:
      lcd.print("Unknown error");
      delay(100);
      return p;
  }
  lcd.clear();
  lcd.print("Remove finger");
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  lcd.clear();
  lcd.print("Place again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
    lcd.clear();
      lcd.print("Taken");
      delay(500);
      break;
    case FINGERPRINT_NOFINGER:
      lcd.print(".");
      delay(100);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      delay(100);
      break;
    case FINGERPRINT_IMAGEFAIL:
      lcd.print("Imaging error");
      delay(100);
      break;
    default:
      lcd.print("Unknown error");
      delay(100);
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Img converted");
      delay(1000);
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.print("Image too messy");
      delay(100);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      delay(100);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.print("Could not find fingerprint features");
      delay(100);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.println("Could not find fingerprint features");
      delay(100);
      return p;
    default:
      lcd.print("Unknown error");
      delay(100);
      return p;
  }
  
  // OK converted!
  
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    lcd.clear();
    lcd.print("Prints matched!");
    delay(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    lcd.clear();
    lcd.print("Communication error");
    delay(100);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    lcd.clear();
    lcd.print("Did not match");
    delay(1000);
    return p;
  } else {
    lcd.clear();
    lcd.print("Unknown error");
    delay(100);
    return p;
  } 
  
  
  
   p = finger.storeModel(id);
  switch (p){
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("ID Stored!");
      delay(1000);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication error");
      return p;
    case FINGERPRINT_BADLOCATION:
      lcd.print("Could not store in that location");
      return p;
    case FINGERPRINT_FLASHERR:
      lcd.print("Error writing to flash");
      return p;
    default:
      lcd.print("Unknown error");
      return p;
  } 
}

//GetId function

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  lcd.clear();
  // found a match!
  lcd.print("ID:"); lcd.print(finger.fingerID); 
  lcd.setCursor(0,1);
  lcd.print(myRTC.hours);
  lcd.print(":");
  lcd.print(myRTC.minutes);
  lcd.print(":");
  lcd.print(myRTC.seconds);
  
  return finger.fingerID; 

  delay(4000);
}

void showDateTime(){
 lcd.clear(); //Clean the LCD
 myRTC.updateTime();
 lcd.setCursor(0,0);
 lcd.print(myRTC.dayofmonth);
 lcd.print("/");
 lcd.print(myRTC.month);
 lcd.print("/");
 lcd.print(myRTC.year);
 lcd.setCursor(0,1);
 lcd.print(myRTC.hours);
 lcd.print(":");
 lcd.print(myRTC.minutes);
 lcd.print(":");
 lcd.print(myRTC.seconds);
 delay(600);  
}
