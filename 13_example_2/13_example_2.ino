#include <Servo.h>

// Arduino pin assignment
#define PIN_SERVO 10

// configurable parameters for Servo
#define _DUTY_MIN 550
#define _DUTY_NEU 1600
#define _DUTY_MAX 2400
#define _POS_START (_DUTY_MIN + 100)    // 650 (0도 근처)
#define _POS_END   (_DUTY_MAX - 100)    // 2300 (180도 근처)

// Loop Interval
#define INTERVAL 20     // servo update interval (unit: msec)

// 실험 단계 정의
#define STAGE_WAIT 0      // 대기 (1초)
#define STAGE_EXP1 1      // 실험1: 60초, 180도, speed=3
#define STAGE_EXP2 2      // 실험2: 300초, 90도, speed=0.3
#define STAGE_DONE 3      // 완료

// global variables
unsigned long last_sampling_time;
unsigned long experiment_start_time;  // 각 실험 시작 시간
int experiment_stage = STAGE_WAIT;    // 현재 실험 단계

Servo myservo;
float duty_change_per_interval;
int duty_target;
int duty_curr;

void setup() {
  // 서보 모터 초기화
  myservo.attach(PIN_SERVO);
  
  // 시작 위치로 초기화
  duty_target = duty_curr = _POS_START;
  myservo.writeMicroseconds(duty_curr);
  
  // initialize serial port
  Serial.begin(57600);
  
  // 초기 대기 시간 설정
  experiment_start_time = millis();
  
  Serial.println("=== Servo Speed Experiment Start ===");
  Serial.println("Waiting 1 second...");
  
  last_sampling_time = 0;
}

void loop() {
  unsigned long current_time = millis();
  
  // wait until next sampling time
  if (current_time < (last_sampling_time + INTERVAL))
    return;
  
  // ===== 실험 단계 전환 로직 =====
  unsigned long elapsed = current_time - experiment_start_time;
  
  switch (experiment_stage) {
    case STAGE_WAIT:
      // 1초 대기 후 실험1 시작
      if (elapsed >= 1000) {
        experiment_stage = STAGE_EXP1;
        experiment_start_time = current_time;
        duty_target = _POS_END;  // 180도 목표
        
        // 실험1 설정: speed = 3, 180도 이동
        float servo_speed_1 = 3.0;
        duty_change_per_interval = 
          (float)(_DUTY_MAX - _DUTY_MIN) * (servo_speed_1 / 180.0) * (INTERVAL / 1000.0);
        
        Serial.println("\n=== Experiment 1 Start ===");
        Serial.print("Speed: 3 deg/sec, Target: 180 degrees, Duration: 60 sec");
        Serial.print(", duty_change_per_interval: ");
        Serial.println(duty_change_per_interval, 4);
      }
      break;
      
    case STAGE_EXP1:
      // 60초 경과 시 실험2 시작
      if (elapsed >= 60000) {
        experiment_stage = STAGE_EXP2;
        experiment_start_time = current_time;
        duty_curr = _POS_START;  // 0도로 리셋
        myservo.writeMicroseconds(duty_curr);
        
        // 중간 위치 (90도) 계산
        int pos_90deg = (_DUTY_MIN + _DUTY_NEU) / 2;  // 약 1075
        duty_target = pos_90deg;
        
        // 실험2 설정: speed = 0.3, 90도 이동
        float servo_speed_2 = 0.3;
        duty_change_per_interval = 
          (float)(_DUTY_MAX - _DUTY_MIN) * (servo_speed_2 / 180.0) * (INTERVAL / 1000.0);
        
        Serial.println("\n=== Experiment 2 Start ===");
        Serial.print("Speed: 0.3 deg/sec, Target: 90 degrees, Duration: 300 sec");
        Serial.print(", duty_change_per_interval: ");
        Serial.println(duty_change_per_interval, 4);
      }
      break;
      
    case STAGE_EXP2:
      // 300초 경과 시 실험 완료
      if (elapsed >= 300000) {
        experiment_stage = STAGE_DONE;
        Serial.println("\n=== All Experiments Completed ===");
        Serial.println("Total duration: 6 minutes (360 seconds)");
      }
      break;
      
    case STAGE_DONE:
      // 완료 후에는 아무 동작도 하지 않음
      return;
  }
  
  // ===== 서보 모터 제어 로직 =====
  if (experiment_stage != STAGE_WAIT && experiment_stage != STAGE_DONE) {
    float duty_float = duty_curr;
    
    if (duty_target > duty_float) {
      duty_float += duty_change_per_interval;
      if (duty_float > duty_target)
        duty_float = duty_target;
    } else if (duty_target < duty_float) {
      duty_float -= duty_change_per_interval;
      if (duty_float < duty_target)
        duty_float = duty_target;
    }
    
    duty_curr = (int)duty_float;
    myservo.writeMicroseconds(duty_curr);
  }
  
  // ===== 시리얼 출력 =====
  Serial.print("Stage:"); Serial.print(experiment_stage);
  Serial.print(",Time:"); Serial.print(elapsed / 1000.0, 1);  // 초 단위
  Serial.print(",Min:600");
  Serial.print(",duty_target:"); Serial.print(duty_target);
  Serial.print(",duty_curr:"); Serial.print(duty_curr);
  Serial.print(",duty_change:"); Serial.print(duty_change_per_interval, 4);
  Serial.println(",Max:2400");
  
  last_sampling_time += INTERVAL;
}
