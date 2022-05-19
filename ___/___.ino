//#include <SharpIR.h>

#include <SharpIR.h>
#include <Wire.h>

#define IR A2 // define signal pin
#define model 20150 // used 1080 because model GP2Y0A21YK0F is used
// Sharp IR code for Robojax.com
// ir: the pin where your sensor is attached
// model: an int that determines your sensor:  1080 for GP2Y0A21Y
//                                            20150 for GP2Y0A02Y
//                                            430 for GP2Y0A41SK   
/*
2 to 15 cm GP2Y0A51SK0F  use 1080
4 to 30 cm GP2Y0A41SK0F / GP2Y0AF30 series  use 430
10 to 80 cm GP2Y0A21YK0F  use 1080
10 to 150 cm GP2Y0A60SZLF use 10150
20 to 150 cm GP2Y0A02YK0F use 20150
100 to 550 cm GP2Y0A710K0F  use 100550

 */

SharpIR SharpIR(IR, model);
void setup() {
 Serial.begin(9600);
 Serial.println("Robojax Sharp IR  ");
 
}

void loop() {
  delay(500);   
  int dis=SharpIR.distance();  // this returns the distance to the object you're measuring
  Serial.print("Mean distance: ");  // returns it to the serial monitor
  Serial.println(dis);
}
