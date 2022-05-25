#include <Servo.h>   //載入函式庫，這是內建的，不用安裝
#include "HX711.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define SERVER_IP2 "http://140.125.207.230:8080/api/machine/"
#define SERVER_IP "http://140.125.207.230:8080/api/machine/17/"
#define MACHINE_ID "126"
const int DT_PIN = 15;
const int SCK_PIN = 14;
const int scale_factor = -417; //比例參數，從校正程式中取得
bool useState=false,islocked=false;
HX711 scale;

Servo myservo;  // 建立SERVO物件
Servo myservo2;

int IRpin = 12;                                    // analog pin for reading the IR sensor
char temp;
String garbageType="";
int buttonStatei=0;
const int BUTTON_PIN = 10;  // 按鍵的接腳
const int BUTTON_PIN2 = 13;
const int LED_PIN =  3;   // LED燈的接腳
float sensorValue, distance,weight;
int buttonState = 0;   // 按鈕的狀態
int buttonState2 = 0;


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
      apicontroller.lockMachine();
      islocked=true;
    }
    if(distance>20){
      apicontroller.unlockMachine();  
      islocked=false;
    }
  }  
};
InfraredController infraredcontroller;

void setup() {
  Serial.begin(115200);
  wifiSetting.connectInternet("A31", "09097661233");  //("STASSID", "STAPSK")                           // start the serial port
  pinMode(LED_PIN, OUTPUT);   //設定LED的PIN腳為輸出
  //Serial.println("1");
  pinMode(BUTTON_PIN, INPUT); //設定按鈕的接腳為輸入，因為我們要讀取它的狀態
  //Serial.println("2");
  pinMode(BUTTON_PIN2, INPUT);
  //Serial.println("3");
  pinMode(IRpin, INPUT);//set infrared sensor
  //Serial.println("4");
  myservo.attach(4);  // 設定要將伺服馬達接到哪一個PIN腳,set motor
  //Serial.println("5");
  myservo2.attach(10);
  
  Serial.println("Initializing the scale");//set weight sensor

  scale.begin(DT_PIN, SCK_PIN);

  Serial.println("Before setting up the scale:"); 
  
  Serial.println(scale.get_units(5), 0);  //未設定比例參數前的數值

  scale.set_scale(scale_factor);       // 設定比例參數
  scale.tare();               // 歸零

  Serial.println("After setting up the scale:"); 

  Serial.println(scale.get_units(5), 0);  //設定比例參數後的數值

  Serial.println("Readings:");  //在這個訊息之前都不要放東西在電子稱上
}

void loop() {
  buttonState = digitalRead(BUTTON_PIN); //讀取按鍵的狀態
  buttonState2 = digitalRead(BUTTON_PIN);
  
  if ((WiFi.status() == WL_CONNECTED)) {
    //if(Serial.available()){
      buttonStatei=1;
    //}
    if(/*buttonState == LOW*/buttonStatei==1&&useState==false){          //如果按鍵按了
      apicontroller.lockUserLink();
      useState = true;
      digitalWrite(LED_PIN, HIGH);   //LED就亮了 
    }
    if(/*buttonState2 == LOW*/buttonStatei==2&&useState==true){
      apicontroller.unlockUserLink();  
      useState = false;
      digitalWrite(LED_PIN, LOW);
    }        
  }
  delay(3000);
  while(garbageType.length()==0){
    while(Serial.available()>0){
        garbageType+=Serial.read();
        delay(2000); 
        Serial.println(garbageType);
    }
  } 
  if(useState){
    infraredcontroller.detect();
    weight=scale.get_units(10);
    Serial.println(distance);
    Serial.println(weight);
    if(weight>=10||useState==true){ 
        motorcontroller.topMotor(garbageType); 
        motorcontroller.bottomMotor(garbageType);
        Serial.println("----------------");
        Serial.println(garbageType);
        //delay(5000);  
        motorcontroller.resetmotor();
        apicontroller.recordRecycleInfo(garbageType,"1","1"); // (垃圾類型id,偵測容量,垃圾重量)
        garbageType="";
      }
    }
  
}
