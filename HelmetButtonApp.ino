/*
 Name:		HelmetButtonApp.ino
 Created:	10/27/2022 9:20:22 AM
 Author:	Imami Joel Betofe
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include<Arduino.h>
#include "BluetoothSerial.h" 

// this header is needed for Bluetooth Serial -> works ONLY on ESP32
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150   // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX 450   // This is the 'maximum' pulse length count (out of 4096)
#define USMIN 600      // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX 2400     // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz updates

uint8_t servonum = 0;
int buttonPin = 14;
int ledPin = 13;
int buttonState = 0;
int globalPos = 1;
unsigned long previousMillis = 0;
unsigned long interval = 5000;
int a = 60;
// init Class:
BluetoothSerial ESP_BT;
// Parameters for Bluetooth interface
int incoming;


const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

int mainServoL = 0;
int mainServoR = 1;
int mainServoClosed = 5;
int mainServoOpen = 90;
int servoBrowCenter = 4;
int servoBrowCenterOpen = 40;
int servoBrowCenterClosed = 120;
int animDelay = 1;

void setup() {
	Serial.begin(19200);
	ESP_BT.begin("ESP32_Control"); //Name of your Bluetooth interface -> will show up on your phone


	Serial.begin(9600);

	Serial.println("Boot Up");
	pwm.begin();
	pwm.setOscillatorFrequency(27000000);
	pwm.setPWMFreq(SERVO_FREQ);
	pinMode(buttonPin, INPUT_PULLUP);
	delay(50);
	pwm.sleep();
	ledcSetup(ledChannel, freq, resolution);

	ledcAttachPin(ledPin, ledChannel);


}

int getAngleToPulse(int angle) {
	return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}
int getAngleToMilli(int angle) {
	return map(angle, 0, 180, USMIN, USMAX);
}

void loop() {

	if (ESP_BT.available())
	{
		incoming = ESP_BT.read(); //Read what we receive 

		// separate button ID from button value -> button ID is 10, 20, 30, etc, value is 1 or 0
		int button = floor(incoming / 10);
		int value = incoming % 10;

		switch (button) {
		case 1:
			if (value == 0) {
				Serial.println("Wake up");
				pwm.wakeup();

				if (globalPos > 0) {
					Serial.println("Opening");
					//CHEEKS
					for (uint16_t pulselen = 20; pulselen <= 90; pulselen++) {
						pwm.setPWM(9, 0, getAngleToPulse(90 + 20 - pulselen));
						pwm.setPWM(8, 0, getAngleToPulse(pulselen));
					}
					delay(animDelay);
					
					// NOSE Side
					for (uint16_t pulselen = 10; pulselen <= 86; pulselen++) {
						pwm.setPWM(6, 0, getAngleToPulse(86 + 10 - pulselen));
						pwm.setPWM(7, 0, getAngleToPulse(pulselen));
					}
					delay(animDelay);

					// Brown Center

					for (uint16_t pulselen = servoBrowCenterOpen; pulselen <= servoBrowCenterClosed; pulselen++) {
						pwm.setPWM(servoBrowCenter, 0, getAngleToPulse(pulselen));
					}
					delay(animDelay);

					// BROW Side
					for (uint16_t pulselen = 90; pulselen >= 30; pulselen--) {
						pwm.setPWM(2, 0, getAngleToPulse(pulselen));
						pwm.setPWM(3, 0, getAngleToPulse(30 + 90 - pulselen));
					}
					delay(animDelay);

					delay(animDelay);
					Serial.println("4. Brow Side Open");
					//NOSE Center
					for (uint16_t pulselen = 1; pulselen <= 110; pulselen++) {
						pwm.setPWM(5, 0, getAngleToPulse(pulselen));
					}
					delay(animDelay);
					// EYES
					ledcWrite(ledChannel, 0);
					// MAIN SERVOS
					for (uint16_t microsec = 750; microsec < 1950; microsec += 5) {
						pwm.writeMicroseconds(mainServoL, microsec);
						pwm.writeMicroseconds(mainServoR, (1950 + 750 - microsec));
					}
					globalPos = 0;
					delay(animDelay);
					Serial.println("6. Main Open");
					//delay(100);
					//Serial.println("Sleep");
					//pwm.sleep();
				}
			}
			else if (value == 1) {
				Serial.println("Closing");
				//MAIN SERVOS
				for (uint16_t microsec = 1950; microsec > 750; microsec -= 5) {
					pwm.writeMicroseconds(mainServoL, microsec);
					pwm.writeMicroseconds(mainServoR, (1950 + 750 - microsec));
				}
				delay(animDelay);
				// NOSE Center
				for (uint16_t pulselen = 110; pulselen >= 1; pulselen--) {
					pwm.setPWM(5, 0, getAngleToPulse(pulselen));
				}
				delay(animDelay);
				Serial.println("5. Nose Center Open");
				// NOSE Side
				for (uint16_t pulselen = 86; pulselen >= 10; pulselen--) {
					pwm.setPWM(6, 0, getAngleToPulse(86 + 10 - pulselen));
					pwm.setPWM(7, 0, getAngleToPulse(pulselen));
				}
				delay(animDelay);
				Serial.println("2. Nose Side Open");

				// BROW Side
				for (uint16_t pulselen = 30; pulselen <= 90; pulselen++) {
					pwm.setPWM(2, 0, getAngleToPulse(pulselen));
					pwm.setPWM(3, 0, getAngleToPulse(30 + 90 - pulselen));
				}
				//BROW Center
				for (uint16_t pulselen = servoBrowCenterClosed; pulselen >= servoBrowCenterOpen; pulselen--) {
					pwm.setPWM(servoBrowCenter, 0, getAngleToPulse(pulselen));
				}
				delay(animDelay);
				Serial.println("3. Brow Center Open");
				// CHEEKS
				for (uint16_t pulselen = 90; pulselen >= 20; pulselen--) {
					pwm.setPWM(9, 0, getAngleToPulse(90 + 20 - pulselen));
					pwm.setPWM(8, 0, getAngleToPulse(pulselen));
				}
				delay(animDelay);
				Serial.println("1. Cheeks Open");
				//EYES
				ledcWrite(ledChannel, 255);
				globalPos = 1;
				delay(100);
				Serial.println("Sleep");
				pwm.sleep();

			}
			delay(500);
			break;
		default:
			break;
		}

	}
	buttonState = digitalRead(buttonPin);
	if (buttonState == 0) {
		Serial.println("Wake up");
		pwm.wakeup();

		if (globalPos > 0) {
			Serial.println("Opening");
			//CHEEKS
			for (uint16_t pulselen = 20; pulselen <= 90; pulselen++) {
				pwm.setPWM(9, 0, getAngleToPulse(90 + 20 - pulselen));
				pwm.setPWM(8, 0, getAngleToPulse(pulselen));
			}
			delay(animDelay);
			
			// NOSE Side
			for (uint16_t pulselen = 10; pulselen <= 86; pulselen++) {
				pwm.setPWM(6, 0, getAngleToPulse(86 + 10 - pulselen));
				pwm.setPWM(7, 0, getAngleToPulse(pulselen));
			}
			delay(animDelay);

			// Brown Center
			for (uint16_t pulselen = servoBrowCenterOpen; pulselen <= servoBrowCenterClosed; pulselen++) {
				pwm.setPWM(servoBrowCenter, 0, getAngleToPulse(pulselen));
			}
			delay(animDelay);

			// BROW Side
			for (uint16_t pulselen = 90; pulselen >= 30; pulselen--) {
				pwm.setPWM(2, 0, getAngleToPulse(pulselen));
				pwm.setPWM(3, 0, getAngleToPulse(30 + 90 - pulselen));
			}
			delay(animDelay);

			delay(animDelay);
			Serial.println("4. Brow Side Open");
			//NOSE Center
			for (uint16_t pulselen = 1; pulselen <= 110; pulselen++) {
				pwm.setPWM(5, 0, getAngleToPulse(pulselen));
			}
			delay(animDelay);
			// EYES
			ledcWrite(ledChannel, 0);
			// MAIN SERVOS
			for (uint16_t microsec = 750; microsec < 1950; microsec += 5) {
				pwm.writeMicroseconds(mainServoL, microsec);
				pwm.writeMicroseconds(mainServoR, (1950 + 750 - microsec));
			}
			globalPos = 0;
			delay(animDelay);
			Serial.println("6. Main Open");
			//delay(100);
			//Serial.println("Sleep");
			//pwm.sleep();
		}
		else {
			Serial.println("Closing");
			//MAIN SERVOS
			for (uint16_t microsec = 1950; microsec > 750; microsec -= 5) {
				pwm.writeMicroseconds(mainServoL, microsec);
				pwm.writeMicroseconds(mainServoR, (1950 + 750 - microsec));
			}
			delay(animDelay);
			// NOSE Center
			for (uint16_t pulselen = 110; pulselen >= 1; pulselen--) {
				pwm.setPWM(5, 0, getAngleToPulse(pulselen));
			}
			delay(animDelay);
			Serial.println("5. Nose Center Open");

			// BROW Side
			for (uint16_t pulselen = 30; pulselen <= 90; pulselen++) {
				pwm.setPWM(2, 0, getAngleToPulse(pulselen));
				pwm.setPWM(3, 0, getAngleToPulse(30 + 90 - pulselen));
			}
			//BROW Center
			for (uint16_t pulselen = servoBrowCenterClosed; pulselen >= servoBrowCenterOpen; pulselen--) {
				pwm.setPWM(servoBrowCenter, 0, getAngleToPulse(pulselen));
			}
			delay(animDelay);
			Serial.println("3. Brow Center Open");
			// NOSE Side
			for (uint16_t pulselen = 86; pulselen >= 10; pulselen--) {
				pwm.setPWM(6, 0, getAngleToPulse(86 + 10 - pulselen));
				pwm.setPWM(7, 0, getAngleToPulse(pulselen));
			}
			delay(animDelay);
			Serial.println("2. Nose Side Open");
			// CHEEKS
			for (uint16_t pulselen = 90; pulselen >= 20; pulselen--) {
				pwm.setPWM(9, 0, getAngleToPulse(90 + 20 - pulselen));
				pwm.setPWM(8, 0, getAngleToPulse(pulselen));
			}
			delay(animDelay);
			Serial.println("1. Cheeks Open");
			
			//EYES
			ledcWrite(ledChannel, 255);
			globalPos = 1;
			delay(100);
			Serial.println("Sleep");
			pwm.sleep();
		}
		delay(500);
	}
	delay(10);
}
