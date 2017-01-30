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

// The sensor inputs:
// steeringWheel, throttle, break
static const uint8_t sensors[] = {0, 1, 2};
static const uint8_t pingOut = 5;

// The maximum and minimum of of the input values
// You need to calibrate this
// WARNING: If the input value is bigger or smaller than the max values,
//          the map function wraps around. Be sure to test properly and
//          ideally have a bit of wiggle room
static const uint32_t calibrationIn[sizeof(sensors)][2] = {
  // Values for the steering wheel
	{1450, 3900},
  // Values for the throttle pedal
	{3300, 5100},
  // Values for the break pedal
	{3100, 4500}
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

uint32_t avgData[sizeof(sensors)];

void setup() {
	// USB doesn't need to be initialized
  // Set ping out to output
	pinMode(pingOut, OUTPUT);
  // Set the sensor inputs as an input
	for (uint8_t i = 0; i < sizeof(sensors); i++) {
		pinMode(sensors[i], INPUT);
	}
}

uint8_t readSensor(uint8_t id) {
  // The special delay functions also handle the usb connections
  // The sensors don't like beeing activated so fast, so we just wait 5 milliseconds
	#ifdef debug
	DigiKeyboard.delay(5);
	#else
	DigiJoystick.delay(5);
	#endif
	// Start the measurment with a pulse to pingOut
	digitalWrite(pingOut, HIGH);
	delayMicroseconds(50);
	digitalWrite(pingOut, LOW);
  // Measure the pulse (high) with timeout and average it
	avgData[id] = (avgData[id] >> 1) + pulseIn(sensors[id], HIGH, 6000);
	#ifdef debug
  // As the return type is too small, the debug just reads directly from the avgData array
	return 0;
	#else
  // Map the input values to the calibrated values
	return map(avgData[id], calibrationIn[id][0], calibrationIn[id][1], calibrationOut[id][0], calibrationOut[id][1]);
	#endif
}


void loop() {
	uint8_t steeringWheel =  readSensor(0);
	uint8_t throttlePedal = readSensor(1);
	uint8_t breakPedal = readSensor(2);
	#ifdef debug
  // As there isn't a serial port, just output debug data as a keyboard
  // (gnuplot likes space seperated values)
	DigiKeyboard.print("\n");
	DigiKeyboard.print(avgData[0]);
	DigiKeyboard.print(" ");
	DigiKeyboard.print(avgData[1]);
	DigiKeyboard.print(" ");
	DigiKeyboard.print(avgData[2]);
	DigiKeyboard.delay(200); // wait 50 milliseconds
	#else
	DigiJoystick.setX((byte) steeringWheel);
  // throttle and break are on the same axis and can cancel each other out
	DigiJoystick.setY((byte)(throttlePedal - breakPedal));
  // Can't hurt
	DigiJoystick.delay(20); // wait 50 milliseconds
	#endif
}
