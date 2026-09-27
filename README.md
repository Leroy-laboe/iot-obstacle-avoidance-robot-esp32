# ESP32 IoT Obstacle-Avoidance Robot

**A Wokwi-simulated ESP32 robotics prototype that combines multi-direction obstacle sensing, rule-based motion decisions, and MQTT telemetry.**

[Open Wokwi Simulation](https://wokwi.com/projects/451565989824641025) · [GitHub Profile](https://github.com/Leroy-laboe) · [LinkedIn](https://www.linkedin.com/in/leroy-nyasha-mangwarara-86185a302/)

> **Simulation note:** this repository demonstrates the control logic in Wokwi. The IR sensors are represented by slide switches and the robot's motion outputs are represented by LEDs rather than physical motors.

---

## Overview

The project models an indoor obstacle-avoidance robot around an **ESP32 DevKit v4**.

The ESP32 continuously reads:

- one HC-SR04 ultrasonic distance sensor;
- five directional obstacle inputs;
- Wi-Fi signal strength.

It then applies a rule-based decision system to choose one of four movement states:

- move forward;
- turn left;
- turn right;
- reverse.

The selected state is reflected through LEDs and a buzzer in the simulation, while telemetry is published over MQTT as JSON.

---

## System Flow

```text
HC-SR04 + 5 obstacle inputs
             │
             ▼
         ESP32 loop
             │
             ▼
   Evaluate blocked regions
             │
             ▼
     Navigation decision
             │
      ┌──────┼───────┐
      ▼      ▼       ▼
  Forward   Turn   Reverse
      │      │       │
      └──────┴───────┘
             │
             ▼
 LEDs + buzzer simulation
             │
             ▼
      JSON MQTT telemetry
             │
             ▼
     test.mosquitto.org
```

The decision cycle runs approximately every **200 ms**.

---

## Navigation Logic

The firmware groups the sensor inputs into three logical regions:

- **front** — ultrasonic distance or centre obstacle input;
- **left** — far-left or left input;
- **right** — right or far-right input.

The obstacle threshold for the ultrasonic sensor is **20 cm**.

| Sensor Condition | Robot State |
| --- | --- |
| Path clear | Move forward |
| Front + left blocked | Turn right |
| Front + right blocked | Turn left |
| Front blocked, sides clear | Turn left |
| Left blocked | Turn right |
| Right blocked | Turn left |
| Front + left + right blocked | Reverse |

The buzzer is activated while turning or reversing.

---

## Hardware Represented in Wokwi

| Component | Simulation Role |
| --- | --- |
| ESP32 DevKit v4 | Main controller |
| HC-SR04 | Front distance sensing |
| 5 slide switches | Directional IR-sensor simulation |
| 4 LEDs | Forward / left / right / reverse motion outputs |
| 1 LED | Wi-Fi connection status |
| Buzzer | Turn / reverse alert |

The simulation intentionally abstracts the motor driver and physical drivetrain so the project can focus on sensing, decision logic, and IoT telemetry.

---

## ESP32 Pin Mapping

| Component | Signal | ESP32 Pin |
| --- | --- | ---: |
| HC-SR04 | TRIG | GPIO 5 |
| HC-SR04 | ECHO | GPIO 18 |
| Far-left obstacle input | OUT | GPIO 32 |
| Left obstacle input | OUT | GPIO 33 |
| Centre obstacle input | OUT | GPIO 34 |
| Right obstacle input | OUT | GPIO 35 |
| Far-right obstacle input | OUT | GPIO 39 / VN |
| Forward LED | Output | GPIO 25 |
| Left LED | Output | GPIO 27 |
| Right LED | Output | GPIO 14 |
| Reverse LED | Output | GPIO 26 |
| Buzzer | Output | GPIO 4 |
| Wi-Fi status LED | Output | GPIO 2 |

---

## MQTT Telemetry

The project publishes telemetry to a public MQTT broker for demonstration.

| Setting | Value |
| --- | --- |
| Broker | `test.mosquitto.org` |
| Port | `1883` |
| Topic | `robots/leroy/telemetry` |
| Format | JSON |

Example payload:

```json
{
  "id": "leroy",
  "ts": 12345,
  "ultra_cm": 30,
  "ir": [0, 0, 1, 0, 0],
  "state": "LEFT",
  "rssi": -65
}
```

### Payload fields

- `id` — robot identifier;
- `ts` — ESP32 uptime in milliseconds;
- `ultra_cm` — ultrasonic distance in centimetres;
- `ir` — five directional obstacle states;
- `state` — current navigation state;
- `rssi` — Wi-Fi signal strength.

The firmware also uses a non-blocking MQTT reconnection interval so a broker outage does not stop the navigation decision loop.

> **Security note:** `test.mosquitto.org` is a public broker and port 1883 is unencrypted. It is appropriate for a classroom/demo simulation, not for production telemetry or sensitive data.

---

## Firmware Structure

The Arduino sketch is organised around a small finite-state control model:

```text
STOP
MOVE_FORWARD
TURN_LEFT
TURN_RIGHT
REVERSE
```

Core functions include:

- `readUltrasonicCM()` — measures front distance;
- `applyState()` — applies the selected robot state;
- `setMotionLEDs()` — visualises movement outputs;
- `publishTelemetry()` — builds and publishes JSON telemetry;
- `mqttEnsureConnectedNonBlocking()` — retries MQTT connection without blocking the main control loop.

---

## Run the Simulation

### Option 1 — Wokwi

Open the referenced simulation:

**https://wokwi.com/projects/451565989824641025**

Use the slide switches to simulate directional obstacle detections and change the HC-SR04 distance to test front-obstacle behaviour.

### Option 2 — Local Arduino workflow

The firmware depends on:

- ESP32 Arduino core;
- `PubSubClient`.

The required library is also listed in `libraries.txt`.

---

## Repository Structure

```text
.
├── sketch.ino          # ESP32 firmware
├── diagram.json        # Wokwi circuit layout
├── libraries.txt       # Wokwi/Arduino library dependency
├── wokwi-project.txt   # Source Wokwi project reference
├── .gitignore
└── README.md
```

---

## What This Project Demonstrates

- ESP32 firmware development;
- ultrasonic sensing;
- multi-direction obstacle logic;
- finite-state control;
- non-blocking timing with `millis()`;
- Wi-Fi connectivity;
- MQTT telemetry publishing;
- JSON IoT telemetry;
- simulation-driven embedded-system testing.

---

## Limitations

This is a simulation-focused prototype.

Current limitations include:

- motion is represented by LEDs rather than DC motors;
- IR sensors are abstracted as manual slide switches;
- no motor driver or drivetrain is modelled;
- Wi-Fi setup blocks during the initial connection;
- MQTT uses an unauthenticated public broker;
- no TLS encryption;
- no battery or power-management logic;
- no camera, SLAM, or localisation.

---

## Possible Extensions

- integrate physical DC motors and a motor driver;
- replace switches with actual IR or ToF sensors;
- add manual Bluetooth control;
- add authenticated MQTT over TLS;
- build a telemetry dashboard;
- add persistent telemetry storage;
- add local recovery strategies for trapped states;
- extend from rule-based avoidance toward mapping / localisation.

---

## Maintainer

**Leroy Nyasha Mangwarara**

Computer Science · Data Science · Software Engineering · IoT

[GitHub](https://github.com/Leroy-laboe) · [LinkedIn](https://www.linkedin.com/in/leroy-nyasha-mangwarara-86185a302/) · [Email](mailto:mangwararaleroy@gmail.com)
