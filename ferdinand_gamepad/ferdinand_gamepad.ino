// This arduino sketch should run on the Arduino Pro Micro.

// Be sure to use this library in v2.0
// https://github.com/MHeironimus/ArduinoJoystickLibrary/tree/version-2.0

#include <Joystick.h>
#include <EEPROM.h>

// Create the Joystick (Only has 1 button, an X axis and a Y axis)
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
		   JOYSTICK_TYPE_MULTI_AXIS, 1, 0,
		   true, true, false, false, false, false,
		   false, false, false, false, false);

// Inputs with an utrasonic sensor
enum sensorInputs {
	STEERINGWHEEL = 0,
	GASPEDAL = 1,
	BREAKPEDAL = 2
};

// The sensor inputs:
// steeringWheel, gasPedal, breakPedal
static const uint8_t sensors[3] = {3, 4, 5};
static const uint8_t pingOut = 2;
// The input for the button
// (If the button is pressed while starting, begin calibration routine)
static const uint8_t button = 6;


// The maximum and minimum of of the input value
// The calibration values first need to filled by the calibrate function or from the eeprom
uint16_t calibrationIn[sizeof(sensors)][2] = {
	{UINT16_MAX, 0},
	{UINT16_MAX, 0},
	{UINT16_MAX, 0},
};

// The values the input values are mapped to
// 512 is center
static const uint16_t calibrationOut[sizeof(sensors)][2] = {
	// Values for the steering wheel
	{0, 1023},
	// Values for the gas pedal
	// center the value when gas pedal isn't pressed
	{512, 1023},
	// Values for the break pedal
	// Substract this values from gas, so when it's fully
	// pressed and the gas pedal isn't, the resulting value is 0
	{0, 512}
};

void calibrate(void) {
	// Wait until the button isn't pressed anymore
	while(!digitalRead(button));
	// Wait a bit, so we don't get bit by debouncing
	delay(50);
	// Start calibration routine
	Serial.println("CALIBRATION MODE");
	// Calibrate until the button is pressed
	while(digitalRead(button)) {
		// Wait a bit each time
		delay(200);
		// Read the sensors
		uint16_t *measurements = readSensor();
		Serial.print("\r\nCalibration Vals:\t");
		// Iterate through the sensors
		for(uint8_t i = 0; i < sizeof(sensors); i++) {
			// If the measurment didn't time out
			if(measurements[i]) {
				if(measurements[i] < calibrationIn[i][0]) {
					calibrationIn[i][0] = measurements[i];
				}
				// The value could be both the maximum and the minimum
				if(measurements[i] > calibrationIn[i][1]) {
					calibrationIn[i][1] = measurements[i];
				}
				// Print the values for the operator
				Serial.print(calibrationIn[i][0]);
				Serial.print("\t");
				Serial.print(calibrationIn[i][1]);
				Serial.print("\t");
			}
		}
	}
	// After being done getting the measurements, save the updated ones
	// Iterate through the sensors
	for(uint8_t i = 0; i < sizeof(sensors); i++) {
		// For testing purposes you can just calibrate with one sensor,
		// so only update calibration values of pins where there were
		// measurements, the higher value has been set
		if (calibrationIn[i][1]) {
			// We need 4 bytes of EEPROM for each sensor, as there are 2 values of 16 bits each
			EEPROM[(i*4) + 0] = calibrationIn[i][0]  & 0xFF;
			EEPROM[(i*4) + 1] = (calibrationIn[i][0] >> 8) & 0xFF;
			EEPROM[(i*4) + 2] = calibrationIn[i][1]  & 0xFF;
			EEPROM[(i*4) + 3] = (calibrationIn[i][1] >> 8) & 0xFF;
		}
	}
}

void setup() {
	// Init Joystick
	Joystick.begin();
	// Set ping out to output
	pinMode(pingOut, OUTPUT);
	// Set the button as an input with a pullup
	pinMode(button, INPUT_PULLUP);
	// Set the sensor inputs as an input
	for (uint8_t i = 0; i < sizeof(sensors); i++) {
		pinMode(sensors[i], INPUT_PULLUP);
	}
	// Enter calibration mode if the button is pressed on startup
	if(!digitalRead(button))
		calibrate();
	// Read the calibration values from the EEPROM
	// Iterate through the sensors
	for(uint8_t i = 0; i < sizeof(sensors); i++) {
		// We need 4 bytes of EEPROM for each sensor, as there are 2 values of 16 bits each
		calibrationIn[i][0] = EEPROM[(i*4) + 0] | (EEPROM[(i*4) + 1] << 8);
		calibrationIn[i][1] = EEPROM[(i*4) + 2] | (EEPROM[(i*4) + 3] << 8);
	}
	// Print the calibrated values
	// Delay a bit so the serial terminal can be opened
	delay(3000);
	Serial.print("\r\nCalibrated:\t");
	// Iterate through the sensors
	for(uint8_t i = 0; i < sizeof(sensors); i++) {
		Serial.print(calibrationIn[i][0]);
		Serial.print("\t");
		Serial.print(calibrationIn[i][1]);
		Serial.print("\t");
	}
}

// This is just a reimplementation of pulseIn() for 3 inputs at the same time
// digitalRead() isn't particularly fast, but it's fast enough here
uint16_t *readSensor(void) {
	// The timeout for the measurements
	static const uint16_t timeout = 8000;
	uint32_t timeout_begin = 0;
	// This is the return array, so we can return a pointer
	// Of course you can't use values of two measurments at the same time,
	// as they'll be overwritten
	uint16_t static time[3];
	uint16_t begin[3] = {0, 0, 0};
	time[0] = 0;
	time[1] = 0;
	time[2] = 0;
	// Start the measurement with a pulse to pingOut
	digitalWrite(pingOut, HIGH);
	delayMicroseconds(50);
	digitalWrite(pingOut, LOW);
	// Begin Timeout
	timeout_begin = micros();
	// Stop when either all sensors have been measured or the timeout was reached
	while(((time[0] == 0) || (time[1] == 0) || (time[2] == 0)) && ((micros() - timeout_begin) < timeout)) {
		// As the measurement for all three sensors is the same, we can just use it in a for loop
		for (uint8_t i = 0; i < 3; i ++) {
			// When no begin value has been recorded (so begin[0] isn't
			// actually written twice) and there's a high pulse
			if (begin[i] == 0) {
				if (digitalRead(sensors[i]) == true) {
					begin[i] = micros();
				}
			}
			// When a measurement has begun, but it hasn't ended yet and the
			// pulse ends
			else if (time[i] == 0) {
				if (digitalRead(sensors[i]) == false) {
					time[i] = micros() - begin[i];
				}
			}
		}
	}
	// Just print the measurement data for debugging purposes
	Serial.print("\r\nSensorRaw\t");
	Serial.print(time[0]);
	Serial.print("\t");
	Serial.print(time[1]);
	Serial.print("\t");
	Serial.print(time[2]);
	return time;
}

// A curve for the steering wheel
// This needs to be adjusted if the output range changes
// There may be curves that are better, the goal was just to get one like this:
//                                  |              ----------------
//                                  |        -----/
//                                  |    /--/
//                                  |  -/
//                                  | /
//                                  /
//    ------------------------------|------------------------------
//                                  /
//                                / |
//                              /-  |
//                          /--/    |
//                    /-----        |
//    ----------------              |
// That way it's pretty easy to steer when in the middle without much movement,
// so you can steer faster
uint16_t logSteering(uint16_t x) {
	int16_t y = x - 512;
	return constrain((sqrt(abs(y)) * 25 * (y / abs(y)))+512, 0, 1023);
}

// Average and convert the data
uint16_t processSensor(enum sensorInputs id, uint16_t val) {
	// As this is a static array, it survives when this function returns
	static uint16_t lastData[sizeof(sensors)];
	// Just average the last two values together
	uint16_t average = (lastData[id] + val)/ 2;
	lastData[id] = val;
	// Constrain the data to the max cal values
	// Otherwise the map function may wrap around
	average = constrain(average, calibrationIn[id][0], calibrationIn[id][1]);
	// Map the input values to the calibrated values
	return map(average, calibrationIn[id][0], calibrationIn[id][1], calibrationOut[id][0], calibrationOut[id][1]);
}

void handleSensors() {
	uint16_t *measurements = readSensor();
	uint16_t steeringWheel =  measurements[STEERINGWHEEL];
	uint16_t gasPedal = measurements[GASPEDAL];
	uint16_t breakPedal = measurements[BREAKPEDAL];
	Joystick.setXAxis(logSteering(processSensor(STEERINGWHEEL, steeringWheel)));
	// gas and break are on the same axis and can cancel each other out
	Joystick.setYAxis(processSensor(GASPEDAL, gasPedal) - processSensor(BREAKPEDAL, breakPedal));
	// We invert the button input, as it's pulled to GND when it's pressed
	Joystick.setButton(0, !digitalRead(button));
}

void loop() {
	handleSensors();
	delay(20);
}
