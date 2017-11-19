#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const char* WIFI_SSID = "your-ssid";
const char* WIFI_PASSWORD = "your-password";

const char* URL_PATTERN = "GET /lights/";
const byte URL_PATTERN_LENGTH = 12;

IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  while (! Serial);

  irsend.begin();

  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.print("HTTP server started at http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Get the IR code to sent depending on request uri
  unsigned long irCode = getIrCodeForRequest(req);

  // Send the IR code if any + HTTP response
  if (irCode == 0) {
    client.stop();
  } else {
    sendIRcode(irCode);
    sendHttpResponse(client);
  }
}

void sendIRcode(unsigned long irCode) {
  Serial.println("Sending IR code...");
  for (int i = 0; i < 3; i++) {
    irsend.sendNEC(irCode, 32);
    delay(40);
  }
}

void sendHttpResponse(WiFiClient client) {
  client.flush();
  client.print("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nOK");
  delay(1);
  Serial.println("Client disconnected");
  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}

unsigned long getIrCodeForRequest(String req) {
  int index = req.indexOf(URL_PATTERN);
  if (index != -1 && req.length() > index + URL_PATTERN_LENGTH) {
    char cmd = req[index + URL_PATTERN_LENGTH];

    if (cmd == '+') { // DIM UP
      return 0xFFA25D;
    } else if (cmd == '-') { // DIM DOWN
      return 0xFF629D;
    } else if (cmd == 'F') { // OFF
      return 0xFFE21D;
    } else if (cmd == 'N') { // ON
      return 0xFFC23D;
    } else if (cmd == '1') { // DIY1
      return 0xFFE01F;
    } else if (cmd == '2') { // DIY2
      return  0xFFA857;
    } else if (cmd == 'R') { // RED
      return 0xFF6897;
    } else if (cmd == 'G') { // GREEN
      return 0xFF9867;
    } else if (cmd == 'B') { // BLUE
      return 0xFFB04F;
    } else if (cmd == 'W') { // WHITE
      return 0xFF38C7;
    } else if (cmd == 'P') { // PARTY/FLASH
      return  0xFF22DD;
    } else if (cmd == 'S') { // SMOOTH
      return  0xFF02FD;
    }
  }
  return 0;
}
