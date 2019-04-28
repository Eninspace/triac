/*
  SecureBearSSLUpdater - SSL encrypted, password-protected firmware update

  This example starts a HTTPS server on the ESP8266 to allow firmware updates
  to be performed.  All communication, including the username and password,
  is encrypted via SSL.  Be sure to update the SSID and PASSWORD before running
  to allow connection to your WiFi network.

  To upload through terminal you can use:
  curl -u admin:admin -F "image=@firmware.bin" esp8266-webupdate.local/firmware

  Adapted by Earle F. Philhower, III, from the SecureWebUpdater.ino example.
  This example is released into the public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>

const char* host = "esp8266-test";
const char* update_path = "/firmware";
const char* update_username = "admin";
const char* update_password = "admin";
const char* ssid = "SSID";
const char* password = "YOUR_PASS";

BearSSL::ESP8266WebServerSecure httpServer(443);
ESP8266HTTPUpdateServer httpUpdater;

static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
cert
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----
key
-----END PRIVATE KEY-----
)EOF";


void setup()
{

  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
//WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  MDNS.begin(host);

  httpServer.setRSACert(new BearSSLX509List(serverCert), new BearSSLPrivateKey(serverKey));
  httpUpdater.setup(&httpServer, update_path, update_username, update_password);
  httpServer.begin();

  MDNS.addService("https", "tcp", 443);
  Serial.printf("BearSSLUpdateServer ready!\nOpen https://%s.local%s in "\
                "your browser and login with username '%s' and password "\
                "'%s'\n", host, update_path, update_username, update_password);
}

void loop()
{
  httpServer.handleClient();
}
