#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h> // Required for Client Characteristic Configuration Descriptor (0x2902)
#include <Bonezegei_DHT11.h>

// DHT11 Configuration (Data pin on GPIO 4)
#define DHTPIN 4
Bonezegei_DHT11 dht(DHTPIN);

// Unique UUIDs for Service and Characteristics
#define SERVICE_UUID               "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define TEMP_CHARACTERISTIC_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define HUM_CHARACTERISTIC_UUID    "a2d04a3f-1234-4b82-9a08-3012a951c890"

BLECharacteristic *pTempCharacteristic;
BLECharacteristic *pHumCharacteristic;

// Callback class for handling incoming WRITE commands from phone
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
      String value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.print("Received from phone: ");
        Serial.println(value);
      }
    }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 BLE + DHT11 Server...");

  // Initialize DHT11 sensor
  dht.begin();

  // Initialize BLE Device
  BLEDevice::init("ESP32_DHT11_Server");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Temperature Characteristic (Read + Write + Notify)
  pTempCharacteristic = pService->createCharacteristic(
                          TEMP_CHARACTERISTIC_UUID,
                          BLECharacteristic::PROPERTY_READ |
                          BLECharacteristic::PROPERTY_WRITE |
                          BLECharacteristic::PROPERTY_NOTIFY
                        );
  pTempCharacteristic->addDescriptor(new BLE2902()); // Adds 0x2902 descriptor for NOTIFY support

  // Humidity Characteristic (Read + Notify)
  pHumCharacteristic = pService->createCharacteristic(
                         HUM_CHARACTERISTIC_UUID,
                         BLECharacteristic::PROPERTY_READ |
                         BLECharacteristic::PROPERTY_NOTIFY
                       );
  pHumCharacteristic->addDescriptor(new BLE2902()); // Adds 0x2902 descriptor for NOTIFY support

  pTempCharacteristic->setCallbacks(new MyCallbacks());
  pTempCharacteristic->setValue("Temp: Initializing");
  pHumCharacteristic->setValue("Hum: Initializing");

  pService->start();

  // Configure Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE Ready! Connect via nRF Connect or LightBlue.");
}

void loop() {
  // Trigger sensor acquisition
  dht.getData();

  // Retrieve temperature (°C) and relative humidity (%)
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();

  // Convert float values to formatted Strings
  String tempStr = String(temperature, 1) + " C";
  String humStr = String(humidity, 1) + " %";

  // Update BLE Characteristic values
  pTempCharacteristic->setValue(tempStr.c_str());
  pHumCharacteristic->setValue(humStr.c_str());

  // Push notifications to subscribed clients automatically
  pTempCharacteristic->notify();
  pHumCharacteristic->notify();

  // Output readings to Serial Monitor
  Serial.printf("Temp: %.1f °C | Humidity: %.1f %%\n", temperature, humidity);

  delay(2000); // 2-second sampling delay
}
