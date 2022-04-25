#include <Wire.h>

int text;

void setup()  
{
  Serial.begin(9600);//start hardware serial
  Serial.println("Starting Hardware Serial");

  Wire.begin();//
  delay(1000); 
}

void loop() 
{
//   if (s_serial.available()){
//     // if software serial is available
//     // read the message from sofrware serial and write that message to hardware serial
//     Serial.write("Recieved: ")
//     Serial.write(s_serial.read());//if 
//   }
    
    if (Serial.available()) {
      // if hardware serial is available
      // read the message from hardware serial and send to it software serial
      // then print the same message on hardware serial 
  
      Serial.print("Sent: \t");
      Wire.beginTransmission(8); // transmit to device #8
      while (Serial.available()) 
      {
        text = Serial.read();       // sends five bytes
        Wire.write(text); 
        Serial.write(text);
      }
      Wire.write("\n"); 
      Serial.println();
      Wire.endTransmission();    // stop transmitting
    }
    else{
        Wire.requestFrom(8, 6);
        Serial.print("Recieved: ");
        while (Wire.available()) { // slave may send less than requested
          char c = Wire.read(); // receive a byte as character
          Serial.print(c);         // print the character
        }
    }
}






