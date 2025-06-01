#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
// #include "./functions.h"
// #include "./constVariables.h"

bool wifiConnect(String ssid, String password)
{
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.print(ssid);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());

    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }
    else
    {
        return false;
    }
}

String getCmd(String ssg_token , String cmdUrl)
{
    WiFiClient client;
    HTTPClient http;
    String payload = "";

    JsonDocument data;
    data["ssg_token"] = ssg_token;

    String jsonData = json_encode(data);

    http.begin(client, cmdUrl);
    http.addHeader("content-type", "application/json");

    int responseCode = http.POST(jsonData);
    if (responseCode > 0)
    {
        Serial.print("HTTP Response code: ");
        Serial.println(responseCode);
        payload = http.getString();
        // Serial.println(payload);
    }
    else
    {
        Serial.print("Error code: ");
        Serial.println(responseCode);
    }
    // Free resources
    http.end();

    return payload;
}