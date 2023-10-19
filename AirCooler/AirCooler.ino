#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DHTPIN 12     // what pin we're connected to

#define DHTTYPE DHT11   // DHT 11 

#define pwm 9

long tempMin = 32;
long tempMax = 38;
long alarming = 50;

long fanLCD;
byte degree[8] = 
              {
                0b00011,
                0b00011,
                0b00000,
                0b00000,
                0b00000,
                0b00000,
                0b00000,
                0b00000
              };
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

void setup() {
 lcd.init();
 lcd.backlight();
 lcd.createChar(1, degree);
 lcd.clear();
lcd.print("Temp Controlled");
 lcd.setCursor(0,1);
 lcd.print("    Cooler");
 delay(2000);

 lcd.clear();
 
  delay(500);
  Serial.begin(9600); 
  dht.begin();
}

void loop() {
  int fanSpeed;
  // Wait a few seconds between measurements.
  delay(1000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in t in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");

  lcd.setCursor(0,0);
  lcd.print("temp: ");
  lcd.print(t);   // Printing terature on LCD
  lcd.print(" C");
  

  if(int(t) < tempMin)
  {
    lcd.setCursor(0,1);
    analogWrite(pwm,0);
    lcd.print("FAN is OFF");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Humidity: ");
    lcd.print((int)h);
    lcd.print("%");
    delay(200);
  }

  else if(t >= tempMin && t <= tempMax)
  {
    
    lcd.setCursor(0,1);
    double minsp = 30, maxsp = 100;
    double unit = (maxsp-minsp)/(tempMax-tempMin);
    double fanLCD = 30+(t-tempMin)*unit;

    double factor = map(t,tempMin,tempMax,30,100);
    double fanSpeed=(factor/100)*255;
    // double fanLCD = map(fanSpeed,3*25.5,255,30,100);
    analogWrite(pwm, fanSpeed);
    lcd.print("Fan speed: ");
    lcd.print(int(fanLCD));
    lcd.print(" %");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Humidity: ");
    lcd.print((int)h);
    lcd.print("%");
    delay(200);
  }

  else if(int(t) > tempMax)
  {

  
    lcd.setCursor(0,1);
    analogWrite(pwm, 255);
    lcd.print("Fan speed: ");
    lcd.print("100");
    lcd.print("%");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Humidity: ");
    lcd.print((int)h);
    lcd.print("%");
    delay(200);
  }
  

  delay(1000);
}