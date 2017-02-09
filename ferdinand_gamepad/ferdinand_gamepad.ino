// This arduino sketch should run on the Arduino Pro Micro.

// Be sure to use this library
// https://github.com/MHeironimus/ArduinoJoystickLibrary/tree/version-2.0

#include <Joystick.h>

// Create the Joystick
Joystick_ Joystick;

// Inputs with a Ultrasonicsensor
enum sensorInputs {
	STEERINGWHEEL = 0,
	THROTTLEPEDAL = 1,
	BREAKPEDAL = 2
};

// The sensor inputs:
// steeringWheel, throttle, break
static const uint8_t sensors[3] = {3, 4, 5};
static const uint8_t pingOut = 2;
static const uint8_t resetPin = 6;


// The maximum and minimum of of the input values
// You need to calibrate this
// WARNING: If the input value is bigger or smaller than the max values,
//          the map function wraps around. Be sure to test properly and
//          ideally have a bit of wiggle room
static const uint16_t calibrationIn[sizeof(sensors)][2] = {
	// Values for the steering wheel
	{1500, 3900},
	// Values for the throttle pedal
	{3300, 5000},
	// Values for the break pedal
	{3100, 4400}
};

// The values the input values are mapped to
// 512 is center
static const uint16_t calibrationOut[sizeof(sensors)][2] = {
	// Values for the steering wheel
	{0, 1023},
	// Values for the throttle pedal
	// center the value when throttle isn't pressed
	{512, 1023},
	// Values for the break pedal
	// Substract this values from throttle, so when it's fully
	// pressed and throttle isn't, the resulting value is 0
	{0, 512}
};


void setup() {
	// Init Joystick
	Joystick.begin();
	// Set ping out to output
	pinMode(pingOut, OUTPUT);
	// Set the sensor inputs as an input
	for (uint8_t i = 0; i < sizeof(sensors); i++) {
		pinMode(sensors[i], INPUT_PULLUP);
	}
}

uint16_t *readSensor(void) {
	uint16_t timeout = 8000;
	uint32_t timeout_begin = 0;
	uint16_t static time[3];
	uint16_t begin[3] = {0, 0, 0};
	time[0] = 0;
	time[1] = 0;
	time[2] = 0;
	// Start the measurment with a pulse to pingOut
	digitalWrite(pingOut, HIGH);
	delayMicroseconds(50);
	digitalWrite(pingOut, LOW);
	// Begin Timeout
	timeout_begin = micros();
	// Stop when either all sensors have been measures or the timeout was reached
	while(((time[0] == 0) || (time[1] == 0) || (time[2] == 0)) && ((micros() - timeout_begin) < timeout)) {
		if ((begin[0] == 0) && (digitalRead(sensors[0]) == true)) {
			begin[0] = micros();
		} else if ((begin[0] != 0) && (time[0] == 0 )&& (digitalRead(sensors[0]) == false)) {
			time[0] = micros() - begin[0];
		}
		if ((begin[1] == 0) && (digitalRead(sensors[1]) == true)) {
			begin[1] = micros();
		} else if ((begin[1] != 0) && (time[1] == 0 )&& (digitalRead(sensors[1]) == false)) {
			time[1] = micros() - begin[1];
		}
		if ((begin[2] == 0) && (digitalRead(sensors[2]) == true)) {
			begin[2] = micros();
		} else if ((begin[2] != 0) && (time[2] == 0 )&& (digitalRead(sensors[2]) == false)) {
			time[2] = micros() - begin[2];
		}
	}
	return time;
}

// Avrage and convert the data
uint16_t processSensor(enum sensorInputs id, uint16_t val) {
	static uint16_t lastData[sizeof(sensors)];
	uint16_t average = lastData[id] + val;
	lastData[id] = val;
	// Constrain the data to the max cal values
	// Otherwise the map function may wrap around
	average = constrain(average, calibrationIn[id][0], calibrationIn[id][1]);
	// Map the input values to the calibrated values
	return map(average, calibrationIn[id][0], calibrationIn[id][1], calibrationOut[id][0], calibrationOut[id][1]);
}

void handleSensors() {

	uint16_t *measurments = readSensor();
	uint16_t steeringWheel =  measurments[STEERINGWHEEL];
	uint16_t throttlePedal = measurments[THROTTLEPEDAL];
	uint16_t breakPedal = measurments[BREAKPEDAL];
	Serial.print("\r\nSensorRaw\t");
	Serial.print(steeringWheel);
	Serial.print("\t");
	Serial.print(throttlePedal);
	Serial.print("\t");
	Serial.print(breakPedal);
	Joystick.setXAxis(processSensor(STEERINGWHEEL, steeringWheel));
	// throttle and break are on the same axis and can cancel each other out
	Joystick.setYAxis(processSensor(THROTTLEPEDAL, throttlePedal) - processSensor(BREAKPEDAL, breakPedal));
}
void loop() {
	handleSensors();
	// Can't hurt
	delay(20);
}
