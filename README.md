# IoT-Enabled Obstacle Detection and Avoidance Robot 🚗📡  
**Design and Implementation Using ESP32 + MQTT Cloud Monitoring**

## 📌 Overview
This project demonstrates an indoor autonomous obstacle avoidance robot that combines:
- **Local real-time decision making** (fast navigation logic)
- **Cloud-based telemetry monitoring** using **MQTT over Wi-Fi**

It is designed to be **low-cost, modular, and suitable for constrained indoor environments** such as labs, warehouses, smart campuses, and industrial facilities.

## 🎯 Problem Statement
Traditional obstacle avoidance robots rely only on local sensing and control, which limits:
- remote feedback
- diagnostics
- system visibility and scalability

This project solves that by enabling **autonomous navigation + real-time telemetry monitoring** without sacrificing local performance.

## ✅ Objectives
- Design an obstacle avoidance robot using **ESP32**
- Integrate **Ultrasonic (HC-SR04)** and **IR sensors** for multi-direction sensing
- Implement **rule-based navigation logic**
- Publish real-time telemetry via **MQTT**
- Demonstrate the system using **Wokwi simulation + MQTT Explorer**

## 🧩 Hardware & Tools
### Hardware (Simulated in Wokwi)
- ESP32 DevKit v4
- Ultrasonic Sensor (HC-SR04)
- Five IR sensors *(simulated using slide switches)*
- LEDs *(simulate motor directions)*
- Buzzer *(turning/reversing alert)*

### Software / Platforms
- Arduino IDE
- Wokwi Simulation
- MQTT Protocol
- MQTT Explorer

## 🧠 System Architecture
The system follows a layered design:

1. **Sensor Layer**: Ultrasonic + IR sensors  
2. **Processing Layer**: ESP32 microcontroller  
3. **Decision Layer**: Rule-based obstacle avoidance logic  
4. **Actuation Layer**: LEDs + buzzer (motor simulation)  
5. **Communication Layer**: Wi-Fi + MQTT cloud telemetry  

## 🔌 ESP32 Pin Mapping
| Component | Signal | ESP32 Pin | Purpose |
|----------|--------|----------|---------|
| Ultrasonic Sensor | TRIG | GPIO 5 | Trigger distance measurement |
| Ultrasonic Sensor | ECHO | GPIO 18 | Receive echo pulse |
| IR Sensor 1 (Left) | OUT | GPIO 32 | Left obstacle detection |
| IR Sensor 2 (Left-Center) | OUT | GPIO 33 | Left-center obstacle detection |
| IR Sensor 3 (Center) | OUT | GPIO 34 | Front obstacle detection |
| IR Sensor 4 (Right-Center) | OUT | GPIO 35 | Right-center obstacle detection |
| IR Sensor 5 (Right) | OUT | GPIO 39 (VN) | Right obstacle detection |
| Forward LED | Anode | GPIO 25 | Forward indication |
| Left LED | Anode | GPIO 27 | Left indication |
| Right LED | Anode | GPIO 14 | Right indication |
| Reverse LED | Anode | GPIO 26 | Reverse indication |
| Buzzer | Signal | GPIO 4 | Audible alert |
| Wi-Fi Status LED | Anode | GPIO 2 | Network status |

## 🤖 Navigation Logic (Rule-Based)
The robot continuously evaluates sensor inputs:

- ✅ No obstacle → **Move Forward**
- ✅ Left obstacle → **Turn Right**
- ✅ Right obstacle → **Turn Left**
- ✅ Obstacles on all sides → **Reverse**
- 🔊 Buzzer activates during **turning and reversing**

## ☁️ MQTT Cloud Telemetry
### Why MQTT?
MQTT was chosen over HTTP due to:
- lightweight publish/subscribe model
- low bandwidth usage
- minimal latency
- excellent fit for IoT devices

### Broker + Topic
- Broker: `test.mosquitto.org`
- Topic: `robots/leroy/telemetry`

Telemetry is published in **JSON format** for easy visualization using MQTT Explorer.

## 🧪 Simulation
This project was simulated in **Wokwi** to validate functionality without physical hardware.

📌 Wokwi Link: **(ADD YOUR LINK HERE)**

## ⚠️ Limitations
- Motors are simulated using LEDs (no motor driver hardware)
- IR sensors are abstracted using switches for controlled testing
- Camera vision / GPS not included (indoor use case focus)

## 🚀 Future Improvements
- Add real DC motors + motor driver
- Bluetooth manual control mode
- Web dashboard (Node-RED)
- GPS support for outdoor navigation
- TLS encryption + MQTT authentication

## 📚 References
- Banks, A., & Gupta, R. (2014). *MQTT Version 3.1.1. OASIS Standard.*
- Espressif Systems. (2023). *ESP32 Series Datasheet.*
- Wokwi. (2024). *Wokwi Arduino and ESP32 Simulator.*
- MQTT.org. (2024). *MQTT Essentials.*
