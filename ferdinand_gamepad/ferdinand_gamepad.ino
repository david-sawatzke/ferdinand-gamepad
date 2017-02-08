// This arduino sketch should run on the digispark.
// It's an attiny85 with the micronucleus bootloader
// You should check the digispark website, so you can select the right
// board in the arduino ide

// https://github.com/MHeironimus/ArduinoJoystickLibrary/tree/version-2.0

// Debug
// If debug is defined, the averaged but not mapped values are
// typed (the device functions like a keyboard). Just use an editor
// with an empty file as the display
// This is intended for calibrating
//#define debug

#ifdef debug
#include "DigiKeyboard.h"
#else
#include "DigiJoystick.h"
#endif
enum inputs {
	STEERINGWHEEL = 0,
	THROTTLEPEDAL = 1,
	BREAKPEDAL = 2
};
// The sensor inputs:
// steeringWheel, throttle, break
static const uint8_t sensors[3] = {1, 2, 0};
static const uint8_t pingOut = 5;

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
// 128 is center
static const uint8_t calibrationOut[sizeof(sensors)][2] = {
	// Values for the steering wheel
	{0, 255},
	// Values for the throttle pedal
	// center the value when throttle isn't pressed
	{128, 255},
	// Values for the break pedal
	// Substract this values from throttle, so when it's fully
	// pressed and throttle isn't, the resulting value is 0
	{0, 128}
};


void setup() {
	// USB doesn't need to be initialized
	// Set ping out to output
	pinMode(pingOut, OUTPUT);
	// Set the sensor inputs as an input
	for (uint8_t i = 0; i < sizeof(sensors); i++) {
		pinMode(sensors[i], INPUT);
	}
}

uint16_t *readSensor(void) {
	uint16_t timeout = 6000;
	uint32_t timeout_begin = 0;
	uint16_t static time[3] = {0, 0, 0};
	uint16_t begin[3] = {0, 0, 0};

	// Start the measurment with a pulse to pingOut
	digitalWrite(pingOut, HIGH);
	delayMicroseconds(50);
	digitalWrite(pingOut, LOW);
	// Begin Timeout
	timeout_begin = micros();
	// Stop when either all sensors have been measures or the timeout was reached
	while((time[0] == 0 || time[1] == 0 || time[2] == 0) && ((micros() - timeout_begin) < timeout)) {
		if (begin[0] == 0 && digitalRead(sensors[0]) == true) {
			begin[0] = micros();
		} else if (begin[0] == 0 && time[0] == 0 && digitalRead(sensors[0]) == false) {
			time[0] = micros() - begin[0];
		}
		if (begin[1] == 0 && digitalRead(sensors[1]) == true) {
			begin[1] = micros();
		} else if (begin[1] == 0 && time[1] == 0 && digitalRead(sensors[1]) == false) {
			time[1] = micros() - begin[1];
		}
		if (begin[2] == 0 && digitalRead(sensors[2]) == true) {
			begin[2] = micros();
		} else if (begin[2] == 0 && time[2] == 0 && digitalRead(sensors[2]) == false) {
			time[2] = micros() - begin[2];
		}
	}
	return time;
}

// Avrage and convert the data
byte processSensor(enum inputs id, uint16_t val) {
	static uint16_t lastData[sizeof(sensors)];
	uint16_t average = lastData[id] + val;
	lastData[id] = val;
	// Map the input values to the calibrated values
	return map(average, calibrationIn[id][0], calibrationIn[id][1], calibrationOut[id][0], calibrationOut[id][1]);
}

void usbDelay(uint8_t val) {
#ifdef debug
	DigiKeyboard.delay(val);
#else
	DigiJoystick.delay(val);
#endif
}

void loop() {
	uint16_t *measurments = readSensor();
	uint16_t steeringWheel =  measurments[STEERINGWHEEL];
	uint16_t throttlePedal = measurments[THROTTLEPEDAL];
	uint16_t breakPedal = measurments[BREAKPEDAL];
#ifdef debug
	// As there isn't a serial port, just output debug data as a keyboard
	// (gnuplot likes space seperated values)
	DigiKeyboard.print("\n");
	DigiKeyboard.print(steeringWheel);
	DigiKeyboard.print(" ");
	DigiKeyboard.print(throttlePedal);
	DigiKeyboard.print(" ");
	DigiKeyboard.print(breakPedal);
	usbDelay(200); // Wait a bit longer than normal, so one can read the values
#else
	DigiJoystick.setX((byte)processSensor(STEERINGWHEEL, steeringWheel));
	// throttle and break are on the same axis and can cancel each other out
	DigiJoystick.setY((byte)(processSensor(THROTTLEPEDAL, throttlePedal) - processSensor(BREAKPEDAL, breakPedal)));
#endif
	// Can't hurt
	usbDelay(20); // wait 20 milliseconds
}
