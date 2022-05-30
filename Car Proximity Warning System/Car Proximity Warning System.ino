#include <NDefs.h>
#include <NFuncs.h>
#include <NHCSR04.h>
#include <NTimer.h>

#define DEBUG

#define SENSOR_COUNT 4
#define MIN_DISTANCE 3
#define MAX_DISTANCE 60
#define SENSITIVITY_MIN 1
#define SENSITIVITY_MAX 3
#define DEFAULT_SENSITIVTY 

#ifdef DEBUG
#include <PerfTimer.h>
#endif

enum SIDES
{
	FRONT,
	RIGHT,
	BACK,
	LEFT
};

enum SENSORPINS
{
	TRIGGER,
	ECHO
};

enum ROTARYPINS
{
	PIN_A,
	PIN_B,
	PIN_S
};

struct PINS
{
	const byte sensors[4][2] =
	{
		{ 5, 6 },
		{ 7, 8 },
		{ 9, 10 },
		{ 11, 12 }
	},
		clickers[4] = { 13, 14, 15, 16 },
		ledEnable = 17,
		rotary[3] = { 2, 3, 4 },
		power = 18;
};

constexpr PINS pins;
SR04 sensors[4] = 
{
	SR04(pins.sensors[FRONT][TRIGGER], pins.sensors[FRONT][ECHO]),
	SR04(pins.sensors[RIGHT][TRIGGER], pins.sensors[RIGHT][ECHO]),
	SR04(pins.sensors[BACK][TRIGGER], pins.sensors[BACK][ECHO]),
	SR04(pins.sensors[LEFT][TRIGGER], pins.sensors[LEFT][ECHO])
};
double sensitivity;

void getSaved()
{
	
}

void save()
{

}

double calculate(uint16_t d)
{
	return ((pow(d, 2) / pow(sensitivity, 3)) + (5 * d) + (60 * sensitivity));
	// y = (x^2/s^3) + 5x + 60s
}

void setup()
{
#ifdef DEBUG
	Serial.begin(1000000);
#endif
}

void loop()
{
	NTimer.update();
}