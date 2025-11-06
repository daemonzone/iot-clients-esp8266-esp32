# 🌐 IoT Client for esp8266 / esp32

## ⚙️ Overview

This project combines firmware and software for microcontrollers, backend services, and monitoring tools.

### 🧠 Wemos / esp8266 / esp32 Clients
 - For IoT nodes based on **ESP8266 / ESP32** microcontrollers (i.e Wemos D1 mini)
 - Each device has its own unique identifier
 - Each device announces itself on a MQTT queue, being identified by the monitoring service.
 - Each device periodically publishes telemetry and status (e.g. temperature, humidity, status) to a per-device MQTT queue


### Requirements / Tech Stack
- **C++ (esp8266 / esp32)**
- Arduino IDE

---

## 📁 Repositories

The whole project is based on different components, split on several repositories

| Component                                                                            | Description                                                              |
|--------------------------------------------------------------------------------------|--------------------------------------------------------------------------|
| [iot-monitor-service](https://github.com/daemonzone/iot-monitor-service)             | Node.js **monitoring and control service**, interacting with the backend |  
| [iot-clients-esp8266-esp32](https://github.com/daemonzone/iot-clients-esp8266-esp32) | esp8266 / esp32 **C++ client sketches**                          |
| [iot-clients-node](https://github.com/daemonzone/iot-clients-node)                   | **Node.js clients** for newer Raspberry/OrangePi devices                 | 
| [iot-clients-node-legacy](https://github.com/daemonzone/iot-clients-node-legacy)     | Node.js clients for **older devices** (Armbian v3, mqtt4)                | 
| [iot-monitor-api](https://github.com/daemonzone/iot-monitor-api)                     | **API interface** to retrieve devices and telemetry from TimescaleDB     |
| [iot-monitor-dashboard](https://github.com/daemonzone/iot-monitor-dashboard)         | React **Web Dashboard** for device charts                                |


### 🛰️ IoT Monitor Service

A **Node.js service** responsible for:
- Identifying devices connected and allowed to send telemetry data
- Receiving telemetry from devices
- Persisting data into TimescaleDB
- Initializing database tables and hypertables
- _Sending commands to devices (wip)_

### 💻 NodeJs Clients
Node.js based client code available for micro-computers (like Raspberry or Orange Pi)

### 🕹️ NodeJs Clients (legacy)
Node.js v10 based client code available for older micro-computers (running Armbian v3.4.113)

### 🌐 IoT Monitor API
A **Node.js service** responsible for:
- Collecting and storing device telemetry into TimescaleDB
- Exposing REST endpoints to query device data and status

### 🖥️ IoT Monitor Dashboard
A **React Web Interface** to:
- Visualize real-time device status and historical telemetry
- Display charts for temperature, humidity, uptime, and other metrics
- Filter, sort, and explore devices and their readings

---

## 🧰 Tech Stack

| Component | Technology             | Provider (with free plan) |
|------------|------------------------|---------------------------|
| Firmware | C++ (Arduino, ESP-IDF) | -                         |
| Backend / Monitor | Node.js                | -                         |
| Database | PostgreSQL + TimescaleDB | [Neon](https://neon.com/) | 
| Messaging | MQTT / Mosquitto      | [HiveMQ](https://www.hivemq.com/) |
| Dashboard | Node.js client or web app | -                         |

---

## 🚀 Getting Started

1. Clone the repository
   ```bash
   git clone https://github.com/daemonzone/iot-clients-esp8266-esp32.git
   cd iot-clients-esp8266-esp32

2. Open the **iot-clients-esp8266-esp32** sketch on **Arduino IDE**


3. Configure the environment

   Configure the connection parameters (WiFi, MQTT) within the sketch code at lines 11-18

   ```
   // WiFi credentials
   const char* ssid = "<WIFI NAME>";
   const char* password = "<WIFI PASSWORD>";
   
   // MQTT Cluster info
   const char* mqtt_server = "<YOUR_MQTT_URL>";
   const char* mqtt_user = "<YOUR_MQTT_USER>";
   const char* mqtt_pass = "<YOUR_MQTT_PASS>";
   ```

   Set Device Name at line: 164
   ```
   reg["model"] = "Wemos D1 Mini";
   ```

4. Flash the compiled sketch to the device (i.e. Wemos D1 mini)

   ### 🚀 Console output (with terminal set to 115200 baud)

   ```
   ...
   WiFi connected
   IP address: 10.94.176.231
   Waiting for NTP time sync: .
   CET Thu Nov  6 19:31:09 2025
   Attempting MQTT connection...connected
   Connected to devices/wemos-46fea4/cmd
   [wemos-46fea4] Registration sent successfully
   [wemos-46fea4] Up status sent
   ...
   ```
---

## 🧑‍💻 Author

**Davide V.**

IoT enthusiast and full-stack developer

📍 Italy  
📫 **GitHub:** [@daemonzone](https://github.com/daemonzone)  
📧 **Email:** daemonzone@users.noreply.github.com
