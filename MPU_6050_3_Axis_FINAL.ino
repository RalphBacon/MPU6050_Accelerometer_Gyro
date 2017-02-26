#include "Arduino.h"

//Gyro - UNO / Nano
//VCC  -  5V
//GND  -  GND
//SDA  -  A4
//SCL  -  A5
//INT  -  GPIO 2

// I2C library for MPU6050 comms
#include <Wire.h>

// Gyro functions pseudo library
#include "GryoFunctions.h"

// Only interrogate the MPU6060 every few milliseconds
long loop_timer = micros();

// This is the Neopixel library we need
#include <FastLED.h>

// How many LEDs are connected?
#define NUM_LEDS 16

// Define the Pins
#define DATA_PIN 7

// Define the array of LEDs
CRGB leds[NUM_LEDS];

// ---------------------------------------------------------------
// SETUP     SETUP     SETUP     SETUP     SETUP     SETUP
// ---------------------------------------------------------------
void setup() {

	// Fast serial debugging
	Serial.begin(115200);

	// Initiate the I2C buss
	Wire.begin();

	// Configure the MPU6050
	configureMPU6050();

	// At rest, the unit will generate some values which must be subtracted from our readings
	calcGyroOffsets();

	// Tell the FAST LED object how many LEDS we have and which GPIO pin controls them
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS-1);
}

// ---------------------------------------------------------------
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP
// ---------------------------------------------------------------
void loop() {
	static unsigned long lastMillis = millis();

	// Calculate the current angles
	float x = calcGyroAngles();

	// Restrict the sensitivity
	const static float LIMIT = 45.00;
	x = x < LIMIT * -1 ? LIMIT * -1 : x > LIMIT ? LIMIT : x;

	// Debug
	if (millis() > lastMillis + 250) {
		Serial.print(F("True angle: "));
		Serial.println(x);
		lastMillis = millis();
	}

	// Set the number of LEDs to light up
	int ledCount = (map(x *10, -LIMIT * 10 , LIMIT * 10, 1, NUM_LEDS)) - 1;

	//Serial.print("\t");
	//Serial.println(ledCount);

	//														 C
	// Map the centre point (always on) 1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
	// 						   			0  1  2  3  4  5  6  7  8  9  0  1  2  3  4
	unsigned int ledArray[NUM_LEDS] = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 };

	// Tilt to left
	if (ledCount < 7) {
		for (int cnt = 7; cnt >= ledCount; cnt--) {
			ledArray[cnt] = 1;
		}
	}

	// Tilt to right
	if (ledCount > 7) {
		for (int cnt = 8; cnt <= ledCount; cnt++) {
			ledArray[cnt] = 1;
		}
	}

	// Our array now has the LEDs to light eg 5, 6, 7 (left tilt) or 7, 8, 9 (right tilt)
	// with the centre point lighting LEDs 7 green to indicate balance

	// Clear the existing led values (no effect display)
	FastLED.clear();

	// Send out the colours
	for (int ledCnt = 0; ledCnt < NUM_LEDS + 1; ledCnt++) {

		// LED lit?
		if (ledArray[ledCnt] > 0) {

			// Default colour is red (out of balance)
			unsigned long ledColour = CRGB::Red;

			// IF we're outputting the centre led
			if (ledCnt == 7) {

				// And we are in balance
				if (ledArray[6] == 0 && ledArray[8] == 0) {
					ledColour = CRGB::Green;
				}
				else
					ledColour = CRGB::Blue;
			}

			// Set the colour for current LED
			leds[ledCnt] = ledColour;
		}
		else {
			// We're not lighting it up but still have to tell LED what to display
			leds[ledCnt] = CRGB::Black;
		}
	}

	// Brightness ranges from 0 (off) to 100 (watch current consumption!)
	FastLED.setBrightness(60);

	// Update the display
	FastLED.show();


	// Don't interrogate the MPU6050 to often - introduce a small delay here
	while (micros() - loop_timer < 4000);
	loop_timer = micros();
}
