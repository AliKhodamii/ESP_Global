#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

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

JsonDocument getCmd(String ssg_token, String cmdUrl)
{
    Serial.println("Getting new commands");
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

    JsonDocument response;
    // response = json_decode(payload);
    DeserializationError error = deserializeJson(response, payload);

    if (error)
    {
        Serial.print("JSON parse failed: ");
        Serial.println(error.c_str());
        return JsonDocument(); // return empty array
    }

    return response;
}

JsonDocument postStatus(bool valveState, int humidity, int duration, String valve_name)
{
    Serial.println("Posting system status");

    WiFiClient client;
    HTTPClient http;
    String payload = "";

    JsonDocument data;

    // 1. Add token
    data["ssg_token"] = "abc";

    // 2. Create 'valves' array
    JsonArray valves = data.createNestedArray("valves");

    JsonObject valve1 = valves.createNestedObject();
    valve1["name"] = valve_name;
    valve1["duration"] = duration;
    valve1["status"] = valveState ? 1 : 0;

    // 3. Create 'humiditySensors' array
    JsonArray humiditySensors = data.createNestedArray("humiditySensors");

    JsonObject sensor1 = humiditySensors.createNestedObject();
    sensor1["name"] = "sensor1";
    sensor1["value"] = humidity;

    // 4. Serialize to string and print
    String jsonString;
    serializeJson(data, jsonString);

    String jsonData = json_encode(data);

    http.begin(client, postSysStatusApi);
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

    JsonDocument response;
    // response = json_decode(payload);
    DeserializationError error = deserializeJson(response, payload);

    if (error)
    {
        Serial.print("JSON parse failed: ");
        Serial.println(error.c_str());
        return JsonDocument(); // return empty array
    }

    Serial.println("Post response status: " + String(response["status"].as<String>()));
    Serial.println("");

    return response;
}