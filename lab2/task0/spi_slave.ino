/*
    SPI Slave Demo Sketch
    Connect the SPI Master device to the following pins on the esp8266:

    GPIO    NodeMCU   Name  |   Uno
  ===================================
     15       D8       SS   |   D10
     13       D7      MOSI  |   D11
     12       D6      MISO  |   D12
     14       D5      SCK   |   D13

    Note: If the ESP is booting at a moment when the SPI Master has the Select line HIGH (deselected)
    the ESP8266 WILL FAIL to boot!
    See SPISlave_SafeMaster example for possible workaround

*/

#include "SPISlave.h"
char text[32];

void setup()
{

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("this is slave");

  SPISlave.onData([](uint8_t *data, size_t len)
                  {
    String message = String((char *)data);
    (void)len;
    Serial.printf("Recieved: %s\n", (char *)data);
    //    SPISlave.setData("hello node");

    //    if (Serial.available()) {
    //        Serial.print("Sent: \t");
    //        int availableBytes = Serial.available();
    //        for(int i=0; i<availableBytes; i++)
    //        {
    //            text[i] = Serial.read();
    //        }
    //        SPISlave.setData("hi");
    //        Serial.print(text);
    //        SPISlave.setData("\n");
    //        Serial.println();
    //    } });

  SPISlave.onDataSent([]()
                      {
      //    Serial.println(":"); });

      SPISlave.begin();
      // Sets the data registers. Limited to 32 bytes at a time.
      // SPISlave.setData(uint8_t * data, size_t len); is also available with the same limitation

      //   SPISlave.setData("Ask me a question!");
}

void loop()
{
      delay(1000);
      if (Serial.available())
      {
        Serial.print("Sent: ");
        memset(text, 0, 32);
        int availableBytes = Serial.available();
        for (int i = 0; i < availableBytes; i++)
        {
          text[i] = Serial.read();
        }
        SPISlave.setData((const char *)text);

        Serial.println(text);
      }
      else
        SPISlave.setData("#");
}