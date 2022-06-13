#include <ArduinoJson.h>
#include <SoftwareSerial.h>

DynamicJsonDocument doc(512);
SoftwareSerial xbee(10, 11);

String messageToSend()
{
    doc["from"] = "xbee_2";
    doc["temp"] = temp.val();
    doc["humid"] = humid.val();
    String msg = "";
    serializeJson(doc, msg);
    return msg;
}

void updateSensor()
{
    temp.run();
    humid.run();
}

void sendData()
{
    String msg = messageToSend();
    xbee.write(msg.c_str());
}
void setup()
{
    Serial.begin(9600);
    Serial.println("Starting Node 2!");

    xbee.begin(9600);
    xbee.println("Hello, world?");
}

void loop()
{
    static uint32_t prev_ms = millis();
    if (millis() - prev_ms > 1000)
    {
        prev_ms = millis();
        sendData();
    }
    if (xbee.available())
    {
        Serial.write(xbee.read());
    }
    updateSensor();
}