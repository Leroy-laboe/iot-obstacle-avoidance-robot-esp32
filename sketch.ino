/****************************************************
 * IoT-Enabled Obstacle Detection & Avoidance Robot
 * Robot ID: LEROY
 *
 * Platform: ESP32 (Wokwi Simulation)
 *
 * MQTT:
 *   Broker : test.mosquitto.org
 *   Port   : 1883
 *   Topic  : robots/leroy/telemetry
 *
 * Telemetry JSON example:
 * {"id":"leroy","ts":12345,"ultra_cm":30,"ir":[0,0,1,0,0],"state":"LEFT","rssi":-65}
 ****************************************************/

#include <WiFi.h>
#include <PubSubClient.h>

/* ================= WIFI ================= */
const char* ssid = "Wokwi-GUEST";
const char* password = "";

/* ================= MQTT ================= */
const char* MQTT_BROKER = "test.mosquitto.org";
const int   MQTT_PORT   = 1883;
const char* MQTT_TOPIC  = "robots/leroy/telemetry";

/* ================= PINS ================= */
// Ultrasonic
#define TRIG_PIN 5
#define ECHO_PIN 18

// IR Sensors (slide switches)
#define IR1_PIN 32   // Far Left
#define IR2_PIN 33   // Left
#define IR3_PIN 34   // Center
#define IR4_PIN 35   // Right
#define IR5_PIN 39   // Far Right (VN)

// LEDs (motor simulation)
#define LED_FWD   25
#define LED_LEFT  27
#define LED_RIGHT 14
#define LED_REV   26

// Status + Buzzer
#define LED_WIFI  2
#define BUZZER_PIN 4

/* ================= PARAMETERS ================= */
const int OBSTACLE_DISTANCE_CM = 20;
const unsigned long DECISION_INTERVAL_MS = 200;
const unsigned long MQTT_RETRY_MS = 3000;

/* ================= GLOBALS ================= */
WiFiClient espClient;
PubSubClient mqtt(espClient);

unsigned long lastDecisionMs = 0;
unsigned long lastMqttAttemptMs = 0;

/* ================= STATES ================= */
enum RobotState {
  STOP,
  MOVE_FORWARD,
  TURN_LEFT,
  TURN_RIGHT,
  REVERSE
};

RobotState currentState = STOP;

/* ================= FUNCTIONS ================= */
long readUltrasonicCM();
void setMotionLEDs(bool fwd, bool left, bool right, bool rev);
void applyState(RobotState s);
const char* stateToString(RobotState s);
void buzz(bool on);
void publishTelemetry(long ultraCm, int ir[5], RobotState s);
void mqttEnsureConnectedNonBlocking();

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(IR1_PIN, INPUT);
  pinMode(IR2_PIN, INPUT);
  pinMode(IR3_PIN, INPUT);
  pinMode(IR4_PIN, INPUT);
  pinMode(IR5_PIN, INPUT);

  pinMode(LED_FWD, OUTPUT);
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
  pinMode(LED_REV, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  setMotionLEDs(false, false, false, false);
  buzz(false);
  digitalWrite(LED_WIFI, LOW);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_WIFI, HIGH);

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  Serial.println("MQTT configured (leroy)");
}

/* ================= LOOP ================= */
void loop() {
  mqttEnsureConnectedNonBlocking();
  mqtt.loop();

  long ultraCm = readUltrasonicCM();

  int ir[5];
  ir[0] = (digitalRead(IR1_PIN) == HIGH) ? 1 : 0;
  ir[1] = (digitalRead(IR2_PIN) == HIGH) ? 1 : 0;
  ir[2] = (digitalRead(IR3_PIN) == HIGH) ? 1 : 0;
  ir[3] = (digitalRead(IR4_PIN) == HIGH) ? 1 : 0;
  ir[4] = (digitalRead(IR5_PIN) == HIGH) ? 1 : 0;

  if (millis() - lastDecisionMs >= DECISION_INTERVAL_MS) {
    lastDecisionMs = millis();

    bool frontBlocked = (ultraCm < OBSTACLE_DISTANCE_CM) || (ir[2] == 1);
    bool leftBlocked  = (ir[0] == 1) || (ir[1] == 1);
    bool rightBlocked = (ir[3] == 1) || (ir[4] == 1);

    if (frontBlocked && leftBlocked && rightBlocked)
      currentState = REVERSE;
    else if (frontBlocked && leftBlocked && !rightBlocked)
      currentState = TURN_RIGHT;
    else if (frontBlocked && !leftBlocked && rightBlocked)
      currentState = TURN_LEFT;
    else if (frontBlocked && !leftBlocked && !rightBlocked)
      currentState = TURN_LEFT;
    else if (leftBlocked && !frontBlocked)
      currentState = TURN_RIGHT;
    else if (rightBlocked && !frontBlocked)
      currentState = TURN_LEFT;
    else
      currentState = MOVE_FORWARD;

    applyState(currentState);
    publishTelemetry(ultraCm, ir, currentState);
  }

  delay(40);
}

/* ================= MQTT SAFE CONNECT ================= */
void mqttEnsureConnectedNonBlocking() {
  if (mqtt.connected()) return;
  if (millis() - lastMqttAttemptMs < MQTT_RETRY_MS) return;

  lastMqttAttemptMs = millis();
  String clientId = "leroy-";
  clientId += String((uint32_t)esp_random(), HEX);

  Serial.print("MQTT connecting... ");
  if (mqtt.connect(clientId.c_str())) {
    Serial.println("CONNECTED");
  } else {
    Serial.print("FAILED (rc=");
    Serial.print(mqtt.state());
    Serial.println(")");
  }
}

/* ================= ULTRASONIC ================= */
long readUltrasonicCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 999;
  return (long)(duration * 0.034 / 2.0);
}

/* ================= ACTIONS ================= */
void applyState(RobotState s) {
  switch (s) {
    case MOVE_FORWARD:
      setMotionLEDs(true, false, false, false);
      buzz(false);
      break;
    case TURN_LEFT:
      setMotionLEDs(false, true, false, false);
      buzz(true);
      break;
    case TURN_RIGHT:
      setMotionLEDs(false, false, true, false);
      buzz(true);
      break;
    case REVERSE:
      setMotionLEDs(false, false, false, true);
      buzz(true);
      break;
    default:
      setMotionLEDs(false, false, false, false);
      buzz(false);
  }
}

void setMotionLEDs(bool fwd, bool left, bool right, bool rev) {
  digitalWrite(LED_FWD, fwd);
  digitalWrite(LED_LEFT, left);
  digitalWrite(LED_RIGHT, right);
  digitalWrite(LED_REV, rev);
}

void buzz(bool on) {
  digitalWrite(BUZZER_PIN, on ? HIGH : LOW);
}

/* ================= HELPERS ================= */
const char* stateToString(RobotState s) {
  switch (s) {
    case MOVE_FORWARD: return "FWD";
    case TURN_LEFT: return "LEFT";
    case TURN_RIGHT: return "RIGHT";
    case REVERSE: return "REV";
    default: return "STOP";
  }
}

/* ================= TELEMETRY ================= */
void publishTelemetry(long ultraCm, int ir[5], RobotState s) {
  int rssi = WiFi.RSSI();
  char payload[220];

  snprintf(payload, sizeof(payload),
    "{\"id\":\"leroy\",\"ts\":%lu,\"ultra_cm\":%ld,\"ir\":[%d,%d,%d,%d,%d],\"state\":\"%s\",\"rssi\":%d}",
    millis(), ultraCm,
    ir[0], ir[1], ir[2], ir[3], ir[4],
    stateToString(s), rssi
  );

  Serial.println(payload);

  if (mqtt.connected()) {
    mqtt.publish(MQTT_TOPIC, payload);
  }
}
