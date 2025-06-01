#include <Arduino.h>
#include <ArduinoJson.h>

// SSG-Token

// pins
int valveP = D7;
int humidityP = A0;

// system variables
bool valve = true;
bool copy = false;
int humidity = 1000;
long unsigned int duration = 0;

// cmd variables
String valveCmd = "";

// urls
String postSysInfoAPI = "https://sed-smarthome.ir/ssg/php/postSysInfoAPI.php";
String postCmdAPI = "https://sed-smarthome.ir/ssg/php/postCmdAPI.php";
String getSysInfoAPI = "https://sed-smarthome.ir/ssg/php/getSysInfoAPI.php";
String getCmdAPI = "https://sed-smarthome.ir/ssg/php/getCmdAPI.php";
String DbUrl = "http://sed-smarthome.ir/dayi_hossein/server/insertIrrRec.php";

JsonDocument sysInfo, cmd;

// jsons
String sysInfoJson;
String cmdJson;

// time
long unsigned int every5s = 0;
long unsigned int valveTimer = 0;
long unsigned int copyTimer = 0;

// request codes
int postResCode = 0;
