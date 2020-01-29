#include "Arduino.h"
#include "Servo.h"
#include "crossfire.h"
#include "bluetooth.h"

#define RSSI crsf_telemetry_get_sensor(CRSF_RX_QUALITY).value
#define VBAT crsf_telemetry_get_sensor(CRSF_BATT_VOLTAGE).value
#define ROLL crsf_telemetry_get_sensor(CRSF_ATTITUDE_ROLL).value

Servo s1;

void setup() {
	Serial.begin(115200);
	bluetooth_start();
	s1.attach(13, -1, -90, 90, 500, 2500);
	s1.write(0);
}
uint32_t tmr;

void loop() {
	// TODO: Detach run task and recalculate angles
	if (millis() > tmr) {
		int deg = (int)(ROLL * 180 / PI);
		if (deg < 0) deg += 360;
		if (deg > 360) deg -= 360;
		if (deg > 180) deg = (360 - deg) * -1;
		Serial.printf("%3.0f%%   %5GV  %10G  %10i\n", RSSI, VBAT, ROLL, deg);
		s1.write(abs(deg) < 45 ? 0 : deg < 0 ? -90 : 90);
		tmr = millis() + 200;
	}
}