#include <Arduino.h>

// ssg_token
const String ssg_token = "abc";

// Connect WiFi
const String ssid = "TP_Sed";
const String password = "87654321";

// Pins
int valvePin = D0;

// Time variable
long long int loop5sec = 0;

// Api path
String cmdUrl = "http://sed-smarthome.ir/ssg/ssgBackend/api/esp/commands.php";
