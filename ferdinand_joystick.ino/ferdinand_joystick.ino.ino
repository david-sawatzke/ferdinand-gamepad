// Debug
#define debug

#ifdef debug
#include "DigiKeyboard.h"
#else
#include "DigiJoystick.h"
#endif
static const uint8_t sensors[] = {2, 0, 5};
static const uint8_t pingOut = 1;

static const uint32_t calibrationIn[sizeof(sensors)][2] = {
	{200, 30000},
	{200, 30000},
	{200, 30000}
};
static const int8_t calibrationOut[sizeof(sensors)][2] = {
	{-128, 127},
	{0, 127},
	{0, -128}
};

uint8_t data[8] = {128, 128, 128, 128, 128, 128, 0, 0};
enum dataPos {
	x = 0,
	y = 1,
	xrot = 2,
	yrot = 3,
	zrot = 4,
	slider = 5,
	buttonLowByte = 6,
	buttonHighByte = 7
};

void setup() {
	// Do nothing? It seems as if the USB hardware is ready to go on reset
	pinMode(pingOut, OUTPUT);
	for (uint8_t i = 0; i < sizeof(sensors); i++) {
		pinMode(sensors[i], INPUT);
	}
}

int8_t readSensor(uint8_t id) {
	DigiJoystick.delay(10);
	uint32_t rawData;
	digitalWrite(pingOut, HIGH);
	delayMicroseconds(50);
	digitalWrite(pingOut, LOW);
	rawData = pulseIn(sensors[id], HIGH, 40000);
	return map(rawData, calibrationIn[id][0], calibrationIn[id][1], calibrationOut[id][0], calibrationOut[id][1]);
}


void loop() {
	uint8_t x = (uint8_t) readSensor(0);
	int8_t y1 = (int8_t)(readSensor(1));
	int8_t y2 = (int8_t)(readSensor(2));
	#ifdef debug
	DigiKeyboard.print("\n");
	DigiKeyboard.print(x);
	DigiKeyboard.print(" ");
	DigiKeyboard.print(y1);
	DigiKeyboard.print(" ");
	DigiKeyboard.print(y2);
	DigiKeyboard.delay(500); // wait 50 milliseconds
	#else
	DigiJoystick.setX(x);
	DigiJoystick.setY(y1 + y2);
	DigiJoystick.delay(50); // wait 50 milliseconds
	#endif

	// If not using plentiful DigiJoystick.delay() calls, make sure to
	//DigiJoystick.update(); // call this at least every 50ms
	// calling more often than that is fine
	// this will actually only send the data every once in a while unless the data is different

	// you can set the values from a raw byte array with:
	// char myBuf[8] = {
	//   x, y, xrot, yrot, zrot, slider,
	//   buttonLowByte, buttonHighByte
	// };
	// DigiJoystick.setValues(myBuf);

	// Or we can also set values like this:
	//DigiJoystick.setX((byte) (millis() / 100)); // scroll X left to right repeatedly
	//DigiJoystick.setY((byte) 0x30);
	//DigiJoystick.setXROT((byte) 0x60);
	//DigiJoystick.setYROT((byte) 0x90);
	//DigiJoystick.setZROT((byte) 0xB0);
	//DigiJoystick.setSLIDER((byte) 0xF0);

	// it's best to use DigiJoystick.delay() because it knows how to talk to
	// the connected computer - otherwise the USB link can crash with the
	// regular arduino delay() function

	// we can also set buttons like this (lowByte, highByte)
	//DigiJoystick.setButtons(0x00, 0x00);
}
