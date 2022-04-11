#include <Ultrasonic.h>
Ultrasonic ultrasonic(8, 9);
#define buzz 4 
String status;
const int max_dist=30;
long duration;
int distance;
void setup() {
  pinMode(buzz, OUTPUT);
  Serial.begin(9600); 
}
void loop() {
  distance=ultrasonic.read();
  Serial.println(distance);
  if(distance<max_dist){
    digitalWrite(buzz, HIGH);
  }else{
    digitalWrite(buzz, LOW);
  }
  if(Serial.available()!=0){
    status=Serial.readString();           
    if (status=="ON"){
      Serial.println("ON");
      digitalWrite(buzz, HIGH);
      while(Serial.available()==0){}
    }
    if (status=="OFF"){
        digitalWrite(buzz, LOW);
        Serial.println("OFF");
    }                   
    }
  delay(1000);
}
