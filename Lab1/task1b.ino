#define ledpin LED_BUILTIN
double randNumber=5;
int timePeriod = 2000; // 2sec
int n = 10;
int data;
double delaytlow;
double delaythigh;


void setup()
{
    Serial.begin(9600);
    pinMode(ledpin, OUTPUT);

    // if analog input pin 0 is unconnected, random analog
    // noise will cause the call to randomSeed() to generate
    // different seed numbers each time the sketch runs.
    // randomSeed() will then shuffle the random function.
    Serial.println("setup started");
    randomSeed(analogRead(0));
}
void loop()
{
    if (n != 0)
    {
        Serial.print("current n: ");
        Serial.print(n);
        Serial.print(" current x: ");
        Serial.println(randNumber);

        n -= 1;
        digitalWrite(ledpin, HIGH);
        delay(delaythigh);
        digitalWrite(ledpin, LOW);
        delay(delaytlow);
    }
    else if (Serial.available())
    {

        n = Serial.parseInt();
        Serial.print("Set n to : ");
        Serial.println(n);     
        randNumber = Serial.parseInt();
        Serial.print("Set randomNumber to :");
        Serial.println(randNumber);
        delaythigh = (timePeriod * randNumber) / 9.0;
        delaytlow = timePeriod * (1.0 - (randNumber / 9.0));
    }
}
