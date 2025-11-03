#include <Servo.h>

// Arduino pin assignment
#define PIN_IR    0         // IR sensor at Pin A0
#define PIN_LED   9
#define PIN_SERVO 10

#define _DUTY_MIN 550  
#define _DUTY_NEU 1600 
#define _DUTY_MAX 2400 

#define _DIST_MIN  100.0   // minimum distance 100mm
#define _DIST_MAX  250.0   // maximum distance 250mm

#define EMA_ALPHA  0.3     // for EMA Filter
#define LOOP_INTERVAL 20   // Loop Interval (unit: msec)

Servo myservo;
unsigned long last_loop_time;   // unit: msec

// 전역 변수 선언
float dist_prev = _DIST_MIN;    // 이전 거리 값 저장
float dist_ema = _DIST_MIN;     // EMA 필터 적용된 거리 값
float dist_filtered = _DIST_MIN; // 범위 필터 적용된 거리 값
bool first = true;              // 첫 실행 여부 플래그

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);
  
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);
  
  Serial.begin(1000000);    // 1,000,000 bps
}

void loop()
{
  unsigned long time_curr = millis();
  int duty;
  float a_value, dist_raw;
  
  // wait until next event time
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;
  
  a_value = analogRead(PIN_IR);
  
  dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0 - 60.0;
  
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
      dist_filtered = dist_prev;          
  } else if (dist_raw < _DIST_MIN) {
      dist_filtered = dist_prev;          
  } else {    
      dist_filtered = dist_raw;            
      dist_prev = dist_raw;               
  }
  
  if (first) {
    dist_ema = dist_filtered;
    first = false;
  }
  
  dist_ema = EMA_ALPHA * dist_filtered + (1.0 - EMA_ALPHA) * dist_ema;
  
  // duty = map(dist_ema, _DIST_MIN, _DIST_MAX, _DUTY_MIN, _DUTY_MAX);
  duty = (dist_ema - _DIST_MIN) * (_DUTY_MAX - _DUTY_MIN) / (_DIST_MAX - _DIST_MIN) + _DUTY_MIN;
  
  if (duty < _DUTY_MIN) duty = _DUTY_MIN;
  if (duty > _DUTY_MAX) duty = _DUTY_MAX;
  
  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX) {
    digitalWrite(PIN_LED, LOW);  // LED 켜기
  } else {
    digitalWrite(PIN_LED, HIGH);   // LED 끄기
  }
  
  myservo.writeMicroseconds(duty);
  
  Serial.print("_DUTY_MIN:");  Serial.print(_DUTY_MIN);
  Serial.print(",_DIST_MIN:"); Serial.print(_DIST_MIN);
  Serial.print(",IR:");        Serial.print(a_value);
  Serial.print(",dist_raw:");  Serial.print(dist_raw);
  Serial.print(",ema:");       Serial.print(dist_ema);
  Serial.print(",servo:");     Serial.print(duty);
  Serial.print(",_DIST_MAX:"); Serial.print(_DIST_MAX);
  Serial.print(",_DUTY_MAX:"); Serial.print(_DUTY_MAX);
  Serial.println("");
}
