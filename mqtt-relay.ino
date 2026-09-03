#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char* ssid = "xxxx";
const char* password = "xxxx";

const char* mqtt_server = "xxxx";

const int mqtt_port = 8883;

const char* mqtt_user = "xxxx";
const char* mqtt_pass = "xxxx";

const char* emqx_ca_cert ="-----BEGIN CERTIFICATE-----\n" \
"xxxx\n" \
"xxxx\n" \
"xxxx\n" \
"-----END CERTIFICATE-----";

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

    // Generate a unique Client ID using a random hex string
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
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
  digitalWrite(RELAY_PIN, HIGH); // relay OFF initially
  setup_wifi();

  // Use official CA certificate validation instead of setInsecure()
  espClient.setCACert(emqx_ca_cert);

  client.setServer(mqtt_server, mqtt_port); // Ensure mqtt_port is 8883
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}
