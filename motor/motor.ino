#include <Servo.h>   //載入函式庫，這是內建的，不用安裝


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
