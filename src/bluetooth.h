//
// Created by Phong Vu on 1/23/20.
//

#ifndef GPS_TRACKER_BLUETOOTH_CPP
#define GPS_TRACKER_BLUETOOTH_CPP

#include <Arduino.h>
#include <BLEDevice.h>
#include <crossfire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define serviceUUID BLEUUID("FFF0")
#define charUUID BLEUUID("FFF6")

BLEAdvertisedDevice *myDevice;

bool shouldScan;
bool shouldConnect;
bool connected;

void telemetryCallback(BLERemoteCharacteristic *characteristic, uint8_t *data, size_t size, bool isNotify) {
	crsf_telemetry_push_byte(data, size);
}

class onDiscovered : public BLEAdvertisedDeviceCallbacks {
	void onResult(BLEAdvertisedDevice device) override {
//		Serial.printf("-- Found device --> %s\n", device.toString().c_str());
		if (device.haveName() && device.getName() == "Hello") {
			BLEDevice::getScan()->stop();
			myDevice = new BLEAdvertisedDevice(device);
			shouldConnect = true;
			shouldScan = true;
		}
	}
};

class MyClientCallback : public BLEClientCallbacks {
	void onConnect(BLEClient *client) override { }

	void onDisconnect(BLEClient *client) override {
		if (connected) {
			BLEDevice::getScan()->clearResults();
		}
		connected = false;
	}
};

void connect() {
	BLEClient *client = BLEDevice::createClient();
	client->setClientCallbacks(new MyClientCallback());
	client->connect(myDevice);

	BLERemoteService *service = client->getService(serviceUUID);
	if (service == nullptr) {
		client->disconnect();
		return;
	}
	BLERemoteCharacteristic *characteristic = service->getCharacteristic(charUUID);
	if (characteristic == nullptr) {
		client->disconnect();
		return;
	}
	if (characteristic->canNotify()) {
		characteristic->registerForNotify(telemetryCallback);
		connected = true;
	}
}


void bluetooth_loop(void *) {
	while (true) {
		if (shouldConnect) {
			connect();
			shouldConnect = false;
		} else if (!connected && shouldScan) {
			BLEDevice::getScan()->start(0);
			shouldScan = false;
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

void bluetooth_start() {
	BLEDevice::init("FPV-GPS-TRACKER");
	BLEScan *pBLEScan = BLEDevice::getScan();
	pBLEScan->setAdvertisedDeviceCallbacks(new onDiscovered());
	pBLEScan->setActiveScan(true);
	pBLEScan->start(5, false);

	xTaskCreatePinnedToCore(bluetooth_loop, "Task Loop", 2048, nullptr, 5, nullptr, 1);
}

#endif //GPS_TRACKER_BLUETOOTH_CPP
