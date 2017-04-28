#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "settings.h"

void connect_to_wifi() {
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  Serial.print(" with password ");
  Serial.println(WIFI_PASSWD);

  // These variables are defined in settings.h
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(A0, INPUT);

  connect_to_wifi();
}

// -------- MQTT ----------------//
char server[] = MQTT_ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/status/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = MQTT_TOKEN;
char clientId[] = "d:" MQTT_ORG ":" MQTT_DEVICE_TYPE ":" MQTT_DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);

void connect_to_mqtt() {
  Serial.print("Connecting MQTT client to ");
  Serial.println(server);
  Serial.print("ClientId: ");
  Serial.println(clientId);
  Serial.print("Auth: ");
  Serial.print(authMethod);
  Serial.print(": ");
  Serial.println(token);
  
  while (!client.connect(clientId, authMethod, token)) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(); 
}

void send_weight(double weight) {
    String payload = "{\"d\":{\"weight\":";
    payload += weight;
    payload += "}}";
    
    Serial.print("Sending payload: ");
    Serial.println(payload);
    
    if (client.publish(topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
    } else {
      Serial.println("Publish failed");
    }
}

// -------- END MQTT -----------//

// ----------- SCALE -----------//

// Scale settings
// offset = ADC value when there is no weight on scale
// factor = ADC resolution is 10-bit -> max weight/1024
unsigned int scale_offset = 201;
double scale_factor = 11.9;

unsigned int read_adc() {
  return (unsigned int) analogRead(A0);
}

unsigned int read_scale() {
  unsigned int v = 0;
  unsigned int n = 20;
  
  for(unsigned int i = 0; i < n; ++i) {
    delay(20);
    v += read_adc();
  }
  
  v /= n;
  return v;
}

double adc2g(unsigned int value) {
  unsigned delta = value - scale_offset;
  Serial.print("Delta:" );
  Serial.println(delta);

  return delta*scale_factor;
}

double get_weight() {
  unsigned int adc_value = read_scale();
  Serial.print("ADC: ");
  Serial.println(adc_value);

  double weight = adc2g(adc_value);
  return weight;
}

// -------- END SCALE -------- //

void loop() {
  if (!client.connected()) {
    connect_to_mqtt();
  }
  
  double weight = get_weight();
  Serial.print("Weight (g): ");
  Serial.println(w);

  send_weight(w);

  while(Serial.available() < 2){}
  Serial.readString();
}
