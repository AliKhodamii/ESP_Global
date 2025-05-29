#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "../lib/functions.h"
#include "../lib/variables.h"

void setup()
{
  // set ssg_token
  sysInfo["ssg_token"] = ssg_token;
  pinMode(valveP, OUTPUT);
  pinMode(humidityP, INPUT);

  digitalWrite(valveP, LOW);

  Serial.begin(9600);
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");

  wifi_connect(ssid, password);

  // set initial values
  valve = false;
  humidity = humidity_read(humidityP);

  // get duration from sysInfo on host
  sysInfo = getSysData(ssg_token);
  duration = sysInfo["duration"];

  // prepare data for sysInfo post
  sysInfo["time"] = time();
  sysInfo["valve"] = valve;
  sysInfo["humidity"] = humidity;
  sysInfo["duration"] = duration;
  sysInfo["copy"] = copy;
  sysInfoJson = json_encode(sysInfo);

  // post request to sysInfo to set new status
  Serial.println("Posting init sysInfo");
  postResCode = httpPost(postSysInfoAPI, ssg_token, sysInfoJson);

  // // prepare data for cmd post
  // cmd["valveCmd"] = "noAction";
  // cmd["restartCmd"] = false;
  // cmd["espConfirm"] = false;
  // cmd["durationCmd"] = duration;
  // cmdJson = json_encode(cmd);

  // // post request to cmd to set now command there
  // Serial.println("Posting init cmdInfo");
  // postResCode = httpPost(postSysInfoAPI, "cmdInfo", cmdJson);
}

void loop()
{
  // loop every 5 sec
  if (millis() - every5s >= 5000)
  {
    Serial.println("");
    Serial.println("");
    Serial.print("--------");
    Serial.print("New loop");
    Serial.println("--------");
    Serial.println("");

    // get cmd
    Serial.println("Getting cmd...");
    cmd = getCmdData(ssg_token);

    // check if any command was pending
    if (String(cmd["status"] == "pending"))
    {
      // if it's a open command
      if (String(cmd["cmd"]) == "open")
      {
        Serial.println("Open command received \n Opening valve");

        // open valve
        valve = true;
        copy = true;
        duration = cmd["duration"];

        // set time variables
        valveTimer = millis();
        copyTimer = millis();

        // post new status
        Serial.println("Posting new status...");
        sysInfo["time"] = time();
        sysInfo["valve"] = valve;
        sysInfo["copy"] = copy;
        sysInfo["duration"] = duration;
        sysInfoJson = json_encode(sysInfo);
        httpPost(postSysInfoAPI, ssg_token, sysInfoJson);

        // post executed
        Serial.println("letting Server Know cmd is executed.");
        cmd["cmd"] = valve;
        cmd["status"] = "executed";
        cmdJson = json_encode(cmd);
        httpPost(postCmdAPI, ssg_token, cmdJson);

        // inserting irr rec to db is with php and with the previous request;
      }
      else if (String(cmd["cmd"] == "close"))
      {
        Serial.println("Close command received \n Closing valve");

        // open valve
        valve = false;
        copy = true;
        duration = cmd["duration"];

        // post new status
        Serial.println("Posting new status...");
        sysInfo["time"] = time();
        sysInfo["valve"] = valve;
        sysInfo["copy"] = copy;
        sysInfo["duration"] = duration;
        sysInfoJson = json_encode(sysInfo);
        httpPost(postSysInfoAPI, "sysInfo", sysInfoJson);

        // post executed
        Serial.println("letting Server Know cmd is executed.");
        cmd["cmd"] = valve;
        cmd["status"] = "executed";
        cmdJson = json_encode(cmd);
        httpPost(postCmdAPI, ssg_token, cmdJson);
      }
    }

    // // check if open command is sent
    // if (String(cmd["valveCmd"]) == "open")
    // {
    //   Serial.println("Open command received \n Opening valve");

    //   // open valve
    //   valve = true;
    //   copy = true;
    //   duration = cmd["durationCmd"];

    //   // set time variables
    //   valveTimer = millis();
    //   copyTimer = millis();

    //   // post new status
    //   Serial.println("Posting new status...");
    //   sysInfo["time"] = time();
    //   sysInfo["valve"] = valve;
    //   sysInfo["copy"] = copy;
    //   sysInfo["duration"] = duration;
    //   sysInfoJson = json_encode(sysInfo);
    //   httpPost(postSysInfoAPI, "sysInfo", sysInfoJson);

    //   // post confirm and no action in cmd
    //   Serial.println("Posting new cmd...");
    //   cmd["valveCmd"] = "noAction";
    //   cmd["confirm"] = true;
    //   cmdJson = json_encode(cmd);
    //   httpPost(postSysInfoAPI, "cmdInfo", cmdJson);

    //   // insert new irr record to database
    //   Serial.println("Inserting new data to db...");
    //   if (httpPost(DbUrl, "insertIrrRec", sysInfoJson))
    //   {
    //     Serial.println("Succeed.");
    //   }
    //   else
    //   {
    //     Serial.println("failed.");
    //   }
    // }

    // // check if close command is sent
    // if (String(cmd["valveCmd"]) == "close")
    // {
    //   Serial.println("Close command received \n Closing valve");

    //   // open valve
    //   valve = false;
    //   copy = true;
    //   duration = cmd["durationCmd"];

    //   // post new status
    //   Serial.println("Posting new status...");
    //   sysInfo["time"] = time();
    //   sysInfo["valve"] = valve;
    //   sysInfo["copy"] = copy;
    //   sysInfo["duration"] = duration;
    //   sysInfoJson = json_encode(sysInfo);
    //   httpPost(postSysInfoAPI, "sysInfo", sysInfoJson);

    //   // post confirm and no action in cmd
    //   Serial.println("Posting new cmd...");
    //   cmd["valveCmd"] = "noAction";
    //   cmd["confirm"] = true;
    //   cmdJson = json_encode(cmd);
    //   httpPost(postSysInfoAPI, "cmdInfo", cmdJson);
    // }

    // valve status
    if (valve)
    {
      digitalWrite(valveP, HIGH);
    }
    else
    {
      digitalWrite(valveP, LOW);
    }

    // check if it's time to close valve
    if (valve && (millis() - valveTimer) > duration * 60000)
    {
      Serial.println("Duration is deon\n Valve is closing");
      valve = false;
      digitalWrite(valveP, LOW);

      // post new status
      Serial.println("Posting new status...");
      sysInfo["time"] = time();
      sysInfo["valve"] = valve;
      sysInfo["copy"] = copy;
      sysInfo["duration"] = duration;
      sysInfoJson = json_encode(sysInfo);
      httpPost(postSysInfoAPI, ssg_token, sysInfoJson);
    }
    // post status
    Serial.println("Posting status");

    humidity = humidity_read(humidityP);
    sysInfo["time"] = time();
    sysInfo["valve"] = valve;
    sysInfo["duration"] = duration;
    sysInfo["copy"] = copy;
    sysInfo["humidity"] = humidity;

    sysInfoJson = json_encode(sysInfo);
    httpPost(postSysInfoAPI, ssg_token, sysInfoJson);

    Serial.println("sysInfoJson:");
    Serial.println(sysInfoJson);

    every5s = millis();
  }

  // check if copy time is done
  if (millis() - copyTimer > 10000)
  {
    copy = false;
  }
}