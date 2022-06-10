#include <Servo.h>   //載入函式庫，這是內建的，不用安裝


Servo myservo;  // 建立SERVO物件
String num;
void setup() {
  myservo.attach(4);  // 設定要將伺服馬達接到哪一個PIN腳
  Serial.begin(115200);
}

void loop(){

    while(Serial.available()){
        num  = Serial.read();
        turn(num);
      }
}

void turn(String num) {  
        if(num == "1"){
         myservo.write(0);//旋轉到0度，就是一般所說的歸零
         Serial.println(num);
          }else if(num == "2"){
         myservo.write(90);//旋轉到0度，就是一般所說的歸零
         Serial.println(num);
            }else if(num == "3"){
         myservo.write(180);//旋轉到0度，就是一般所說的歸零
         Serial.println(num);
              }
        
}
