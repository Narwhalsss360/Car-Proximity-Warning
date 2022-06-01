#include <NFuncs.h>
#include <NHCSR04.h>
#include <NTimer.h>
#include <NRotary.h>
#include <NPush.h>
#include <EEPROM.h>

#define DEBUG
//#define TIME
//#define DIST
#define SAVE

#define SAVE_INTERVAL 3m

#define ROTARY_DEBOUNCE_TIME 30
#define PUSH_DEBOUNCE 30

#define SENSOR_COUNT 4
#define MIN_DISTANCE 3
#define MAX_DISTANCE 60
#define SENSITIVITY_MIN 3
#define SENSITIVITY_MAX 20

#define ROTARY_SENSITIVITY_MIN 0
#define ROTARY_SENSITIVITY_MAX 99
#define DEFAULT_ROTARY_SENSITIVITY 26

#ifdef DEBUG
#include <PerfTimer.h>
double oldSensitivity;
#endif

enum EEPROMADDRESS
{
	ROTARY_SENSITIVITY
};

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
		rotary[3] = { 2, 3, 4 },
		ledEnable = 17;
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
uint32_t lastClick[4] = { ZERO };

Push push(pins.rotary[PIN_S], INPUT_PULLUP, PUSH_DEBOUNCE);
Rotary rotary(pins.rotary[PIN_A], pins.rotary[PIN_B], true, INPUT_PULLUP, ROTARY_DEBOUNCE_TIME);
ROTARYSTATES rotaryState;
uint8_t rotarySensitivity = DEFAULT_ROTARY_SENSITIVITY;

void rotaryISR()
{
	rotary.serviceRoutine();
}

void getSaved()
{
	rotarySensitivity = EEPROM.read(ROTARY_SENSITIVITY);
	if (rotarySensitivity > 99)
		rotarySensitivity = DEFAULT_ROTARY_SENSITIVITY;
#if defined(DEBUG) && defined(SAVE)
	Serial.println("Got from saved: " + String(rotarySensitivity));
#endif

}

void save(ElapsedEvent e)
{
	EEPROM.update(ROTARY_SENSITIVITY, rotarySensitivity);
#if defined(DEBUG) && defined(SAVE)
	Serial.println("Saved: " + String(rotarySensitivity));
#endif
}

void inputs()
{
	rotaryState = (ROTARYSTATES)rotary.getState();
	push.update();
	
	switch (rotaryState)
	{
	case COUNTER_CLOCKWISE:
		if (rotarySensitivity < 99)
			rotarySensitivity++;
		break;
	case CLOCKWISE:
		if (rotarySensitivity > 0)
			rotarySensitivity--;
		break;
	default:
		break;
	}

	sensitivity = mapf(rotarySensitivity, ROTARY_SENSITIVITY_MIN, ROTARY_SENSITIVITY_MAX, SENSITIVITY_MIN, SENSITIVITY_MAX);
}

double getInterval(double d)
{
	if (d > 99)
		return UINT16_MAX;
	return ((pow(d, 2) / pow(sensitivity, 3)) + d + (6 * sensitivity));
	// y = (x^2/s^3) + x + 6s
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
#ifdef DEBUG

#ifdef TIME

	t.stop();
	Serial.println("click(): " + String(t.totalTime) + ".");

#endif

#ifdef DIST
	String output = "RS: ";
	output += String(rotarySensitivity) + " S: " + String(sensitivity) + " Distance:\n";
	for (uint8_t side = ZERO; side < SENSOR_COUNT; side++)
	{
		output += "    [" + String(side) + "]: " + String(sensors[side].centimeters(false)) + " Time: " + String(sensors[side].ping()) + '\n';
	}
	Serial.println(output);
#endif

#endif
}

void setup()
{
#ifdef DEBUG
	Serial.begin(1000000);
#endif
	getSaved();
	pinMode(pins.clickers[FRONT], OUTPUT);
	pinMode(pins.clickers[RIGHT], OUTPUT);
	pinMode(pins.clickers[BACK], OUTPUT);
	pinMode(pins.clickers[LEFT], OUTPUT);
	pinMode(pins.ledEnable, INPUT_PULLUP);
	addInterrupt(pins.rotary[PIN_A], rotaryISR, rotary.mode);
	NTimer.addEvent({ 0, SAVE_INTERVAL, save });
	NTimer.start();
}

void loop()
{
	NTimer.update();
	inputs();
	click();
}