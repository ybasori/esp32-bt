#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* ssid = "xxxx";
const char* password = "xxxx";

const char* mqtt_server = "xxxx";

const int mqtt_port = 8883;

const char* mqtt_user = "xxxx";
const char* mqtt_pass = "xxxx";

#define RELAY_PIN 18

WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void callback(char* topic, byte* payload, unsigned int length) {

  String message = "";

  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message received: ");
  Serial.println(message);

  // HIGH trigger relay
  if (message == "PRESSED") {
    digitalWrite(RELAY_PIN, LOW);
    delay(500);
    digitalWrite(RELAY_PIN, HIGH);
  }
}

void reconnect() {

  while (!client.connected()) {

    Serial.print("Connecting to MQTT...");

    if (client.connect(
      "ESP32Client",
      mqtt_user,
      mqtt_pass
    )) {

      Serial.println("connected");

      client.subscribe("home/yusuf-pc");

    } else {

      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying...");

      delay(2000);
    }
  }
}

void setup() {

  pinMode(RELAY_PIN, OUTPUT);

  // relay OFF initially
  digitalWrite(RELAY_PIN, HIGH);

  setup_wifi();

  // required for EMQX TLS
  espClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);

  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}
