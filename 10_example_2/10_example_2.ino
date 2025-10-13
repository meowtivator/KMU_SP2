#include <Servo.h>
#define PIN_SERVO 10

Servo myservo;

void setup() {
  myservo.attach(PIN_SERVO); 
  // 0 == 6
  // 90 == 105
  myservo.writeMicroseconds(1600);
  delay(1000);
}

void loop() {
    // add code here.
//    myservo.write(0);
//    delay(500);
//    myservo.write(90);
//    delay(500);
//    myservo.write(180);
//    delay(500);
//    myservo.write(90);
//    delay(500);
//    myservo.write(0);
//    delay(500);
}
