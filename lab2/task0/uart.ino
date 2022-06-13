
#include <SoftwareSerial.h>

SoftwareSerial s_serial(10, 11); // RX, TX

int text;

void setup()
{
  Serial.begin(9600); // start hardware serial
  Serial.println("Starting Hardware Serial");

  s_serial.begin(9600); // start software serial
  delay(1000);

  s_serial.println("Start Sending Message");
}

void loop()
{
  if (s_serial.available())
  {
    // if software serial is available
    // read the message from sofrware serial and write that message to hardware serial
    Serial.write("Recieved: ")
        Serial.write(s_serial.read()); // if
  }

  if (Serial.available())
  {
    // if hardware serial is available
    // read the message from hardware serial and send to it software serial
    // then print the same message on hardware serial
    Serial.print("Sent: \t");
    while (Serial.available())
    {
      text = Serial.read();
      s_serial.write(text);
      Serial.write(text);
    }
    s_serial.println();
    Serial.println();
  }
}
