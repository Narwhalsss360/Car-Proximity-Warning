#include <SR04.h>
#include "NarsLibraries.h"

#pragma region Defenitions
#define TRIGGER 0
#define ECHO 1
#define SENSLOW 0.0001
#define SENSHIGH 1
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
const PROGMEM PINS pins;

unsigned long uptime;
unsigned long previousCycle[4];
unsigned int distance[4];
unsigned int clickInterval[4];
float sensitivity;
byte brightness;

SR04 sensors[4] = { SR04(pins.sensors.front[ECHO], pins.sensors.front[TRIGGER]), SR04(pins.sensors.right[ECHO], pins.sensors.right[TRIGGER]), SR04(pins.sensors.back[ECHO], pins.sensors.back[TRIGGER]), SR04(pins.sensors.left[ECHO], pins.sensors.left[TRIGGER]) };
#pragma endregion

#pragma region Methods
void setup()
{
	for (byte i = 0; i < 4; i++)
	{
		pinMode(pins.clicker[i], OUTPUT);
	}
	pinMode(pins.sensors.front[TRIGGER], OUTPUT);
	pinMode(pins.sensors.front[ECHO], INPUT);
	pinMode(pins.sensors.right[TRIGGER], OUTPUT);
	pinMode(pins.sensors.right[ECHO], INPUT);
	pinMode(pins.sensors.back[TRIGGER], OUTPUT);
	pinMode(pins.sensors.back[ECHO], INPUT);
	pinMode(pins.sensors.left[TRIGGER], OUTPUT);
	pinMode(pins.sensors.left[ECHO], INPUT);
	pinMode(pins.brightnessOutput, OUTPUT);
}

unsigned int click(unsigned int distance)
{
	return (sensitivity * (pow(distance, 2)) + distance + 6);
}

void loop()
{
	uptime = millis();
	sensitivity = (float)mapValue(analogRead(pins.sensitivity), 0, 1023, SENSLOW, SENSHIGH);
	brightness = map(analogRead(pins.brightnessWiper), 0, 1023, 0, 255);
	analogWrite(pins.brightnessOutput, brightness);

	for (int i = 0; i < 4; i++)
	{
		uptime = millis();
		long distance = sensors[i].Distance();
		clickInterval[i] = click(distance);

		if (distance > 13)
		{
			if (uptime - previousCycle[i] >= clickInterval[i])
			{
				digitalWrite(pins.clicker[i], HIGH);
			}
			else
			{
				digitalWrite(pins.clicker[i], LOW);
			}
		}
		else
		{
			digitalWrite(pins.clicker[i], LOW);
		}
	}
}
#pragma endregion