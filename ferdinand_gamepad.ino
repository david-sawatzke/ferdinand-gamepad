// This arduino sketch should run on the digispark.
// It's an attiny85 with the micronucleus bootloader
// You should check the digispark website, so you can select the right
// board in the arduino ise

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
static const uint8_t sensors[] = {1, 2, 0};
static const uint8_t pingOut = 5;

// The maximum and minimum of of the input values
// You need to calibrate this
// WARNING: If the input value is bigger or smaller than the max values,
//          the map function wraps around. Be sure to test properly and
//          ideally have a bit of wiggle room
static const uint32_t calibrationIn[sizeof(sensors)][2] = {
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

uint32_t readSensor(enum inputs id) {
	// Start the measurment with a pulse to pingOut
	digitalWrite(pingOut, HIGH);
	delayMicroseconds(50);
	digitalWrite(pingOut, LOW);
	return pulseIn(sensors[id], HIGH, 6000);
}

// Avrage and convert the data
byte processSensor(enum inputs id, uint32_t val) {
	static uint32_t lastData[sizeof(sensors)];
	uint32_t average = lastData[id] + val;
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
	uint32_t steeringWheel =  readSensor(STEERINGWHEEL);
	// The sensors don't like beeing activated so fast, so we just wait a bit
	// This function also handles USB processing
	usbDelay(5);
	uint32_t throttlePedal = readSensor(THROTTLEPEDAL);
	usbDelay(5);
	uint32_t breakPedal = readSensor(BREAKPEDAL);
#ifdef debug
	// As there isn't a serial port, just output debug data as a keyboard
	// (gnuplot likes space seperated values)
	DigiKeyboard.print("\n");
	DigiKeyboard.print(steeringWheel);
	DigiKeyboard.print(" ");
	DigiKeyboard.print(throttlePedal);
	DigiKeyboard.print(" ");
	DigiKeyboard.print(breakPedal);
	usbDelay(200); // wait 200 milliseconds as we don't need the values that often
#else
	DigiJoystick.setX((byte)processSensor(STEERINGWHEEL, steeringWheel));
	// throttle and break are on the same axis and can cancel each other out
	DigiJoystick.setY((byte)(processSensor(THROTTLEPEDAL, throttlePedal) - processSensor(BREAKPEDAL, breakPedal)));
	// Can't hurt
	usbDelay(20); // wait 20 milliseconds
#endif
}
