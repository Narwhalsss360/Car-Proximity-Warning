#include <NDefs.h>
#include <NFuncs.h>
#include <NHCSR04.h>
#include <NTimer.h>
#include <NRotary.h>
#include <NPush.h>

#define DEBUG
#define TIME

#define ROTARY_DEBOUNCE_TIME 30
#define PUSH_DEBOUNCE 30

#define SENSOR_COUNT 1
#define MIN_DISTANCE 3
#define MAX_DISTANCE 60
#define SENSITIVITY_MIN 1
#define SENSITIVITY_MAX 20
#define DEFAULT_SENSITIVTY 6

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
		{ 15, 14 },
		{ 7, 8 },
		{ 9, 10 },
		{ 11, 12 }
	},
		clickers[4] = { 13, 14, 15, 16 },
		ledEnable = 17,
		rotary[3] = { 2, 3, 4 };
};

constexpr PINS pins;

SR04 sensors[4] = 
{
	SR04(pins.sensors[FRONT][TRIGGER], pins.sensors[FRONT][ECHO]),
	SR04(pins.sensors[RIGHT][TRIGGER], pins.sensors[RIGHT][ECHO]),
	SR04(pins.sensors[BACK][TRIGGER], pins.sensors[BACK][ECHO]),
	SR04(pins.sensors[LEFT][TRIGGER], pins.sensors[LEFT][ECHO])
};
double sensitivity = DEFAULT_SENSITIVTY;
uint32_t lastClick[4] = { ZERO };

Push push(pins.rotary[PIN_S], INPUT_PULLUP, PUSH_DEBOUNCE);
Rotary rotary(pins.rotary[PIN_A], pins.rotary[PIN_B], true, INPUT_PULLUP, ROTARY_DEBOUNCE_TIME);
ROTARYSTATES rotaryState;

void rotaryISR()
{
	rotary.serviceRoutine();
}

void getSaved()
{
	
}

void save()
{
#ifdef DEBUG
	Serial.println("Saving...");
#endif
}

double getInterval(double d)
{
	if (d > 99)
		return UINT16_MAX;
	return ((pow(d, 2) / pow(sensitivity, 3)) + d + (6 * sensitivity));
	// y = (x^2/s^3) + 5x + 60s
}

void setup()
{
	pinMode(pins.clickers[FRONT], OUTPUT);
#ifdef DEBUG
	Serial.begin(1000000);
#endif // DEBUG
}

void click()
{
#if (defined(DEBUG) && defined(TIME))
	PerfTimer t(true, micros);
#endif
	for (uint8_t side = ZERO; side < SENSOR_COUNT; side++)
	{
		if (interval(lastClick[side], getInterval(sensors[side].centimeters())))
			digitalWrite(pins.clickers[side], HIGH);
		else
			digitalWrite(pins.clickers[side], LOW);
	}
#if (defined(DEBUG) && defined(TIME))
	t.stop();
	Serial.println("click(): " + String(t.totalTime));
#endif
}

void loop()
{
	rotaryState = (ROTARYSTATES)rotary.getState();
	push.update();
	click();
	NTimer.update();
}