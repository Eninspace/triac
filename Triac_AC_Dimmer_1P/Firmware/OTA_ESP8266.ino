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
MIIElDCCAnygAwIBAgIJAOMV88OQ2oJNMA0GCSqGSIb3DQEBCwUAMFMxCzAJBgNV
BAYTAkVBMQ4wDAYDVQQIDAVFYXJ0aDEVMBMGA1UECgwMSG9tZSBFbmNyeXB0MR0w
GwYDVQQDDBRIb21lIEVuY3J5cHQgUm9vdCBDQTAeFw0xODEwMDcwOTIwMjlaFw0y
MTA3MDMwOTIwMjlaME4xCzAJBgNVBAYTAkVBMQ4wDAYDVQQIDAVFYXJ0aDESMBAG
A1UECgwJU21hcnRob21lMRswGQYDVQQDDBJlc3A4MjY2LXRlc3QubG9jYWwwggEi
MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCwDUz2TqkgArukVcm2HfOHuR3W
cUxqE6/ABHkFwaVA5h+fwFwzbe4wzhRt4hTduAVImDKIS9yLaPs5j6hmrGcENxpL
D35ROZU9RZDu4t7BkzHv0ZKUOGqfsfKfnuQudoUJww+Arjta8moBEJHEBlTLPoL9
g+C5GFvD803sQyFR0fJDxJvBj09B4MssBe03vaN10WS9HLgHptpGCTnBo61OCnoj
KH1yP6WZJEv6na7cS3ldvbKGLZ7rotqaedRnTJkqtALzYn5PQSsoy+KwAZIarllz
6P994c3ilPTQ6iVIji9pzjj/BJO5ACOV1sEB8u/gEg6rvbQSqU2L24FlOlJNAgMB
AAGjcDBuMB8GA1UdIwQYMBaAFHLpqROuOTpW9UR6uAmlpFV7ZwSwMAkGA1UdEwQC
MAAwCwYDVR0PBAQDAgTwMDMGA1UdEQQsMCqCEmVzcDgyNjYtdGVzdC5sb2NhbIIU
Ki5lc3A4MjY2LXRlc3QubG9jYWwwDQYJKoZIhvcNAQELBQADggIBAB0a53AYqWs5
8qHfM0j5e7IGOdVLrSQ8i5d2sbv4ulgVt+vgOJGLZ9GuFuGFN5oRKpqiEPQUVVyr
9rE2CfnU0zonLeFX5FwUCYUyBA6TJd40/VHjLf81tkVA3YIoXQopsIoyQ6fHyZVq
cKc8qULn4FVCTAxp/tlxkuGDCHdjUaSIhuH2mpCSDAhs9sdnRYWLehUWDlbcN3O
hmRWucP4OzD8N+MzOmZvHIddD9uEq6XgT8Oz51v0sLMoYceEEXIO+B2S71rDkexY
jo3NezOaScFQNsg08+XKyufZ4bFIT8tkvsCU8cYEbXFbTIB7hJna9W0OsDl+pXkD
y+rxMXvFAhhH5CBPbLbh/vYxXOEPLbq2h6TZbmyNdCQ/7L+Ku3JGXXFeENap7PLq
uUzsGOsT4569qtVSJh/Y6syiZlOhJlp9KNLLnny1A7xj6AgTZ2Ksk3udQiLCEHGW
enYNRbratYWT2L7vn7akE/VD9MImc3BFtjbiLdznwBOFpooliSff7WjW9BLGOLGj
t2S6viuAXEHjEkU33FuBkCDbG3fR5cxqjpHK5Vdf7ZfvobFDRSMPr/bbOqZhYhmm
fi3mVGs0iaYB5kA1xxmHmxP5FTAThBSzbYNb5nOQhTOJ+zXB3PeOGixSz4qLwlzt
RBibbZRhjEdacaGZEP75fuw7DIvABqh/
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCwDUz2TqkgAruk
Vcm2HfOHuR3WcUxqE6/ABHkFwaVA5h+fwFwzbe4wzhRt4hTduAVImDKIS9yLaPs5
j6hmrGcENxpLD35ROZU9RZDu4t7BkzHv0ZKUOGqfsfKfnuQudoUJww+Arjta8moB
EJHEBlTLPoL9g+C5GFvD803sQyFR0fJDxJvBj09B4MssBe03vaN10WS9HLgHptpG
CTnBo61OCnojKH1yP6WZJEv6na7cS3ldvbKGLZ7rotqaedRnTJkqtALzYn5PQSso
y+KwAZIarllz6P994c3ilPTQ6iVIji9pzjj/BJO5ACOV1sEB8u/gEg6rvbQSqU2L
24FlOlJNAgMBAAECggEBAIrtZA920YW+Ql8Th8McbaJsFby02c1Xzbgkj3BYeNT3
BHuna9r15CJFbd+sqjSxVBIOcMoXu2mUJbQtxoGHiR/NcAVNq6vjWf7pF8873vc4
9g4+/oHS52kwHruRwpVb1+51LJWqaJKDe91tOupK6L0ejnYKjNrj+v5t0M8hxnhu
eoU19ZXllErtjAT4U2iX7OSO45BiPC3ot31OTnDtG2ij30pRh7naECXn7g/njcxW
29rtvCjY4CgURlHbBL/ZCoGAcs1KaUy3vHw3tIrQosObk1m9QDQL/grFsZ/Z+lHg
7hBDpw+ApXheZAQ6HZ+sH1gUGQv5SaIsNYY2PE+G+80CgYEA3KaLVHrHMeIWM2x6
SlBVseABeTtnIUpRDPYtrapGebVKZPouJy9vBSiL2eFtPL241Y1h5FKF6uV3Bjdw
M75YcHJLPWmsmW8CEnxwnpT5IqmYZg5qluy/GFQILFTnZ+Q0h926PKZRr1Quzjhf
L5QWzq0PtN70r/nok/9ZSnOruOMCgYEAzEGmqPIrlZpFi+Z4fiU/+EuHQ1QZ0ulJ
cJkYF5YmrOr6hdxxDZArfoHzKjC5OJ2YzTyXvqpuOT5+zMIcY91SLYyDQzrj7mTq
qHJcdc5fBKTYrLp8z1UvWeI58A1ZoGkNLpUpqkoO353yUEIyX17kbQNBEmK9Rsk2
OEq9LTfNHw8CgYAsGhGhKgUkYEcFsjnjHplyOw6wV4Iucjcu+wl4O59odibPdf3B
kUaEZm1FeDwoNtd5v4+AXko//9BO9VuUi5FE2McfEGmTTzByyOLXsVI+1LdmyYM
0ncDetBDZBj4d/FT8qplsuyCubW9PCEyVkwUtRx9j06i2bV110TWj7YT9wKBgCHH
QZ5y3g5RgurN5GNIGbogE3Bo+hRNDB4Y00kjRaYW/fMhL3kEvIyhROtXP2pDdb+L
I7UeTjgdQ67s8vD46twpfpOhq4wDGGNe9jdtN313HXYDKRzIg5ojtV0jeAz1En7/
n/JPaQJGBoqLR/VJphD5FsvJlG+eWt8YTQyrS7q1AoGAIN/RSHvOhPgjTf+i4P9f
VnuwPk6PGn5VEG/1i++nwFiwXv3BRQxCn/uHCewzU8sP91lyRKX79UhivN8kvV8k
7a4MvJF2ltU0FX1BFux6I0OOIft2rm5JmafesQ3J8SWeXKlYdFPfvfG1rDkhuR7f
d0eLqZEx24d0enhQPvXqEFs=
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
