#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "../lib/pins.h"
#include "../lib/variables.h"
#include "../lib/functions.h"
#include "../lib/network.h"
#include "../lib/handleCommand.h"

void setup()
{
    Serial.begin(9600);

    wifiConnect(ssid, password);

    // Initial pin setting
    pinMode(valvePin, OUTPUT);
    pinMode(humiditySensorEn, OUTPUT);
    pinMode(humidityPin, INPUT);

    digitalWrite(valvePin, LOW);
    digitalWrite(humiditySensorEn, LOW);

    // Read humidity
    humidity = humidity_read(humidityPin);

    // Post system status
    postStatus(valveState, humidity, duration, "valve1");
}

void loop()
{

    if (millis() - loop5sec > 5000)
    {

        Serial.println("");
        Serial.println("");
        Serial.println("");

        Serial.println("=========================================");

        // Printing the current state
        valveState = digitalRead(valvePin);
        Serial.println("current state:");
        Serial.print("Valve State: ");
        Serial.println(valveState ? "Open" : "Closed");
        Serial.print("Humidity: ");
        Serial.println(humidity);
        Serial.print("Duration: ");
        Serial.println(duration);
        Serial.println("");

        // Get new commands
        JsonDocument cmd;
        cmd = getCmd(ssg_token, cmdUrl);
        if (!cmd.isNull())
        {
            // Handle the command
            JsonDocument command = handleCommand(cmd);
            if (command["status"] == "new command received")
            {
                String valve_name = command["valve_name"].as<String>();
                String commandType = command["command"].as<String>();
                duration = command["duration"].as<int>();

                Serial.print("Valve Name: ");
                Serial.println(valve_name);
                Serial.print("Command: ");
                Serial.println(commandType);
                Serial.print("Duration: ");
                Serial.println(duration);

                // Execute the command
                if (commandType == "open")
                {
                    valveState = true;
                    valveOpenTime = millis();
                    openValve(valvePin, valve_name);
                }
                else if (commandType == "close")
                {
                    valveState = false;
                    closeValve(valvePin, valve_name);
                }
                else
                {
                    Serial.println("Unknown command received");
                }
            }
            else
            {
                Serial.println(command["status"].as<String>());
            }
        }
        else
        {
            Serial.println("No command received or command is null");
        }
        Serial.println("");

        // Post system info
        postStatus(valveState, humidity, duration, "valve1");

        loop5sec = millis();
    }

    if (millis() - loop1sec > 1000)
    {
        // If the valve is open, keep it open for the specified duration
        if (valveState && (millis() - valveOpenTime >= duration * 60 * 1000))
        {
            closeValve(valvePin, "valve1");
            valveState = false;
            Serial.println("Valve closed after duration");
        }
        else if (valveState && (millis() - valveOpenTime < duration * 60 * 1000))
        {
            Serial.println("Valve is still open, waiting for duration to complete");
        }
        loop1sec = millis();
    }

    // Read humidity every 30 min or if valve is open every 1 min
    if (millis() - readHumidityLoop > 1800000 || (valveState && millis() - readHumidityLoop > 60000))
    {
        Serial.println("reading humidity...");
        humidity = humidity_read(humidityPin);
        readHumidityLoop = millis();
        Serial.println("");
    }

    // Reset ESP after max unsuccessful request reached
    if (total_unsuccessful_request_count >= max_total_unsuccessful_request)
    {
        ESP.restart();
    }
}
