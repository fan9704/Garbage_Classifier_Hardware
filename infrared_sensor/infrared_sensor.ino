int IRpin = 0;                                    // analog pin for reading the IR sensor
float sensorValue, distance;

void setup()
{
  Serial.begin(9600);                             // start the serial port
  pinMode(IRpin, INPUT);//set infrared sensor
}

void loop()
{
  sensorValue = analogRead(IRpin);
  sensorValue *= 0.0048828125;
  distance = 60.495 * pow(sensorValue,-1.1904);

  Serial.println(distance);

  delay(1000);  
}
