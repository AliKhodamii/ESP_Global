#include <arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

JsonDocument handleCommand(JsonDocument cmd)
{
    JsonDocument responseDoc;

    if (cmd.containsKey("commands"))
    {
        if (String(cmd["commands"]) == "no commands")
        {
            Serial.println("There's no command");
            responseDoc["status"] = "No commands to process";
        }
        else
        {

            JsonArray commands = cmd["commands"].as<JsonArray>();
            if (!commands.isNull() && commands.size() > 0)
            {
                JsonObject firstCommand = commands[0]; // Access the first element of the array
                if (firstCommand.containsKey("valve_name"))
                {
                    String valve_name = firstCommand["valve_name"].as<String>();
                    String command = firstCommand["command"].as<String>();
                    int duration = firstCommand["duration"].as<int>();

                    if (command == "open")
                    {
                        responseDoc["status"] = "new command received";
                        responseDoc["command"] = "open";
                        responseDoc["valve_name"] = valve_name;
                        responseDoc["duration"] = duration;
                    }
                    else if (command == "close")
                    {
                        responseDoc["status"] = "new command received";
                        responseDoc["command"] = "close";
                        responseDoc["valve_name"] = valve_name;
                    }
                    else
                    {
                        Serial.println("Unknown command");
                        responseDoc["status"] = "Unknown command";
                        return responseDoc;
                    }
                }
                else
                {
                    Serial.println("Key 'valve_name' not found in the first command");
                    responseDoc["status"] = "Key 'valve_name' not found in the first command";
                    return responseDoc;
                }
            }
            else
            {
                Serial.println("'commands' array is empty or invalid");
                responseDoc["status"] = "'commands' array is empty or invalid";
                return responseDoc;
            }
        }
    }
    else
    {
        Serial.println("Key 'commands' not found");
        responseDoc["status"] = "Key 'commands' not found";
        return responseDoc;
    }
    return responseDoc;
}

bool commandAck(String valve_name, String commandType, int duration)
{
    HTTPClient http;
    WiFiClient client;
    String payload = "";

    JsonDocument data;

    data["ssg_token"] = "abc";

    JsonArray executed = data.createNestedArray("executed");

    JsonObject item = executed.createNestedObject();
    item["valve_name"] = valve_name;
    item["command"] = commandType;
    item["duration"] = duration;

    String jsonData = json_encode(data);

    http.begin(client, "http://sed-smarthome.ir/ssg/ssgBackend/api/esp/command_ack.php");
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
        return "error"; // return empty array
    }

    if (response["status"] == "acknowledged")
    {
        Serial.println("Command acknowledged successfully");
        return "acknowledged";
    }
    else
    {
        Serial.println("Failed to acknowledge command");
        return "failed";
    }

    return "error";
}
bool openValve(int valvePin, String valve_name)
{
    digitalWrite(valvePin, HIGH);
    commandAck(valve_name, "open", duration);
    return true;
}

bool closeValve(int valvePin, String valve_name)
{
    digitalWrite(valvePin, LOW);
    commandAck(valve_name, "close", duration);
    return true;
}