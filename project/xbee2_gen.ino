#include <ArduinoJson.h>
#include <SoftwareSerial.h>

DynamicJsonDocument doc(512);
SoftwareSerial xbee(10, 11);

class Generator
{

    double randNumber;
    int timePeriod;
    int n = 5;
    double delaytlow;
    double delaythigh;
    int debug;
    int _ledpin;
    int _start;
    int _end;
    double diff;

public:
    Generator(int pin, int st, int ed);
    void run();
    int val();
};

Generator::Generator(int ledpin, int st, int ed)
{
    _ledpin = ledpin;
    _start = st;
    _end = ed;
    diff = ed - st;
    pinMode(_ledpin, OUTPUT);
    randomSeed(analogRead(A0));
    timePeriod = 1000;
    n = 10;
    debug = 0;
}
void Generator::run()
{
    if (n != 0)
    {
        if (debug)
        {
            Serial.print("current n: ");
            Serial.print(n);
            Serial.print(" current x: ");
            Serial.println(randNumber);
        }
        n -= 1;
        // digitalWrite(_ledpin, HIGH);
        // delay(delaytlow);
        // digitalWrite(_ledpin, LOW);
        // delay(delaytlow);
    }
    else
    {
        randNumber = random(_start, _end);
        if (debug)
        {
            Serial.print("Generated number: ");
            Serial.println(randNumber);
        }
        n = 10;
        delaytlow = (timePeriod * randNumber) / diff;
        delaythigh = timePeriod * (1.0 - (randNumber / diff));
    }
}
int Generator::val()
{
    return this->randNumber;
}

Generator temp(13, 15, 50);  // range ->(1,100)
Generator humid(3, 15, 100); // range ->(15,50)

// String messageToSend()
// {
//     doc["from"] = "xbee_2";
//     doc["to"] = "ble_1";
//     doc["temp"] = temp.val();
//     doc["humid"] = humid.val();
//     String msg = "";
//     serializeJson(doc, msg);
//     doc.clear();
//     return msg;
// }

void updateSensor()
{
    temp.run();
    humid.run();
}

// void sendData()
// {
//     String msg = messageToSend();
//     // Serial.println(msg);
//     xbee.println(msg.c_str());
// }
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
    if (millis() - prev_ms > 5000)
    {
        prev_ms = millis();
        doc["from"] = "xbee2";
        doc["to"] = "ble_1";
        doc["temp"] = temp.val();
        doc["humid"] = humid.val();
        String msg = "";
        serializeJson(doc, msg);
        doc.clear();
        xbee.write(msg.c_str());
    }
    if (xbee.available())
    {
        Serial.write(xbee.read());
    }
    updateSensor();
}
