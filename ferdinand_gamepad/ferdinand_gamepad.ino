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
	THROTTLEPEDAL = 1,
	BREAKPEDAL = 2
};

// The sensor inputs:
// steeringWheel, throttlePedal, breakPedal
static const uint8_t sensors[3] = {3, 4, 5};
static const uint8_t pingOut = 2;
// The input for the button
// (If the button is pressed while starting, begin calibration routine)
static const uint8_t button = 6;


// The maximum and minimum of of the input values
// These are so strange, because they first need to
// filled by the calibrate function or values from the eeprom
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
	// Values for the throttle pedal
	// center the value when throttle isn't pressed
	{512, 1023},
	// Values for the break pedal
	// Substract this values from throttle, so when it's fully
	// pressed and throttle isn't, the resulting value is 0
	{0, 512}
};

void calibrate(void) {
	Serial.println("CALIBRATION MODE");
	// Wait until the button isn't pressed anymore
	while(!digitalRead(button));
	// Wait a bit, so we don't get bit by debouncing
	delay(50);
	//Output it again
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
				// It could happen that it's both the maximum and the minimum
				if(measurements[i] > calibrationIn[i][1]) {
					calibrationIn[i][1] = measurements[i];
				}
				Serial.print(calibrationIn[i][0]);
				Serial.print("\t");
				Serial.print(calibrationIn[i][1]);
				Serial.print("\t");
			}
		}
	}
	// After we are done getting the measurements, let's save the updated ones
	// Iterate through the sensors
	for(uint8_t i = 0; i < sizeof(sensors); i++) {
		// If the higher Value has been set
		// (the lower one would be set too)
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
	// Set the button as an input
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
		// If the higher Value has been set
		// (the lower one would be set too)
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

uint16_t *readSensor(void) {
	uint16_t timeout = 8000;
	uint32_t timeout_begin = 0;
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
	Serial.print("\r\nSensorRaw\t");
	Serial.print(time[0]);
	Serial.print("\t");
	Serial.print(time[1]);
	Serial.print("\t");
	Serial.print(time[2]);
	return time;
}

// Avrage and convert the data
uint16_t processSensor(enum sensorInputs id, uint16_t val) {
	static uint16_t lastData[sizeof(sensors)];
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
	uint16_t throttlePedal = measurements[THROTTLEPEDAL];
	uint16_t breakPedal = measurements[BREAKPEDAL];
	Joystick.setXAxis(processSensor(STEERINGWHEEL, steeringWheel));
	// throttle and break are on the same axis and can cancel each other out
	Joystick.setYAxis(processSensor(THROTTLEPEDAL, throttlePedal) - processSensor(BREAKPEDAL, breakPedal));
	// We invert the button input, as it's pulled to GND when it's pressed
	Joystick.setButton(0, !digitalRead(button));
}

void loop() {
	handleSensors();
	delay(20);
}
