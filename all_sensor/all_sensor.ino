#include <Servo.h>   //載入函式庫，這是內建的，不用安裝
#include "HX711.h"

// 接線設定
const int DT_PIN = 6;
const int SCK_PIN = 5;

const int scale_factor = -417; //比例參數，從校正程式中取得

HX711 scale;

Servo myservo;  // 建立SERVO物件


int IRpin = 0;                                    // analog pin for reading the IR sensor
float sensorValue, distance;

void setup()
{
  Serial.begin(9600);                             // start the serial port
  pinMode(IRpin, INPUT);//set infrared sensor
  
  myservo.attach(9);  // 設定要將伺服馬達接到哪一個PIN腳,set motor

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

void loop()
{
  sensorValue = analogRead(IRpin);
  sensorValue *= 0.0048828125;
  distance = 60.495 * pow(sensorValue,-1.1904);

  Serial.println(distance);
  Serial.print("weight:");
  Serial.print(scale.get_units(10), 0);
  Serial.print("\n");
  delay(1000);  

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
