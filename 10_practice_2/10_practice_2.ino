/*
 * 주차장 차단기 - Ease-in-out 방식
 * 삼각함수(코사인)를 이용한 부드러운 움직임
 */

#include <Servo.h>

// 핀 정의
#define PIN_SERVO 10
#define PIN_TRIG 12
#define PIN_ECHO 13

// 서보 설정
Servo barrier;
const int ANGLE_DOWN = 100;   // 차단기 내린 각도
const int ANGLE_UP = 180;    // 차단기 올린 각도


// 이동 시간 설정
const unsigned long MOVING_TIME = 2000;

// 상태 변수
unsigned long moveStartTime = 0;
int startAngle = ANGLE_DOWN;
int targetAngle = ANGLE_DOWN;
bool isMoving = false;

// 차량 감지 설정
const int DETECT_DISTANCE = 20;
const int CHECK_INTERVAL = 100;
unsigned long lastCheckTime = 0;


void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  barrier.attach(PIN_SERVO);
  barrier.write(ANGLE_DOWN);
  delay(500);
  
  Serial.println("주차장 차단기 시작 (Ease-in-out)");
}


void loop() {
  unsigned long currentTime = millis();
  
  // 거리 측정
  if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
    lastCheckTime = currentTime;
    
    int distance = measureDistance();
    Serial.print("거리: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    // 차량 감지 로직
    if (distance > 0 && distance <= DETECT_DISTANCE) {
      if (targetAngle != ANGLE_UP) {
        Serial.println("차량 감지! 차단기 올림");
        startMove(ANGLE_UP);
      }
    } else {
      if (targetAngle != ANGLE_DOWN && distance > DETECT_DISTANCE) {
        Serial.println("차량 통과! 차단기 내림");
        startMove(ANGLE_DOWN);
      }
    }
  }
  
  // 서보 부드럽게 이동
  if (isMoving) {
    updateServoEaseInOut();
  }
}


// 거리 측정 함수
int measureDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  long duration = pulseIn(PIN_ECHO, HIGH, 30000);
  if (duration == 0) return -1;
  
  return duration * 0.034 / 2;
}


// 이동 시작 함수
void startMove(int target) {
  startAngle = barrier.read(); // 현재 각도 저장
  targetAngle = target;
  moveStartTime = millis();
  isMoving = true;
}


// Ease-in-out 함수를 이용한 서보 제어
void updateServoEaseInOut() {
  unsigned long elapsed = millis() - moveStartTime;
  
  if (elapsed >= MOVING_TIME) {
    barrier.write(targetAngle);
    isMoving = false;
    Serial.println("이동 완료");
    return;
  }
  
  // 진행률 계산 (0.0 ~ 1.0)
  float t = (float)elapsed / MOVING_TIME;
  
  // Ease-in-out 공식 (코사인 기반)
  // easeInOut(t) = (1 - cos(t * PI)) / 2
  float easeValue = (1.0 - cos(t * PI)) / 2.0;
  
  // 각도 계산
  int angle = startAngle + (targetAngle - startAngle) * easeValue;
  
  barrier.write(angle);
}
