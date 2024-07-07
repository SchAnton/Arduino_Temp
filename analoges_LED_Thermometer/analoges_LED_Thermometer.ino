// analoges_LED_Thermometer.ino
//
// Abgeleitet von Beispielprogramm
// Copyright (c) 2018 Gregor Christandl
// Anton Schwarzott 2022-2024
//
// Verbinden BMP280 und Neopixel
//
// Arduino - BMP280 / BME280
// 3.3V ---- VCC
// GND ----- GND
// SDA ----- SDA
// SCL ----- SCL

// Arduino - Neopixel
// Pin 6 --- DIn
// 5V ------ +5V
// GND ----- GND
#include <Arduino.h>
#include <Wire.h>
#include <BMx280I2C.h>
#include <Adafruit_NeoPixel.h>

#define I2C_ADDRESS 0x76

//create a BMx280I2C object using the I2C interface with I2C Address 0x76
BMx280I2C bmx280(I2C_ADDRESS);

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 60

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void setup() {
  // put your setup code here, to run once:
	Serial.begin(9600);

	//wait for serial connection to open (only necessary on some boards)
	while (!Serial);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(20); // Set BRIGHTNESS to about 1/5 (max = 255)

	Wire.begin();

	//begin() checks the Interface, reads the sensor ID (to differentiate between BMP280 and BME280)
	//and reads compensation parameters.
	if (!bmx280.begin())
	{
		Serial.println("begin() failed. check your BMx280 Interface and I2C Address.");
		while (1);
	}

	if (bmx280.isBME280())
		Serial.println("sensor is a BME280");
	else
		Serial.println("sensor is a BMP280");

	//reset sensor to default parameters.
	bmx280.resetToDefaults();

	//by default sensing is disabled and must be enabled by setting a non-zero
	//oversampling setting.
	//set an oversampling setting for temperature measurements. 
	bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
}

void drawBar(uint32_t color, int num) {
  for(int i=0; i<num; i++) {
    strip.setPixelColor(i, color);
  }
}

int maxTempLed = 0;
int minTempLed = 100;

void loop() {
	delay(1000);

	//starte Messung
	if (!bmx280.measure())
	{
		Serial.println("could not start measurement, is a measurement already running?");
		return;
	}

	//warte auf die Messung
	do
	{
		delay(100);
	} while (!bmx280.hasValue());

  float temp = bmx280.getTemperature();
	Serial.print("Temperature: "); Serial.println(temp);

  int lednum = (temp * 2) + 0.5;
  if (minTempLed > lednum)
  {
    minTempLed = lednum;
  }
  if (maxTempLed < lednum)
  {
    maxTempLed = lednum;
  }
  Serial.print("LEDs: "); 
  Serial.println(lednum);
    
  strip.clear(); // alle LED aus (im speicher)
  drawBar(strip.Color(50, 50, 50), lednum);
  strip.setPixelColor(minTempLed, strip.Color(0, 0, 255));
  strip.setPixelColor(maxTempLed, strip.Color(255, 0, 0));
  strip.setPixelColor(lednum, strip.Color(0, 255, 0));
  strip.show(); // sende farben an LEDs
}
