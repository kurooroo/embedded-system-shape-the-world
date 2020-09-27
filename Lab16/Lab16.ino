#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "ssid";
const char* pwd = "password";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, pwd);

  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected");
  HTTPClient http;
  http.begin("http://embsysmooc.appspot.com/query?city=Hanoi%20Vietnam&id=nta%203100&greet=Thank%20you%20for%20this%20wonderful%20course.&edxcode=3100");
  int httpCode = http.GET();
  if(httpCode > 0){
    String payload = http.getString();
    Serial.println(payload);
  }
  http.end();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}
