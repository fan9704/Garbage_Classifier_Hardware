#include <Servo.h>   //載入函式庫，這是內建的，不用安裝
#include <ESP8266WiFi.h> 
#include <ESP8266HTTPClient.h>

Servo myservo;  // 建立SERVO物件

void setup() {
  myservo.attach(9);  // 設定要將伺服馬達接到哪一個PIN腳
  Serial.begin(9600);
}

void loop(){
    int num;
    while(Serial.available()){
        num  = Serial.read();
        turn(num);
      }
}

void turn(int num) {  
  switch(num){
        case '1':
         myservo.write(0);//旋轉到0度，就是一般所說的歸零
         Serial.println(num);
         break;
     
        case '2':
         myservo.write(90);//旋轉到0度，就是一般所說的歸零
         Serial.println(num);
         break;
        
        case '3':
         myservo.write(180);//旋轉到0度，就是一般所說的歸零
         Serial.println(num);
         break;
        
        case '4':
         myservo.write(90);//旋轉到0度，就是一般所說的歸零
         Serial.println(num);
         break;

         default:
         Serial.println("0");
        }
}

void internet_link(){
    WiFiClient client;
    HTTP Client http;

    //Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    http.begin(client, "http://140.125.207.230:8080/api/machines_storage"); //HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header and body
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  }
