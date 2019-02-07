#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

// Update with suitable values
//#define MAX_BUFFER 8192
#define SSID "wirelessname"
#define SSID_PASSWORD "password"
#define MQTT_SERVER "openhab"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "christmastree"
#define MQTT_USER "openhab"
#define MQTT_PASSWORD "password"
#define MQTT_WILL_TOPIC "/home/christmastree/connected"
#define MQTT_PUB_TOPIC "/home/christmastree/water"
#define MQTT_RETAIN false

#define LED_PIN D4
#define WATER_PIN D1

// Timer
unsigned long interval = 100;
unsigned long previousMillis = millis();

WiFiClient espClient;
PubSubClient mqttClient(espClient);


void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(WATER_PIN, INPUT);

  Serial.begin(115200);
  Serial.setTimeout(5000);
  setup_wifi();
  ArduinoOTA.begin();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

void setup_wifi() {
  WiFi.begin(SSID, SSID_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}


void mqttReconnect() {
  unsigned int retries = 0;
  while (!mqttClient.connected() && retries < 5) {
    yield();
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD, MQTT_WILL_TOPIC, 0, true, "0")) {
      mqttClient.publish(MQTT_WILL_TOPIC, "2", true);
    } else {
      ArduinoOTA.handle();
      retries++;
      delay(5000);
    }
  }
}

int waterState=99;
int lastWaterState=98;
void loop() {

  ArduinoOTA.handle();

  if (WiFi.status() != WL_CONNECTED){
    setup_wifi();
  }

  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  
  if (mqttClient.connected()) {
    mqttClient.loop();
  }

  
  if ((millis() - previousMillis > interval) || (waterState == 99)){

    waterState = digitalRead(WATER_PIN);
    bool bouncingReading = false;
    for (unsigned long i = 0 ; i < 10000 ; i++){
      if (digitalRead(WATER_PIN) != waterState) {
        bouncingReading = true;
      }
      delay(1);
    }

    if (!bouncingReading) {
      if (lastWaterState != waterState){
        lastWaterState = waterState;
        if (waterState == HIGH) {
          Serial.println("No water detected");
          digitalWrite(LED_PIN, HIGH);
          mqttClient.publish(MQTT_PUB_TOPIC, "LOW", true);

        } else if (waterState == LOW) {
          Serial.println("water level ok");
          digitalWrite(LED_PIN, LOW);
          mqttClient.publish(MQTT_PUB_TOPIC, "OK", true);
        }
      }
    }
    
    previousMillis = millis();
  }

}
