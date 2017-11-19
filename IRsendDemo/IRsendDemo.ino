#include <IRremote.h>

String serialData;
unsigned long irCode;

IRsend irsend;

void setup() {
  Serial.begin(9600);
  while (! Serial);
}

void loop() {
  if (Serial.available()) {
    serialData = Serial.readString();

    // Make sure to send data with the "No line ending" option
    if (serialData.length() > 0) {
      if (serialData == "+") { // DIM UP
        irCode = 0xFFA25D;
      } else if (serialData == "-") { // DIM DOWN
        irCode = 0xFF629D;
      } else if (serialData == "F") { // OFF
        irCode = 0xFFE21D;
      } else if (serialData == "N") { // ON
        irCode = 0xFFC23D;
      } else if (serialData == "1") { // DIY1
        irCode = 0xFFE01F;
      } else if (serialData == "2") { // DIY2
        irCode = 0xFFA857;
      } else if (serialData == "R") { // RED
        irCode = 0xFF6897;
      } else if (serialData == "G") { // GREEN
        irCode = 0xFF9867;
      } else if (serialData == "B") { // BLUE
        irCode = 0xFFB04F;
      } else if (serialData == "W") { // WHITE
        irCode = 0xFF38C7;
      } else if (serialData == "P") { // PARTY/FLASH
        irCode = 0xFF22DD;
      } else if (serialData == "S") { // SMOOTH
        irCode = 0xFF02FD;
      } else {
        irCode = 0;
      }

      if (irCode != 0) {
        for (int i = 0; i < 3; i++) {
          irsend.sendNEC(irCode, 32);
          delay(40);
        }
      }
    }

    delay(1000); //1 second delay between each signal burst
  }
}
