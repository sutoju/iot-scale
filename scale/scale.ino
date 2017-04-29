#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "settings.h"

// -------- LED -----------------//
#define LED_YELLOW D1
#define LED_RED D3
#define LED_GREEN D5

void blink(int pin) {
  digitalWrite(pin, LOW);
  delay(150);
  digitalWrite(pin, HIGH);
  delay(150);
  digitalWrite(pin, LOW);
}
// -------- END LED -------------//

void connect_to_wifi() {
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  Serial.print(" with password ");
  Serial.println(WIFI_PASSWD);

  // These variables are defined in settings.h
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while(WiFi.status() != WL_CONNECTED) {
    blink(LED_YELLOW);
    Serial.print(".");
  }

  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());
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
    blink(LED_YELLOW);
  }

  Serial.println(); 
}

void send_weight(double weight, double diff) {
    String payload = "{\"d\":{\"weight\":\"";
    payload += weight;
    payload += "\",\"difference\":\"";
    payload += diff;
    payload += "\"}}";
    
    Serial.print("Sending payload: ");
    Serial.println(payload);
    
    if (client.publish(topic, (char*) payload.c_str())) {
      Serial.println("Publish ok");
      blink(LED_YELLOW);
    } else {
      Serial.println("Publish failed");
      blink(LED_RED);

      if (!client.connected()) {
        connect_to_mqtt();
      }     
      if (client.publish(topic, (char*) payload.c_str())) {
        Serial.println("Publish ok");
        blink(LED_YELLOW);
      } else {
        Serial.println("Publish failed");
        blink(LED_RED);
      }
    }
}

// -------- END MQTT -----------//

// ----------- SCALE -----------//

// Scale settings
// offset = ADC value when there is no weight on scale
// factor = ADC resolution is 10-bit -> max weight/1024
unsigned int scale_offset = 0;
double scale_factor = 11.9;

unsigned int read_adc() {
  return (unsigned int) analogRead(A0);
}

int read_scale() {
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
  int delta = value - scale_offset;
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

void setup() {
  Serial.begin(115200);
  Serial.println();

  pinMode(A0, INPUT);
  
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  
  connect_to_wifi();

  blink(LED_GREEN);

  for(int i = 0; i < 5; ++i) {
    scale_offset += read_adc();
  }
  scale_offset /= 5;
  scale_offset -= 1;
  
  Serial.print("Set scale offset to ");
  Serial.println(scale_offset);
}

void loop() {
  static double old_weight = 0;
  static unsigned int trigger_diff = 50;
  
  if (!client.connected()) {
    connect_to_mqtt();
  }
  
  double weight = get_weight();
  Serial.print("Weight (g): ");
  Serial.println(weight);
    
  if (weight >= 0.0 && weight <= 10000.0) {
    int delta = abs(weight - old_weight);
    if (delta > trigger_diff) {
      double final_weight = 0;
      
      delay(1000);
      for(unsigned int i = 0; i < 5; ++i) {
        final_weight += get_weight();
      }
      final_weight /= 10;
  
      delta = abs(final_weight - old_weight);
      if (delta > trigger_diff) {
        Serial.println("Send weight");
        send_weight(final_weight, final_weight - old_weight);
        old_weight = final_weight;
      }
    }
  }

  delay(1000);
}
