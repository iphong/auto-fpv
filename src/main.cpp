#include "Arduino.h"
#include "crossfire.h"
#include "bluetooth.h"

#define RSSI crsf_telemetry_get_sensor(CRSF_RX_QUALITY).value
#define VBAT crsf_telemetry_get_sensor(CRSF_BATT_VOLTAGE).value
#define ROLL crsf_telemetry_get_sensor(CRSF_ATTITUDE_ROLL).value

void setup() {
	Serial.begin(115200);
	bluetooth_start();
}

void loop() {
	Serial.printf("%3.0f%%   %5GV  %4G\n", RSSI, VBAT, ROLL);
	delay(100);
}