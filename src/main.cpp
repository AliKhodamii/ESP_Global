#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "../lib/functions.h"
#include "../lib/network.h"
#include "../lib/constVariables.h"

void setup()
{
    Serial.begin(9600);

    wifiConnect(ssid, password);

    // Initial pin setting
    pinMode(valvePin, OUTPUT);
    digitalWrite(valvePin, LOW);
}

void loop()
{
    if (millis() - loop5sec > 5000)
    {
        String cmd = getCmd(ssg_token,cmdUrl);
        Serial.println("cmd: " + cmd);
        loop5sec = millis();
    }
}
