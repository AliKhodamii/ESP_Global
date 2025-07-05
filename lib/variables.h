#include <Arduino.h>
#include <ArduinoJson.h>

// limits
const int max_total_unsuccessful_request = 20;
const int max_unsuccessful_request = 3;
const int max_disconnect_time = 180;
int unsuccessful_request_count = 0;
int total_unsuccessful_request_count = 0;

// ssg_token
const String ssg_token = "abc";

// Connect WiFi
const String ssid = "TP_Sed";
const String password = "87654321";

// Pins
int valvePin = D0;
int humiditySensorEn = D1;
int humidityPin = A0;

// Time variable
unsigned long int loop5sec = 0;
unsigned long int loop1sec = 0;
unsigned long int readHumidityLoop = 0;
unsigned long int valveOpenTime = 0;
unsigned long int duration = 0;

// Api path
String cmdUrl = "http://sed-smarthome.ir/ssg/ssgBackend/api/esp/commands.php";
String postSysStatusApi = "http://sed-smarthome.ir/ssg/ssgBackend/api/esp/data.php";

// System variables
bool valveState = false;
int humidity = 0;

int t = 0;
void pt()
{
    Serial.println("pt in variables.h: " + String(t));
}

void callpt()
{
    pt();
}