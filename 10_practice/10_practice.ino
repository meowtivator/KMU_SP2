/*
  Sigmoid 및 Ease-in-out은 코드 하단에 작성해놓았습니다.
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
const unsigned long MOVING_TIME = 2000; // 2초에 걸쳐 이동

// 상태 변수
unsigned long moveStartTime = 0;
int currentAngle = ANGLE_DOWN;
int targetAngle = ANGLE_DOWN;
bool isMoving = false;

// 차량 감지 설정
const int DETECT_DISTANCE = 20; // 20cm 이내 감지
const int CHECK_INTERVAL = 100;  // 100ms마다 거리 체크
unsigned long lastCheckTime = 0;


void setup() {
  Serial.begin(9600);
  
  // 초음파 센서 핀 설정
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  // 서보 초기화
  barrier.attach(PIN_SERVO);
  barrier.write(ANGLE_DOWN); // 초기 위치: 차단기 내림
  delay(500);
  
  Serial.println("주차장 차단기 시작");
}


void loop() {
  unsigned long currentTime = millis();
  
  // 일정 간격으로 거리 측정
  if (currentTime - lastCheckTime >= CHECK_INTERVAL) {
    lastCheckTime = currentTime;
    
    int distance = measureDistance();
    Serial.print("거리: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    // 차량 감지 로직
    if (distance > 0 && distance <= DETECT_DISTANCE) {
      // 차량 감지 → 차단기 올림
      if (targetAngle != ANGLE_UP) {
        Serial.println("차량 감지! 차단기 올림");
        startMove(ANGLE_UP);
      }
    } else {
      // 차량 없음 → 차단기 내림
      if (targetAngle != ANGLE_DOWN && distance > DETECT_DISTANCE) {
        Serial.println("차량 통과! 차단기 내림");
        startMove(ANGLE_DOWN);
      }
    }
  }
  
  // 서보 부드럽게 이동
  if (isMoving) {
    updateServoSigmoid();
  }
}


// 거리 측정 함수
int measureDistance() {
  // 트리거 핀 초기화
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  
  // 10μs 펄스 발생
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  // 에코 핀에서 펄스 수신
  long duration = pulseIn(PIN_ECHO, HIGH, 30000); // 30ms 타임아웃
  
  // 거리 계산 (cm)
  if (duration == 0) {
    return -1; // 측정 실패
  }
  
  int distance = duration * 0.034 / 2;
  return distance;
}


// 이동 시작 함수
void startMove(int target) {
  targetAngle = target;
  moveStartTime = millis();
  isMoving = true;
}


// Sigmoid 함수를 이용한 서보 제어
void updateServoSigmoid() {
  unsigned long elapsed = millis() - moveStartTime;
  
  if (elapsed >= MOVING_TIME) {
    // 이동 완료
    barrier.write(targetAngle);
    currentAngle = targetAngle;
    isMoving = false;
    Serial.println("이동 완료");
    return;
  }
  
  // 진행률 계산 (0.0 ~ 1.0)
  float progress = (float)elapsed / MOVING_TIME;
  
  // Sigmoid 함수 적용
  // sigmoid(x) = 1 / (1 + e^(-k*(x-0.5)))
  // k=10으로 설정하여 적당한 곡선 생성
  float k = 10.0;
  float sigmoid = 1.0 / (1.0 + exp(-k * (progress - 0.5)));
  
  // 0~1 범위로 정규화 (sigmoid는 0.0~1.0 범위가 아니므로)
  float normalized = (sigmoid - 0.0067) / (0.9933 - 0.0067);
  
  // 각도 계산
  int angle;
  if (targetAngle > currentAngle) {
    // 올라갈 때
    angle = currentAngle + (targetAngle - currentAngle) * normalized;
  } else {
    // 내려갈 때
    angle = currentAngle - (currentAngle - targetAngle) * normalized;
  }
  
  barrier.write(angle);
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
