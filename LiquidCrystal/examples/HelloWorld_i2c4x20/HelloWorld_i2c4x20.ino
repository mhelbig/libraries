// lcd_I2C_test 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x3F
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin,BACKLIGHT_PIN,POSITIVE);

int loopCount = 0;

void setup()
{
  lcd.begin(20,4);               // initialize the lcd 

  lcd.home ();                   // go home
  lcd.print("Hello, ARDUINO ");  
  lcd.setCursor ( 0, 1 );        // go to the next line
  lcd.print ("     WORLD!  ");      
}

void loop()
{
  lcd.clear();                // clear LCD screen
  lcd.noBacklight();
  delay(200);
  lcd.setCursor ( 3, 0 );        // go to the next line
  lcd.print("Hello, ARDUINO "); 
  lcd.setCursor ( 7, 1 );        // go to the next line
  lcd.backlight();
  lcd.print ("WORLD!  ");      
  delay(500);
lcd.setCursor ( 7, 3 );        // go to the next line
  lcd.print (loopCount++);
  delay(500);  
}
