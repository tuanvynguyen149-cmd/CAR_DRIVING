#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Định nghĩa chân LED (Chân 2 là LED tích hợp trên hầu hết kit ESP32 Dev)
#define LED_PIN 2

// UUID cho Service và Characteristic (phải khớp với bên Website)
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

// Callback xử lý kết nối
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println(">> Đã có thiết bị kết nối!");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println(">> Thiết bị đã ngắt kết nối. Đang phát lại...");
      // Phát lại tín hiệu quảng bá để kết nối lại
      BLEDevice::startAdvertising();
    }
};

// Callback xử lý dữ liệu nhận được từ Web
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        char cmd = value[0];
        Serial.print("Dữ liệu nhận được: ");
        Serial.println(cmd);

        if (cmd == '1') {
          digitalWrite(LED_PIN, HIGH);
          Serial.println("LED: BẬT");
        } else if (cmd == '0') {
          digitalWrite(LED_PIN, LOW);
          Serial.println("LED: TẮT");
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Khởi tạo BLE
  BLEDevice::init("ESP32-Web-BLE");

  // Tạo Server & gán Callback
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Tạo Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Tạo Characteristic cho phép Đọc/Ghi
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  // Bắt đầu quảng bá BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("ESP32 BLE sẵn sàng kết nối!");
}

void loop() {
  delay(1000);
}