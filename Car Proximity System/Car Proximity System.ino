#include <SR04.h>
#include "NarsLibraries.h"

#pragma region Defenitions
#define TRIGGER 0
#define ECHO 1
#define SENSLOW 0.0001
#define SENSHIGH 1
#define SENSCOUNT 4
#define MININTERVAL 5
#define FRONTI 0
#define RIGHTI 0
#define BACKI 0
#define LEFTI 0
#define TESTTIME 1000
#define SAVEINTERVAL 300000
#define SKIPTEST
#define DEBUG
#pragma endregion

#pragma region Typedef
struct SENSORS
{
	const byte front[2] = { 4, 5 },
		right[2] = { 6, 7 },
		back[2] = { 8, 9 },
		left[2] = { 10, 11 };
};

struct PINS
{
	const SENSORS sensors;
	const byte brightnessOutput = 2,
		clicker[4] = { 12, 13, 17, 16 },
		brightnessWiper = A0,
		sensitivity = A1;
};
#pragma endregion

#pragma region Globals
const PINS pins;

unsigned long uptime = 0;
unsigned long previousCycle[4] = { 0, 0, 0, 0};
unsigned long lastTimeSaved = 0;
unsigned int distance[4] = { 0, 0, 0, 0 };
unsigned int clickInterval[4] = { 0, 0, 0, 0 };
float sensitivity = 1;
byte brightness = 255;

SR04 sensors[4] = { SR04(pins.sensors.front[ECHO], pins.sensors.front[TRIGGER]), SR04(pins.sensors.right[ECHO], pins.sensors.right[TRIGGER]), SR04(pins.sensors.back[ECHO], pins.sensors.back[TRIGGER]), SR04(pins.sensors.left[ECHO], pins.sensors.left[TRIGGER]) };
#pragma endregion

#pragma region Methods
void setup()
{
	pinMode(pins.clicker[FRONTI], OUTPUT);
	pinMode(pins.clicker[RIGHTI], OUTPUT);
	pinMode(pins.clicker[BACKI], OUTPUT);
	pinMode(pins.clicker[LEFTI], OUTPUT);
	pinMode(pins.brightnessOutput, OUTPUT); 
	getSaved();
	Serial.begin(1000000);
	#ifndef SKIPTEST
	digitalWrite(pins.clicker[0], HIGH);
	digitalWrite(pins.clicker[1], HIGH);
	digitalWrite(pins.clicker[2], HIGH);
	digitalWrite(pins.clicker[3], HIGH);
	delay(TESTTIME);
	digitalWrite(pins.clicker[0], LOW);
	digitalWrite(pins.clicker[1], LOW);
	digitalWrite(pins.clicker[2], LOW);
	digitalWrite(pins.clicker[3], LOW);
	#endif // SKIPTEST
}

void getSaved()
{

}

void save()
{

}

unsigned int click(unsigned int distance)
{
	return (sensitivity * (pow(distance, 2)) + distance + 6);
	//s = slog(1.3), c = s(d^2 + d + 6)
}

void getDistances()
{
	//slower, in its own loop. minimum 100ms processing time, delay(25) processed 4 times.
	for (int i = FRONTI; i < SENSCOUNT; i++)
	{
		//distance[i] = (unsigned int)sensors[i].Distance();
	}
}

void pulseOut()
{
	for (int i = FRONTI; i < SENSCOUNT; i++)
	{
		distance[FRONTI] = 10;
		clickInterval[i] = click(distance[i]);

		if (distance[i] > MININTERVAL)
		{
			if (uptime - previousCycle[i] >= clickInterval[i])
			{
				previousCycle[i] = uptime;
				digitalWrite(pins.clicker[i], HIGH);
				#ifdef DEBUG
				Serial.print("INTERVAL FOR: ");
				Serial.print(i);
				Serial.print(" DIST:");
				Serial.print(distance[FRONTI]);
				Serial.print(" INT:");
				Serial.print(clickInterval[FRONTI]);
				Serial.print(" PIN:");
				Serial.println(pins.clicker[i]);
				#endif // DEBUG
			}
			else
			{
				#ifdef DEBUG
				digitalWrite(pins.clicker[i], LOW);
				Serial.print("NON-INTERVAL FOR: ");
				Serial.print(i);
				Serial.print(" DIST:");
				Serial.print(distance[FRONTI]);
				Serial.print(" INT:");
				Serial.print(clickInterval[FRONTI]);
				Serial.print(" PIN:");
				Serial.println(pins.clicker[i]);
				#endif // DEBUG
			}
		}
		else
		{
			digitalWrite(pins.clicker[i], HIGH);
			#ifdef DEBUG
			Serial.print("FULL FOR: ");
			Serial.print(i);
			Serial.print(" DIST:");
			Serial.print(distance[FRONTI]);
			Serial.print(" INT:");
			Serial.print(clickInterval[FRONTI]);
			Serial.print(" PIN:");
			Serial.println(pins.clicker[i]);
			#endif // DEBUG
		}
	}
}

void loop()
{
	sensitivity = ((float)mapValue(analogRead(pins.sensitivity), 0, 1023, SENSLOW, SENSHIGH) * log(1.3));
	brightness = map(analogRead(pins.brightnessWiper), 0, 1023, 0, 255);
	analogWrite(pins.brightnessOutput, brightness);
	getDistances();
	uptime = millis();
	pulseOut();
	
	if (uptime - lastTimeSaved >= SAVEINTERVAL)
	{
		lastTimeSaved = uptime;
		save();
	}
}
#pragma endregion