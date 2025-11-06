#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <time.h>
#include <TZ.h>
#include <FS.h>
#include <LittleFS.h>
#include <CertStoreBearSSL.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "<YOUR_SSID>";
const char* password = "<YOUR_WIFI_PASSWORD>";

// HiveMQ Cluster info
const char* mqtt_server = "<YOUR_MQTT_BROKER_URL>";
const char* mqtt_user = "<YOUR_MQTT_USER>";
const char* mqtt_pass = "<YOUR_MQTT_PASS>";
const int reconnect_delay = 5000;
const int status_delay = 60000;
boolean led_status = false;  // ON
char registerTopic[64];
String cmdTopic;
String statusTopic;

// CA certificate chain
const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFGTCCBAGgAwIBAgISBtYpojzbu6YILpLtnYRbYbC/MA0GCSqGSIb3DQEBCwUA\n" \
"MDMxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBFbmNyeXB0MQwwCgYDVQQD\n" \
"EwNSMTIwHhcNMjUxMDAzMDQ0NzMyWhcNMjYwMTAxMDQ0NzMxWjAfMR0wGwYDVQQD\n" \
"DBQqLnMyLmV1LmhpdmVtcS5jbG91ZDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC\n" \
"AQoCggEBAMSjskkjT3lienCFm5ptT7HN1Zi/2CviuTWY9hpTiSWjCqKW+stAPFe8\n" \
"MmDNJGoiHuHxn3S8UisKBa0l3vILJmjpIHVO9+DsHjhgj2lN1tGYyKmc0c4aU/BW\n" \
"Pc4/DZaWaVf0Bm25mAjsumHwIsaWddC/S9YO+AjvTOhrn+9Og34PRNTuDeSMFkK9\n" \
"YmL9Yb3a9bUx5W5Y9A3BVbvp2GidESD7Ih7d5X5XaYVxrsNaSBKbr+tQ9dtX7q2a\n" \
"kA96J1sNaEN3lISWFCP5yqqzUsq2A872HJHczp54rf5QriagM5hXWLxJsh75lsEu\n" \
"xxjhGCojmI7VpK3yurIhLNqapP8zjWkCAwEAAaOCAjkwggI1MA4GA1UdDwEB/wQE\n" \
"AwIFoDAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDAYDVR0TAQH/BAIw\n" \
"ADAdBgNVHQ4EFgQUlL6goqpL++sQlIGTQXv8bP6Ck94wHwYDVR0jBBgwFoAUALUp\n" \
"8i2ObzHom0yteD763OkM0dIwMwYIKwYBBQUHAQEEJzAlMCMGCCsGAQUFBzAChhdo\n" \
"dHRwOi8vcjEyLmkubGVuY3Iub3JnLzAzBgNVHREELDAqghQqLnMyLmV1LmhpdmVt\n" \
"cS5jbG91ZIISczIuZXUuaGl2ZW1xLmNsb3VkMBMGA1UdIAQMMAowCAYGZ4EMAQIB\n" \
"MC4GA1UdHwQnMCUwI6AhoB+GHWh0dHA6Ly9yMTIuYy5sZW5jci5vcmcvNjQuY3Js\n" \
"MIIBBQYKKwYBBAHWeQIEAgSB9gSB8wDxAHcA7TxL1ugGwqSiAFfbyyTiOAHfUS/t\n" \
"xIbFcA8g3bc+P+AAAAGZqJseDwAABAMASDBGAiEAg50XZlV6D3C3FdsuQI8FohyU\n" \
"PQC3evkoSldjh9d5ZekCIQCRgIKVL7YcO7ZBYKGX2QZiNqKnLYqWTyjSU/nw3W2r\n" \
"sgB2AJaXZL9VWJet90OHaDcIQnfp8DrV9qTzNm5GpD8PyqnGAAABmaibHjwAAAQD\n" \
"AEcwRQIgVXSzlK9rN5heWgdUi9mGGy9dtfajc/rcLIvzzuPpbhYCIQDoO2nKM4pz\n" \
"ftKfqY1LwdNrwYbwUMVNbdLezn8GVJOR6jANBgkqhkiG9w0BAQsFAAOCAQEAmcgt\n" \
"6VWtVR32eHscRuDHmUiXuAho0RCsGbxD0j9/l3bYBna/qLYtr3ArbHGrvkQcCurZ\n" \
"0jS2RHANhVgnm5PjfAR4ovKkzIJy1PY7Y71MLqtCwSUAtLuKuhpYtnwhu4iI2F7H\n" \
"0IPQAR0MLfEUrjmV+MEBEpK6yT9CUjtuSVEXJQr6Kzyz+Lyfmjyv01K8HkIGj3mk\n" \
"ArZnsN0cfjSC5K2GiIQu6pFFrBXhcpvBxaipQ5DI2AUZVsI9CGx34Igpj/8WjE8D\n" \
"Rt2FVBvNXMPw13Gcrf9AayJdkhjVBGMUciFAiCh4J6qvbrQ3qd1G65Hqsb6l32f6\n" \
"g2AWh3WXA9kSs4jUXg==\n" \
"-----END CERTIFICATE-----\n" \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFBjCCAu6gAwIBAgIRAMISMktwqbSRcdxA9+KFJjwwDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjQwMzEzMDAwMDAw\n" \
"WhcNMjcwMzEyMjM1OTU5WjAzMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n" \
"RW5jcnlwdDEMMAoGA1UEAxMDUjEyMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\n" \
"CgKCAQEA2pgodK2+lP474B7i5Ut1qywSf+2nAzJ+Npfs6DGPpRONC5kuHs0BUT1M\n" \
"5ShuCVUxqqUiXXL0LQfCTUA83wEjuXg39RplMjTmhnGdBO+ECFu9AhqZ66YBAJpz\n" \
"kG2Pogeg0JfT2kVhgTU9FPnEwF9q3AuWGrCf4yrqvSrWmMebcas7dA8827JgvlpL\n" \
"Thjp2ypzXIlhZZ7+7Tymy05v5J75AEaz/xlNKmOzjmbGGIVwx1Blbzt05UiDDwhY\n" \
"XS0jnV6j/ujbAKHS9OMZTfLuevYnnuXNnC2i8n+cF63vEzc50bTILEHWhsDp7CH4\n" \
"WRt/uTp8n1wBnWIEwii9Cq08yhDsGwIDAQABo4H4MIH1MA4GA1UdDwEB/wQEAwIB\n" \
"hjAdBgNVHSUEFjAUBggrBgEFBQcDAgYIKwYBBQUHAwEwEgYDVR0TAQH/BAgwBgEB\n" \
"/wIBADAdBgNVHQ4EFgQUALUp8i2ObzHom0yteD763OkM0dIwHwYDVR0jBBgwFoAU\n" \
"ebRZ5nu25eQBc4AIiMgaWPbpm24wMgYIKwYBBQUHAQEEJjAkMCIGCCsGAQUFBzAC\n" \
"hhZodHRwOi8veDEuaS5sZW5jci5vcmcvMBMGA1UdIAQMMAowCAYGZ4EMAQIBMCcG\n" \
"A1UdHwQgMB4wHKAaoBiGFmh0dHA6Ly94MS5jLmxlbmNyLm9yZy8wDQYJKoZIhvcN\n" \
"AQELBQADggIBAI910AnPanZIZTKS3rVEyIV29BWEjAK/duuz8eL5boSoVpHhkkv3\n" \
"4eoAeEiPdZLj5EZ7G2ArIK+gzhTlRQ1q4FKGpPPaFBSpqV/xbUb5UlAXQOnkHn3m\n" \
"FVj+qYv87/WeY+Bm4sN3Ox8BhyaU7UAQ3LeZ7N1X01xxQe4wIAAE3JVLUCiHmZL+\n" \
"qoCUtgYIFPgcg350QMUIWgxPXNGEncT921ne7nluI02V8pLUmClqXOsCwULw+PVO\n" \
"ZCB7qOMxxMBoCUeL2Ll4oMpOSr5pJCpLN3tRA2s6P1KLs9TSrVhOk+7LX28NMUlI\n" \
"usQ/nxLJID0RhAeFtPjyOCOscQBA53+NRjSCak7P4A5jX7ppmkcJECL+S0i3kXVU\n" \
"y5Me5BbrU8973jZNv/ax6+ZK6TM8jWmimL6of6OrX7ZU6E2WqazzsFrLG3o2kySb\n" \
"zlhSgJ81Cl4tv3SbYiYXnJExKQvzf83DYotox3f0fwv7xln1A2ZLplCb0O+l/AK0\n" \
"YE0DS2FPxSAHi0iwMfW2nNHJrXcY3LLHD77gRgje4Eveubi2xxa+Nmk/hmhLdIET\n" \
"iVDFanoCrMVIpQ59XWHkzdFmoHXHBV7oibVjGSO7ULSQ7MJ1Nz51phuDJSgAIU7A\n" \
"0zrLnOrAj/dfrlEWRhCvAgbuwLZX1A2sjNjXoPOHbsPiy+lO1KF8/XY7\n" \
"-----END CERTIFICATE-----\n";

// Device ID
String deviceId;

// BearSSL objects
BearSSL::X509List x509(ca_cert);
BearSSL::WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE 500
char msg[MSG_BUFFER_SIZE];
int value = 0;

// ====== HELPERS ======
String getChipId() {
  uint32_t chipId = ESP.getChipId();
  String id = String(chipId, HEX);
  id.toLowerCase();
  return id;
}

void publishStatus() {
    // Create alive JSON status
    StaticJsonDocument<200> status;
    status["id"] = deviceId;
    status["status"] = "up";
    status["led"] = led_status ? "OFF" : "ON";
    status["ip"] = WiFi.localIP().toString();
    status["uptime"] = getAliveSeconds();       // seconds since boot
    status["timestamp"] = currentTimestamp();

    char buf[256];
    serializeJson(status, buf);

    // Publish to device-specific status topic
    if (client.publish(statusTopic.c_str(), buf, false)) { // retained
      Serial.printf("[%s] Up status sent\n", deviceId.c_str());
    } else {
      Serial.printf("[%s] Failed to send up status\n", deviceId.c_str());
    }
}

unsigned long getAliveSeconds() {
  return millis() / 1000; // millis() returns ms since boot
}

String currentTimestamp() {
    time_t t = time(nullptr);        // Get current time
    struct tm timeinfo;
    localtime_r(&t, &timeinfo);      // convert to local time

    char timestamp[26];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S%z", &timeinfo);

    return String(timestamp);        // convert char[] to Arduino String
}

void setDateTime() {
  configTime(TZ_Europe_Berlin, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(100);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();

  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.printf("%s %s", tzname[0], asctime(&timeinfo));
}

// Publish device registration
void publishRegistration() {
  StaticJsonDocument<200> reg;
  reg["model"] = "Wemos D1 Mini";
  reg["id"]    = deviceId;
  reg["ip"]    = WiFi.localIP().toString();
  char buf[256];
  serializeJson(reg, buf);

  snprintf(registerTopic, sizeof(registerTopic), "devices/%s/register", deviceId.c_str());

  if (client.publish(registerTopic, buf, true)) {
    Serial.printf("[%s] Registration sent successfully\n", deviceId.c_str());
  } else {
    Serial.printf("[%s] Failed to send registration\n", deviceId.c_str());
  }
}

// ====== SETUP ======
void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to String
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();  // remove whitespace/newlines

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Handle LED commands
  if (message == "LED_ON") {
    led_status = false;
    digitalWrite(LED_BUILTIN, LOW);   // LOW = LED ON on Wemos
    Serial.println("LED turned ON");
  } else if (message == "LED_OFF") {
    led_status = true;
    digitalWrite(LED_BUILTIN, HIGH);  // HIGH = LED OFF
    Serial.println("LED turned OFF");
  } else {
    Serial.println("Unknown command");
  }

  publishStatus();
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "Wemos-" + deviceId;
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");

      // Subscribe to your topics
      String cmdTopic = "devices/" + deviceId + "/cmd";
      client.subscribe(cmdTopic.c_str());
      Serial.printf("Connected to %s\n", cmdTopic.c_str());

      // Publish registration after connecting
      publishRegistration();

      // Publish first status after connecting
      publishStatus();

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.printf(" try again in %s seconds", (reconnect_delay / 1000));
      delay(reconnect_delay);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  deviceId = "wemos-" + getChipId();
  cmdTopic = "devices/" + deviceId + "/cmd";
  statusTopic = "devices/" + deviceId + "/status";

  LittleFS.begin();
  setup_wifi();
  setDateTime();

  // Set the CA certificate for BearSSL
  espClient.setTrustAnchors(&x509);

  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

    static unsigned long lastAlive = 0;
    unsigned long now = millis();
    if (now - lastMsg > status_delay) {  // every <status_delay> seconds
    lastMsg = now;

    publishStatus();
  }
}