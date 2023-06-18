#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Pin untuk sensor LED
#define LED_RED D5

// Pin untuk sensor MQ-2
const int MQ2_PIN = A0;

// Pin untuk buzzer
 const int BUZZER_PIN = D2;

// Nilai ambang batas untuk mendeteksi gas atau asap rokok
const int THRESHOLD = 550;

const char* ssid = "ROG Phone 3";
const char* password = "ROGPHONE";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topic = "kelompok-3/smoke-sensor";

String macAddr = "";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(MQ2_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  connectWifi();
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()){
    reconnect();
  }
  if (!client.loop()){
    client.connect(macAddr.c_str());
  }

  int sensorValue = analogRead(MQ2_PIN);

  if (sensorValue > THRESHOLD) {
    Serial.println("Detected smoke or gas!");

    digitalWrite(LED_RED, HIGH);
    playSiren();
    digitalWrite(LED_RED, LOW);
   
  }

   // Mengirimkan nilai sensor melalui Serial Monitor
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);

  char payload[10];
  sprintf(payload, "%d", sensorValue);

  client.publish(topic, payload);

  delay(1000);

}

void connectWifi(){
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi connected - ESP IP address: ");
    Serial.println(WiFi.localIP());
    macAddr = WiFi.macAddress();
    Serial.println(macAddr);
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(macAddr.c_str()))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void playSiren() {
  int noteDuration = 200;

  // Pola nada bunyi alarm kebakaran
  int fireAlarmPattern[] = {
    880, 0, 880, 0, 880, 0, 880, 0, 880, 0, 880, 0,
  };

  // Memainkan pola nada
  for (int i = 0; i < sizeof(fireAlarmPattern) / sizeof(int); i++) {
    tone(BUZZER_PIN, fireAlarmPattern[i], noteDuration);
    delay(noteDuration * 1.3);
    noTone(BUZZER_PIN);
  }
}
