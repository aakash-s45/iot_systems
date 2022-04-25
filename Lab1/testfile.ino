#include <items.h>

TV tv(13);

void setup(){
    serial.begin(9600);
}

void loop(){
    Serial.print("Tv Status: ");
    // Serial.println(tv);
    tv.toggle();
}