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
const int salaryButtonPin = 9;     // the number of the pushbutton pin
const int weekButtonPin = 12;     // the number of the pushbutton pin
const int weekEndButtonPin = 13;     // the number of the pushbutton pin

int dayType=1;
int fingerId;

int arrPreh[30]={0};
int arrPrem[30]={0};
int arrPres[30]={0};
int regCount[30]={0};
int salary[30]={0};
int time_sec = 0;

void setup() {
  
  pinMode(enrollButtonPin, INPUT);
  pinMode(okButtonPin, INPUT);
  pinMode(addButtonPin, INPUT);
  pinMode(subButtonPin, INPUT);
  pinMode(salaryButtonPin, INPUT);
  pinMode(weekButtonPin, INPUT);
  pinMode(weekEndButtonPin, INPUT);

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

showDayType();  //Created function for show the Day type

int  enrollButtonState = 0;
enrollButtonState = digitalRead(enrollButtonPin);
if (enrollButtonState == HIGH) {
     
 getFingerprintEnroll();
  
  }

int salaryButtonState = 0; 
salaryButtonState = digitalRead(salaryButtonPin);
if (salaryButtonState == HIGH) {
     showSalary();
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
  
  fingerId = finger.fingerID;

  getFingerprintCal();
  delay(1000);
  int ok2ButtonState = 0;
  while(ok2ButtonState == 0)
    {
      ok2ButtonState = digitalRead(okButtonPin);
      int salaryButtonState = 0;
      salaryButtonState = digitalRead(salaryButtonPin);
      if(salaryButtonState == 1)
      {
      lcd.clear();
      lcd.print("Salary : ");
      lcd.print(salary[fingerId-1]);
      delay(1000);
      }
    }
  
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

void showDayType(){
  int weekButtonState = 0;
  int weekEndButtonState = 0;
  weekButtonState = digitalRead(weekButtonPin);
  weekEndButtonState = digitalRead(weekEndButtonPin);
  lcd.clear();

  if(weekButtonState==HIGH){
    dayType=1;
  }
  if(weekEndButtonState==HIGH){
    dayType=2;
  }
  if(dayType==1){
    lcd.print("Week Day");
  }
  if(dayType==2){
    lcd.print("Weekend Day");
  }
}

void getFingerprintCal() {

  int finId = fingerId-1;
  lcd.clear();
  // found a match!
  lcd.print("ID:"); lcd.print(fingerId); 
  lcd.setCursor(0,1);
  
  int seconds = myRTC.seconds;
  int minutes = myRTC.minutes;
  int hours = myRTC.hours;

  int regHour = 0;
  int regMin = 0;
  int regSec = 0;

  regCount[finId]++;
  if((regCount[finId]%2)==1){
    regHour=hours;
    regMin=minutes;
    regSec =seconds;

    arrPreh[finId]=regHour;
    arrPrem[finId]=regMin;
    arrPres[finId]=regSec;

    time_sec =0;
  }
  else if((regCount[finId]%2)==0){
    regHour=hours;
    regMin=minutes;
    regSec=seconds;

    int ts,tm,th;
    if(arrPres[finId]<=regSec)
    {
      ts=regSec-arrPres[finId];
    }
    else
    {
      ts=regSec+60-arrPres[finId];
      regMin--;
    }

    if(arrPrem[finId]<=regMin)
    {
      tm=regMin-arrPrem[finId];
    }
    else
    {
      tm=regMin+60-arrPrem[finId];
      regHour--;
    }
     th = regHour-arrPreh[finId];
     time_sec =ts+(tm*60)+(th*60*60);
  }
  
  if(dayType==1)
  {
    salary[finId] = salary[finId] + (time_sec*5);
  }
  if(dayType==2)
  {
    salary[finId] = salary[finId] + (time_sec*7);
  }
  
  lcd.print(hours); lcd.print(":"); lcd.print(minutes); lcd.print(":"); lcd.print(seconds);
  Serial.print("ID:"); Serial.print(finger.fingerID);
  Serial.print("  ");
  Serial.print(hours);Serial.print(":");Serial.print(minutes);
  Serial.print(":");Serial.println(seconds);
  Serial.print("Total Salary : ");
  Serial.println(salary[finId]);
 
}

void showSalary()
{
  int i;
  for(i=1;i<=30;i++)
  {
    Serial.print("ID : ");
    Serial.print(i);
    Serial.print("\t");
    Serial.println(salary[i-1]);
  }

}
