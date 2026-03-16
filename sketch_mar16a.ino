// connect and receive message with bluetooth from android

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Device connected");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Device disconnected");
      BLEDevice::startAdvertising(); // Restart advertising
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue();
      
      if (rxValue.length() > 0) {
          // ✅ METHOD 1: Direct c_str() with length (SIMPLEST)
            Serial.write((uint8_t*)rxValue.c_str(), rxValue.length());
            Serial.println();
            
            // ✅ METHOD 2: Manual null-terminated copy (SAFE)
            char buffer[128] = {0};  // Zero-initialized
            size_t copyLen = rxValue.length() > 127 ? 127 : rxValue.length();
            
            // Copy bytes safely
            memcpy(buffer, rxValue.c_str(), copyLen);
            buffer[copyLen] = '\0';
            
            Serial.print("✅ Clean: '");
            Serial.print(buffer);
            Serial.print("' (");
            Serial.print(strlen(buffer));
            Serial.println(" chars)");
            
            // Process command
            String command(buffer);
            command.trim();
        if (command == "LED ON") {
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println("LED ON");
        } else if (command == "LED OFF") {
          digitalWrite(LED_BUILTIN, LOW);
          Serial.println("LED OFF");
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  BLEDevice::init("ESP32_BLE");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_WRITE |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
  Serial.println("Waiting for connection...");
}

void loop() {
  delay(1000);
}
