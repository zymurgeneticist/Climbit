/*********************************************************************
This sketch is for CLimbit v1, a fitness tracker for climbers.
Please see the full build at Instructables http://www.instructables.com/id/Climbit/
Below is the standard text from Adafruit for the components used:

This uses libraries and code for Adafruit's Monochrome OLEDs 
based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution

This also used library and code for Adafruit's BMP280 humidity,
temperature & pressure sensor

  Designed specifically to work with the Adafruit BMEP280 Breakout 
  ----> http://www.adafruit.com/products/2651

These sensors use I2C or SPI to communicate, 2 or 4 pins are required 
to interface.

Adafruit invests time and resources providing this open source code,
please support Adafruit andopen-source hardware by purchasing products
from Adafruit!

Written by Limor Fried & Kevin Townsend for Adafruit Industries.  
BSD license, all text above must be included in any redistribution
*********************************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bme; // Use I2C for the sensor

#define OLED_RESET 16 // (A2) Define the display reset pin

const int buttonPin = 15; // (A1) Define the button pin, remember a pulldown resistor of sufficient resistance (I used a 51k)

Adafruit_SSD1306 display(OLED_RESET);

// Smoothing altitude variables
float a0 = 0.0;
float a1 = 0.0;
float a2 = 0.0;
float a3 = 0.0;
float a4 = 0.0;
float a5 = 0.0;
float a6 = 0.0;
float a7 = 0.0;
float a8 = 0.0;
float a9 = 0.0;
// End smoothing altitude variables
float asens = 0.0; // Variable for raw sensor altitude before averaging
float anew = 0.0; // Variable for new smoothed a0:a9
float aold = 0.0; // Variable for old smoothed a0:a9
float cal = 960; // Altitude to use as a reference alt
float base = 0.0; // Lowest altitude
float base1 = 0.0; // Holding variable for new Alt reading compared to base
float total = 0.0; // Total amount climbed
int buttonState = 0; // Variable to read the button state
int lastButton = 0; // Variable for the last button state
unsigned long buttonTime = 0; // Variable for time the button is depressed
unsigned long Time = 0; // Variable for time since program started
unsigned long dispTime = 0; // Variable for time since display started

void setup()   {                
  
  if (!bme.begin()) {  
    while (1);
  }

  // Initialize the buttonPin as an input
  pinMode(buttonPin, INPUT);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  
  // Clear the buffer.
  display.clearDisplay();

   // Display Climbit Start Screen
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("ClimbIt");
  display.display();
  delay(2000);

  base = bme.readAltitude(cal); // set the base altitude upon power on

}


void loop() {
// Check if at new base level
base1 = bme.readAltitude(cal);
if((base - base1) > 1)
{
  base = base1;
}

buttonState = digitalRead(buttonPin); // Read the button

if(buttonState == 0 && lastButton == 1) dispTime = millis(); // Set dispTime as current time if button has just been let go
if(buttonState == 1 && lastButton == 0) Time = millis(); // Set the Time variable to know when the button was pressed

if(buttonTime >= 5000) // If the button has been depressed for over 5 sec (each loop takes ~500ms)
{
  a0 = 0.0; a1 = 0.0; a2 = 0.0; a3 = 0.0; a4 = 0.0; a5 = 0.0; a6 = 0.0; a7 = 0.0; a8 = 0.0; a9 = 0.0; // Reset all altitude variables
  asens = 0.0; anew = 0.0; aold = 0.0; base = 0.0; base1 = 0.0; total = 0.0; // Reset the other altitude variables
  // Display the startup screen again
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("ClimbIt");
  display.display();
  delay(2000);
}

// Assign new values for the smoothing altitudes
a9 = a8; a8 = a7; a7 = a6; a6 = a5; a5 = a4; a4 = a3; a3 = a2; a2 = a1; a1 = a0;

// Take three interspaced readings from the sensor to get an average a0
asens = 0;
asens += bme.readAltitude(cal);
delay(150);
asens += bme.readAltitude(cal);
delay(150);
asens += bme.readAltitude(cal);
asens /= 3;

a0 = (asens - base); // take new average altitude reading for a0
anew = ((a0 + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9) / 10.0); // New anew

if(anew > aold && asens >= base+1) // if the new smoothed altitude is more than before
{
  total += (anew - aold); // add any positive difference to the total
  aold = anew; // set anew as the next aold value
}
else // if there was no positive change in smoothed altitude
{
  aold = anew;  // set anew as the next aold value
}
if(buttonState == 1 || dispTime <= millis() - 300 && dispTime >= millis() - 5300){  // Display the total on the OLED display

  if(buttonState == 1)  buttonTime = millis() - Time; // Track the time the button is depressed
  else{
    buttonTime = 0; // Reset the buttonTime
    Time = millis();
  }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("Total:   "); display.print(((bme.readTemperature()*1.8)+32),0); display.print((char)223);display.println("F");
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.print((total*3.28),0); display.println(" ft");
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print((total),0); display.print(" m   ");display.print(bme.readTemperature(),1); display.print((char)223);display.println("C");
    display.display();
   
}
else{
  display.clearDisplay();
  display.display();
  Time = millis(); // Reset the Time variable
  buttonTime = 0; // Reset buttonTime
  if(millis() > dispTime + 5000) dispTime = 0; // Reset dispTime
}

  lastButton = buttonState; // Set lastButton

}


