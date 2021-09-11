/* Name:		Car_Proximity_Warning.ino Created:	9/10/2021 10:03:27 PM Author:	Narwhalsss*/#pragma region Pre-Processor#include <SR04.h>
#define Sensor1Trigger 0#define Sensor1Echo 1#define Sensor2Trigger 2#define Sensor2Echo 3#define Sensor3Trigger 4#define Sensor3Echo 5 #define Sensor4Trigger 6#define Sensor4Echo 7#define Buzzer1Pin 14#define Buzzer2Pin 15#define Buzzer3Pin 16#define Buzzer4Pin 17#define LOWpin 18#define HIGHpin 19#define SwitchPin 8#define PotPin A5#pragma endregion#pragma region Global Objectsenum switchStates {	UP,	MIDDLE,	DOWN};unsigned intdistanceForward,distanceBackwards,distanceLeft,distanceRight,clickForward,clickBackwards,clickLeft,clickRight,switchState = MIDDLE;float sensitivity;unsigned long runtime;SR04 forwardSensor(Sensor1Echo, Sensor1Trigger);SR04 backwardsSensor(Sensor2Echo, Sensor2Trigger);SR04 leftSensor(Sensor3Echo, Sensor3Trigger);SR04 rightSensor(Sensor4Echo, Sensor4Trigger);#pragma endregion#pragma region Main Methodsvoid setup() {	pinMode(Sensor1Trigger, OUTPUT);	pinMode(Sensor1Echo, INPUT);	pinMode(Sensor2Trigger, OUTPUT);	pinMode(Sensor2Echo, INPUT);	pinMode(Sensor3Trigger, OUTPUT);	pinMode(Sensor3Echo, INPUT);	pinMode(Sensor4Trigger, OUTPUT);	pinMode(Sensor4Echo, INPUT);	pinMode(Buzzer1Pin, OUTPUT);	pinMode(Buzzer2Pin, OUTPUT);	pinMode(Buzzer3Pin, OUTPUT);	pinMode(Buzzer4Pin, OUTPUT);	pinMode(HIGHpin, OUTPUT);	pinMode(LOWpin, OUTPUT);	pinMode(SwitchPin, INPUT_PULLUP);}void loop() {	runtime = millis();	inputRead();	getDistance();	clickBuzzers();}void inputRead() {	if (!digitalRead(SwitchPin))
	{
		digitalWrite(HIGHpin, LOW);
		digitalWrite(LOWpin, HIGH);
	}	else {		digitalWrite(LOWpin, LOW);		digitalWrite(HIGHpin, HIGH);	}	sensitivity = analogRead(PotPin);	sensitivity = map(0, 1023, 0.0001, 1, sensitivity);}void getDistance() {	distanceForward = forwardSensor.Distance();	distanceBackwards = backwardsSensor.Distance();	distanceLeft = leftSensor.Distance();	distanceRight = rightSensor.Distance();}unsigned long calculateClickInterval(unsigned int distance) {	return (sensitivity * (pow(distance, 2)) + distance + 6);}void clickBuzzers() {	if (distanceForward > 13)
	{
		unsigned long interval = calculateClickInterval(distanceForward);
		unsigned long previousCycle;
		if (runtime - previousCycle >= interval)
		{
			digitalWrite(Buzzer1Pin, HIGH);
			digitalWrite(Buzzer2Pin, HIGH);
		}
		else {
			digitalWrite(Buzzer1Pin, LOW);
			digitalWrite(Buzzer2Pin, LOW);
		}
	}
	else {
		digitalWrite(Buzzer1Pin, HIGH);
		digitalWrite(Buzzer2Pin, HIGH);
	}	if (distanceBackwards > 13)
	{
		unsigned long interval = calculateClickInterval(distanceBackwards);
		unsigned long previousCycle;
		if (runtime - previousCycle >= interval)
		{
			digitalWrite(Buzzer3Pin, HIGH);
			digitalWrite(Buzzer4Pin, HIGH);
		}
		else {
			digitalWrite(Buzzer3Pin, LOW);
			digitalWrite(Buzzer4Pin, LOW);
		}
	}
	else {
		digitalWrite(Buzzer3Pin, HIGH);
		digitalWrite(Buzzer4Pin, HIGH);
	}	if (distanceLeft > 13)
	{
		unsigned long interval = calculateClickInterval(distanceLeft);
		unsigned long previousCycle;
		if (runtime - previousCycle >= interval)
		{
			digitalWrite(Buzzer1Pin, HIGH);
			digitalWrite(Buzzer3Pin, HIGH);
		}
		else {
			digitalWrite(Buzzer1Pin, LOW);
			digitalWrite(Buzzer3Pin, LOW);
		}
	}
	else {
		digitalWrite(Buzzer1Pin, HIGH);
		digitalWrite(Buzzer3Pin, HIGH);
	}	if (distanceRight > 13)
	{
		unsigned long interval = calculateClickInterval(distanceRight);
		unsigned long previousCycle;
		if (runtime - previousCycle >= interval)
		{
			digitalWrite(Buzzer2Pin, HIGH);
			digitalWrite(Buzzer4Pin, HIGH);
		}
		else {
			digitalWrite(Buzzer2Pin, LOW);
			digitalWrite(Buzzer4Pin, LOW);
		}
	}
	else {
		digitalWrite(Buzzer2Pin, HIGH);
		digitalWrite(Buzzer4Pin, HIGH);
	}}#pragma endregion