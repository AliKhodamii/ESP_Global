#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include "./variables.h"

bool wifi_connect(String ssid, String password)
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

String json_encode(JsonDocument data)
{
    String result = "";
    serializeJson(data, result);
    return result;
}

JsonDocument json_decode(String json)
{
    JsonDocument data;
    deserializeJson(data, json);
    return data;
}

int httpPost(String url, String ssg_token, String data)
{
    data = "ssg_token=" + ssg_token + "data=" + data;

    WiFiClient client;
    HTTPClient http;
    int httpResCode = -1;
    while (httpResCode != 200)
    {
        http.begin(client, url);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        httpResCode = http.POST(data);
        Serial.print("Http post response code: ");
        Serial.print(httpResCode);
        if (httpResCode != 200)
        {
            Serial.println(" ...Failed.");
        }
    }

    Serial.println(" ...Succeed.");
    return httpResCode;
}

String httpGet(String url, String sst_token)
{
    WiFiClient client;
    HTTPClient http;
    int httpResponseCode = -1;

    url = url + "?ssg_token=" + ssg_token;
    String payload = "{}";

    while (httpResponseCode != 200)
    {
        http.begin(client, url);
        httpResponseCode = http.GET();

        if (httpResponseCode > 0)
        {
            Serial.print("Http get response code:  ");
            Serial.print(httpResponseCode);
            Serial.println(" ...Succeed.");
            payload = http.getString();
        }
        else
        {
            Serial.print("Http get response code:  ");
            Serial.print(httpResponseCode);
            Serial.println(" ...Failed.");
        }
    }

    http.end();
    return payload;
}

int humidity_read(int analogPin)
{
    int humidity = 0;
    for (int i = 0; i < 10; i++)
    {
        humidity += analogRead(analogPin);
    }
    humidity = humidity / 10;
    return humidity;
}

String time()
{
    int mil = millis();
    int d = mil / 86400000;
    mil -= (d * 86400000);
    int h = mil / 3600000;
    mil -= h * 3600000;
    int m = mil / 60000;
    mil -= m * 60000;
    int s = mil / 1000;

    String time = String(d) + " : " + String(h) + " : " + String(m) + " : " + String(s);
    return time;
}

JsonDocument getSysData(String ssg_token)
{
    JsonDocument sysInfo;
    String sysInfoJson;

    sysInfoJson = httpGet(getSysInfoAPI, ssg_token);
    sysInfo = json_decode(sysInfoJson);
    if (sysInfo != "")
    {
        return sysInfo;
    }
    else
    {
        return;
    }
}

JsonDocument getCmdData(String ssg_token)
{
    JsonDocument cmd;
    String cmdJson;

    cmdJson = httpGet(getCmdAPI, ssg_token);
    cmd = json_decode(cmdJson);
    if (cmd != "")
    {
        return cmd;
    }
    else
    {
        return;
    }
}
