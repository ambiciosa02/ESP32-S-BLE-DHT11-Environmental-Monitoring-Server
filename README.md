# ESP32-S-BLE-DHT11-Environmental-Monitoring-Server

This project configures an ESP32-S module as a Bluetooth Low Energy (BLE) GATT server named ESP32_DHT11_Server. It reads environmental metrics (temperature and humidity) from a DHT11 sensor connected to GPIO 4, updates custom GATT characteristics, and streams real-time push notifications over BLE to mobile client applications such as nRF Connect.

## 1. Bootloader Setup & DHT11 Initialization Log
Upon powering up, the ESP32-S initializes system registers, launches the BLE stack, starts the DHT11 sensor interface, and prints real-time sensor readings to the USB UART Serial Monitor at 115200 baud.
<br><br>
<img width="650" height="400" alt="image" src="https://github.com/user-attachments/assets/ab522a7a-f819-45e6-b305-3a1f1d98f26e" /><img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/1b8611ec-3e40-43b1-a115-f6c1a41891c3" /><br><br>
## 2. Peripheral Discovery in nRF Connect
The mobile scanner app detects the ESP32-S advertising packets in the surrounding BLE environment.
<img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/16cd559a-d57f-4817-b116-8c6857959cc0" />
<br><br>
## 3. Advertising Payload Inspection
Expanding the device card exposes full BLE advertising metadata:
<img width="400" height="400" alt="image" src="https://github.com/user-attachments/assets/d4c5ff42-54ae-447d-a46c-28721809a54d" /><br><br>
## 4. Signal Strength & RSSI Tracking
The historical RSSI graph demonstrates strong signal stability during proximity testing:
<img width="400" height="600" alt="image" src="https://github.com/user-attachments/assets/8d0ad40f-45e7-43d5-b88c-e5f97f3dfe84" /><br><br>
## 5. GATT Service Structure & CCCD Descriptors
Once connected, nRF Connect discovers the GATT table hierarchy. Adding BLE2902 descriptors enables Client Characteristic Configuration Descriptors (CCCD 0x2902), allowing clients to subscribe to real-time notification streams.
<img width="400" height="600" alt="image" src="https://github.com/user-attachments/assets/e1e05099-b2ab-4ee0-99ac-e3e78e37f26c" /><br><br>
## 6. Live Push Notification Stream Log
With notifications enabled, the ESP32 pushes incoming sensor readings directly to the mobile client every 2 seconds via .notify(), eliminating the need to manually click the read button.
<img width="500" height="400" alt="image" src="https://github.com/user-attachments/assets/67d895cb-0d20-4a62-ab60-c1cb33a48f24" /><br><br>
## Project Conclusion
This project successfully demonstrates the deployment of an ESP32-S microcontroller as a Bluetooth Low Energy (BLE) GATT server for real-time environmental monitoring. By integrating the BLE2902 Client Characteristic Configuration Descriptors (CCCD) alongside the custom GATT service architecture, the system achieves seamless, event-driven data streaming to external mobile clients.

### Key Achievements
Automated Data Streaming: Pushed live temperature and humidity updates directly to mobile clients using BLE notification properties, eliminating manual read operations.

Standard GATT Architecture: Successfully implemented primary custom Service and Characteristic UUIDs with read, write, and notification permissions.

Robust Connectivity: Maintained high connection reliability and fast advertising response intervals under continuous proximity testing.
