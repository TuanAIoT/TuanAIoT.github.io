#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>

const char* cfg_wifi_ssid = "AIoT Tang 1";
const char* cfg_wifi_pwd = "aiot1234@";
const char* mqtt_server = "aiot-jsc1.ddns.net";
const unsigned int mqtt_port = 8883;
const char deviceUserName[] = "ca59d42346fe657da58a506e8faf599e";
const char devicePassword[] = "ugAwIBAgIUbvcszjYWz4";

const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIID0zCCArugAwIBAgIUbvcszjYWz4GXdSIRHb9y6w/2okMwDQYJKoZIhvcNAQEL" \
"BQAweTELMAkGA1UEBhMCVk4xDjAMBgNVBAgMBUhhTm9pMQ4wDAYDVQQHDAVIYU5v" \
"aTENMAsGA1UECgwEQUlvVDENMAsGA1UECwwEQUlvVDENMAsGA1UEAwwEQUlvVDEd" \
"MBsGCSqGSIb3DQEJARYOYWlvdEBnbWFpbC5jb20wHhcNMjIxMTExMDc1MzI1WhcN" \
"MzIxMTA4MDc1MzI1WjB5MQswCQYDVQQGEwJWTjEOMAwGA1UECAwFSGFOb2kxDjAM" \
"BgNVBAcMBUhhTm9pMQ0wCwYDVQQKDARBSW9UMQ0wCwYDVQQLDARBSW9UMQ0wCwYD" \
"VQQDDARBSW9UMR0wGwYJKoZIhvcNAQkBFg5haW90QGdtYWlsLmNvbTCCASIwDQYJ" \
"KoZIhvcNAQEBBQADggEPADCCAQoCggEBAKwd6AXCATG1a/l/F9YQCwVKIa22f33b" \
"1VSbBXMI+FaVP/rWw6C60n7gr8Ot6EomBweAlj8ylx31jDOcnlgbq+wIFZMbpXim" \
"MrgtZbbESAv7CCWm3ts1rX05EY6K27QcUQB/zMz2+c3aVnOkJCKezrTvMlMjmVWV" \
"B7HW0rpesu4SX1pW77bpwGjdpaK+L78QS2KCghZMvnzkJ/2cVUhvZYUAw0W1Lkeb" \
"ZSbeUV3w40rI7RLmNQp6+aV+WMCP8N84piYWA/aazu8cRW37OcWN2zEyHa3mZwtS" \
"vwUWBD2VnJ9b1Q7R8H9a47JQPTbIN9hGJvl0Ak5Igv8oWmmBW8qpLfUCAwEAAaNT" \
"MFEwHQYDVR0OBBYEFHfyMPAqXmJ/pZXvhHfY/KlKROfkMB8GA1UdIwQYMBaAFHfy" \
"MPAqXmJ/pZXvhHfY/KlKROfkMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEL" \
"BQADggEBADI2AYq3m+rYOOvnOoLti/agoWq7V29L3AOOhHKOR0bbC40/whvesA7K" \
"wO6HFrFvYok/63vyr0HxeKwZtlrdUhchxY4SPPYo5n0b1oHUwFqENqLuBoBGu5IA" \
"A8FQwTo9QTEvilmxg24nMm9kPKRdVtyCjIdAEYxeoZphX6JwBz0+M/FZ0T109rhl" \
"OmtxtPw/cJzVhPfe6kQka+Lk/umDLQty4ex9mA+dwFKXmTwJoiOi19VvVjGnKWau" \
"AFPVl5iTOW2CNbj0IuSxLiEqx94WGpj+RaPcJr/YprKVTp4xcAoPPEzvYxpDRNAI" \
"aTIA7Lxot40x9vvpMvo8mpEck2phc8c=" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';
  Serial.println(message);
}

void verifyFingerprint() {
  if(client.connected() || espClient.connected()) return;
  
  if (!espClient.connect(mqtt_server, mqtt_port)) {
    Serial.println("Connection failed. Rebooting.");
    
  }
  // if (espClient.verify(root_ca, mqtt_server)) {
  //   Serial.print("Connection secure -> .");
  // } else {
  //   Serial.println("Connection insecure! Rebooting.");
  //   ESP.restart();
  // }
}
void reconnect(){
  BearSSL::X509List cert(root_ca);
  espClient.setTrustAnchors(&cert);
  // espClient.setCACert(root_ca);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    verifyFingerprint();
    if (client.connect(deviceUserName, deviceUserName, devicePassword)) {
      Serial.println("connected");
      client.subscribe("demostm8/#");
      client.publish("demostm8/status", "hello");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.begin(cfg_wifi_ssid, cfg_wifi_pwd);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}