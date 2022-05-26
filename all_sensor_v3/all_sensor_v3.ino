#include <Servo.h>   //載入函式庫，這是內建的，不用安裝
#include "HX711.h"
//#include <arduino-timer.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define SERVER_IP2 "http://140.125.207.230:8080/api/machine/"
#define SERVER_IP "http://140.125.207.230:8080/api/machine/17/"
#define MACHINE_ID "126"
#define SCK_PIN D5
#define DT_PIN D11
#define IRpin A0
const int scale_factor = -417; //比例參數，從校正程式中取得
HX711 scale;

Servo myservo;  // 建立SERVO物件
Servo myservo2;                                  // analog pin for reading the IR sensor
String garbageType,buttonStatei;
int detectfinish=0,useState=0,islocked=0;
const int BUTTON_PIN = 10;  // 按鍵的接腳
const int BUTTON_PIN2 = 13;
const int LED_PIN =  3;   // LED燈的接腳
float sensorValue, distance,weight;
int buttonState = 0;   // 按鈕的狀態
int buttonState2 = 0;
//Timer timer;

class WifiSetting {
  public:
    void connectInternet(String STASSID, String STAPSK) {
      WiFi.begin(STASSID, STAPSK);

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.print("Connected! IP address: ");
      Serial.println(WiFi.localIP());
    };

};

class ApiController {
  public:
    HTTPClient http;
    WiFiClient client;
    int httpCode;

    void lockUserLink(){
      http.begin(client, SERVER_IP "lockUserLink" );          
      http.addHeader("Content-Type", "application/json");
      httpCode = http.PATCH("");

      Serial.println(SERVER_IP  "lockUserLink");
      Serial.println(httpCode);
      http.end();
    };
    
    void unlockUserLink(){
      http.begin(client, SERVER_IP "unlink" );    
      http.addHeader("Content-Type", "application/json");
      httpCode = http.PATCH("");
      
      Serial.println(SERVER_IP  "unlink");
      Serial.println(httpCode);
      http.end();      
    };
    void lockMachine(){
      http.begin(client, SERVER_IP2 "lock/17" );    
      http.addHeader("Content-Type", "application/json");
      httpCode = http.PATCH("");
      
      Serial.println(SERVER_IP  "lockMachine");
      Serial.println(httpCode);
      http.end();      
    };
    void unlockMachine(){
      http.begin(client, SERVER_IP2 "unlock/17" );    
      http.addHeader("Content-Type", "application/json");
      httpCode = http.PATCH("");
      
      Serial.println(SERVER_IP  "unlockMachine");
      Serial.println(httpCode);
      http.end();      
    };
    void recordRecycleInfo(String garbageType, String storage, String garbageWeight ){
      http.begin(client, SERVER_IP "?garbage_records.garbage_type="  +garbageType+ "&garbage_records.weight="  +garbageWeight+ 
      "&machineStorages.garbage_type=" +garbageType+  "&machineStorages.storage=" +storage );         
      
      http.addHeader("Content-Type", "application/json");
      httpCode = http.PATCH("");
      
      Serial.println(SERVER_IP "?garbage_records.garbage_type="  +garbageType+ "&garbage_records.weight="  +garbageWeight+ 
      "&machineStorages.garbage_type=" +garbageType+  "&machineStorages.storage=" +storage);
      Serial.println(httpCode);
      http.end();      
    }
    void printResponse();
};
class MotorController {
  public:
    String garbageType;
  void topMotor(String garbageType) {
    Serial.println("top");  
    if(garbageType=="0"){
        myservo2.write(0);//旋轉到0度，就是一般所說的歸零
    }
    else if(garbageType=="1"){
        myservo2.write(90);//旋轉到0度，就是一般所說的歸零
    }
    else if(garbageType=="2"){
        myservo2.write(180);//旋轉到0度，就是一般所說的歸零
    }
  }
  void bottomMotor(String garbageType) {  
    Serial.println("botton");
    if(garbageType=="0"||garbageType=="2"){
        myservo.write(0);//旋轉到0度，就是一般所說的歸零
    }
    else if(garbageType=="1"){
        myservo.write(90);//旋轉到0度，就是一般所說的歸零
    }
  }
  void resetmotor(){
    myservo2.write(0);
    myservo.write(90);
  }
};
MotorController motorcontroller;
ApiController apicontroller;
WifiSetting wifiSetting;

class InfraredController {
  public:
  void detect() {  
    sensorValue = analogRead(IRpin);
    sensorValue *= 0.0048828125;
    distance = 60.495 * pow(sensorValue,-1.1904);
    if(distance<10){
      Serial.println("鎖住機器...");
      apicontroller.lockMachine();
      islocked=true;
    }
    if(distance>20){
      Serial.println("解鎖機器");
      apicontroller.unlockMachine();  
      islocked=false;
    }
  }  
};
InfraredController infraredcontroller;
bool buttonStatereset(){
  buttonStatei="0";
  return 1;
}

void setup() {
  Serial.begin(115200);
  wifiSetting.connectInternet("ertw", "52695269");  //("STASSID", "STAPSK")                           // start the serial port
  //pinMode(LED_PIN, OUTPUT);   //設定LED的PIN腳為輸出
  //pinMode(BUTTON_PIN, INPUT); //設定按鈕的接腳為輸入，因為我們要讀取它的狀態
  //pinMode(BUTTON_PIN2, INPUT);
  Serial.println("按按鈕以開始使用");
  pinMode(IRpin, INPUT);//set infrared sensor
  Serial.println("按按鈕以開始使用");
  myservo.attach(12);  // 設定要將伺服馬達接到哪一個PIN腳,set motor
  Serial.println("按按鈕以開始使用");
  myservo2.attach(4);
  scale.begin(DT_PIN, SCK_PIN);
  Serial.println("按按鈕以開始使用");
  scale.set_scale(scale_factor);       // 設定比例參數
  Serial.println("按按鈕以開始使用");
  scale.tare();               // 歸零
  Serial.println("按按鈕以開始使用");
  motorcontroller.resetmotor();
  Serial.println("初始化完成");
}

void loop() {
  //buttonState = digitalRead(BUTTON_PIN); //讀取按鍵的狀態
  //buttonState2 = digitalRead(BUTTON_PIN);
  if ((WiFi.status() == WL_CONNECTED)) {     //成功接上網路
    if(useState==0){
      Serial.println("按按鈕以開始使用");
      while(Serial.available()){
        Serial.read();
      } 
      while(!Serial.available()){}                    //未按按鈕不會動            
      buttonStatei=Serial.read();
      delay(5);
      Serial.println("按下按鈕");
      Serial.println("鎖住連結功能...");
      apicontroller.lockUserLink();
      useState = 1;
      //digitalWrite(LED_PIN, HIGH);   //LED就亮了 
    }
  }      
  while(useState==1){
    weight=scale.get_units(10);
    Serial.print("重量:");
    Serial.println(weight);
    delay(1000);
    if(weight>=10&&detectfinish==0){ 
      Serial.println("輸入垃圾類型");
      while(Serial.available()){
        Serial.read();
      } 
      while(!Serial.available()){}
      if(Serial.available()){
        garbageType=Serial.read();
      }
      Serial.println(garbageType);
      motorcontroller.topMotor(garbageType); 
      motorcontroller.bottomMotor(garbageType);
      detectfinish=1;
    }
    else if(weight<10&&detectfinish==1) {
      Serial.println("重制馬達");
      motorcontroller.resetmotor();
      infraredcontroller.detect();
      Serial.print("距離:");
      Serial.println(distance);
      Serial.println("紀錄重量與距離");
      apicontroller.recordRecycleInfo(garbageType,"1","1"); // (垃圾類型id,偵測容量,垃圾重量)
      detectfinish=0;
      Serial.println("垃圾掉進桶子");
      Serial.println("輸入0或繼續放垃圾");
   }
    if(Serial.available()){
      buttonStatei=Serial.read();
    }
    //timer.tick();
    //timer.at(5, buttonStatereset);
    if(buttonStatei=="0"){ 
      useState = 0;
      Serial.println("解鎖連接功能");
      apicontroller.unlockUserLink(); 
      //timer.cancel();
      Serial.println("結束使用");
      Serial.println("");
      Serial.println("");
    }
  }
}
