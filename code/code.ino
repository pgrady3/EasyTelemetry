#include <i2c_t3.h>
#include <SD.h>
#include "Adafruit_GPS.h"
#include "LiquidCrystal_I2C.h"
#include "INA.h"

#define LED1 21
#define LED2 22

#define HALL 4
#define SD_CS 8

#define WHEEL_CIRC 1.552
#define WHEEL_TICKS 9
#define TICK_DIST (WHEEL_CIRC / WHEEL_TICKS)

#define TARGET_CURRENT 5

volatile uint32_t tickTimes[WHEEL_TICKS];
volatile uint32_t tickPos;

volatile uint32_t loopTime = 0;
volatile uint32_t lastHallPulse = 0;
volatile uint32_t lastInaMeasurement = 0;
volatile uint32_t countIntervals = 0;
volatile int32_t avgdT = 1000000;
volatile uint32_t distTicks = 0;

double energyUsed = 0.0;
double distance = 0.0;
double currentSpeed = 0.0;
double currentRPM = 0.0;
double InaVoltage = 0.0;
double InaCurrent = 0.0;
double InaPower = 0;
double batteryVoltage = 0.0;
double effScore = 0;

File myFile;
Adafruit_GPS GPS(&Serial1);
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 400000);
  INAinit();

  Serial.begin(115200);
  SD.begin(SD_CS);
  lcd.begin();
  lcd.backlight();
  lcd.print("Hello, world!");

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);

  pinMode(HALL, INPUT_PULLUP);
  attachInterrupt(HALL, countHallPulse, FALLING);

  myFile = SD.open("data.txt", FILE_WRITE);
  
  GPSInit();
}

void loop() {  
  GPSPoll();//must be called rapidly
  
  uint32_t curTime = millis();
  if(curTime < loopTime + 100)//if less than 100ms, start over
    return;

  digitalWrite(LED2, !digitalRead(LED2));
  loopTime = curTime;
  
  updateINA();
  updateSpeed();

  writeToBtSd();
  /*char b = GPS.read();
  if(b)
    Serial.print(b);*/
}

void updateINA()
{
  InaVoltage = INAvoltage();
  InaCurrent = INAcurrent();
  InaPower = InaVoltage * InaCurrent;
  
  double currentInaTime = millis();
  energyUsed += InaPower * (currentInaTime - lastInaMeasurement) / 1000;
  lastInaMeasurement = currentInaTime;

  effScore = distance / energyUsed * 3600;
}

void updateSpeed()
{
  currentSpeed = 1000000.0 / avgdT * WHEEL_CIRC; 
  if(micros() - lastHallPulse > 2000000)//if nothing for more than 2 seconds, set speed to zero
    currentSpeed = 0;
  
  distance = distTicks * TICK_DIST;
}

void countHallPulse() {
  uint32_t current = micros();

  uint32_t prevTime = tickTimes[tickPos];// time 1 rev ago. fixed 7/5/2018 :(

  tickTimes[tickPos++] = current;
  tickPos %= WHEEL_TICKS;

  avgdT = current - prevTime;

  distTicks++;
  
  lastHallPulse = current;

  digitalWrite(LED1, (distTicks) & 1);
}

const unsigned char UBLOX_INIT[] = {
  0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12,//(10Hz)
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2B, // GxGLL off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39, // GxGSV off
  0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x47, // GxVTG off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x24, // GxGGA off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x32, // GxGSA off
  //0xb5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xd0, 0x08, 0x00, 0x00, 0x00, 0xc2, 0x01, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x96, 0xb5, 0x62, 0x06, 0x00, 0x01, //baud rate 115200
  0xB5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xD0,0x08,0x00,0x00,0x00,0xE1,0x00,0x00,0x07,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0xDE,0xC9,//baud rate 57600
};

void GPSInit()
{
  GPS.begin(9600);
  for (uint32_t i = 0; i < sizeof(UBLOX_INIT); i++) {
    Serial1.write(UBLOX_INIT[i]);
  }
  
  delay(50);
  Serial1.end();
  
  delay(50);
  GPS.begin(57600);
}

void GPSPoll()
{
  while(GPS.read());
  
  if (GPS.newNMEAreceived())
    GPS.parse(GPS.lastNMEA());
}

void writeToBtSd() {
  String outputStr = String(InaVoltage, 3) + " " + String(InaCurrent, 3) + " " + String(InaPower) + " "+ String(currentSpeed) + " " +
                     String(energyUsed) + " " + String(distance) + " " + String(millis()) + " " + String(GPS.latitudeDegrees, 7) + 
                     " " + String(GPS.longitudeDegrees, 7);
  
  Serial.println(outputStr);//usb 
  //Serial.println(GPS.milliseconds); 
  GPSPoll();//super hacky bc short GPS buffer
  
  myFile.println(outputStr);
  myFile.flush();//need this to make sure we actually write to card and don't buffer

  GPSPoll();//super hacky bc short GPS buffer

  lcd.clear();
  LCDWriteFloat(0, 0, InaVoltage, " V");
  LCDWriteFloat(0, 1, InaCurrent, " A");  
  LCDWriteInt(0, 3, GPS.milliseconds, " ms");

  LCDWriteFloat(10, 0, currentSpeed * 3.6, " km/h");
  LCDWriteFloat(10, 1, InaPower, " W");
  LCDWriteFloat(8, 3, effScore, " km/kWh");
}


void LCDWriteFloat(uint8_t x, uint8_t y, float data, const char* s2)
{
  lcd.setCursor(x, y);
  lcd.print(data);
  lcd.print(s2);
  GPSPoll();
}

void LCDWriteInt(uint8_t x, uint8_t y, uint32_t data, const char* s2)
{
  lcd.setCursor(x, y);
  lcd.print(data);
  lcd.print(s2);
  GPSPoll();
}

