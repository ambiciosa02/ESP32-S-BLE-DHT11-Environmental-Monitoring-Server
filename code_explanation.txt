Here is a detailed, section-by-section breakdown of how the ESP32 BLE + DHT11 code operates:

---

### 1. Library Includes & Hardware Definitions

```cpp
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Bonezegei_DHT11.h>

#define DHTPIN 4
Bonezegei_DHT11 dht(DHTPIN);

```

* **BLE Headers:** Load the ESP32 BLE stack to manage GATT server setup, device initialization, characteristic attributes, and client configuration descriptors.
* **DHT Library & Pin Setup:** Imports the `Bonezegei_DHT11` library and initializes an instance connected to **GPIO 4**.

---

### 2. UUID Definitions & Global Pointers

```cpp
#define SERVICE_UUID               "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define TEMP_CHARACTERISTIC_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define HUM_CHARACTERISTIC_UUID    "a2d04a3f-1234-4b82-9a08-3012a951c890"

BLECharacteristic *pTempCharacteristic;
BLECharacteristic *pHumCharacteristic;

```

* **128-bit UUIDs:** Uniquely identify the custom **Environmental Sensing Service** and its two data attributes (**Temperature** and **Humidity**).
* **Global Characteristic Pointers:** Allow the program to update values and emit notifications from anywhere in the script (specifically inside `loop()`).

---

### 3. Write Callback Handler

```cpp
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
      String value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.print("Received from phone: ");
        Serial.println(value);
      }
    }
};

```

* Subclasses `BLECharacteristicCallbacks` to listen for incoming data when a connected client (like nRF Connect) sends a **WRITE** command to the Temperature characteristic.
* Intercepts the raw bytes, converts them into a string, and prints them to the Serial Monitor.

---

### 4. GATT Server Setup & Configuration (`setup()`)

```cpp
dht.begin();
BLEDevice::init("ESP32_DHT11_Server");
BLEServer *pServer = BLEDevice::createServer();
BLEService *pService = pServer->createService(SERVICE_UUID);

```

* Initializes the DHT11 hardware interface.
* Initializes the BLE controller and sets the broadcast local name to `ESP32_DHT11_Server`.
* Creates a GATT Server instance and registers the primary service using `SERVICE_UUID`.

```cpp
pTempCharacteristic = pService->createCharacteristic(
                        TEMP_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
pTempCharacteristic->addDescriptor(new BLE2902());

pHumCharacteristic = pService->createCharacteristic(
                       HUM_CHARACTERISTIC_UUID,
                       BLECharacteristic::PROPERTY_READ |
                       BLECharacteristic::PROPERTY_NOTIFY
                     );
pHumCharacteristic->addDescriptor(new BLE2902());

```

* **Characteristic Creation:** Configures access permissions for each data point:
* **Temperature:** Supports `READ`, `WRITE`, and `NOTIFY`.
* **Humidity:** Supports `READ` and `NOTIFY`.


* **`BLE2902` Descriptors:** Registers Client Characteristic Configuration Descriptors (CCCD `0x2902`) on both characteristics. This allows client apps to toggle subscriptions and receive automatic push notifications without polling.

```cpp
pTempCharacteristic->setCallbacks(new MyCallbacks());
pTempCharacteristic->setValue("Temp: Initializing");
pHumCharacteristic->setValue("Hum: Initializing");

pService->start();

```

* Attaches the `MyCallbacks` instance to handle write operations on the Temperature attribute.
* Sets placeholder buffer strings and starts the GATT service.

```cpp
BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
pAdvertising->addServiceUUID(SERVICE_UUID);
pAdvertising->setScanResponse(true);
pAdvertising->setMinPreferred(0x06);
pAdvertising->setMinPreferred(0x12);
BLEDevice::startAdvertising();

```

* Includes `SERVICE_UUID` in the advertising payload so scanning apps can filter and locate the device by its service.
* Configures connection interval preferences and begins broadcasting BLE discovery packets.

---

### 5. Sensor Sampling & Notification Loop (`loop()`)

```cpp
dht.getData();
float temperature = dht.getTemperature();
float humidity = dht.getHumidity();

String tempStr = String(temperature, 1) + " C";
String humStr = String(humidity, 1) + " %";

```

* Triggers a sensor reading from the DHT11 every 2 seconds.
* Extracts float values and formats them as standard string representations (e.g., `"27.1 C"` and `"69.0 %"`).

```cpp
pTempCharacteristic->setValue(tempStr.c_str());
pHumCharacteristic->setValue(humStr.c_str());

pTempCharacteristic->notify();
pHumCharacteristic->notify();

```

* **`.setValue()`:** Updates the internal GATT characteristic buffers on the ESP32.
* **`.notify()`:** Transmits an active push notification containing the updated values to all subscribed BLE clients over the active connection link.

```cpp
Serial.printf("Temp: %.1f °C | Humidity: %.1f %%\n", temperature, humidity);
delay(2000);

```

* Outputs formatted telemetry to the UART Serial Monitor and pauses execution for 2 seconds to match the DHT11 refresh rate limit.
