#include <Servo.h>

#define PIN_SERVO 10
#define _DUTY_MIN 550  
#define _DUTY_NEU 1600 
#define _DUTY_MAX 2400 
#define _POS_START (_DUTY_MIN + 100)   
#define _POS_END   (_DUTY_MAX - 100)    
#define INTERVAL 20

unsigned long last_sampling_time;
Servo myservo;
float duty_change_per_interval;
int duty_target;
float duty_curr_float;
float _SERVO_SPEED = 0.3;
bool flag = false;

void setup() {
  myservo.attach(PIN_SERVO); 
  
  duty_target = _POS_END;
  duty_curr_float = _POS_START;
  myservo.writeMicroseconds((int)duty_curr_float);
  
  Serial.begin(57600);
  
  duty_change_per_interval = 
    (float)(_DUTY_MAX - _DUTY_MIN) * (_SERVO_SPEED / 180.0) * (INTERVAL / 1000.0);
  
  Serial.println("=== Servo Control Start ===");
  Serial.print("SERVO_SPEED: "); 
  Serial.print(_SERVO_SPEED); 
  Serial.println(" deg/sec");
  
  Serial.print("duty_change_per_interval: "); 
  Serial.println(duty_change_per_interval, 6);
  
  last_sampling_time = 0;
}

void loop() {
  if (millis() < (last_sampling_time + INTERVAL))
    return;
  
  if (duty_target > duty_curr_float) {
    duty_curr_float += duty_change_per_interval;
    if (duty_curr_float >= duty_target) {
      duty_curr_float = duty_target;
      flag = true;
    }
  } 
  else if (duty_target < duty_curr_float) {
    duty_curr_float -= duty_change_per_interval;
    if (duty_curr_float <= duty_target) {
      duty_curr_float = duty_target;
      flag = true;
    }
  }
  
  int duty_curr_int = (int)duty_curr_float;
  myservo.writeMicroseconds(duty_curr_int);
  
  Serial.print("Min:500");
  Serial.print(",duty_target:"); Serial.print(duty_target);
  Serial.print(",duty_curr:"); Serial.print(duty_curr_int);
  Serial.print(",duty_float:"); Serial.print(duty_curr_float, 2);
  Serial.println(",Max:2500");
  
  if (flag) {
    flag = false;
    _SERVO_SPEED = 3.0;
    
    duty_change_per_interval = 
      (float)(_DUTY_MAX - _DUTY_MIN) * (_SERVO_SPEED / 180.0) * (INTERVAL / 1000.0);
    
    duty_curr_float = _POS_START;
    duty_target = _POS_END;
    myservo.writeMicroseconds((int)duty_curr_float);
    
    Serial.println("\n>>> Target reached! Restarting with SERVO_SPEED = 3.0");
    Serial.print("duty_change_per_interval: "); 
    Serial.println(duty_change_per_interval, 6);
  }
  
  last_sampling_time += INTERVAL;
}
