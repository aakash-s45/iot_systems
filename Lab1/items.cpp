#include "Arduino.h"
#include "items.h"

TV::TV(int tvpin){
    tvPin = tvpin;
    pinMode(tvPin, OUTPUT);
    tvState = LOW;
}
void TV::toggle(){
    if (tvState == LOW){
        tvState = HIGH;
        digitalWrite(tvPin, tvState);
    }
    else{
        tvState = LOW;
        digitalWrite(tvPin, tvState);
    }
}

Fan::Fan(int fanpin){
        fanPin = fanpin;
        pinMode(fanPin, OUTPUT);
        fanState = LOW;
    }
void Fan::toggle(){
    if (fanState == LOW){
            fanState = HIGH;
            digitalWrite(fanPin, fanState);
    }
    else{
            fanState = LOW;
            digitalWrite(fanPin, fanState);
    }
}

AC::AC(int acpin){
        acPin = acpin;
        pinMode(acPin, OUTPUT);
        acState = LOW;
    }
void AC::toggle(){
    if (acState == LOW){
            acState = HIGH;
            digitalWrite(acPin, acState);
    }
    else{
            acState = LOW;
            digitalWrite(acPin, acState);
    }
}

Light::Light(int red_pin,int green_pin,int blue_pin){
        redPin=red_pin;
        greenPin=green_pin;
        bluePin=blue_pin;
        pinMode(redPin, OUTPUT);
        pinMode(greenPin, OUTPUT);
        pinMode(bluePin, OUTPUT);
        lightTemp=0;
        lightState=0;
    }
    void Light::warm{
        lightState=HIGH;
        analogWrite(redPin, 253);
        analogWrite(greenPin, 244);
        analogWrite(bluePin, 220);
    }
    void Light::cold{
        lightState=HIGH;
        analogWrite(redPin, 255);
        analogWrite(greenPin, 255);
        analogWrite(bluePin, 255);
    }
    void Light::toggle(){
        if (lightState == LOW){
            lightState=HIGH;
            digitalWrite(lightPin, acState);
        }
        else{
            analogWrite(redPin, 255);
            analogWrite(greenPin, 255);
            analogWrite(bluePin, 255);
            lightState=HIGH;
        }
    }