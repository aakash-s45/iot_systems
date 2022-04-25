#include <Wire.h>

int text;

void setup(){
    Wire.begin(8);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
    Serial.begin(9600);//start hardware serial
}

void loop(){
  delay(100); 
}
void requestEvent() {
    if (Serial.available()) {
        Serial.print("Sent: \t");
        while (Serial.available()) 
        {
          text = Serial.read();       // sends five bytes
          Wire.write(text); 
          Serial.write(text);
        }
        Wire.write("\n"); 
        Serial.println();   // stop transmitting
    }
}
void receiveEvent(int howMany) {
    Serial.print("Recieved: ");
    while (1 < Wire.available()) { // loop through all but the last
        char c = Wire.read(); // receive byte as a character
        Serial.print(c);         // print the character
    }        // print the integer
}






